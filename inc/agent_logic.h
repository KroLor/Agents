#pragma once

#include <vector>
#include <memory>
#include "gene.h"
#include "cells.h"

using namespace std;

/**
 * @brief Класс - агент.
 * 
 * Содержит методы для управления агентом.
 */
class Agent {
private:
    unique_ptr<Gene> gene;               // Уникальный указатель на ген (нейросеть)
    vector<Cell> surroundings;           // 8 окружающих клеток
    int energy;                          // Количество энергии
    int steps;                           // Количество шагов
    pair<int, int> directionToFood;      // Вектор направления к ближайшей еде
    int x, y;                            // Координаты положения
    
public:
    Agent();
    Agent(unique_ptr<Gene> gene, int x, int y, int initialEnergy);
    ~Agent();
    
    // Основные методы жизненного цикла
    
    /**
     * @brief Сканирует и запоминает состояние 8 окружающих клеток.
     */
    void lookAround();

    /**
     * @brief Принимает решение о действии на основе входных данных, используя нейросеть.
     */
    void decideAction();

    /**
     * @brief Увеличивает возраст агента на 1 такт.
     */
    void stepTick();

    // Методы работы с энергией

    /**
     * @brief Уменьшает энергию агента на указанное количество.
     * @param amount Количество энергии.
     */
    void consumeEnergy(int amount);

    /**
     * @brief Увеличивает энергию агента на указанное количество.
     * @param amount Количество энергии.
     */
    void gainEnergy(int amount);

    // Геттеры и сеттеры

    /**
     * @brief Возвращает текущее значение энергии агента.
     * @return Количество энергии.
     */
    int getEnergy() const;

    /**
     * @brief Устанавливает новое значение энергии.
     * @param newEnergy Количество энергии.
     */
    void setEnergy(int newEnergy);

    /**
     * @brief Возвращает возраст агента в тактах.
     * @return Возраст (количество шагов).
     */
    int getSteps() const;

    /**
     * @brief Возвращает массив окружающих клеток.
     * @return Массив клеток.
     */
    const vector<Cell>& getSurroundings() const;

    /**
     * @brief Возвращает направление к ближайшей еде.
     * @return Вектор направления.
     */
    const pair<int, int>& getDirectionToFood() const;

    // Методы взаимодействия с окружением

    /**
     * @brief Переместить агента в указанную клетку.
     * @param toX Новое положение по X.
     * @param toY Новое положение по Y.
     * @return true если перемещение успешно, иначе false.
     */
    bool move(int toX, int toY);
};