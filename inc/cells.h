#pragma once

#define SYMBOL_EMPTY ' '
#define SYMBOL_FOOD '*'
#define SYMBOL_WALL '#'
#define SYMBOL_AGENT '@'

enum CellType {
    EMPTY,
    FOOD,
    WALL,
    AGENT
};

struct Cell {
    CellType type;
    int foodValue;
};