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

NeuralGene::NeuralGene() {
    neuralNet = make_unique<NeuralNetwork>();

    neuralNet->addLayer(make_unique<GeneLayer>(INPUT_VALUES, NEURONS_IN_HIDDEN_LAYER, "relu")); // Скрытый слой
    neuralNet->addLayer(make_unique<GeneLayer>(NEURONS_IN_HIDDEN_LAYER, OUTPUT_VALUES, "sigmoid")); // Выходной слой
}

pair<int, int> NeuralGene::decideDirection(const vector<Cell>& surroundings, int energy, pair<int, int> directionToFood) {
    vector<double> inputs(11);
    
    for (int i = 0; i < 8; i++) {
        // Представляем клетки в виде чисел: EMPTY = 0; FOOD = 1; WALL = -1; AGENT = -1
        switch (surroundings[i].type) {
            case EMPTY: inputs[i] = 0.0; break;
            case FOOD: inputs[i] = 1.0; break;
            case WALL: inputs[i] = -1.0; break;
            case AGENT: inputs[i] = -1.0; break;
        }
    }
    
    // Сдвинем крайние значения ({-1, 1} => {-0.1, 0.1}), чтобы уменьшить влияние этого параметра
    inputs[8] = directionToFood.first / 10.0; // dx
    inputs[9] = directionToFood.second / 10.0; // dy
    
    // Нормируем кол-во энергии
    inputs[10] = (double)energy / (double)INIT_ENERGY_AGENT;
    
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
        default: return {0, 0};
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