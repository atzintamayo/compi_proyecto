# ============================================
# Biblioteca Core - Analizador Léxico
# ============================================
add_library(AnalizadorLexicoCore STATIC
    analizador_lexico/AFN.cpp
    analizador_lexico/AFN.h
    analizador_lexico/AFD.cpp
    analizador_lexico/AFD.h
    analizador_lexico/Estado.cpp
    analizador_lexico/Estado.h
    analizador_lexico/Scanner.cpp
    analizador_lexico/Scanner.h
    analizador_lexico/AFNManager.cpp
    analizador_lexico/AFNManager.h
    analizador_lexico/Conjunto.h
)

target_include_directories(AnalizadorLexicoCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/analizador_lexico
)

# ============================================
# Biblioteca Core - Calculadora
# ============================================
add_library(CalculadoraCore STATIC
    calculadora/Calculadora.cpp
    calculadora/Calculadora.h
)

target_include_directories(CalculadoraCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/calculadora
)

# Puedes agregar más módulos core aquí siguiendo el mismo patrón