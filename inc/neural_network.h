#pragma once

#include <vector>
#include <memory>
#include <string>
#include "gene.h"
#include "cells.h"

using namespace std;

/**
 * @brief Класс слоя нейронной сети с матрицами весов.
 */
class GeneLayer {
private:
    vector<vector<double>> weights; // [input_size][output_size]
    string activation;              // "sigmoid" или "relu"

public:
    /**
     * @brief Конструктор слоя
     * @param inputSize Кол-во нейронов входного слоя.
     * @param outputSize Кло-во нейронов выходного слоя (связей с выходным слоем).
     * @param activation Функция активации (relu или sigmoid)
     */
    GeneLayer(int inputSize, int outputSize, const string& activation);
    
    /**
     * @brief Распространяет входные данные через слой.
     * @param inputs Входные значения для слоя.
     * @return Выходные значения слоя после вычисления.
     */
    vector<double> forward(const vector<double>& inputs) const;
    
    /**
     * @brief Возвращает веса слоя.
     * @return Ссылка на матрицу весов.
     */
    const vector<vector<double>>& getWeights() const { return weights; }


    // unique_ptr<GeneLayer> clone() const;
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
    vector<double> predict(const vector<double>& inputs) const;
    
    /**
     * @brief Возвращает слои нейронной сети.
     * @return Константная ссылка на вектор слоев сети.
     */
    const vector<unique_ptr<GeneLayer>>& getLayers() const { return layers; }
    
    /**
     * @brief Применяет мутации к весам нейронной сети.
     * @param mutationRate Интенсивность мутаций (вероятность изменения веса).
     */
    // void mutate(double mutationRate);
    
    /**
     * @brief Создает полную копию нейронной сети.
     * @return Умный указатель на копию сети.
     */
    // unique_ptr<NeuralNetwork> clone() const;
};

/**
 * @brief Реализация гена на основе нейронной сети.
 */
class NeuralGene : public Gene {
private:
    unique_ptr<NeuralNetwork> neuralNet;

public:
    NeuralGene();
    // NeuralGene(unique_ptr<NeuralNetwork> network);
    
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
     * @return Умный указатель на мутировавший ген.
     */
    // unique_ptr<Gene> mutate() const override;
    
    /**
     * @brief Создает точную копию гена.
     * @return Умный указатель на копию гена.
     */
    // unique_ptr<Gene> clone() const override;
};