#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include "neural_network.h"
#include "main.h"

using namespace std;

// Вспомогательная функция для генерации случайных чисел
static mt19937 rng(random_device{}());

// Функции активации
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x)); // F(x) = 1 / (1 + e^(-x)), "плющим" значение до диапозона от 0 до 1
}

double relu(double x) {
    return max(0.0, x); // Отсекаем всё, что меньше 0
}

GeneLayer::GeneLayer(int inputSize, int outputSize, const string& activation) : activation(activation) {
    weights.resize(inputSize, vector<double>(outputSize));
    
    uniform_real_distribution<double> dist(-1.0, 1.0);
    
    // Инициализация случайными весами
    for (int i = 0; i < inputSize; i++) {
        for (int j = 0; j < outputSize; j++) {
            weights[i][j] = dist(rng);
        }
    }
}

vector<double> GeneLayer::forward(const vector<double>& inputs) const {
    int outputSize = weights[0].size();
    vector<double> outputs(outputSize, 0.0);
    
    // Матричное умножение
    for (int j = 0; j < outputSize; j++) {
        for (int i = 0; i < inputs.size(); i++) {
            outputs[j] += inputs[i] * weights[i][j];
        }
        
        // Функции активации
        if (activation == "sigmoid") {
            outputs[j] = sigmoid(outputs[j]);
        } else if (activation == "relu") {
            outputs[j] = relu(outputs[j]);
        }
    }
    
    return outputs;
}

void GeneLayer::mutate(float mutationPower) {
    uniform_real_distribution<double> dist(-mutationPower, mutationPower);
    for (auto& row : weights) {
        for (auto& weight : row) {
            weight += dist(rng);
        }
    }
}

void NeuralNetwork::addLayer(unique_ptr<GeneLayer> layer) {
    layers.push_back(move(layer));
}

vector<double> NeuralNetwork::predict(const vector<double>& inputs) const {
    vector<double> outputs = inputs;
    
    for (const auto& layer : layers) {
        outputs = layer->forward(outputs);
    }
    
    return outputs;
}

unique_ptr<NeuralNetwork> NeuralNetwork::clone() const {
    auto newNet = make_unique<NeuralNetwork>();
    
    // Копируем послойно
    for (const auto& layer : layers) {
        const auto& weights = layer->getWeights();
        string activation = layer->getActivation();
        
        auto newLayer = make_unique<GeneLayer>(weights.size(), weights[0].size(), activation);
        
        newLayer->setWeights(weights);
        newNet->addLayer(move(newLayer));
    }
    
    return newNet;
}

void NeuralNetwork::mutate(float mutationPower) {
    for (auto& layer : layers) {
        layer->mutate(mutationPower);
    }
}

unique_ptr<NeuralNetwork> NeuralNetwork::crossing(const NeuralNetwork& otherNet) const {
    auto newNet = make_unique<NeuralNetwork>();
    const auto& otherlayers = otherNet.getLayers();

    uniform_real_distribution<float> dist(0.0, 1.0);

    for (int i = 0; i < layers.size() && i < otherlayers.size(); i++) {
        const auto& weights1 = layers[i]->getWeights();
        const auto& weights2 = otherlayers[i]->getWeights();

        auto newLayer = make_unique<GeneLayer>(weights1.size(), weights1[0].size(), layers[i]->getActivation());

        vector<vector<double>> newWeights = weights1;

        // Скрещивание
        for (int j = 0; j < weights1.size() && j < weights2.size(); j++) {
            for (int i = 0; i < weights1[j].size() && i < weights2[j].size(); i++) {
                // С шансом 33.3% прошлый вес в слое нынешней нейросети заменится на новый, тот же вес из того же слоя, но другой нейросети
                if (dist(rng) < 0.333f) {
                    newWeights[j][i] = weights2[j][i];
                }
            }
        }

        newLayer->setWeights(newWeights);
        newNet->addLayer(move(newLayer));
    }

    return newNet;
}

NeuralGene::NeuralGene() {
    neuralNet = make_unique<NeuralNetwork>();

    neuralNet->addLayer(make_unique<GeneLayer>(INPUT_VALUES, NEURONS_IN_HIDDEN_LAYER, "relu")); // Скрытый слой
    neuralNet->addLayer(make_unique<GeneLayer>(NEURONS_IN_HIDDEN_LAYER, OUTPUT_VALUES, "sigmoid")); // Выходной слой
}

NeuralGene::NeuralGene(unique_ptr<NeuralNetwork> network) : neuralNet(move(network)) {}

pair<int, int> NeuralGene::decideDirection(const vector<Cell>& surroundings, int energy, pair<int, int> directionToFood) {
    vector<double> inputs(INPUT_VALUES);
    
    for (int i = 0; i < 4; i++) {
        // Представляем клетки в виде чисел: EMPTY = -1; FOOD = 1; WALL = 0; AGENT = 0
        switch (surroundings[i].type) {
            case EMPTY: inputs[i] = -1.0; break;
            case FOOD: inputs[i] = 1.0; break;
            case WALL: inputs[i] = 0.0; break;
            case AGENT: inputs[i] = 0.0; break;
        }
    }
    
    inputs[4] = directionToFood.first;  // dx
    inputs[5] = directionToFood.second; // dy
    
    // Нормируем кол-во энергии
    // inputs[6] = min((double)energy / ((double)INIT_ENERGY_AGENT * 2), 1.0);
    
    vector<double> outputs = neuralNet->predict(inputs); // 0 - Вниз, 1 - Вверх, 2 - Влево, 3 - Вправо
    
    // Находим направление с максимальным значением
    int index = 0;
    for (int i = 1; i < 4; i++) {
        if (outputs[i] > outputs[index]) { // Если выходы с индексами от 1 до 3 не наибольшие, то наибольший 0
            index = i;
        }
    }
    
    // Переводим индекс в направление
    switch (index) {
        case 0: return {0, -1}; // Вниз
        case 1: return {0, 1};  // Вверх
        case 2: return {-1, 0}; // Влево
        case 3: return {1, 0};  // Вправо
        default: return {0, 0}; // На месте
    }
    /** 
     * Всего по dx и dy вариантов:
     * (-1, 1)   (0, 1)   (1, 1)
     * (-1, 0)   (0, 0)   (1, 0)
     * (-1, -1)  (0, -1)  (1, -1)
     * 
     * Движение не возможно по диагонали, значит возможно лишь 4 варианта:
     *        (0, 1)
     * (-1, 0)   @   (1, 0)
     *        (0, -1)
     */
}

unique_ptr<Gene> NeuralGene::clone() const {
    auto newNeuralNet = neuralNet->clone();
    return make_unique<NeuralGene>(move(newNeuralNet)); // NeuralGene === Gene
}

unique_ptr<Gene> NeuralGene::mutation(float mutationPower) const {
    auto mutatedNet = neuralNet->clone();
    mutatedNet->mutate(mutationPower);
    return make_unique<NeuralGene>(move(mutatedNet));
}

unique_ptr<Gene> NeuralGene::crossing(const Gene& pairGene) const {
    const NeuralGene* otherNeuralGene = dynamic_cast<const NeuralGene*>(&pairGene);
    // Подходит ли ген
    if (!otherNeuralGene) {
        // Если типы не совпадают, возвращаем клон текущего гена
        return clone();
    }
    
    auto newNeuralNet = neuralNet->crossing(otherNeuralGene->getNeuralNet());
    return make_unique<NeuralGene>(move(newNeuralNet));
}