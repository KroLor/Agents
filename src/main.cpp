#include <thread>
#include <chrono>
#include "main.h"
#include "streamout.h"
#include "simulation.h"

int main() {
    auto field = createField(FIELD_HEIGHT, FIELD_WIDTH);
    
    EvolutionSimulation sim(field);
    
    printField(sim.getGrid());

    int i = 0;
    while (i < ROUNDS) {
        if (!sim.isSimulationPaused()) {
            sim.simulateStep(1.0f / sim.getSimulationSpeed());
        }
        
        updateField(sim.getGrid());
        
        i++;
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
    }

    // 

    return 0;
}