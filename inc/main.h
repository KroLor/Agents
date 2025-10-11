#pragma once

#define FIELD_WIDTH 50 // Ширина поля
#define FIELD_HEIGHT 20 // Высота поля

#define GENERATIONS 100 // Всего поколений

#define INIT_POP_SIZE 30 // Размер популяции
#define SKIP_GENERATIONS 10 // Кол-во пропусков раундов/поколений
#define NUMBER_OF_STEPS 30 // Кол-во шагов в раунде
#define INIT_ENERGY_AGENT 100 // Кол-во энергии у агента
#define ENERGY_LOSS_PER_STEP 10 // Потеря энергии за шаг //
#define ENERGY_LOSS_DUE_TO_INACTION 5 // Потеря энергии за бездействие //

#define INIT_FOOD_COUNT 50 // Кол-во еды, появляющейся в начале раунда
#define FOOD_SPAWN_INTERVAL 2 // Каждый N-й тик появляется еда
#define FOOD_ADD_TIMES 50 // Кол-во появлений еды за раз
#define CHANCE_OF_FOOD_APPEARANCE 0.5f // Шанс появления еды в клетке
#define ENERGY_FOOD_VALUE 30 // Энергетическая ценность еды //

#define TICK_MS 125 // Интервал между тиками (мс)

#define USE_A_NEURAL_NETWORK 1 // Отвечает за использование нейросети в агентах
#define INPUT_VALUES 11 // Входные значения
// #define HIDDEN_LAYERS 1 // Скрытых слоев
#define NEURONS_IN_HIDDEN_LAYER 13 // Кол-во нейронов в скрытых(ом) слоях(е) // (одинаково)
#define OUTPUT_VALUES 4 // Выходные значения