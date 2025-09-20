#pragma once

#include <vector>
#include "cells.h"

using namespace std;

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