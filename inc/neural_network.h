#pragma once

#include <vector>
#include <memory>
#include "gene.h"

using namespace std;

/**
 * @brief Базовый класс нейрона/гена искусственной нейронной сети.
 * 
 * Представляет отдельный нейрон с функцией активации, весами и способностью к клонированию.
 */
class Neuron {
public:
    virtual ~Neuron() = default;
    
    /**
     * @brief Вычисляет выходное значение нейрона на основе входных данных.
     * @param inputs Входные значения для нейрона.
     * @return Результат функции активации.
     */
    virtual double activate(const vector<double>& inputs) const = 0;
    
    /**
     * @brief Возвращает весовые коэффициенты нейрона.
     * @return Константная ссылка на вектор весов.
     */
    virtual const vector<double>& getWeights() const = 0;
    
    /**
     * @brief Устанавливает новые весовые коэффициенты нейрона.
     * @param weights Новые значения весов.
     */
    virtual void setWeights(const vector<double>& weights) = 0;
    
    /**
     * @brief Создает полную копию нейрона.
     * @return Умный указатель на копию нейрона.
     */
    virtual unique_ptr<Neuron> clone() const = 0;
};

/**
 * @brief Базовый класс слоя нейронной сети.
 * 
 * Содержит массив нейронов и обеспечивает связь между слоями.
 */
class GeneLayer {
public:
    virtual ~GeneLayer() = default;
    
    /**
     * @brief Распространяет входные данные через слой.
     * @param inputs Входные значения для слоя.
     * @return Выходные значения слоя после обработки.
     */
    virtual vector<double> forward(const vector<double>& inputs) const = 0;
    
    /**
     * @brief Возвращает нейроны, содержащиеся в слое.
     * @return Константная ссылка на вектор нейронов слоя.
     */
    virtual const vector<unique_ptr<Neuron>>& getNeurons() const = 0;
    
    /**
     * @brief Добавляет нейрон в слой.
     * @param neuron Умный указатель на добавляемый нейрон.
     */
    virtual void addNeuron(unique_ptr<Neuron> neuron) = 0;
    
    /**
     * @brief Создает полную копию слоя.
     * @return Умный указатель на копию слоя.
     */
    virtual unique_ptr<GeneLayer> clone() const = 0;
};

/**
 * @brief Класс нейронной сети, состоящей из нескольких слоев.
 * 
 * Делает предсказания, мутации и клонирование сети.
 */
class NeuralNetwork {
public:
    virtual ~NeuralNetwork() = default;
    
    /**
     * @brief Выполняет предсказание на основе входных данных.
     * @param inputs Входные значения для сети.
     * @return Выходные значения сети.
     */
    virtual vector<double> predict(const vector<double>& inputs) const = 0;
    
    /**
     * @brief Добавляет слой в нейронную сеть.
     * @param layer Умный указатель на добавляемый слой.
     */
    virtual void addLayer(unique_ptr<GeneLayer> layer) = 0;
    
    /**
     * @brief Возвращает слои нейронной сети.
     * @return Константная ссылка на вектор слоев сети.
     */
    virtual const vector<unique_ptr<GeneLayer>>& getLayers() const = 0;
    
    /**
     * @brief Применяет мутации к весам нейронной сети.
     * @param mutationRate Интенсивность мутаций (вероятность изменения веса).
     */
    virtual void mutate(double mutationRate) = 0;
    
    /**
     * @brief Создает полную копию нейронной сети.
     * @return Умный указатель на копию сети.
     */
    virtual unique_ptr<NeuralNetwork> clone() const = 0;
};

/**
 * @brief Реализация гена на основе нейронной сети.
 * 
 * Наследует абстрактный класс Gene и реализует его методы с использованием нейросети.
 */
class NeuralGene : public Gene {
private:
    unique_ptr<NeuralNetwork> neuralNet; // Нейронная сеть, реализующая логику гена
    
public:
    /**
     * @brief Конструктор, инициализирующий ген.
     * @param network Умный указатель на нейронную сеть.
     */
    NeuralGene(unique_ptr<NeuralNetwork> network);
    
    /**
     * @brief Определяет направление движения на основе окружения.
     * @param surroundings Вектор клеток окружения.
     * @return (delta_x, delta_y) - вектор направления.
     */
    pair<int, int> decideDirection(const vector<Cell>& surroundings) override;
    
    // /**
    //  * @brief Принимает решение о необходимости размножения.
    //  * @param agent Ссылка на агента для оценки его состояния.
    //  * @return true если агент должен размножиться, иначе false.
    //  */
    // bool decideReproduction(const Agent& agent) override;
    
    /**
     * @brief Создает мутированную копию гена.
     * @return Умный указатель на мутировавший ген.
     */
    unique_ptr<Gene> mutate() const override;
    
    /**
     * @brief Создает точную копию гена.
     * @return Умный указатель на копию гена.
     */
    unique_ptr<Gene> clone() const override;
    
    // /**
    //  * @brief Сериализует состояние гена для сохранения.
    //  */
    // void serialize() const override;
    
    // /**
    //  * @brief Восстанавливает состояние гена из сериализованных данных.
    //  */
    // void deserialize() override;
};