#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
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
    std::vector<float> weightsBiases;
    
    if (!std::getline(file, line)) {
        return param;
    }

    // Парсим вторую строку с параметрами
    if (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string value;
        std::vector<std::string> values;
        
        while (std::getline(ss, value, ';')) {
            values.push_back(value);
        }
        
        if (values.size() >= 7) {
            // Устанавливаем параметры из файла
            param.useNeuralNetwork = true;
            param.InputValues = std::stoi(values[2]);
            param.NeuronsInHiddenLayer = std::stoi(values[3]);
            param.OutputValues = std::stoi(values[4]);
            param.activationMid = values[5];
            param.activationLast = values[6];
        }
    }

    // Парсим веса и смещения
    while (std::getline(file, line)) {
        float weight = (float)std::stod(line);
        weightsBiases.push_back(weight);
    }

    // Общее количество параметров
    int totalWeightsBiases = param.InputValues * param.NeuronsInHiddenLayer + // веса1
                            param.NeuronsInHiddenLayer +                      // смещения1
                            param.NeuronsInHiddenLayer * param.OutputValues + // веса2
                            param.OutputValues;                               // смещения2
    
    // Проверяем целостность весов
    if (weightsBiases.size() == totalWeightsBiases) {
        int num = 0;
        
        // Веса первого слоя
        std::vector<float> layer1_w;
        int weights1_size = param.InputValues * param.NeuronsInHiddenLayer;
        layer1_w.insert(layer1_w.end(), weightsBiases.begin() + num, weightsBiases.begin() + num + weights1_size);
        num += weights1_size;
        
        // Смещения первого слоя
        std::vector<float> layer1_b;
        layer1_b.insert(layer1_b.end(), weightsBiases.begin() + num, weightsBiases.begin() + num + param.NeuronsInHiddenLayer);
        num += param.NeuronsInHiddenLayer;
        
        // Веса второго слоя
        std::vector<float> layer2_w;
        int weights2_size = param.NeuronsInHiddenLayer * param.OutputValues;
        layer2_w.insert(layer2_w.end(), weightsBiases.begin() + num, weightsBiases.begin() + num + weights2_size);
        num += weights2_size;
        
        // Смещения второго слоя
        std::vector<float> layer2_b;
        layer2_b.insert(layer2_b.end(), weightsBiases.begin() + num, weightsBiases.begin() + num + param.OutputValues);
        
        param.weights.clear();
        param.weights.push_back(layer1_w);
        param.weights.push_back(layer2_w);
        
        param.biases.clear();
        param.biases.push_back(layer1_b);
        param.biases.push_back(layer2_b);
    }
    
    file.close();
    
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
        file << sim.getGeneration() << ";" << sim.getSimulationData().averageEnergyLevel << ";" << sim.getPopulation()[0]->getSteps() << ";" << sim.getSimulationData().totalAlives << std::endl;
        file.flush();
    }
    // Сохранение конфигурации и весов лучшей нейросети
    else if (typeSave == 'd') {
        // Сохраняем лучшую нейросеть (отсортировано после ген. алгоритма)
        string data = sim.getPopulation()[0]->getGene().saveDataCSV();
        file << data;
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

void _train() {
    std::ofstream statsFile("simulation_stats.csv", std::ios::app);
    std::ofstream dataFile("simulation_data.csv", std::ios::app);
    statsFile << "Generation;AvgEnergy;TopSteps;AliveAgents" << std::endl;

    auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
    EvolutionSimulation sim(field);

    while (sim.getGeneration() < GENERATIONS) {
        runARound(sim, true);

        saveStatistic(statsFile, sim, 's');
        sim.geneticAlgorithm();
        sim.reloadGrid();
        
        // Пропуск раундов/поколений без визуализации
        for (int gen_skip = 1; gen_skip <= SKIP_GENERATIONS - 1; gen_skip++) {
            runARound(sim, false);

            saveStatistic(statsFile, sim, 's');

            // Проверяем удачные ли гены
            if (sim.getSimulationData().averageEnergyLevel >= 2000) {
                sim.geneticAlgorithm();
                saveStatistic(dataFile, sim, 'd');

                sim.reloadGrid();
                runARound(sim, true);
                sim.reloadGrid();
            } else {
                sim.geneticAlgorithm();
                sim.reloadGrid();
            }
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