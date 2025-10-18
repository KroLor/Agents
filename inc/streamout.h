#pragma once

#include <vector>
#include "cells.h"
#include "simulation.h"
#include "main.h"

using namespace std;

/**
 * @brief Создает поле, ограниченное стенами.
 * @param width Ширина.
 * @param height Высота.
 * @return Двумерное поле.
 */
vector<vector<Cell>> createField(int width, int height);

/**
 * @brief Обновляет поле и таблицу статистики.
 * @param field Двумерное поле.
 * @param sim Ссылка на симуляцию.
 * @param generation Всего поколений.
 * @param skipGen Пропусков поколений.
 * @param currentStep Текущий шаг.
 * @param totalSteps Всего шагов.
 */
void updateField(const vector<vector<Cell>>& field, const EvolutionSimulation& sim, int generation, int skipGen, int currentStep, int totalSteps);

/**
 * @brief Обновляет таблицу статистики.
 * @param sim Ссылка на симуляцию.
 * @param generation Всего поколений.
 * @param skipGen Пропусков поколений.
 * @param currentStep Текущий шаг.
 * @param totalSteps Всего шагов.
 */
void updateTable(const EvolutionSimulation& sim, int generation, int skipGen, int currentStep, int totalSteps);