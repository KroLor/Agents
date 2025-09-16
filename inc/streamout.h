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
#define SYMBOL_WALL '#'

struct Cell {
    CellType type;
};

/**
 * @brief Создает поле, ограниченное стенами.
 * @param rows Количество строк поля.
 * @param cols Количество столбцов поля.
 * @retval Двумерное поле.
 */
vector<vector<Cell>> createField(int rows, int cols);

/**
 * @brief Выводит/обновляет поле в консоли.
 * @param field Двумерное поле.
 */
void printField(const vector<vector<Cell>>& field);