#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

// Enum de tokens para expresiones regulares
enum TokenType {
    TOKEN_OR = 256,          // |
    TOKEN_CONC,              // &
    TOKEN_MAS,               // +
    TOKEN_ASTERISCO,         // *
    TOKEN_INTERROGACION,     // ?
    TOKEN_PARIZQ,            // (
    TOKEN_PARDER,            // )
    TOKEN_CORCHIZQ,          // [
    TOKEN_CORCHDER,          // ]
    TOKEN_GUION,             // -
    TOKEN_SIMB,              // cualquier símbolo del alfabeto
    TOKEN_FIN,               // fin de cadena
    TOKEN_ERROR              // error léxico
};

// Estructura para representar un token
struct TokenLL1 {
    TokenType tipo;
    std::string lexema;
    int posicion;
    
    TokenLL1(TokenType t, const std::string& lex, int pos) 
        : tipo(t), lexema(lex), posicion(pos) {}
};

// Clase Lexer tabular (yylex style)
class Lexer {
public:
    Lexer();
    ~Lexer();
    
    // Configurar entrada
    void setTexto(const std::string& texto);
    
    // Obtener siguiente token (yylex)
    TokenLL1 yylex();
    
    // Reiniciar
    void reiniciar();
    
    // Obtener todos los tokens
    std::vector<TokenLL1> tokenizar();
    
    // Convertir token a string
    static std::string tokenToString(TokenType tipo);
    
private:
    std::string texto;
    size_t posicion;
    
    // AFD hardcodeado - tabla de transiciones
    int tablaAFD[2][256];  // 2 estados: 0=inicial, 1=aceptación
    
    void inicializarTabla();
    bool esMetacaracter(char c);
};

#endif // LEXER_H