#include <iostream>
#include <vector>
#include "streamout.h"

using namespace std;

vector<vector<Cell>> createField(int rows, int cols) {
    auto field = vector<vector<Cell>>(rows + 2, vector<Cell>(cols + 2, {IS_FREE}));

    // Назначаем стены на границах (первую и последнюю строку)
    for (int i = 0; i < cols + 2; ++i) {
        field[0][i].state = IS_WALL;        // Верхняя граница
        field[rows + 1][i].state = IS_WALL; // Нижняя граница
    }

    // Назначаем стены на границах (первый и последний столбец)
    for (int j = 0; j < rows + 2; ++j) {
        field[j][0].state = IS_WALL;        // Левая граница
        field[j][cols + 1].state = IS_WALL; // Правая граница
    }

    return field;
}

void printField(const vector<vector<Cell>>& field) {
    for (const auto& row : field) {
        for (const auto& cell : row) {
            switch (cell.state) {
                case IS_FREE: cout << SYMBOL_FREE; break;
                case IS_WALL: cout << SYMBOL_WALL; break;
            }
        }
        cout << '\n';
    }
}