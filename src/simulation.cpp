#include <algorithm>
#include <random>
#include <iostream>
#include "simulation.h"
#include "neural_network.h"
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


void EvolutionSimulation::initializeFood(int initialFoodCount) {
    uniform_int_distribution<int> random((int)ENERGY_FOOD_VALUE / 2, ENERGY_FOOD_VALUE);
    for (int i = 0; i < initialFoodCount; i++) {
        FoodValue.push_back(random(rng));
    }

    for (int i = 0; i < initialFoodCount; i++) {
        int x, y;
        if (findRandomEmptyPosition(x, y)) {
            addFood(x, y, FoodValue[i]);
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
        uniform_int_distribution<int> random((int)ENERGY_FOOD_VALUE / 2, ENERGY_FOOD_VALUE);
        for (int times = 0; times < FOOD_ADD_TIMES; times++) {
            spawnNewFood(random);
        }
    }
    
    updateGrid();

    return true;
}

bool EvolutionSimulation::updateAgents() {
    if (totalAlives == 0) { return false; }

    // Перемешаем популяцию
    shuffle(population.begin(), population.end(), rng);

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
            // for (int i = 0; i < 16; i++) {
            //     if (agent->decideAction(grid)) {
            //         break;
            //     }
            // }
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

void EvolutionSimulation::sortPop() {
    sort(population.begin(), population.end(),
    [](const unique_ptr<Agent>& a, const unique_ptr<Agent>& b) { 
        int A = (float)a->getSteps() * 0.3f + (float)a->getEnergy() * 0.7f;
        int B = (float)b->getSteps() * 0.3f + (float)b->getEnergy() * 0.7f;
        return A > B;
    });
    /*
        return a->getSteps() > b->getSteps();

        return a->getEnergy() > b->getEnergy();
    */
}

void EvolutionSimulation::geneticAlgorithm() {
    // Сортируем агентов по эффективности (лучшие - первые)
    sortPop();
    
    vector<unique_ptr<Agent>> newPop;
    uniform_real_distribution<float> random(0.0f, 1.0f);
    uniform_int_distribution<int> randomAg(2, population.size() - 1);

    // 1. СОХРАНЯЕМ ЛУЧШИХ АГЕНТОВ
    newPop.push_back(population[0]->clone()); // 1
    newPop.push_back(population[1]->clone()); // 2

    // // 2. ФОРМИРУЕМ ТОП ЛУЧШИХ И ХУДШИХ
    // vector<unique_ptr<Agent>> goodPop;
    // vector<unique_ptr<Agent>> badPop;

    // 3. СКРЕЩИВАЕМ ПЕРВУЮ ПОЛОВИНУ
    for (int i = 2; i < population.size() / 2; i++) {
        auto newAgent = population[i]->clone();
        
        if (random(rng) < AGENT_CHANCE_TO_CROSS_OVER) {
            newAgent->crossing(*population[i+1]);
        }

        newPop.push_back(move(newAgent));
        // newPop.push_back(population[i]->clone());
    }

    // 4. ПРИМЕНЯЕМ МУТАЦИИ КО ВТОРОЙ ПОЛОВИНЕ
    for (int i = population.size() / 2; i < population.size(); i++) {
        newPop.push_back(population[i]->clone());

        if (random(rng) < AGENT_MUTATION_CHANCE) {
            newPop[i]->mutateGene(mutationPower);
        }
    }

    // 3. ПОДГОНКА СИЛЫ МУТАЦИИ
    if (newPop.size() >= 1) {
        int bestEnergy = newPop[0]->getEnergy();
        
        if (bestEnergy >= INIT_ENERGY_AGENT * 1.2f) {
            // Успешные агенты - уменьшаем мутацию
            mutationPower = max(AGENT_MUTATION_POWER * 0.25f, mutationPower * 0.95f);
        } else if (bestEnergy < INIT_ENERGY_AGENT * 0.75f) {
            // Неуспешные агенты - увеличиваем мутацию
            mutationPower = min(AGENT_MUTATION_POWER * 2.0f, mutationPower * 1.1f);
        }
        
        // Периодическое увеличение мутации
        if (generation % 50 == 0 && bestEnergy < INIT_ENERGY_AGENT * 0.5f) {
            mutationPower = min(AGENT_MUTATION_POWER, mutationPower * 1.5f);
        }
    }

    population = move(newPop);
    generation++;
}

void EvolutionSimulation::spawnNewFood(uniform_int_distribution<int> foodV) {
    // Добавляем новую еду с вероятностью CHANCE_OF_FOOD_APPEARANCE%
    uniform_real_distribution<float> chance(0.0f, 1.0f);

    if (chance(rng) < CHANCE_OF_FOOD_APPEARANCE) {
        int x, y;
        if (findRandomEmptyPosition(x, y)) {
            addFood(x, y, FoodValue[foodV(rng)]);
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

unique_ptr<NeuralNetwork> EvolutionSimulation::createNetw(const ProgramParameters& param) {
    auto neuralNet = make_unique<NeuralNetwork>();
    
    auto layer1 = make_unique<GeneLayer>(param.InputValues, param.NeuronsInHiddenLayer, param.activationMid);
    
    const auto& weights1 = param.weights[0];
    vector<vector<float>> layer1_w(param.InputValues, vector<float>(param.NeuronsInHiddenLayer));
    
    for (int i = 0; i < param.InputValues; i++) {
        for (int o = 0; o < param.NeuronsInHiddenLayer; o++) {
            int indx = i * param.NeuronsInHiddenLayer + o;
            layer1_w[i][o] = weights1[indx];
        }
    }
    layer1->setWeights(layer1_w);
    
    // Устанавливаем смещения для среднего слоя
    layer1->setBiases(param.biases[0]);
    
    neuralNet->addLayer(move(layer1));
    
    auto layer2 = make_unique<GeneLayer>(param.NeuronsInHiddenLayer, param.OutputValues, param.activationLast);
    
    // Устанавливаем веса для второго слоя
    const auto& weights2 = param.weights[1];
    vector<vector<float>> layer2_w(param.NeuronsInHiddenLayer, vector<float>(param.OutputValues));
    
    // Преобразуем плоский вектор в матрицу [hidden_size][output_size]
    for (int i = 0; i < param.NeuronsInHiddenLayer; i++) {
        for (int o = 0; o < param.OutputValues; o++) {
            int indx = i * param.OutputValues + o;
            layer2_w[i][o] = weights2[indx];
        }
    }
    layer2->setWeights(layer2_w);
    
    // Устанавливаем смещения для последнего слоя
    layer2->setBiases(param.biases[1]);
    
    neuralNet->addLayer(move(layer2));
    
    return neuralNet;
}

void EvolutionSimulation::tuneSimWithTrainedAgents(vector<vector<Cell>> field, const ProgramParameters& param) {
    // Создаем одну нейросеть
    auto neuralNet = createNetw(param);
    
    // Создаем NeuralGene с этой нейросетью
    auto newNeuralGene = make_unique<NeuralGene>(move(neuralNet));
    
    // Создаем агентов с одной нейросетью
    for (int i = 0; i < INIT_POP_SIZE; i++) {
        int x, y;
        if (findRandomEmptyPosition(x, y)) {
            addAgent(x, y, INIT_ENERGY_AGENT, newNeuralGene->clone());
        }
    }
    
    updateGrid();
}

Agent* EvolutionSimulation::addAgent(int x, int y, int energy, unique_ptr<Gene> genome) {
    if ((x < 0 || x >= grid.size()) || (y < 0 || y >= grid[0].size())) {
        return nullptr;
    }
    
    if (grid[x][y].type != EMPTY) {
        return nullptr;
    }
    
    // Создаем агента
    auto agent = make_unique<Agent>(x, y, energy, std::move(genome));
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
    
    // generateFixedFood(INIT_FOOD_COUNT);
    initializePopulation(INIT_POP_SIZE);
    initializeFood(INIT_FOOD_COUNT);
}