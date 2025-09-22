#include "main.h"
#include "streamout.h"
#include "simulation.h"

int main() {
    auto field = createField(FIELD_HEIGHT, FIELD_WIDTH);
    
    EvolutionSimulation sim(field);
    
    printField(sim.getGrid());

    // Основной цикл
    while (sim.getGeneration() < GENERATIONS) {
        if (sim.isSimulationPaused()) {
            
        }
        
        // Избавится от двойного вызова
        
        // Прогоняем шаги симуляции и выводим N-ный раунд
        for (int step = 0; step < NUMBER_OF_STEPS; step++) {
            sim.simulateStep(1.0f / sim.getSimulationSpeed());

            // Выводим состояние поля
            updateField(sim.getGrid());
        }

        sim.geneticAlgorithm(); // Естественный отбор

        // Пропуск поколений
        for (int gen = 0; gen < SKIP_GENERATIONS; gen++) {
            // Прогоняем шаги симуляции
            for (int step = 0; step < NUMBER_OF_STEPS; step++) {
                sim.simulateStep(1.0f / sim.getSimulationSpeed());
            }

            sim.geneticAlgorithm(); // Естественный отбор
        }
    }

    // 

    return 0;
}