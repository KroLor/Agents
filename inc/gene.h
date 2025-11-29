#pragma once

#include <vector>
#include <memory>
// #include "agent_logic.h"
#include "cells.h"

using namespace std;

/**
 * @brief Абстрактный класс, принимающий решение (мозг агента).
 * 
 * Может мутировать, самовоспроизводится и принимать решение о наравлении следующего шага.
 */
class Gene {
public:
    virtual ~Gene() = default;
    
    // Основные методы нейросети

    /**
     * @brief Выбрать направление движения на основе окружения, энергии и направления к еде.
     * @param surroundings Окружающие клетки (8 клеток).
     * @param energy Кол-во энергии агента.
     * @param directionToFood Вектор направления к ближайшей еде.
     * @return (delta_x, delta_y) - вектор направления.
     */
    virtual pair<int, int> decideDirection(const vector<Cell>& surroundings, int energy, pair<int, int> directionToFood) = 0;

    /**
     * @brief Создать мутированную копию гена.
     * @param mutationPower Сила мутации.
     * @return Указатель на мутированный ген.
     */
    virtual unique_ptr<Gene> mutation(float mutationPower) const = 0;

    /**
     * @brief Создать копию гена.
     * @return Копия гена.
     */
    virtual unique_ptr<Gene> clone() const = 0;

    /**
     * @brief Скрещивает гены с другим геном, изменяя оба
     * @param otherGene Другой ген для скрещивания
     */
    virtual void crossing(Gene& otherGene) = 0;
    
    // Методы для сохранения/загрузки гена

    /**
     * 
     */
    virtual string saveDataCSV() const = 0;
    
    // virtual void deserialize() = 0;
};