#include <algorithm>
#include <iostream>
#include <limits>
#include "neural_network.h"
#include "agent_logic.h"
#include "main.h"

using namespace std;

// Вспомогательная функция для генерации случайных чисел
static mt19937 rng(random_device{}());

Agent::Agent() : x(0), y(0), energy(INIT_ENERGY_AGENT), steps(0), isAlive(true), directionToFood({0,0}) {}

Agent::Agent(int x, int y, int energy, unique_ptr<Gene> gene)
    : x(x), y(y), energy(energy), steps(0), isAlive(true), directionToFood({0,0}), gene(std::move(gene)) 
{
    if (gene == nullptr) {
        initializeBrain();
    }
}

Agent::~Agent() {}

void Agent::dEnergy(int amount) {
    energy += amount;
    if (energy < 0) energy = 0;
}

void Agent::stepTick() {
    steps++;
}

void Agent::lookAround(vector<vector<Cell>>* grid) {
    surroundings.clear();
    
    // 8 клеток вокруг агента
    vector<pair<int, int>> directions = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };
    
    for (const auto& [dx, dy] : directions) {
        int newX = x + dx;
        int newY = y + dy;

        surroundings.push_back((*grid)[newX][newY]);
    }
}

const pair<int, int>& Agent::getDirectionToFood(vector<vector<Cell>>* grid) {
    int minDistance = INT_MAX;
    directionToFood = {0, 0};
    
    for (int i = 0; i < grid->size(); ++i) {
        for (int j = 0; j < (*grid)[i].size(); ++j) {
            if ((*grid)[i][j].type == FOOD) {
                int distance = abs(i - x) + abs(j - y);

                if (distance < minDistance) {
                    minDistance = distance;
                    directionToFood = {i - x, j - y}; // Если агент на {5; 5}, а еда на {4; 4}, то {4; 4} - {5; 5} = {-1; -1}, что соответсвует напрвлению влево вверх
                }
            }
        }
    }
    
    return directionToFood;
}

bool Agent::decideAction(const vector<vector<Cell>>& grid) {
    #ifdef USE_A_NEURAL_NETWORK
        // Использование гена для принятия решения
        auto direction = gene->decideDirection(surroundings, energy, directionToFood);
        
        if (grid[x + direction.first][y + direction.second].type == WALL || grid[x + direction.first][y + direction.second].type == AGENT) {
            // Случайное движение
            vector<pair<int, int>> directions = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}}; // Вверх, вниз, влево, вправо
            vector<pair<int, int>> availableDirections;

            for (const auto& [dx, dy] : directions) {
                int newX = x + dx;
                int newY = y + dy;

                if (grid[newX][newY].type == EMPTY || grid[newX][newY].type == FOOD) {
                    availableDirections.push_back({dx, dy});
                }
            }
            
            if (availableDirections.empty()) {
                return move(0, 0, grid);
            }
            
            uniform_int_distribution<int> dist(0, availableDirections.size() - 1);
            auto [dx, dy] = availableDirections[dist(rng)];
            return move(dx, dy, grid);
        }
        return move(direction.first, direction.second, grid);
    #else
        // Случайное движение
        vector<pair<int, int>> directions = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}}; // Вверх, вниз, влево, вправо
        vector<pair<int, int>> availableDirections;

        for (const auto& [dx, dy] : directions) {
            int newX = x + dx;
            int newY = y + dy;

            if (grid[newX][newY].type == EMPTY || grid[newX][newY].type == FOOD) {
                availableDirections.push_back({dx, dy});
            }
        }
        
        if (availableDirections.empty()) {
            return move(0, 0, grid);
        }
        
        uniform_int_distribution<int> dist(0, availableDirections.size() - 1);
        auto [dx, dy] = availableDirections[dist(rng)];
        return move(dx, dy, grid);
    #endif
}

bool Agent::move(int dx, int dy, const vector<vector<Cell>>& grid) {
    if (dx == 0 && dy == 0) {
        dEnergy(-ENERGY_LOSS_DUE_TO_INACTION);
        return false;
    }
    
    int newX = x + dx;
    int newY = y + dy;

    if (grid[newX][newY].type == WALL || grid[newX][newY].type == AGENT) {
        dEnergy(-ENERGY_LOSS_DUE_TO_INACTION);
        return false;
    }
    
    x = newX;
    y = newY;
    
    dEnergy(-ENERGY_LOSS_PER_STEP);

    if (grid[newX][newY].type == FOOD) {
        dEnergy(grid[newX][newY].foodValue);
    }
    
    return true;
}

unique_ptr<Agent> Agent::clone() {
    unique_ptr<Gene> clonedGene = gene->clone();

    auto newAgent = make_unique<Agent>(x, y, energy, std::move(clonedGene));
    
    return newAgent;
}

void Agent::mutateGene(float mutationPower) {
    gene = gene->mutation(mutationPower);
}

void Agent::crossing(const Agent& pair) {
    unique_ptr<Gene> newGene = gene->crossing(pair.getGene());
    gene = std::move(newGene);
}

void Agent::die() {
    isAlive = false;
}

void Agent::initializeBrain() {
    gene = make_unique<NeuralGene>();
}