#pragma once

#include <vector>
#include "cells.h"

using namespace std;

/**
 * @brief Создает поле, ограниченное стенами.
 * @param cols Количество столбцов поля.
 * @param rows Количество строк поля.
 * @return Двумерное поле.
 */
vector<vector<Cell>> createField(int cols, int rows);

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