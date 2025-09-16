#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "streamout.h"

using namespace std;

class Agent {
private:
    unique_ptr<Gene> gene;               // Уникальный указатель на ген (нейросеть)
    const vector<vector<Cell>>& field;   // Поле, в котором находится агент
    vector<CellType> surroundings;       // 8 окружающих клеток
    int energy;                          // Количество энергии
    pair<int, int> directionToFood;      // Вектор направления к ближайшей еде
    int x, y;                            // Координаты положения
    bool isAlive;                        // Состояние агента
    
public:
    Agent();
    Agent(unique_ptr<Gene> gene, int x, int y, int initialEnergy);
    ~Agent();
    
    // Основные методы жизненного цикла
    /**
     * @brief Запоминает состояние окружающих клеток.
     */
    void lookAround();                                      ////////////////////////////////////////////////

    void decideAction();
    void executeAction(std::vector<std::vector<Cell>>& field, int x, int y);
    void update();
    
    // Методы работы с энергией
    void consumeEnergy(int amount);
    void gainEnergy(int amount);
    bool canReproduce() const;
    
    // Методы размножения и мутации
    std::unique_ptr<Agent> reproduce();
    void mutate();
    
    // Геттеры и сеттеры
    int getEnergy() const;
    void setEnergy(int newEnergy);
    bool getIsAlive() const;
    void setIsAlive(bool alive);
    int getAge() const;
    const std::vector<CellType>& getSurroundings() const;
    const std::pair<int, int>& getDirectionToFood() const;
    
    // Методы взаимодействия с окружением
    bool move(std::vector<std::vector<Cell>>& field, int fromX, int fromY, int toX, int toY);
    void eat(std::vector<std::vector<Cell>>& field, int x, int y);
    void attack(Agent& otherAgent);
    
    // Вспомогательные методы
    bool isStarving() const;
    bool isOld() const;
    void ageTick();
};