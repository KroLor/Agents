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
            printStatistics(sim.getSimulationData(), step, NUMBER_OF_STEPS);

            sim.simulateStep();
        }

        sim.geneticAlgorithm();
        
        // Пропуск раундов/поколений без визуализации
        for (int gen_skip = 1; gen_skip <= SKIP_GENERATIONS - 1; gen_skip++) {
            for (int step = 0; step < NUMBER_OF_STEPS; step++) {
                sim.simulateStep();
            }
            
            sim.geneticAlgorithm();
        }

        sim.resetGrid(field);
    }
    // printStatistics(sim.getSimulationData(), 1, NUMBER_OF_STEPS);
    
    return 0;
}