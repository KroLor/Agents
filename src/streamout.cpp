#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "streamout.h"
#include "cells.h"

using namespace std;

vector<vector<Cell>> createField(int width, int height) {
    auto field = vector<vector<Cell>>(width + 2, vector<Cell>(height + 2, {EMPTY}));

    // Границы поля
    for (int i = 0; i < height + 2; i++) {
        field[0][i].type = WALL;
        field[width + 1][i].type = WALL;
    }

    for (int j = 0; j < width + 2; j++) {
        field[j][0].type = WALL;
        field[j][height + 1].type = WALL;
    }

    return field;
}

void printField(const vector<vector<Cell>>& field) {
    // Простой вывод поля без очистки консоли (очистка делается в main)
    for (int j = 0; j < field[0].size(); j++) {
        for (int i = 0; i < field.size(); i++) {
            switch (field[i][j].type) {
                case EMPTY: cout << SYMBOL_EMPTY; break;
                case WALL: cout << SYMBOL_WALL; break;
                case AGENT: cout << SYMBOL_AGENT; break;
                case FOOD: cout << SYMBOL_FOOD; break;
            }
        }
        cout << '\n';
    }
    cout.flush();
}