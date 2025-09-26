#include "main.h"
#include "streamout.h"
#include "simulation.h"
#include <iostream>
#include <iomanip>

#include <thread>
#include <chrono>

using namespace std;

// Функция для вывода статистики под полем
void printStatistics(const EvolutionSimulation& sim, int currentStep, int totalSteps) {
    auto data = sim.getSimulationData();
    
    cout << "|--------------------------------------------------|" << endl;
    cout << "| Generation: " << setw(3) << data.generation 
         << "| Step: " << setw(3) << currentStep << "/" << setw(3) << totalSteps 
         << "| Agents: " << setw(3) << data.populationSize << "|" << endl;
    cout << "| Food: " << setw(3) << data.totalFood 
         << "| Avg Energy: " << setw(3) << data.averageEnergyLevel 
         << "| Min: " << setw(3) << data.minEnergyLevel 
         << "| Max: " << setw(3) << data.maxEnergyLevel << "|" << endl;
    cout << "| Deaths: " << setw(3) << data.totalDeaths 
         << "| Mutation Rate: " << fixed << setprecision(2) << data.mutationRate << "|" << endl;
    cout << "|--------------------------------------------------|" << endl;
}

int main() {
    auto field = createField(FIELD_WIDTH, FIELD_HEIGHT);
    EvolutionSimulation sim(field);
    
    cout << "Evolution Simulation MVP" << endl;
    cout << "Field: " << FIELD_WIDTH << "x" << FIELD_HEIGHT 
         << "| Steps: " << NUMBER_OF_STEPS 
         << "| Skip: " << SKIP_GENERATIONS << endl;
    cout << "--------------------------------------------------" << endl;

    for (int gen = 0; gen < GENERATIONS; gen++) {
        // Показ текущего поколения
        cout << "\n=== Generation " << sim.getGeneration() << " ===" << endl;
        
        for (int step = 0; step < NUMBER_OF_STEPS; step++) {
            // Очищаем только область поля, оставляя статистику
            cout << "\033[2J\033[H"; // Очищаем консоль и перемещаем курсор в начало
            
            // Выводим поле
            printField(sim.getGrid());
            
            // Выводим статистику ПОД полем
            printStatistics(sim, step + 1, NUMBER_OF_STEPS);
            
            // Выполняем шаг симуляции
            sim.simulateStep(1.0f);
            
            // Задержка для визуализации
            std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
        }
        
        // Пропуск поколений без визуализации
        for (int skip = 0; skip < SKIP_GENERATIONS; skip++) {
            for (int step = 0; step < NUMBER_OF_STEPS; step++) {
                sim.simulateStep(1.0f);
            }
            sim.geneticAlgorithm();
        }
        
        sim.resetSimulation(field);
        sim.geneticAlgorithm();
    }
    
    cout << "\nSimulation completed after " << GENERATIONS << " generations!" << endl;
    return 0;
}