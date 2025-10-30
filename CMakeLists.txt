cmake_minimum_required(VERSION 3.16)
project(AnalizadorLexico VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuración de Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Buscar Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Subdirectorios
add_subdirectory(core)
add_subdirectory(gui)