#pragma once

#include <vector>
#include <memory>
#include <string>
#include "gene.h"
#include "cells.h"

using namespace std;

/**
 * @brief Класс слоя нейронной сети с матрицами весов и смещениями.
 */
class GeneLayer {
private:
    vector<vector<float>> weights; // [input] [output]
    vector<float> biases;          // Смещения для каждого нейрона
    string activation;             // "sigmoid" или "relu"

public:
    GeneLayer(int inputSize, int outputSize, const string& activation);
    
    vector<float> forward(const vector<float>& inputs) const;
    
    /**
     * @brief Устанавливает веса слоя.
     */
    void setWeights(const vector<vector<float>>& newWeights) { weights = newWeights; }
    
    /**
     * @brief Устанавливает bias слоя.
     */
    void setBiases(const vector<float>& newBiases) { biases = newBiases; }
    
    /**
     * @brief Возвращает копию весов слоя.
     */
    vector<vector<float>> getWeightsCopy() const { return weights; }
    
    /**
     * @brief Возвращает копию bias слоя.
     */
    vector<float> getBiasesCopy() const { return biases; }
    
    /**
     * @brief Возвращает тип активации.
     */
    string getActivation() const { return activation; }
    
    /**
     * @brief Возвращает веса слоя.
     */
    const vector<vector<float>>& getWeights() const { return weights; }
    
    /**
     * @brief Возвращает bias.
     */
    const vector<float>& getBiases() const { return biases; }
    
    /**
     * @brief Мутирует веса и bias.
     */
    void mutate(float mutationPower);
};

/**
 * @brief Класс нейронной сети.
 */
class NeuralNetwork {
private:
    vector<unique_ptr<GeneLayer>> layers;

public:
    NeuralNetwork() = default;
    
    /**
     * @brief Добавляет слой в нейронную сеть.
     * @param layer Умный указатель на добавляемый слой.
     */
    void addLayer(unique_ptr<GeneLayer> layer);
    
    /**
     * @brief Выполняет предсказание на основе входных данных.
     * @param inputs Входные значения для сети.
     * @return Выходные значения сети.
     */
    vector<float> predict(const vector<float>& inputs) const;
    
    /**
     * @brief Возвращает слои нейронной сети.
     * @return Константная ссылка на вектор слоев сети.
     */
    const vector<unique_ptr<GeneLayer>>& getLayers() const { return layers; }
    
    /**
     * @brief Применяет мутации к весам и bias.
     * @param mutationPower Интенсивность мутаций.
     */
    void mutate(float mutationPower);

    unique_ptr<NeuralNetwork> crossing(const NeuralNetwork& otherNet) const;
    
    /**
     * @brief Создает полную копию нейронной сети.
     * @return Умный указатель на копию сети.
     */
    unique_ptr<NeuralNetwork> clone() const;

    vector<double> getWeights() const;

    /**
     * @brief Устанавливает веса для всех слоев
     */
    void setAllWeights(const std::vector<std::vector<std::vector<double>>>& allWeights) {
        if (allWeights.size() != layers.size()) {
            throw std::invalid_argument("Несоответствие количества слоев");
        }
        
        for (size_t i = 0; i < layers.size(); i++) {
            layers[i]->setWeights(allWeights[i]);
        }
    }
};

/**
 * @brief Реализация гена на основе нейронной сети.
 */
class NeuralGene : public Gene {
private:
    unique_ptr<NeuralNetwork> neuralNet;

public:
    NeuralGene();
    NeuralGene(unique_ptr<NeuralNetwork> network);
    NeuralGene(const NeuralGene& other) {
        if (other.neuralNet) {
            this->neuralNet = other.neuralNet->clone();
        }
    }
    
    /**
     * @brief Определяет направление движения на основе окружения, энергии и направления к еде.
     * @param surroundings Вектор клеток окружения (8 клеток)
     * @param energy Уровень энергии агента
     * @param directionToFood Вектор направления к ближайшей еде
     * @return (delta_x, delta_y) - вектор направления.
     */
    pair<int, int> decideDirection(const vector<Cell>& surroundings, int energy, pair<int, int> directionToFood) override;
    
    /**
     * @brief Создает мутированную копию гена.
     * @param mutationPower Сила мутации.
     * @return Умный указатель на мутировавший ген.
     */
    unique_ptr<Gene> mutation(float mutationPower) const override;
    
    /**
     * @brief Создает точную копию гена.
     * @return Умный указатель на копию гена.
     */
    unique_ptr<Gene> clone() const override;

    /**
     * @brief Скрещивает нейросети.
     */
    unique_ptr<Gene> crossing(const Gene& pairGene) const;

    const NeuralNetwork& getNeuralNet() const { return *neuralNet; }

    /**
     * 
     */
    void setNewNeuralNet(unique_ptr<NeuralNetwork>& newNeuralNet) { neuralNet = move(newNeuralNet); };

    string saveDataCSV() const;
};