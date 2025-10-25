#pragma once

#define FIELD_WIDTH 51 // Ширина поля
#define FIELD_HEIGHT 20 // Высота поля

#define GENERATIONS 100000 // Всего поколений

#define INIT_POP_SIZE 20 //40 Размер популяции
#define SKIP_GENERATIONS 5000 //1000 Кол-во пропусков раундов/поколений
#define NUMBER_OF_STEPS 50 // Кол-во шагов в раунде
#define INIT_ENERGY_AGENT 100 // Кол-во энергии у агента
#define ENERGY_LOSS_PER_STEP 4 // Потеря энергии за шаг //
#define ENERGY_LOSS_DUE_TO_INACTION 6 // Потеря энергии за бездействие //

#define INIT_FOOD_COUNT 60 //70 Кол-во еды, появляющейся в начале раунда
#define FOOD_SPAWN_INTERVAL 20 // Каждый N-й тик появляется еда
#define FOOD_ADD_TIMES 50 // Кол-во появлений еды за раз
#define CHANCE_OF_FOOD_APPEARANCE 0.4f //0.5f Шанс появления еды в клетке
#define ENERGY_FOOD_VALUE 40 // Энергетическая ценность еды //

#define TICK_MS 75 //150 Интервал между тиками (мс)

#define USE_A_NEURAL_NETWORK 1 // Отвечает за использование нейросети в агентах
#define INPUT_VALUES 11 // Входные значения
// #define HIDDEN_LAYERS 1 // Скрытых слоев
#define NEURONS_IN_HIDDEN_LAYER 13 // Кол-во нейронов в скрытых(ом) слоях(е) // (одинаково)
#define OUTPUT_VALUES 4 // Выходные значения

#define AGENT_MUTATION_CHANCE 0.05f // Шанс мутации гена
#define AGENT_MUTATION_POWER 0.22222 // Число-диапозон (+, -), которое суммируется с каждым весом
#define AGENT_CHANCE_TO_CROSS_OVER 0.2f // Шанс скрещивания (кроссинговера)

/**
 * 
 */
void assignConstants();