#include <iostream>
#include <thread>
#include <chrono>

#include "main.h"
#include "streamout.h"

int main() {
    auto field = createField(FIELD_HEIGHT, FIELD_WIDTH);

    // Основной цикл
    while (true) {
        // update
        

        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        printField(field);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}