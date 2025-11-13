#include "Lexer.h"
#include <cctype>

Lexer::Lexer() : posicion(0) {
    inicializarTabla();
}

Lexer::~Lexer() {}

void Lexer::inicializarTabla() {
    // Inicializar todo a estado de error (-1)
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 256; j++) {
            tablaAFD[i][j] = -1;
        }
    }
    
    // Estado 0 (inicial) - reconoce todos los caracteres válidos
    // Los metacaracteres van a tokens específicos (estado 1)
    // Los demás son SIMB (estado 1)
    for (int c = 0; c < 256; c++) {
        if (!esMetacaracter(c) && c != '\0' && c != ' ' && c != '\t' && c != '\n') {
            tablaAFD[0][c] = 1;  // SIMB
        }
    }
    
    // Metacaracteres van al estado de aceptación
    tablaAFD[0][(unsigned char)'|'] = 1;
    tablaAFD[0][(unsigned char)'&'] = 1;
    tablaAFD[0][(unsigned char)'+'] = 1;
    tablaAFD[0][(unsigned char)'*'] = 1;
    tablaAFD[0][(unsigned char)'?'] = 1;
    tablaAFD[0][(unsigned char)'('] = 1;
    tablaAFD[0][(unsigned char)')'] = 1;
    tablaAFD[0][(unsigned char)'['] = 1;
    tablaAFD[0][(unsigned char)']'] = 1;
    tablaAFD[0][(unsigned char)'-'] = 1;
}

bool Lexer::esMetacaracter(char c) {
    return c == '|' || c == '&' || c == '+' || c == '*' || 
           c == '?' || c == '(' || c == ')' || c == '[' || 
           c == ']' || c == '-';
}

void Lexer::setTexto(const std::string& txt) {
    texto = txt;
    posicion = 0;
}

void Lexer::reiniciar() {
    posicion = 0;
}

TokenLL1 Lexer::yylex() {
    // Saltar espacios en blanco
    while (posicion < texto.length() && 
           (texto[posicion] == ' ' || texto[posicion] == '\t' || texto[posicion] == '\n')) {
        posicion++;
    }
    
    // Fin de cadena
    if (posicion >= texto.length()) {
        return TokenLL1(TOKEN_FIN, "", posicion);
    }
    
    char c = texto[posicion];
    int inicio = posicion;
    
    // Reconocer token según el carácter
    TokenType tipo;
    
    switch (c) {
        case '|': tipo = TOKEN_OR; break;
        case '&': tipo = TOKEN_CONC; break;
        case '+': tipo = TOKEN_MAS; break;
        case '*': tipo = TOKEN_ASTERISCO; break;
        case '?': tipo = TOKEN_INTERROGACION; break;
        case '(': tipo = TOKEN_PARIZQ; break;
        case ')': tipo = TOKEN_PARDER; break;
        case '[': tipo = TOKEN_CORCHIZQ; break;
        case ']': tipo = TOKEN_CORCHDER; break;
        case '-': tipo = TOKEN_GUION; break;
        default:
            // Cualquier otro carácter es SIMB
            if (!esMetacaracter(c) && c != ' ' && c != '\t' && c != '\n') {
                tipo = TOKEN_SIMB;
            } else {
                posicion++;
                return TokenLL1(TOKEN_ERROR, std::string(1, c), inicio);
            }
    }
    
    posicion++;
    return TokenLL1(tipo, std::string(1, c), inicio);
}

std::vector<TokenLL1> Lexer::tokenizar() {
    std::vector<TokenLL1> tokens;
    reiniciar();
    
    TokenLL1 tok = yylex();
    while (tok.tipo != TOKEN_FIN) {
        tokens.push_back(tok);
        tok = yylex();
    }
    tokens.push_back(tok); // Agregar TOKEN_FIN
    
    return tokens;
}

std::string Lexer::tokenToString(TokenType tipo) {
    switch (tipo) {
        case TOKEN_OR: return "OR";
        case TOKEN_CONC: return "CONC";
        case TOKEN_MAS: return "MAS";
        case TOKEN_ASTERISCO: return "ASTERISCO";
        case TOKEN_INTERROGACION: return "INTERROGACION";
        case TOKEN_PARIZQ: return "PARIZQ";
        case TOKEN_PARDER: return "PARDER";
        case TOKEN_CORCHIZQ: return "CORCHIZQ";
        case TOKEN_CORCHDER: return "CORCHDER";
        case TOKEN_GUION: return "GUION";
        case TOKEN_SIMB: return "SIMB";
        case TOKEN_FIN: return "$";
        case TOKEN_ERROR: return "ERROR";
        default: return "DESCONOCIDO";
    }
}