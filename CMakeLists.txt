cmake_minimum_required(VERSION 4.1.0)
project(Agents)

# Установка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включение каталогов с заголовками
include_directories(inc)

# Директория исходных файлов
set(SOURSE_DIRECTORY src)

# Поиск всех исходных файлов
file(GLOB SOURCES "${SOURSE_DIRECTORY}/*.cpp")

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES})

# Установка свойств для отладки и релиза
set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX "_d"
    RELEASE_POSTFIX ""
)

# Установка выходного каталога
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)