#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "streamout.h"

using namespace std;

//зовыйосети)
class Gene {                                        ///////////////////////////////////////////
public:
    virtual ~Gene() = default;
    
    // Основные методы нейросети

    virtual pair<int, int> decideDirection(const vector<Cell>& surroundings) = 0;

    virtual bool decideReproduction(const Agent& agent) = 0;

    virtual unique_ptr<Gene> mutate() const = 0;

    virtual unique_ptr<Gene> clone() const = 0;
    
    // Методы для сохранения/загрузки

    virtual void serialize() const = 0;

    virtual void deserialize() = 0;
};

class Agent {
private:
    unique_ptr<Gene> gene;               // Уникальный указатель на ген (нейросеть)
    vector<Cell> surroundings;           // 8 окружающих клеток
    int energy;                          // Количество энергии
    int age;                             // Возраст (количество шагов)
    pair<int, int> directionToFood;      // Вектор направления к ближайшей еде
    int x, y;                            // Координаты положения
    bool isAlive;                        // Состояние агента
    
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
    void ageTick();

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
     * @brief Проверяет, жив ли агент.
     * @return Состояние агента.
     */
    bool getIsAlive() const;

    /**
     * @brief Устанавливает состояние агента.
     * @param alive Состояние агента (жив/не жив).
     */
    void setIsAlive(bool alive);

    /**
     * @brief Возвращает возраст агента в тактах.
     * @return Возраст (количество шагов).
     */
    int getAge() const;

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
     * @param fromX Прошлое положение по X.
     * @param fromY Прошлое положение по Y.
     * @param toX Новое положение по X.
     * @param toY Новое положение по Y.
     * @return true если перемещение успешно, иначе false.
     */
    bool move(int fromX, int fromY, int toX, int toY);
};