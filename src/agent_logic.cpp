#include <algorithm>
#include <iostream>
#include "limits.h"
#include "agent_logic.h"

using namespace std;

// Вспомогательная функция для генерации случайных чисел
static mt19937 rng(random_device{}());

Agent::Agent() : x(0), y(0), energy(100), steps(0), isAlive(true), 
                directionToFood({0,0}) {}

// В конструктор добавляем инициализацию "мозга"
Agent::Agent(int x, int y, int energy, unique_ptr<Gene> gene)
    : x(x), y(y), energy(energy), steps(0), isAlive(true), 
      directionToFood({0,0}), gene(std::move(gene)) 
{
    initializeBrain();  // Инициализация нейросети (заглушка)
}

Agent::~Agent() {}

void Agent::consumeEnergy(int amount) {
    energy -= amount;
    if (energy < 0) energy = 0;
}

void Agent::gainEnergy(int amount) {
    energy += amount;
}

void Agent::stepTick() {
    steps++;
}

void Agent::lookAround(vector<vector<Cell>>* grid) {
    surroundings.clear();
    
    // 8 направлений вокруг агента
    vector<pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    
    for (const auto& [dx, dy] : directions) {
        int newX = x + dx;
        int newY = y + dy;
        
        // Проверка границ
        if (newX >= 0 && newX < grid->size() && 
            newY >= 0 && newY < (*grid)[0].size()) {
            surroundings.push_back((*grid)[newX][newY]);
        } else {
            // За пределами - стена
            surroundings.push_back(Cell{WALL});
        }
    }
}

const pair<int, int>& Agent::getDirectionToFood(vector<vector<Cell>>* grid) {
    int minDistance = INT_MAX;
    directionToFood = {0, 0};
    
    // Ищем ближайшую еду
    for (int i = 0; i < grid->size(); ++i) {
        for (int j = 0; j < (*grid)[i].size(); ++j) {
            if ((*grid)[i][j].type == FOOD) {
                int distance = abs(i - x) + abs(j - y);
                if (distance < minDistance) {
                    minDistance = distance;
                    directionToFood = {i - x, j - y};
                }
            }
        }
    }
    
    return directionToFood;
}

void Agent::decideAction(const vector<vector<Cell>>& grid) {
    // Временное решение: случайное движение с учетом окружения
    
    vector<pair<int, int>> directions = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}  // Вверх, вниз, влево, вправо
    };
    
    // Фильтруем доступные направления
    vector<pair<int, int>> availableDirs;
    for (const auto& [dx, dy] : directions) {
        int newX = x + dx;
        int newY = y + dy;
        
        // Проверяем границы и тип клетки
        if (newX >= 0 && newX < grid.size() && 
            newY >= 0 && newY < grid[0].size()) {
            
            const Cell& targetCell = grid[newX][newY];
            if (targetCell.type == EMPTY || targetCell.type == FOOD) {
                availableDirs.push_back({dx, dy});
            }
        }
    }
    
    // Если нет доступных направлений, остаемся на месте
    if (availableDirs.empty()) {
        move(0, 0, grid);
        return;
    }
    
    // Выбираем случайное направление из доступных
    uniform_int_distribution<int> dist(0, availableDirs.size() - 1);
    auto [dx, dy] = availableDirs[dist(rng)];
    move(dx, dy, grid);
}

bool Agent::move(int dx, int dy, const vector<vector<Cell>>& grid) {
    if (dx == 0 && dy == 0) {
        consumeEnergy(1); // Все равно тратим энергию, даже если стоим на месте
        return true;
    }
    
    int newX = x + dx;
    int newY = y + dy;
    
    // Проверка границ
    if (newX < 0 || newX >= grid.size() || newY < 0 || newY >= grid[0].size()) {
        return false;
    }
    
    // Проверка типа клетки
    const Cell& targetCell = grid[newX][newY];
    if (targetCell.type == WALL || targetCell.type == AGENT) {
        return false;
    }
    
    // Перемещаем агента
    x = newX;
    y = newY;
    
    // Если клетка с едой - едим
    if (targetCell.type == FOOD) {
        gainEnergy(targetCell.foodValue);
    }
    
    // Тратим энергию на движение
    consumeEnergy(1);
    
    return true;
}

bool Agent::canReproduce() {
    // Временное условие для размножения
    return energy > 150 && steps > 10;
}

void Agent::die() {
    isAlive = false;
}

void Agent::initializeBrain() {
    
}