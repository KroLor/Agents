#pragma once

#include <vector>
#include "cells.h"
#include "simulation.h"
#include "main.h"

using namespace std;

/**
 * @brief Очищает консоль и создает поле, ограниченное стенами.
 * @param width Ширина.
 * @param height Высота.
 * @return Двумерное поле.
 */
vector<vector<Cell>> createField(int width, int height);

/**
 * @brief Обновляет поле.
 * @param field Двумерное поле.
 */
void updateField(const vector<vector<Cell>>& field);

/**
 * @brief Выводит статистику в консоль.
 * @param sim Класс симуляции.
 * @param currentStep Текущий шаг.
 * @param totalSteps Всего шагов в раундах.
 */
void printStatistics(const EvolutionSimulation& sim, int currentStep, int totalSteps);