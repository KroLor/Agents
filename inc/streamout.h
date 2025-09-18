#pragma once

#include <vector>

using namespace std;

enum CellType {
    EMPTY,
    FOOD,
    WALL,
    AGENT
};

#define SYMBOL_EMPTY ' '
#define SYMBOL_FOOD '*'
#define SYMBOL_WALL '#'
#define SYMBOL_AGENT '@'

struct Cell {
    CellType type;
};

/**
 * @brief Создает поле, ограниченное стенами.
 * @param rows Количество строк поля.
 * @param cols Количество столбцов поля.
 * @return Двумерное поле.
 */
vector<vector<Cell>> createField(int rows, int cols);

/**
 * @brief Обновляет поле.
 * @param field Двумерное поле.
 */
void updateField(const vector<vector<Cell>>& field);

/**
 * @brief Выводит поле в консоль.
 * @param field Двумерное поле.
 */
void printField(const vector<vector<Cell>>& field);