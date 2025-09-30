#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <iomanip>
#include "streamout.h"

using namespace std;

static vector<vector<Cell>> previousField; // Буфер поля

vector<vector<Cell>> createField(int height, int width) {
    previousField = vector<vector<Cell>>(width + 2, vector<Cell>(height + 2, {EMPTY}));
    auto field = vector<vector<Cell>>(width + 2, vector<Cell>(height + 2, {EMPTY}));

    // Задаем стены на границах
    for (int i = 0; i < height + 2; i++) {
        field[0][i].type = WALL;         // Левая граница
        field[width + 1][i].type = WALL; // Правая граница
    }

    for (int j = 0; j < width + 2; j++) {
        field[j][0].type = WALL;          // Верхняя граница
        field[j][height + 1].type = WALL; // Нижняя граница
    }

    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    return field;
}

void updateField(const vector<vector<Cell>>& field) {
    for (int y = 0; y < field.size(); y++) {
        for (int x = 0; x < field[y].size(); x++) {
            if (field[y][x].type != previousField[y][x].type) {
                cout << "\033[" << y + 1 << ";" << x + 1 << "H"; // Перенос каретки и дальнейшая замена

                switch (field[y][x].type) {
                    case EMPTY: cout << SYMBOL_EMPTY; break;
                    case WALL: cout << SYMBOL_WALL; break;
                    case AGENT: cout << SYMBOL_AGENT; break;
                    case FOOD: cout << SYMBOL_FOOD; break;
                }

                cout.flush();
            }
        }
    }
    previousField = field;

    this_thread::sleep_for(chrono::milliseconds(TICK_MS)); // FPS
}

void printStatistics(const EvolutionSimulation& sim, int currentStep, int totalSteps) {
    auto data = sim.getSimulationData();
    
    cout << "|---------------------------------------------------|" << endl;
    cout << "| Generation: " << setw(4) << data.generation
         << "| Food: " << setw(9) << data.totalFood
         << "| Avg Energy: " << setw(3) << data.averageEnergyLevel << "|" << endl;
    cout << "| Step: " << setw(7) << currentStep << "/" << totalSteps
         << "| Deaths: " << setw(7) << data.totalDeaths
         << "| Max Energy: " << setw(3) << data.maxEnergyLevel << "|" << endl;
    cout << "| Agents: " << setw(8) << data.populationSize
         << "| Mut. Rate: " << fixed << setprecision(2) << data.mutationRate
         << "| Min Energy: " << setw(3) << data.minEnergyLevel << "|" << endl;
    cout << "|---------------------------------------------------|" << endl;
}