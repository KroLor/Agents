#include <thread>
#include <chrono>
#include "main.h"
#include "streamout.h"
#include "simulation.h"

int main() {
    auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
    EvolutionSimulation sim(field);

    while (sim.getGeneration() <= GENERATIONS) {
        // Визуализация раунда/поколения
        for (int step = 1; step <= NUMBER_OF_STEPS; step++) {
            updateField(sim.getGrid());
            printStatistics(sim, step, NUMBER_OF_STEPS);

            sim.simulateStep();

            this_thread::sleep_for(chrono::milliseconds(TICK_MS)); // FPS
        }

        sim.reloadGrid();
        sim.geneticAlgorithm();
        
        // Пропуск раундов/поколений без визуализации
        for (int gen_skip = 1; gen_skip <= SKIP_GENERATIONS - 1; gen_skip++) {
            for (int step = 0; step < NUMBER_OF_STEPS; step++) {
                sim.simulateStep();
            }
            
            sim.reloadGrid();
            sim.geneticAlgorithm();
        }
    }
    // printStatistics(sim, 1, NUMBER_OF_STEPS);
    
    return 0;
}