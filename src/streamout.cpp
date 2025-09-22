#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "streamout.h"
#include "cells.h"

using namespace std;

static vector<vector<Cell>> previousField; // Буфер состояния поля

vector<vector<Cell>> createField(int cols, int rows) {
    auto field = vector<vector<Cell>>(cols + 2, vector<Cell>(rows + 2, {EMPTY}));

    // Назначаем стены на границах (первую и последнюю строку)
    for (int i = 0; i < rows + 2; i++) {
        field[0][i].type = WALL;        // Верхняя граница
        field[cols + 1][i].type = WALL; // Нижняя граница
    }

    // Назначаем стены на границах (первый и последний столбец)
    for (int j = 0; j < cols + 2; j++) {
        field[j][0].type = WALL;        // Левая граница
        field[j][rows + 1].type = WALL; // Правая граница
    }

    previousField = field; // Сохранение предыдущего состояния поля в буфер

    return field;
}

void updateField(const vector<vector<Cell>>& field) {
    for (int i = 0; i < field.size(); i++) {
        for (int j = 0; j < field[i].size(); j++) {
            if (field[i][j].type != previousField[i][j].type) {
                cout << "\033[" << i + 1 << ";" << j + 1 << "H"; // Перенос каретки

                switch (field[i][j].type) {
                    case EMPTY: cout << SYMBOL_EMPTY; break;
                    case WALL: cout << SYMBOL_WALL; break;
                    case AGENT: cout << SYMBOL_AGENT; break;
                    case FOOD: cout << SYMBOL_FOOD; break;
                }

                cout.flush(); // Принудительная отправка буфера в консоль
                std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
            }
        }
    }
    previousField = field;
}

void printField(const vector<vector<Cell>>& field) {
    for (const auto& row : field) {
        for (const auto& cell : row) {
            switch (cell.type) {
                case EMPTY: cout << SYMBOL_EMPTY; break;
                case WALL: cout << SYMBOL_WALL; break;
                case AGENT: cout << SYMBOL_AGENT; break;
                case FOOD: cout << SYMBOL_FOOD; break;
            }
        }
        cout << '\n';
    }
}