cmake_minimum_required(VERSION 3.16)
project(TransportCatalogue VERSION 1.0 LANGUAGES CXX)

# Убедитесь, что папки bin существуют
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Release)

# Настройка выходных директорий для всех конфигураций
foreach(CONFIGURATION Debug Release RelWithDebInfo MinSizeRel)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIGURATION} ${CMAKE_SOURCE_DIR}/bin/${CONFIGURATION})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIGURATION} ${CMAKE_SOURCE_DIR}/bin/${CONFIGURATION})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIGURATION} ${CMAKE_SOURCE_DIR}/bin/${CONFIGURATION})
endforeach()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/permissive-)
endif()

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

add_executable(transport_catalogue ${SOURCES} ${HEADERS})

target_include_directories(transport_catalogue PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Альтернативное решение через install (рекомендуется)
# install(TARGETS transport_catalogue DESTINATION bin)

add_custom_command(
    TARGET transport_catalogue
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:transport_catalogue> ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/
)