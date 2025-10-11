#pragma once

#include <vector>
#include <memory>
#include <random>
#include "gene.h"
#include "cells.h"

using namespace std;

/**
 * @brief Класс - агент.
 * 
 * Содержит методы для управления агентом.
 * Агент знает свои координаты (вне поля), энергию, ген, количество шагов, состояние и собственное окружение. (Вектор направления к еде промежуточный параметр)
 */
class Agent {
private:
    unique_ptr<Gene> gene;           // Уникальный указатель на ген (нейросеть)
    vector<Cell> surroundings;       // 8 окружающих клеток
    int energy;                      // Количество энергии
    int steps;                       // Количество шагов
    pair<int, int> directionToFood;  // Вектор направления к ближайшей еде
    int x, y;                        // Координаты положения
    bool isAlive;                    // Состояние агента

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


    void initializeBrain();

public:
    Agent();
    Agent(int x, int y, int energy, unique_ptr<Gene> gene = nullptr);
    ~Agent();
    
    /**
     * @brief Сканирует и запоминает состояние 8 окружающих клеток.
     * @param grid Поле, в котором агент осматривается.
     */
    void lookAround(vector<vector<Cell>>* grid);

    /**
     * @brief Принимает решение о действии на основе входных данных, используя нейросеть.
     * @return true если ход выполнен успешно, иначе false.
     */
    bool decideAction(const vector<vector<Cell>>& grid);

    /**
     * @brief 
     * @return 
     */
    bool canReproduce();

    /**
     * @brief Агенту капут.
     */
    void die();

    /**
     * @brief Агент делает свой ход.
     * @return true если перемещение или бездействие успешно, иначе false.
     */
    bool move(int dx, int dy, const vector<vector<Cell>>& grid);

    /**
     * @brief Увеличивает возраст агента на 1 такт.
     */
    void stepTick();

    /**
     * @brief Возвращает текущее положение по x.
     * @return Координату x.
     */
    int getX() const { return x; }

    /**
     * @brief Возвращает текущее положение по y.
     * @return Координату y.
     */
    int getY() const { return y; }

    /**
     * @brief Задает новое положение по x.
     * @param newX Новое положение по x.
     */
    void setX(int newX) { x = newX; }

    /**
     * @brief Задает новое положение по y.
     * @param newY Новое положение по y.
     */
    void setY(int newY) { y = newY; }

    /**
     * @brief Возвращает текущее кол-во энергии.
     * @return Кол-во энергии.
     */
    int getEnergy() const { return energy; }

    /**
     * @brief Задает кол-во энергии.
     * @param Кол-во задаваемой энергии.
     */
    void setEnergy(int newEnergy) { energy = newEnergy; }

    /**
     * @brief Возвращает кол-во шагов.
     * @return Кол-во шагов.
     */
    int getSteps() const { return steps; }

    /**
     * @brief Возвращает состояние агента.
     * @return true жив, иначе false.
     */
    bool getIsAlive() const { return isAlive; }

    /**
     * @brief Возвращает массив окружающих клеток.
     * @return Массив клеток.
     */
    const vector<Cell>& getSurroundings() const { return surroundings; }

    /**
     * @brief Возвращает направление к ближайшей еде.
     * @return Вектор направления.
     */
    const pair<int, int>& getDirectionToFood(vector<vector<Cell>>* grid);
};