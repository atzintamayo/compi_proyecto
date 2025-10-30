# Core library - lógica de autómatas sin dependencias de UI

add_library(automatas_core STATIC
    Estado.cpp
    AFN.cpp
    AFD.cpp
    Scanner.cpp
    AFNManager.cpp
)

target_include_directories(automatas_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Sin dependencias de Qt en el core
target_compile_definitions(automatas_core PRIVATE CORE_LIBRARY)