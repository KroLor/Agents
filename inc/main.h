#pragma once

#include <vector>

#define FIELD_WIDTH 80 //51 Ширина поля
#define FIELD_HEIGHT 21 //20 Высота поля

#define GENERATIONS 100000 // Всего поколений
#define SKIP_GENERATIONS 1000 // Кол-во пропусков раундов/поколений

#define INIT_POP_SIZE 100 //35 Размер популяции
#define NUMBER_OF_STEPS 150 // Кол-во шагов в раунде
#define INIT_ENERGY_AGENT 200 // Кол-во энергии у агента
#define ENERGY_LOSS_PER_STEP 4 // Потеря энергии за шаг //
#define ENERGY_LOSS_DUE_TO_INACTION 4 // Потеря энергии за бездействие //

#define INIT_FOOD_COUNT 150 // Кол-во еды, появляющейся в начале раунда
#define FOOD_SPAWN_INTERVAL 15 // Каждый N-й тик появляется еда
#define FOOD_ADD_TIMES 70 // Кол-во появлений еды за раз
#define CHANCE_OF_FOOD_APPEARANCE 0.5f //0.5f Шанс появления еды в клетке
#define ENERGY_FOOD_VALUE 40 //40 Энергетическая ценность еды //

#define TICK_MS 50 //150 Интервал между тиками (мс)

#define USE_A_NEURAL_NETWORK 1 // Отвечает за использование нейросети в агентах
#define INPUT_VALUES 11 // Входные значения
// #define HIDDEN_LAYERS 1 // Скрытых слоев
#define NEURONS_IN_HIDDEN_LAYER 7 // Кол-во нейронов в скрытых(ом) слоях(е) // (одинаково)
#define OUTPUT_VALUES 4 // Выходные значения

#define AGENT_MUTATION_CHANCE 0.2f // Шанс мутации гена
#define AGENT_MUTATION_POWER 0.3 // Число-диапозон (+, -), которое суммируется с каждым весом
#define AGENT_CHANCE_TO_CROSS_OVER 0.3f // Шанс скрещивания (кроссинговера)

extern bool UseNeuralNetwork;
extern int InputValues;
extern int NeuronsInHiddenLayer;
extern int OutputValues;

struct ProgramParameters {
    bool useNeuralNetwork;
    char type; // 1 - обучение, 0 - обзор
    int InputValues;
    int NeuronsInHiddenLayer;
    int OutputValues;
    std::vector<std::vector<double>> weights;
};

void settingConstants(ProgramParameters param);

void _train();

void _show();