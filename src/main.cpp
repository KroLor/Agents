#include <thread>
#include <chrono>
#include <fstream>
#include "main.h"
#include "streamout.h"
#include "simulation.h"

// void assignConstants() {

// }

void saveStatistic(std::ofstream& statsFile, EvolutionSimulation& sim) {
    statsFile << sim.getGeneration() << ";" << sim.getSimulationData().totalAlives << std::endl;
    statsFile.flush();
}

int main() {
    // assignConstants();

    std::ofstream statsFile("simulation_stats.csv", std::ios::app);
    statsFile << "Generation;AliveAgents" << std::endl;

    auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
    EvolutionSimulation sim(field);

    while (sim.getGeneration() < GENERATIONS) {
        // Визуализация раунда/поколения
        for (int step = 1; step <= NUMBER_OF_STEPS; step++) {
            updateField(sim.getGrid(), sim, GENERATIONS, SKIP_GENERATIONS, step, NUMBER_OF_STEPS);

            if (!sim.simulateStep()) { break; }

            this_thread::sleep_for(chrono::milliseconds(TICK_MS)); // FPS
        }

        saveStatistic(statsFile, sim);
        sim.geneticAlgorithm();
        sim.reloadGrid();
        
        // Пропуск раундов/поколений без визуализации
        for (int gen_skip = 1; gen_skip <= SKIP_GENERATIONS - 1; gen_skip++) {
            for (int step = 0; step < NUMBER_OF_STEPS; step++) {
                if (!sim.simulateStep()) { break; }
            }
            
            saveStatistic(statsFile, sim);
            sim.geneticAlgorithm();
            sim.reloadGrid();
        }
    }
    updateField(sim.getGrid(), sim, GENERATIONS, SKIP_GENERATIONS, NUMBER_OF_STEPS, NUMBER_OF_STEPS);
    
    statsFile.close();
    return 0;
}