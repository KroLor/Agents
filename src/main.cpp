#include <iostream>
#include <thread>
#include <chrono>
#include "main.h"
#include "streamout.h"

int main() {
    auto field = createField(FIELD_HEIGHT, FIELD_WIDTH);

    printField(field);

    // Основной цикл
    while (true) {
        // Основной алгоритм
        

        updateField(field);

        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
    }

    return 0;
}