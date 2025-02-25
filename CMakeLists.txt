# Минимальная требуемая версия CMake
cmake_minimum_required(VERSION 3.16)

# Название проекта
project(TransportCatalogue VERSION 1.0 LANGUAGES CXX)

# Установка стандарта C++ (C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Флаги для MSVC
if (MSVC)
    add_compile_options(/permissive-)  # Включаем строгий режим C++17
endif()

# Добавление исходных файлов
set(SOURCES
    src/domain.cpp
    src/geo.cpp
    src/json.cpp
    src/json_builder.cpp
    src/json_reader.cpp
    src/map_renderer.cpp
    src/request_handler.cpp
    src/svg.cpp
    src/transport_catalogue.cpp
    src/transport_router.cpp
    src/main.cpp
)

# Добавление заголовочных файлов
set(HEADERS
    src/domain.h
    src/geo.h
    src/graph.h
    src/json.h
    src/json_builder.h
    src/json_reader.h
    src/map_renderer.h
    src/ranges.h
    src/request_handler.h
    src/router.h
    src/svg.h
    src/transport_catalogue.h
    src/transport_router.h
)

# Создание исполняемого файла
add_executable(transport_catalogue ${SOURCES} ${HEADERS})

# Указание директории с заголовочными файлами
target_include_directories(transport_catalogue PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Оптимизация для релизной сборки
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(transport_catalogue PRIVATE /O2)
endif()

# Установка выходного файла в папку bin
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)