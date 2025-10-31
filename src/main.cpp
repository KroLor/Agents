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
    try {
        std::ifstream file("simulation_data.csv");
        
        if (!file.is_open()) {
            return param;
        }

        std::string line;
        std::vector<double> weights;
        
        // Читаем заголовок
        if (!std::getline(file, line)) {
            return param;
        }

        // Читаем вторую строку с параметрами
        if (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            
            // Парсим вторую строку с параметрами
            while (std::getline(ss, token, ';')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 7) {
                // Устанавливаем параметры из файла
                param.useNeuralNetwork = true;
                param.InputValues = std::stoi(tokens[2]);      // 7
                param.NeuronsInHiddenLayer = std::stoi(tokens[3]); // 6
                param.OutputValues = std::stoi(tokens[4]);     // 4
            }
        }

        // Читаем веса нейросети
        while (std::getline(file, line)) {
            double weight = std::stod(line);
            weights.push_back(weight);
        }

        param.weights = std::vector<std::vector<double>>();
        if (!weights.empty()) {
            // Преобразуем одномерный вектор весов в двумерную структуру
            // Предполагаем структуру: [InputValues x NeuronsInHiddenLayer] + [NeuronsInHiddenLayer x OutputValues]
            int totalWeightsExpected = param.InputValues * param.NeuronsInHiddenLayer + 
                                     param.NeuronsInHiddenLayer * param.OutputValues;
            
            if (weights.size() == totalWeightsExpected) {
                // Первый слой: InputValues x NeuronsInHiddenLayer
                std::vector<double> layer1Weights;
                layer1Weights.insert(layer1Weights.end(), 
                    weights.begin(), 
                    weights.begin() + param.InputValues * param.NeuronsInHiddenLayer);
                
                // Второй слой: NeuronsInHiddenLayer x OutputValues  
                std::vector<double> layer2Weights;
                layer2Weights.insert(layer2Weights.end(),
                    weights.begin() + param.InputValues * param.NeuronsInHiddenLayer,
                    weights.end());
                
                param.weights.push_back(layer1Weights);
                param.weights.push_back(layer2Weights);
            }
        }
        
        file.close();
        
    } catch (const std::exception& err) {
        std::cerr << "Ошибка при чтении файла: " << err.what() << '\n';
    }
    
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
        file << sim.getGeneration() << ";" << sim.getSimulationData().averageEnergyLevel << ";" << sim.getSimulationData().totalAlives << std::endl;
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
    }
}

void _train() {
    std::ofstream statsFile("simulation_stats.csv", std::ios::app);
    std::ofstream dataFile("simulation_data.csv", std::ios::app);
    statsFile << "Generation;AliveAgents" << std::endl;

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
                sim.reloadGrid();
                runARound(sim, true);

                sim.geneticAlgorithm();
                saveStatistic(dataFile, sim, 'd');
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

void _show() {
    ProgramParameters param;
    param.type = 1;
    param = parseFile(param);
    
    if (param.weights.empty()) {
        auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
        EvolutionSimulation sim(field);
        
        while (true) {
            runARound(sim, true);
            sim.reloadGrid();
        }
    } else {
        // Используем ТОЛЬКО статический метод из EvolutionSimulation
        settingConstants(param);
        auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
        auto sim = EvolutionSimulation::createWithTrainedAgents(move(field), param);
        
        if (sim->getPopulation().empty()) {
            cerr << "Ошибка: не удалось создать агентов с обученной моделью" << endl;
            return;
        }
        
        while (true) {
            runARound(*sim, true);
            sim->reloadGrid();
        }
    }
}

int main(int argc, char* argv[]) {
    ProgramParameters param;

    if (argc == 1) {
        param.type = 0;
        param.useNeuralNetwork = USE_A_NEURAL_NETWORK;
        param.InputValues = INPUT_VALUES;
        param.NeuronsInHiddenLayer = NEURONS_IN_HIDDEN_LAYER;
        param.OutputValues = OUTPUT_VALUES;
        settingConstants(param);
        _train();
    } else if (argc == 2 && std::string(argv[1]) == "-v") {
        param.type = 1;
        _show();
    }
    
    return 0;
}