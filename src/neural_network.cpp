#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <sstream>
#include "neural_network.h"
#include "main.h"

using namespace std;

// Вспомогательная функция для генерации случайных чисел
static mt19937 rng(random_device{}());

// Функции активации
float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x * 3.0f)); // F(x) = 1 / (1 + e^(-x * 3)), "плющим" значение до диапозона от 0 до 1
}

float relu(float x) {
    // return max(0.0f, x); // Отсекаем всё, что меньше 0
    x = max(-1.0f, x);
    x = min(x, 1.0f);
    return x;
}

GeneLayer::GeneLayer(int inputSize, int outputSize, const string& activation) : activation(activation) {
    weights.resize(inputSize, vector<float>(outputSize));
    biases.resize(outputSize, 0.0f);
    
    uniform_real_distribution<float> dist(-1.0, 1.0);
    
    // Инициализация случайными весами
    for (int i = 0; i < inputSize; i++) {
        for (int j = 0; j < outputSize; j++) {
            weights[i][j] = dist(rng);
        }
    }
    
    // Инициализация biases
    for (int j = 0; j < outputSize; j++) {
        biases[j] = dist(rng) * 0.1f;
    }
}

vector<float> GeneLayer::forward(const vector<float>& inputs) const {
    int outputSize = weights[0].size();
    vector<float> outputs(outputSize, 0.0);
    
    // Матричное умножение
    for (int j = 0; j < outputSize; j++) {
        for (int i = 0; i < inputs.size(); i++) {
            outputs[j] += inputs[i] * weights[i][j];
        }
        
        // Смещение
        outputs[j] += biases[j];
        
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
    uniform_real_distribution<float> dist(-mutationPower, mutationPower);
    
    // Мутируем веса
    for (auto& row : weights) {
        for (auto& weight : row) {
            weight += dist(rng);
            weight = min(weight, 1.0f);
            weight = max(weight, -1.0f);
        }
    }
    
    // Мутируем смещения
    for (auto& bias : biases) {
        bias += dist(rng) * 0.3f;
        bias = min(bias, 1.0f);
        bias = max(bias, -1.0f);
    }
}

void NeuralNetwork::addLayer(unique_ptr<GeneLayer> layer) {
    layers.push_back(move(layer));
}

vector<float> NeuralNetwork::predict(const vector<float>& inputs) const {
    vector<float> outputs = inputs;
    
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
        const auto& biases = layer->getBiases();
        string activation = layer->getActivation();
        
        auto newLayer = make_unique<GeneLayer>(weights.size(), weights[0].size(), activation);
        
        newLayer->setWeights(weights);
        newLayer->setBiases(biases);
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
    const auto& otherLayers = otherNet.getLayers();

    uniform_real_distribution<float> dist(0.0, 1.0);

    for (int i = 0; i < layers.size(); i++) {
        const auto& weights1 = layers[i]->getWeights();
        const auto& weights2 = otherLayers[i]->getWeights();
        const auto& biases1 = layers[i]->getBiases();
        const auto& biases2 = otherLayers[i]->getBiases();

        auto newLayer = make_unique<GeneLayer>(weights1.size(), weights1[0].size(), layers[i]->getActivation());

        vector<vector<float>> newWeights(weights1.size(), vector<float>(weights1[0].size()));
        vector<float> newBiases(biases1.size());

        // Скрещивание весов
        for (int i = 0; i < weights1.size(); i++) {
            for (int j = 0; j < weights1[i].size(); j++) {
                if (dist(rng) < 0.5f) {
                    newWeights[i][j] = weights1[i][j];
                } else {
                    newWeights[i][j] = weights2[i][j];
                }
            }
        }

        // Скрещивание смещений
        for (int i = 0; i < biases1.size(); i++) {
            if (dist(rng) < 0.5f) {
                newBiases[i] = biases1[i];
            } else {
                newBiases[i] = biases2[i];
            }
        }

        newLayer->setWeights(newWeights);
        newLayer->setBiases(newBiases);
        newNet->addLayer(move(newLayer));
    }

    return newNet;
}

NeuralGene::NeuralGene() {
    neuralNet = make_unique<NeuralNetwork>();

    neuralNet->addLayer(make_unique<GeneLayer>(InputValues, NeuronsInHiddenLayer, "relu"));
    neuralNet->addLayer(make_unique<GeneLayer>(NeuronsInHiddenLayer, OutputValues, "sigmoid"));
}

NeuralGene::NeuralGene(unique_ptr<NeuralNetwork> network) : neuralNet(move(network)) {}

pair<int, int> NeuralGene::decideDirection(const vector<Cell>& surroundings, int energy, pair<int, int> directionToFood) {
    vector<float> inputs(InputValues); // InputValues должно быть 11
    
    // ДОЛЖНО БЫТЬ 8 клеток окружения!
    for (int i = 0; i < 8; i++) {  // ИЗМЕНИТЬ с 4 на 8
        switch (surroundings[i].type) {
            case EMPTY: inputs[i] = 0.0; break;
            case FOOD: inputs[i] = 1.0; break;
            case WALL: inputs[i] = -1.0; break;
            case AGENT: inputs[i] = -1.0; break;
        }
    }
    
    inputs[4] = directionToFood.first;  // dx
    inputs[5] = directionToFood.second; // dy
    
    // Нормируем кол-во энергии
    inputs[6] = min((float)energy / (INIT_ENERGY_AGENT * 2), 1.0f);
    
    vector<float> outputs = neuralNet->predict(inputs); // 0 - Вниз, 1 - Вверх, 2 - Влево, 3 - Вправо
    
    // Находим направление с максимальным значением
    int index = -1;
    auto el = max_element(outputs.begin(), outputs.end());
    int max_i = distance(outputs.begin(), el);
    if (outputs[max_i] > 0.5f) {
        index = max_i;
    }
    
    // Переводим индекс в направление
    switch (index) {
        case 0: return {0, -1}; // Вверх
        case 1: return {-1, 0}; // Влево
        case 2: return {1, 0};  // Вправо
        case 3: return {0, 1};  // Вниз
        default: return {0, 0}; // На месте
    }
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
    auto newNeuralNet = neuralNet->crossing(otherNeuralGene->getNeuralNet());
    return make_unique<NeuralGene>(move(newNeuralNet));
}

string NeuralGene::saveDataCSV() const {
    // Пока всегда 2 слоя
    const auto& weights1 = neuralNet->getLayers()[0]->getWeights();
    const auto& weights2 = neuralNet->getLayers()[1]->getWeights();
    const auto& biases1 = neuralNet->getLayers()[0]->getBiases();
    const auto& biases2 = neuralNet->getLayers()[1]->getBiases();
    
    stringstream data;
    data << fixed;
    
    data << "Weights;Layers;InputValues;InHiddenLayer;OutputValues;ActivationMid;ActivationLast\n";
    data << (float)weights1[0][0] << ";";
    data << (int)neuralNet->getLayers().size() + 1 << ";";
    data << (int)weights1.size() << ";";
    data << (int)weights2.size() << ";";
    data << (int)weights2[0].size() << ";";
    data << neuralNet->getLayers()[0]->getActivation() << ";";
    data << neuralNet->getLayers()[1]->getActivation() << "\n";

    // Сохраняем веса первого слоя
    for (int i = 0; i < weights1.size(); i++) {
        for (int j = 0; j < weights1[i].size(); j++) {
            data << (float)weights1[i][j] << "\n";
        }
    }
    
    // Сохраняем смещения первого слоя
    for (int i = 0; i < biases1.size(); i++) {
        data << (float)biases1[i] << "\n";
    }
    
    // Сохраняем веса второго слоя
    for (int i = 0; i < weights2.size(); i++) {
        for (int j = 0; j < weights2[i].size(); j++) {
            data << (float)weights2[i][j] << "\n";
        }
    }
    
    // Сохраняем смещения второго слоя
    for (int i = 0; i < biases2.size(); i++) {
        data << (float)biases2[i] << "\n";
    }

    return data.str();
}