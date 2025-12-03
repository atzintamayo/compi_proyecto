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

# ============================================
# Biblioteca Core - LL(1)
# ============================================
add_library(LL1Core STATIC
    ll1/Lexer.cpp
    ll1/Lexer.h
    ll1/Gramatica.cpp
    ll1/Gramatica.h
    ll1/ConjuntosLL1.cpp
    ll1/ConjuntosLL1.h
    ll1/TablaLL1.cpp
    ll1/TablaLL1.h
    ll1/ParserLL1.cpp
    ll1/ParserLL1.h
)

target_include_directories(LL1Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ll1
)

# ============================================
# Biblioteca Core - LR(0)
# ============================================
add_library(LR0Core STATIC
    lr0/ItemLR0.cpp
    lr0/ItemLR0.h
    lr0/ElemTablaLR0.cpp
    lr0/ElemTablaLR0.h
    lr0/RenglonLR0.cpp
    lr0/RenglonLR0.h
    lr0/Inf_IrA.cpp
    lr0/Inf_IrA.h
    lr0/Simb.cpp
    lr0/Simb.h
    lr0/AnalizadorLR0.cpp
    lr0/AnalizadorLR0.h
)

target_include_directories(LR0Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lr0
)

# LR0Core depende de LL1Core porque usa Gramatica
target_link_libraries(LR0Core PUBLIC
    LL1Core
)

# Puedes agregar más módulos core aquí siguiendo el mismo patrón

# ============================================
# Biblioteca Core - Gramatica de gramaticas
# ============================================
add_library(Gramatica_de_gramaticasCore STATIC
    gramatica_gramaticas/Gramaticas.cpp
    gramatica_gramaticas/Gramaticas.h
)

target_include_directories(Gramatica_de_gramaticasCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/gramatica_gramaticas
)

# Puedes agregar más módulos core aquí siguiendo el mismo patrón
