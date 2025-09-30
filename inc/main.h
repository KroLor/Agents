#pragma once

#define FIELD_WIDTH 50 // Ширина поля
#define FIELD_HEIGHT 20 // Высота поля

#define GENERATIONS 100 // Всего поколений

#define INIT_POP_SIZE 30 // Размер популяции
#define SKIP_GENERATIONS 10 // Кол-во пропусков раундов/поколений
#define NUMBER_OF_STEPS 30 // Кол-во шагов в раунде
#define INT_ENERGY_AGENT 100 // Кол-во энергии у агента
#define ENERGY_LOSS_PER_STEP 10 // Потеря энергии за шаг //
#define ENERGY_LOSS_DUE_TO_INACTION 5 // Потеря энергии за бездействие //

#define INIT_FOOD_COUNT 50 // Кол-во еды, появляющейся в начале раунда
#define FOOD_SPAWN_INTERVAL 2 // Каждый N-й тик появляется еда
#define FOOD_ADD_TIMES 100 // Кол-во появлений еды за раз
#define CHANCE_OF_FOOD_APPEARANCE 0.5f // Шанс появления еды в клетке
#define ENERGY_FOOD_VALUE 30 // Энергетическая ценность еды //

#define TICK_MS 100 // Интервал между тиками (мс)