#pragma once

#include <vector>

using namespace std;

typedef enum {
    IS_FREE = 0,
    IS_WALL
} cell_state;

#define SYMBOL_FREE ' '
#define SYMBOL_WALL '#'

struct Cell {
    cell_state state;
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