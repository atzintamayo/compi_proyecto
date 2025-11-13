# ============================================
# Ejecutable principal con GUI modular
# ============================================

set(GUI_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    
    # Módulo: Analizador Léxico
    modules/analizador_lexico/AnalizadorLexicoWidget.cpp
    modules/analizador_lexico/AnalizadorLexicoWidget.h
    modules/analizador_lexico/dialogs/CrearAFNDialog.cpp
    modules/analizador_lexico/dialogs/CrearAFNDialog.h
    modules/analizador_lexico/dialogs/AsignarTokenDialog.cpp
    modules/analizador_lexico/dialogs/AsignarTokenDialog.h
    modules/analizador_lexico/widgets/AFNTableWidget.cpp
    modules/analizador_lexico/widgets/AFNTableWidget.h
    modules/analizador_lexico/widgets/AFDTableWidget.cpp
    modules/analizador_lexico/widgets/AFDTableWidget.h
    
    # Módulo: Calculadora
    modules/calculadora/CalculadoraWidget.cpp
    modules/calculadora/CalculadoraWidget.h
    
    # Módulo: LL(1)
    modules/ll1/LL1Widget.cpp
    modules/ll1/LL1Widget.h
)

add_executable(CompiladorTools ${GUI_SOURCES})

target_link_libraries(CompiladorTools PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    AnalizadorLexicoCore
    CalculadoraCore
    LL1Core
)

target_include_directories(CompiladorTools PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/analizador_lexico
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/calculadora
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/ll1
    ${CMAKE_SOURCE_DIR}/core/analizador_lexico
    ${CMAKE_SOURCE_DIR}/core/calculadora
    ${CMAKE_SOURCE_DIR}/core/ll1
)