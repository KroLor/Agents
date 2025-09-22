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
 * Агент не знает о поле. Он знает свою координату (вне поля), энергию, ген, количество шагов, состояние и собственное окружение. (Вектор направления к еде промежуточный параметр)
 */
class Agent {
private:
    unique_ptr<Gene> gene;               // Уникальный указатель на ген (нейросеть)
    vector<Cell> surroundings;           // 8 окружающих клеток
    int energy;                          // Количество энергии
    int steps;                           // Количество шагов
    pair<int, int> directionToFood;      // Вектор направления к ближайшей еде
    int x, y;                            // Координаты положения
    bool isAlive;                        // Состояние агента

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

    /**
     * @brief Увеличивает возраст агента на 1 такт.
     */
    void stepTick();
    
public:
    Agent();
    Agent(int x, int y, int initialEnergy, unique_ptr<Gene> gene);
    ~Agent();
    
    // Основные методы жизненного цикла
    
    /**
     * @brief Сканирует и запоминает состояние 8 окружающих клеток.
     * @param grid Поле, в котором агент осматривается.
     */
    void lookAround(vector<vector<Cell>>* grid);

    /**
     * @brief Принимает решение о действии на основе входных данных, используя нейросеть.
     */
    void decideAction();

    /**
     * @brief 
     * @return 
     */
    bool canReproduce();

    /**
     * @brief Агенту капут.
     */
    void die();

    // Геттеры и сеттеры

    /**
     * @brief Возвращает текущее положение по x.
     * @return Координату x.
     */
    int getX();

    /**
     * @brief Возвращает текущее положение по y.
     * @return ККоординату y.
     */
    int getY();
    
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
     * @brief Возвращает количество шагов.
     * @return Количество шагов.
     */
    int getSteps() const;

    /**
     * @brief Возвращает состояние агента.
     * @return true жив, иначе false.
     */
    bool getIsAlive();

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
     * @brief Агент делает свой ход.
     * @return true если перемещение успешно, иначе false.
     */
    bool move();
};