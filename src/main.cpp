#include <thread>
#include <chrono>
#include "main.h"
#include "streamout.h"
#include "simulation.h"

// void assignConstants() {

// }

int main() {
    // assignConstants();

    auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
    EvolutionSimulation sim(field);

    while (sim.getGeneration() <= GENERATIONS) {
        // Визуализация раунда/поколения
        for (int step = 1; step <= NUMBER_OF_STEPS; step++) {
            updateField(sim.getGrid(), sim, GENERATIONS, SKIP_GENERATIONS, step, NUMBER_OF_STEPS);

            if (!sim.simulateStep()) { break; }

            this_thread::sleep_for(chrono::milliseconds(TICK_MS)); // FPS
        }

        sim.geneticAlgorithm();
        sim.reloadGrid();
        
        // Пропуск раундов/поколений без визуализации
        for (int gen_skip = 1; gen_skip <= SKIP_GENERATIONS - 1; gen_skip++) {
            for (int step = 0; step < NUMBER_OF_STEPS; step++) {
                if (!sim.simulateStep()) { break; }
            }
            
            sim.geneticAlgorithm();
            sim.reloadGrid();
        }
    }
    
    return 0;
}