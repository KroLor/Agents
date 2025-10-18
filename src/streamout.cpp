#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include "streamout.h"

using namespace std;

static vector<vector<Cell>> previousField; // Буфер состояния поля
static vector<vector<char>> previousTable; // Буфер таблицы
vector<vector<char>> table;                // Таблица статистики

vector<vector<Cell>> createField(int width, int height) {
    // Инициализируем буферы
    previousField = vector<vector<Cell>>(height + 2, vector<Cell>(width + 2, {EMPTY}));
    previousTable = vector<vector<char>>(5, vector<char>(width + 2, ' '));
    table = vector<vector<char>>(5, vector<char>(width + 2, ' '));
    
    auto field = vector<vector<Cell>>(height + 2, vector<Cell>(width + 2, {EMPTY}));

    // Задаем стены на границах
    for (int i = 0; i < width + 2; i++) {
        field[0][i].type = WALL;          // Верхняя граница
        field[height + 1][i].type = WALL; // Нижняя граница
    }

    for (int j = 0; j < height + 2; j++) {
        field[j][0].type = WALL;          // Левая граница
        field[j][width + 1].type = WALL;  // Правая граница
    }

    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    cout << "\033[1;1H";

    return field;
}

void updateTable(const EvolutionSimulation& sim, int generation, int skipGen, int currentStep, int totalSteps) {
    auto data = sim.getSimulationData();
    
    // Очищаем таблицу
    for (auto& row : table) {
        fill(row.begin(), row.end(), ' ');
    }
    
    // Создаем строки статистики
    stringstream line1, line2, line3, line4, line5;
    
    line1 << "|--------------------------------------------------|";
    line2 << "| Gen: " << setw(4) << data.generation << "/" << generation
          << "| Food: " << setw(9) << data.totalFood
          << "| Avg Energy: " << setw(3) << data.averageEnergyLevel << "|";
    line3 << "| Step: " << setw(6) << currentStep << "/" << totalSteps
          << "| Skip Gen: " << setw(5) << skipGen
          << "| Max Energy: " << setw(3) << data.maxEnergyLevel << "|";
    line4 << "| Agents: " << setw(4) << (data.populationSize - data.totalDeaths) << "/" << data.populationSize
          << "| Mut Power: " << fixed << setprecision(2) << data.mutationPower
          << "| Min Energy: " << setw(3) << data.minEnergyLevel << "|";
    line5 << "|--------------------------------------------------|";
    
    string lines[] = {line1.str(), line2.str(), line3.str(), line4.str(), line5.str()};
    
    for (int i = 0; i < table.size(); i++) {
        const string& line = lines[i];
        for (int j = 0; j < line.length() && j < table[i].size(); j++) {
            table[i][j] = line[j];
        }
    }
}

void updateField(const vector<vector<Cell>>& field, const EvolutionSimulation& sim, int generation, int skipGen, int currentStep, int totalSteps) {
    updateTable(sim, generation, skipGen, currentStep, totalSteps);
    
    int fieldHeight = field.size();
    int tableStartY = fieldHeight + 1;
    
    // Обновляем поле
    for (int y = 0; y < field.size(); y++) {
        for (int x = 0; x < field[y].size(); x++) {
            if (field[y][x].type != previousField[y][x].type) {
                
                cout << "\033[" << y + 1 << ";" << x + 1 << "H"; // Перемещаем каретку
                
                switch (field[y][x].type) {
                    case EMPTY: cout << SYMBOL_EMPTY; break;
                    case WALL: cout << SYMBOL_WALL; break;
                    case AGENT: cout << SYMBOL_AGENT; break;
                    case FOOD: cout << SYMBOL_FOOD; break;
                }
            }
        }
    }

    // Обновляем таблицу
    for (int y = 0; y < table.size(); y++) {
        for (int x = 0; x < table[y].size(); x++) {
            if (table[y][x] != previousTable[y][x]) {
                cout << "\033[" << tableStartY + y << ";" << x + 1 << "H"; // Перемещаем каретку под поле
                cout << table[y][x];
            }
        }
    }
    
    cout.flush();

    previousField = field;
    previousTable = table;
    
    cout << "\033[" << tableStartY + table.size() + 2 << ";1H";
}