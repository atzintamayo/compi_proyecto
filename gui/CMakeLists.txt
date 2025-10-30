# GUI Application - Qt6

set(GUI_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    dialogs/CrearAFNDialog.cpp
    dialogs/CrearAFNDialog.h
    dialogs/AsignarTokenDialog.cpp
    dialogs/AsignarTokenDialog.h
    widgets/AFNTableWidget.cpp
    widgets/AFNTableWidget.h
    widgets/AFDTableWidget.cpp
    widgets/AFDTableWidget.h
)

add_executable(AnalizadorLexicoGUI ${GUI_SOURCES})

target_link_libraries(AnalizadorLexicoGUI PRIVATE
    automatas_core
    Qt6::Core
    Qt6::Widgets
)

target_include_directories(AnalizadorLexicoGUI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
)

# Configuración específica de Qt
set_target_properties(AnalizadorLexicoGUI PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)