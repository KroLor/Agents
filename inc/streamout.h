#pragma once

#include <vector>
#include "cells.h"

#define TICK_MS 500

using namespace std;

vector<vector<Cell>> createField(int width, int height);
void printField(const vector<vector<Cell>>& field);