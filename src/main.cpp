#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <memory>
#include "main.h"
#include "streamout.h"
#include "simulation.h"

bool UseNeuralNetwork = USE_A_NEURAL_NETWORK;
int InputValues = INPUT_VALUES;
int NeuronsInHiddenLayer = NEURONS_IN_HIDDEN_LAYER;
int OutputValues = OUTPUT_VALUES;

ProgramParameters parseFile(ProgramParameters param) {
    std::ifstream file("simulation_data.csv");
    
    if (!file.is_open()) {
        return param;
    }

    std::string line;
    
    // Пропускаем первую строку с заголовком
    if (!std::getline(file, line)) {
        return param;
    }

    // Читаем вторую строку с параметрами сети
    if (!std::getline(file, line)) {
        return param;
    }

    std::istringstream ss(line);
    std::string value;
    std::vector<std::string> values;
    
    while (std::getline(ss, value, ';')) {
        values.push_back(value);
    }
    
    if (values.size() >= 5) {
        param.useNeuralNetwork = true;
        param.InputValues = std::stoi(values[0]);
        param.NeuronsInHiddenLayer = std::stoi(values[1]);
        param.OutputValues = std::stoi(values[2]);
        param.activationMid = values[3];
        param.activationLast = values[4];
    }

    // Читаем все веса и смещения
    std::vector<float> weightsBiases;
    while (std::getline(file, line)) {
        weightsBiases.push_back((float)std::stod(line));
    }

    // Рассчитываем ожидаемое количество параметров
    const int expectedC = param.InputValues * param.NeuronsInHiddenLayer +     // веса 1 слоя
                             param.NeuronsInHiddenLayer +                      // смещения 1 слоя
                             param.NeuronsInHiddenLayer * param.OutputValues + // веса 2 слоя
                             param.OutputValues;                               // смещения 2 слоя
    
    if (weightsBiases.size() != expectedC) {
        // Если кол-во параметров не совпадает
        return param;
    }
    
    // Веса первого слоя
    std::vector<float> layer1_w;
    layer1_w.insert(layer1_w.end(), weightsBiases.begin(), weightsBiases.begin() + param.InputValues * param.NeuronsInHiddenLayer);
    weightsBiases.begin() += param.InputValues * param.NeuronsInHiddenLayer;
    
    // Смещения первого слоя
    std::vector<float> layer1_b;
    layer1_b.insert(layer1_b.end(), weightsBiases.begin(), weightsBiases.begin() + param.NeuronsInHiddenLayer);
    weightsBiases.begin() += param.NeuronsInHiddenLayer;
    
    // Веса второго слоя
    std::vector<float> layer2_w;
    layer2_w.insert(layer2_w.end(), weightsBiases.begin(), weightsBiases.begin() + param.NeuronsInHiddenLayer * param.OutputValues);
    weightsBiases.begin() += param.NeuronsInHiddenLayer * param.OutputValues;
    
    // Смещения второго слоя
    std::vector<float> layer2_b;
    layer2_b.insert(layer2_b.end(), weightsBiases.begin(), weightsBiases.begin() + param.OutputValues);
    
    // Сохраняем в структуру
    param.weights = {layer1_w, layer2_w};
    param.biases = {layer1_b, layer2_b};
    
    return param;
}

void settingConstants(ProgramParameters param) {
    UseNeuralNetwork = param.useNeuralNetwork;
    InputValues = param.InputValues;
    NeuronsInHiddenLayer = param.NeuronsInHiddenLayer;
    OutputValues = param.OutputValues;
}

void saveStatistic(std::ofstream& file, EvolutionSimulation& sim, char typeSave) {
    // Краткая информация
    if (typeSave == 's') {
        file << sim.getGeneration() << ";" << sim.getSimulationData().maxEnergyLevel << ";" << sim.getPopulation()[0]->getSteps() << ";" << sim.getSimulationData().totalAlives << std::endl;
        file.flush();
    }
    // Сохранение конфигурации и весов лучшей нейросети
    else if (typeSave == 'd') {
        string data = sim.getPopulation()[0]->getGene().saveDataCSV();
        file << data << "               " << sim.getGeneration() << "               " << sim.getPopulation()[0]->getEnergy() << "\n";
        file.flush();
    }
}

void runARound(EvolutionSimulation& sim, bool visualize) {
    if (visualize) {
        // Визуализация раунда/поколения
        for (int step = 1; step <= NUMBER_OF_STEPS; step++) {
            updateField(sim.getGrid(), sim, GENERATIONS, SKIP_GENERATIONS, step, NUMBER_OF_STEPS);

            if (!sim.simulateStep()) { break; }

            this_thread::sleep_for(chrono::milliseconds(TICK_MS)); // FPS
        }
    } else {
        for (int step = 1; step <= NUMBER_OF_STEPS; step++) {
            if (!sim.simulateStep()) { break; }
        }
        updateField(sim.getGrid(), sim, GENERATIONS, SKIP_GENERATIONS, NUMBER_OF_STEPS, NUMBER_OF_STEPS);
    }
}

void visualBestAgent(EvolutionSimulation& sim) {
    auto bestAgent = sim.getPopulation()[0]->clone();

    auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
    EvolutionSimulation visualSim(field, 0, 0);

    // Добавляем только лучшего агента
    int x, y;
    for (int i = 0; i < 10; i++) {
        visualSim.findRandomEmptyPosition(x, y);
        visualSim.addAgent(x, y, INIT_ENERGY_AGENT, bestAgent->getGene().clone());
    }
    visualSim.reloadGrid();

    runARound(visualSim, true);
    sim.reloadGrid();
}

void _train() {
    std::ofstream statsFile("simulation_stats.csv", std::ios::app);
    std::ofstream dataFile("simulation_data.csv", std::ios::app);
    statsFile << "Generation;MaxEnergy;TopSteps;AliveAgents" << std::endl;

    auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
    EvolutionSimulation sim(field);

    while (sim.getGeneration() < GENERATIONS) {
        runARound(sim, true);
        saveStatistic(statsFile, sim, 's');
        
        // Проверяем удачные ли гены
        sim.sortPop();
        if (sim.getPopulation()[0]->getEnergy() >= 1000) {
            saveStatistic(dataFile, sim, 'd');
            visualBestAgent(sim);
        }
        
        sim.geneticAlgorithm();
        sim.reloadGrid();
        
        // Пропуск раундов без визуализации
        for (int gen_skip = 1; gen_skip <= SKIP_GENERATIONS - 1; gen_skip++) {
            runARound(sim, false);
            saveStatistic(statsFile, sim, 's');
            
            // Проверяем удачные ли гены
            sim.sortPop();
            if (sim.getPopulation()[0]->getEnergy() >= 1000) {
                saveStatistic(dataFile, sim, 'd');
                visualBestAgent(sim);
            }
            
            sim.geneticAlgorithm();
            sim.reloadGrid();
        }
    }
    updateField(sim.getGrid(), sim, GENERATIONS, SKIP_GENERATIONS, NUMBER_OF_STEPS, NUMBER_OF_STEPS);
    
    statsFile.close();
    dataFile.close();
}

void _show(ProgramParameters param) {
    param = parseFile(param);
    settingConstants(param);
    
    auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
    auto sim = make_unique<EvolutionSimulation>(move(field), 0, 0);
    sim->tuneSimWithTrainedAgents(field, param);
    
    while (true) {
        runARound(*sim, true);
        sim->reloadGrid();
    }
}

int main(int argc, char* argv[]) {
    ProgramParameters param;

    if (argc == 1) {
        param.type = 't';
        param.useNeuralNetwork = USE_A_NEURAL_NETWORK;
        param.InputValues = INPUT_VALUES;
        param.NeuronsInHiddenLayer = NEURONS_IN_HIDDEN_LAYER;
        param.OutputValues = OUTPUT_VALUES;
        param.activationMid = "relu";
        param.activationLast = "sigmoid";
        settingConstants(param);
        _train();
    } else if (argc == 2 && std::string(argv[1]) == "-v") {
        param.type = 'v';
        param.activationMid = "relu";
        param.activationLast = "sigmoid";
        _show(param);
    }
    
    return 0;
}