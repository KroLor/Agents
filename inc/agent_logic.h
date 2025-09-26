#pragma once

#include <vector>
#include <memory>
#include <random>
#include "gene.h"
#include "cells.h"

using namespace std;

class Agent {
private:
    unique_ptr<Gene> gene;
    vector<Cell> surroundings;
    int energy;
    int steps;
    pair<int, int> directionToFood;
    int x, y;
    bool isAlive;

    void consumeEnergy(int amount);
    void gainEnergy(int amount);

public:
    Agent();
    Agent(int x, int y, int energy, unique_ptr<Gene> gene = nullptr);
    ~Agent();
    
    // Основные методы
    void lookAround(vector<vector<Cell>>* grid);
    void decideAction(const vector<vector<Cell>>& grid);
    bool canReproduce();
    void die();
    bool move(int dx, int dy, const vector<vector<Cell>>& grid);
    void stepTick();

    // Геттеры и сеттеры
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int newX) { x = newX; }
    void setY(int newY) { y = newY; }
    int getEnergy() const { return energy; }
    void setEnergy(int newEnergy) { energy = newEnergy; }
    int getSteps() const { return steps; }
    bool getIsAlive() const { return isAlive; }
    const vector<Cell>& getSurroundings() const { return surroundings; }
    const pair<int, int>& getDirectionToFood(vector<vector<Cell>>* grid);
    void initializeBrain();
};