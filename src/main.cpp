#include <thread>
#include <chrono>
#include <fstream>
#include "main.h"
#include "streamout.h"
#include "simulation.h"

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

int main() {
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
            if (sim.getSimulationData().averageEnergyLevel >= 1000) {
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
    return 0;
}