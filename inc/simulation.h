#pragma once

#include <vector>
#include <memory>
#include <string>
#include "cells.h"
#include "agent_logic.h"
#include "main.h"

using namespace std;

/**
 * @brief Класс, отвечающий за сеанс симуляции эволюции.
 * 
 * Предоставляет методы для контролируемого процесса эволюции.
 */
class EvolutionSimulation {
private:
    vector<vector<Cell>> grid;            // Двумерное поле клеток
    vector<unique_ptr<Agent>> population; // Популяция агентов
    bool paused;                          // Флаг паузы симуляции
    float simulationSpeed;                // Множитель скорости симуляции
    float mutationRate;                   // Коэффициент мутации
    int generation;                       // Текущее поколение
    int totalDeaths;                      // Общее количество смертей
    int totalAlives;                      // Общее количество рождений
    int currentTick;                      // Счетчик тиков для контроля появления еды

    /**
     * @brief Создает начальную популяцию агентов.
     * @param initialPopulationSize Начальный размер популяции.
     */
    void initializePopulation(int initialPopulationSize);

    /**
     * @brief Создает начальное количество еды.
     * @param initialFoodCount Начальное количество еды.
     */
    void initializeFood(int initialFoodCount);

    /**
     * @brief Находит случайную свободную позицию на поле.
     * @param x Ссылка для сохранения координаты X.
     * @param y Ссылка для сохранения координаты Y.
     * @return true если свободная позиция найдена, иначе false.
     */
    bool findRandomEmptyPosition(int& x, int& y) const;

    /**
     * @brief Обновляет состояние всех агентов.
     */
    void updateAgents();

    /**
     * @brief Генерирует новую еду на поле.
     */
    void spawnNewFood();

public:
    /**
     * @brief Конструктор симуляции эволюции.
     * @param grid Готовое поле клеток с учетом стен.
     * @param initialPopulationSize Начальный размер популяции.
     * @param initialFoodCount Начальное количество еды.
     */
    EvolutionSimulation(vector<vector<Cell>> grid, int initialPopulationSize = INIT_POP_SIZE, int initialFoodCount = INIT_FOOD_COUNT);
    
    ~EvolutionSimulation();
    
    /**
     * @brief Выполняет один такт симуляции.
     */
    void simulateStep();
    
    /**
     * @brief Обновляет состояние поля.
     */
    void updateGrid();

    /**
     * @brief Выполняет процесс эволюции агентов.
     */
    void geneticAlgorithm();
    
    /**
     * @brief Добавляет агента в указанную позицию.
     * @param x Координата X.
     * @param y Координата Y.
     * @param energy Начальная энергия агента.
     * @param genome Уникальный указатель на геном.
     * @return Указатель на созданного агента.
     */
    Agent* addAgent(int x, int y, int energy = INT_ENERGY_AGENT, unique_ptr<Gene> genome = nullptr);
    
    /**
     * @brief Добавляет еду в указанную позицию.
     * @param x Координата X.
     * @param y Координата Y.
     * @param energyValue Количество еды.
     * @return true если ресурс добавлен успешно, иначе false.
     */
    bool addFood(int x, int y, int energyValue = ENERGY_FOOD_VALUE);
    
    /**
     * @brief Возвращает клетку в указанной позиции.
     * @param x Координата X.
     * @param y Координата Y.
     * @return Ссылка на клетку.
     */
    const Cell& getCell(int x, int y) const;

    /**
     * @brief Возвращает номер текущего поколения.
     * @return Текущее поколение.
     */
    int getGeneration() const { return generation; }
    
    /**
     * @brief Возвращает все клетки поля.
     * @return Константная ссылка на двумерный вектор клеток.
     */
    const vector<vector<Cell>>& getGrid() const { return grid; }
    
    /**
     * @brief Возвращает всех агентов в симуляции.
     * @return Константная ссылка на вектор агентов.
     */
    const vector<unique_ptr<Agent>>& getPopulation() const { return population; }

    /**
     * @brief Структура для хранения данных о симуляции.
     */
    struct SimulationData {
        int populationSize;      // Текущий размер популяции
        int totalFood;           // Общее количество еды на поле
        int averageEnergyLevel;  // Средний уровень энергии агентов
        int maxEnergyLevel;      // Максимальный уровень энергии
        int minEnergyLevel;      // Минимальный уровень энергии
        int generation;          // Текущая итерация симуляции
        int totalAlives;         // Общее количество живых
        int totalDeaths;         // Общее количество мертвых
        float mutationRate;      // Текущий коэффициент мутации
    };

    /**
     * @brief Возвращает статистику по текущему состоянию симуляции.
     * @return Структура со статистическими данными.
     */
    SimulationData getSimulationData() const;
    
    /**
     * @brief Сохраняет текущее состояние симуляции в файл.
     * @param filename Имя файла для сохранения.
     * @return true если сохранение успешно, иначе false.
     */
    bool saveSimulationState(const string& filename) const;
    
    /**
     * @brief Загружает состояние симуляции из файла.
     * @param filename Имя файла для загрузки.
     * @return true если загрузка успешна, иначе false.
     */
    bool loadSimulationState(const string& filename);
    
    /**
     * @brief Приостанавливает или возобновляет симуляцию.
     * @param paused true для паузы, false для продолжения.
     */
    void setSimulationPaused(bool paused) { this->paused = paused; }
    
    /**
     * @brief Проверяет, находится ли симуляция на паузе.
     * @return true если симуляция на паузе, иначе false.
     */
    bool isSimulationPaused() const { return paused; }
    
    /**
     * @brief Устанавливает скорость симуляции.
     * @param speed Множитель скорости (1.0 = нормальная скорость).
     */
    void setSimulationSpeed(float speed) { simulationSpeed = speed; }
    
    /**
     * @brief Возвращает текущую скорость симуляции.
     * @return Текущий множитель скорости.
     */
    float getSimulationSpeed() const { return simulationSpeed; }
    
    /**
     * @brief Устанавливает коэффициент мутации для новых агентов.
     * @param mutationRate Новый коэффициент мутации (0.0 - 1.0).
     */
    void setMutationRate(float mutationRate) { this->mutationRate = mutationRate; }
    
    /**
     * @brief Возвращает текущий коэффициент мутации.
     * @return Текущий коэффициент мутации.
     */
    float getMutationRate() const { return mutationRate; }
    
    /**
     * @brief Сбрасывает поле в начальное состояние для нового раунда.
     * @param newGrid Новое поле для симуляции.
     */
    void resetGrid(vector<vector<Cell>> newGrid = {});
};