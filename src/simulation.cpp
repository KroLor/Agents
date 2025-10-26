#include <algorithm>
#include <random>
#include <iostream>
#include "simulation.h"
#include "main.h"

using namespace std;

// Вспомогательная функция для генерации случайных чисел
static mt19937 rng(random_device{}());

EvolutionSimulation::EvolutionSimulation(vector<vector<Cell>> grid, int initialPopulationSize, int initialFoodCount)
    : grid(move(grid)), mutationPower(AGENT_MUTATION_POWER), generation(0), totalDeaths(0), totalAlives(INIT_POP_SIZE), currentTick(0)
{
    initializePopulation(initialPopulationSize);
    initializeFood(initialFoodCount);
}

EvolutionSimulation::~EvolutionSimulation()
{
    // Умные указатели автоматически почистят память
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
    
    // Свободно ли
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

bool EvolutionSimulation::simulateStep()
{
    if (!updateAgents()) {
        return false;
    }
    
    currentTick++;
    // Добавляем новую еду FOOD_ADD_TIMES раз каждые FOOD_SPAWN_INTERVAL тиков
    if (currentTick % FOOD_SPAWN_INTERVAL == 0) {
        for (int times = 0; times < FOOD_ADD_TIMES; times++) {
            spawnNewFood();
        }
    }
    
    updateGrid();

    return true;
}

bool EvolutionSimulation::updateAgents() {
    shuffle(population.begin(), population.end(), rng); // Перемешать популяцию

    if (totalAlives == 0) { return false; }
    
    for (auto& agent : population) {
        if (agent->getIsAlive()) {
            // Проверяем смерть от голода
            if (agent->getEnergy() <= 0) {
                agent->die();
                totalDeaths++;
                totalAlives--;

                int x = agent->getX();
                int y = agent->getY();
                grid[x][y].type = EMPTY;
                continue;
            }

            // Агент осматривается
            agent->lookAround(&grid);
            agent->getDirectionToFood(&grid);
            
            // Сохраняем старую позицию
            int oldX = agent->getX();
            int oldY = agent->getY();
            
            // Агент думает и делает свой ход
            agent->decideAction(grid);
            
            // Обновляем новую позицию
            int newX = agent->getX();
            int newY = agent->getY();
            
            // Если агент съел еду, обновляем клетку
            if (grid[newX][newY].type == FOOD) {
                grid[newX][newY].type = AGENT;
                grid[newX][newY].foodValue = 0;
                agent->stepTick();
            }
            else if (grid[newX][newY].type == EMPTY) {
                grid[newX][newY].type = AGENT;
                agent->stepTick();
            } else {
                // Если клетка занята, возвращаемся на старое место
                agent->setX(oldX);
                agent->setY(oldY);
                grid[oldX][oldY].type = AGENT;
            }
        }
    }

    return true;
}

void EvolutionSimulation::geneticAlgorithm() {
    // Сортируем по количеству шагов (по убыванию)
    sort(population.begin(), population.end(), /* Лямбда функция для принцыпа сравнения */
                    [](const unique_ptr<Agent>& a, const unique_ptr<Agent>& b) { 
                        return a->getSteps() > b->getSteps();
                    });
                                            /*
                                            return (a->getSteps() + a->getEnergy()) > (b->getSteps() + b->getEnergy());

                                            int A = a->getSteps() * 0.1 + a->getEnergy() * 0.9;
                                            int B = b->getSteps() * 0.1 + b->getEnergy() * 0.9;
                                            return A > B;
                                            */
    int bestCount = population.size() / 2; // Берем первую лучшую половину
    vector<unique_ptr<Agent>> newPopulation;
    
    uniform_real_distribution<float> chance(0.0f, 1.0f);

    // Создаем новое поколение на основе старого
    for (int i = 0; i < bestCount; i++) {
        // Клонируем лучшего агента
        newPopulation.push_back(population[i]->clone()); // population[0]
    }
    // Несколько первых оставляем без изменения
    for (int i = 3; i < bestCount - 1; i++) {
        // С некоторым шансом скрещиваем первую половину
        if (chance(rng) < AGENT_CHANCE_TO_CROSS_OVER) {
            newPopulation[i]->crossing(*newPopulation[i+1]);
        }
    }
    
    for (int i = bestCount; i < population.size(); i++) {
        // Клонируем худшего агента с его текущим геном
        newPopulation.push_back(population[i]->clone());
    }
    // Скрещиваем низших
    for (int i = bestCount; i < population.size() - 1; i++) {
        // С некоторым шансом скрещиваем вторую половину
        if (chance(rng) < AGENT_CHANCE_TO_CROSS_OVER) {
            newPopulation[i]->crossing(*newPopulation[i+1]);
        }
    }
    // Мутируем вторую половину
    for (int i = bestCount; i < population.size(); i++) {
        // Случайная мутация для худших
        if (chance(rng) < AGENT_MUTATION_CHANCE) {
            newPopulation[i]->mutateGene(mutationPower);
        }
    }
    
    // Заменяем старую популяцию новой
    population = move(newPopulation);
    generation++;
}

void EvolutionSimulation::spawnNewFood() {
    // Добавляем новую еду с вероятностью CHANCE_OF_FOOD_APPEARANCE%
    uniform_real_distribution<float> chance(0.0f, 1.0f);
    if (chance(rng) < CHANCE_OF_FOOD_APPEARANCE) {
        int x, y;
        if (findRandomEmptyPosition(x, y)) {
            addFood(x, y);
        }
    }
}

void EvolutionSimulation::updateGrid() {
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

Agent* EvolutionSimulation::addAgent(int x, int y, int energy, unique_ptr<Gene> genome) {
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

bool EvolutionSimulation::addFood(int x, int y, int energyValue) {
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

EvolutionSimulation::SimulationData EvolutionSimulation::getSimulationData() const {
    SimulationData data{};
    data.populationSize = population.size();
    data.generation = generation;
    data.mutationPower = mutationPower;
    data.totalAlives = totalAlives;
    data.totalDeaths = totalDeaths;
    int alive = 0;
    
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
        data.minEnergyLevel = INT_MAX;
        data.maxEnergyLevel = INT_MIN;
        
        for (const auto& agent : population) {
            if (agent->getIsAlive()) {
                int energy = agent->getEnergy();
                alive++;

                totalEnergy += energy;
                data.minEnergyLevel = min(data.minEnergyLevel, energy);
                data.maxEnergyLevel = max(data.maxEnergyLevel, energy);
            }
        }

        if (alive != 0) {
            data.averageEnergyLevel = totalEnergy / alive;
        } else {
            data.averageEnergyLevel = 0;
            data.minEnergyLevel = 0;
            data.maxEnergyLevel = 0;
        }
    } else {
        data.averageEnergyLevel = 0;
        data.minEnergyLevel = 0;
        data.maxEnergyLevel = 0;
    }
    
    return data;
}

void EvolutionSimulation::reloadGrid() {
    for (auto& row : grid) {
        for (auto& cell : row) {
            if (cell.type != WALL) {
                cell.type = EMPTY;
                cell.foodValue = 0;
            }
        }
    }

    for (auto& agent : population) {
        int x, y;
        agent->setEnergy(INIT_ENERGY_AGENT);
        agent->setIsAlive(true);
        agent->setSteps(0);
        findRandomEmptyPosition(x, y);
        agent->setX(x);
        agent->setY(y);
    }

    totalDeaths = 0;
    totalAlives = INIT_POP_SIZE;
    currentTick = 0;

    initializeFood(INIT_FOOD_COUNT);
    updateGrid();
}

void EvolutionSimulation::resetSim() {
    for (auto& row : grid) {
        for (auto& cell : row) {
            if (cell.type != WALL) {
                cell.type = EMPTY;
                cell.foodValue = 0;
            }
        }
    }
    
    population.clear();
    totalDeaths = 0;
    totalAlives = 0;
    currentTick = 0;
    generation = 0;
    
    initializePopulation(INIT_POP_SIZE);
    initializeFood(INIT_FOOD_COUNT);
}