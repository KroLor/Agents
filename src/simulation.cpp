#include <algorithm>
#include <random>
// #include <fstream>
#include <iostream>
#include "simulation.h"

using namespace std;

// Вспомогательная функция для генерации случайных чисел
static mt19937 rng(random_device{}());

EvolutionSimulation::EvolutionSimulation(vector<vector<Cell>> grid, 
                                       int initialPopulationSize, 
                                       int initialFoodCount)
    : grid(move(grid)), paused(false), simulationSpeed(1.0f), 
      mutationRate(0.1f), simulationIteration(0), 
      totalDeaths(0), totalAlives(0)
{
    initializePopulation(initialPopulationSize);
    initializeFood(initialFoodCount);
}

EvolutionSimulation::~EvolutionSimulation()
{
    // Умные указатели проведут автоматическую чистку памяти
}

void EvolutionSimulation::initializePopulation(int initialPopulationSize)
{
    population.clear(); // Удалим прошлую популяцию
    for (int i = 0; i < initialPopulationSize; i++) {
        int x, y;
        if (findRandomEmptyPosition(x, y)) {
            addAgent(x, y);
        }
    }
}

void EvolutionSimulation::initializeFood(int initialFoodCount)
{
    for (int i = 0; i < initialFoodCount; i++) {
        int x, y;
        if (findRandomEmptyPosition(x, y)) {
            addFood(x, y);
        }
    }
}

bool EvolutionSimulation::findRandomEmptyPosition(int& x, int& y) const
{
    vector<pair<int, int>> emptyPos;
    
    // Собираем все пустые клетки
    for (int x = 1; x < grid.size() - 1; x++) {
        for (int y = 1; y < grid[x].size() - 1; y++) {
            if (grid[x][y].type == EMPTY) {
                emptyPos.emplace_back(x, y); /* Выбираем пары координат (x, y), тип клетки которых EMPTY */
            }
        }
    }
    
    if (emptyPos.empty()) {
        return false;
    }
    
    // Выбираем случайную позицию из массива пар (x, y)
    uniform_int_distribution<int> dist(0, emptyPos.size() - 1); // Равномерное распределение от 0 до size - 1
    auto [posX, posY] = emptyPos[dist(rng)]; // Выбираем случайный индекс emptyPos из {0, 1, ..., emptyPos.size() - 1} и берём его координаты
    x = posX;
    y = posY;
    return true;
}

void EvolutionSimulation::simulateStep(float deltaTime)
{
    if (paused) return;
    
    simulationIteration++;

    updateAgents(deltaTime);
    spawnNewFood();
    updateState();
}

void EvolutionSimulation::updateAgents(float deltaTime)
{
    // Перемешиваем агентов для случайного порядка обработки
    shuffle(population.begin(), population.end(), rng);
    
    for (auto& agent : population) {
        if (agent->getIsAlive()) {
            // Проверяем смерть от голода
            if (agent->getEnergy() <= 0) {
                agent->die();
                totalDeaths++;
            }

            agent->lookAround(&grid);
            agent->decideAction();
            agent->move();
        }
        // Ждать deltaTime                                                    ................................
    }
}

void EvolutionSimulation::geneticAlgorithm()
{
    vector<Agent*> potentialParents;
    
    // Собираем агентов
    for (auto& agent : population) {
        if (agent->canReproduce()) {
            potentialParents.push_back(agent.get());
        }
    }
    
    if (potentialParents.size() < 2) return; // Недостаточно агентов
    
    // Перемешиваем поколение
    shuffle(potentialParents.begin(), potentialParents.end(), rng);
    
    // to do                                                          ................................        
}

void EvolutionSimulation::spawnNewFood()
{
    // С вероятностью 20% добавляем новую еду
    uniform_real_distribution<float> chance(0.0f, 1.0f);
    if (chance(rng) < 0.2f) {
        int x, y;
        if (findRandomEmptyPosition(x, y)) {
            addFood(x, y);
        }
    }
}

void EvolutionSimulation::updateState()
{
    // Обновляем тип клеток
    for (auto& row : grid) {
        for (auto& cell : row) {
            if (cell.type != WALL && cell.type != FOOD) {
                cell.type = EMPTY;
            }
        }
    }
    
    // Размещаем агентов на поле
    for (auto& agent : population) {
        if (agent->getIsAlive()) {
            grid[agent->getX()][agent->getY()].type = AGENT;
        }
    }
}

Agent* EvolutionSimulation::addAgent(int x, int y, int energy, unique_ptr<Gene> genome)
{
    if ((x < 0 || x >= grid.size()) || (y < 0 || y >= grid[0].size())) {
        return nullptr;
    }
    
    if (grid[x][y].type != EMPTY) {
        return nullptr;
    }
    
    // Создаем агента
    auto agent = make_unique<Agent>(x, y, energy, move(genome));
    Agent* agent_ptr = agent.get();
    population.push_back(move(agent));
    
    // Обновляем клетку
    grid[x][y].type = AGENT;
    
    return agent_ptr;
}

bool EvolutionSimulation::addFood(int x, int y, int energyValue)
{
    if ((x < 0 || x >= grid.size()) || (y < 0 || y >= grid[0].size())) {
        return false;
    }
    
    if (grid[x][y].type != EMPTY) {
        return false;
    }
    
    grid[x][y].type = FOOD;
    grid[x][y].foodValue = energyValue;

    return true;
}

const Cell& EvolutionSimulation::getCell(int x, int y) const
{
    static Cell invalidCell{WALL}; // Возвращаем стену для невалидных координат
    
    if ((x >= 0 && x < grid.size()) && (y >= 0 && y < grid[0].size())) {
        return grid[x][y];
    }
    
    return invalidCell;
}

EvolutionSimulation::SimulationData EvolutionSimulation::getSimulationData() const
{
    SimulationData data{};
    data.populationSize = population.size();
    data.simulationIteration = simulationIteration;
    data.mutationRate = mutationRate;
    data.totalAlives = totalAlives;
    data.totalDeaths = totalDeaths;
    
    // Считаем статистику по еде
    data.totalFood = 0;
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            if (cell.type == FOOD) {
                data.totalFood++; // Количество именно клеток еды (пока-что энергетическая ценность для всех одинакова)
            }
        }
    }
    
    // Считаем статистику по энергии агентов
    if (!population.empty()) {
        int totalEnergy = 0;
        data.minEnergyLevel = numeric_limits<int>::max();
        data.maxEnergyLevel = numeric_limits<int>::min();
        
        for (const auto& agent : population) {
            int energy = agent->getEnergy();
            totalEnergy += energy;
            data.minEnergyLevel = min(data.minEnergyLevel, energy);
            data.maxEnergyLevel = max(data.maxEnergyLevel, energy);
        }
        
        data.averageEnergyLevel = totalEnergy / population.size();
    } else {
        data.averageEnergyLevel = 0;
        data.minEnergyLevel = 0;
        data.maxEnergyLevel = 0;
    }
    
    return data;
}

// bool EvolutionSimulation::saveSimulationState(const string& filename) const
// {
//     ofstream file(filename, ios::binary);
//     if (!file.is_open()) {
//         return false;
//     }
    
//     // TODO: Реализовать сериализацию состояния
//     // Это сложная задача, требующая сериализации всех агентов и их геномов
    
//     return true;
// }

// bool EvolutionSimulation::loadSimulationState(const string& filename)
// {
//     ifstream file(filename, ios::binary);
//     if (!file.is_open()) {
//         return false;
//     }
    
//     // TODO: Реализовать десериализацию состояния
//     // Это сложная задача, требующая восстановления всех агентов и их геномов
    
//     return true;
// }

void EvolutionSimulation::resetSimulation(vector<vector<Cell>> newGrid)
{
    if (!newGrid.empty()) {
        grid = move(newGrid);
    }
    
    population.clear();
    simulationIteration = 0;
    totalDeaths = 0;
    totalAlives = 0;
    
    // Переинициализируем начальное состояние
    initializePopulation(10);
    initializeFood(20);
}