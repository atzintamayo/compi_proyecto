#ifndef SCANNER_H
#define SCANNER_H

#include "AFD.h"
#include <string>
#include <vector>

// Token especial para espacios/blancos
const int TOKEN_ESP = 0;

// Estructura para un token reconocido
struct Token {
    int tokenId;        // ID del token (-1 = error, 0 = ESP)
    std::string lexema; // Texto reconocido
    int inicio;         // Posición inicial en el texto
    int fin;            // Posición final en el texto
    
    Token(int id, std::string lex, int ini, int f) 
        : tokenId(id), lexema(lex), inicio(ini), fin(f) {}
};

// Estructura para error léxico
struct ErrorLexico {
    char caracter;      // Carácter no reconocido
    int posicion;       // Posición en el texto
    std::string contexto; // Contexto alrededor del error
    
    ErrorLexico(char c, int pos, std::string ctx) 
        : caracter(c), posicion(pos), contexto(ctx) {}
};

// Clase Scanner
class Scanner {
private:
    AFD* afd;
    std::string texto;
    int posicion;
    std::vector<Token> tokens;
    std::vector<ErrorLexico> errores;
    bool omitirEspacios;
    
    // Maximal Munch: encuentra el lexema más largo posible
    Token maximalMunch(int inicio);
    
    // Verifica si es espacio/tab/newline
    bool esEspacio(char c);
    
    // Obtiene contexto alrededor de una posición (para errores)
    std::string obtenerContexto(int pos, int radio = 10);
    
public:
    Scanner(AFD* automata);
    ~Scanner();
    
    // Configuración
    void setTexto(std::string txt);
    void setOmitirEspacios(bool omitir);
    
    // Escaneo
    bool escanear();
    
    // Resultados
    const std::vector<Token>& getTokens() const;
    const std::vector<ErrorLexico>& getErrores() const;
    bool tieneErrores() const;
    
    // Utilidades
    std::string toString() const;
    void limpiar();
};

#endif // SCANNER_H