#include <thread>
#include <chrono>
#include "main.h"
#include "streamout.h"
#include "simulation.h"

int main() {
    auto field = createField(FIELD_HEIGHT, FIELD_WIDTH);
    
    EvolutionSimulation sim(field);
    
    printField(sim.getGrid());

    while (true) {
        if (!sim.isSimulationPaused()) {
            sim.simulateStep(1.0f / sim.getSimulationSpeed());
        }
        
        updateField(sim.getGrid());
        
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
    }

    return 0;
}