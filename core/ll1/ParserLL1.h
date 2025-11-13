#ifndef PARSERLL1_H
#define PARSERLL1_H

#include "Gramatica.h"
#include "TablaLL1.h"
#include "Lexer.h"
#include <vector>
#include <stack>
#include <string>

// Estructura para una entrada de la traza
struct PasoParser {
    std::string pila;           // Contenido de la pila
    std::string entrada;        // Entrada restante
    std::string accion;         // Acción realizada
    bool error;                 // Si hubo error
    
    PasoParser(const std::string& p, const std::string& e, const std::string& a, bool err = false)
        : pila(p), entrada(e), accion(a), error(err) {}
};

// Clase para el parser predictivo LL(1)
class ParserLL1 {
public:
    ParserLL1(Gramatica* gramatica, TablaLL1* tabla, Lexer* lexer);
    ~ParserLL1();
    
    // Parsear una cadena de tokens
    bool parsear(const std::vector<TokenLL1>& tokens);
    
    // Obtener la traza del parseo
    std::vector<PasoParser> getTraza() const { return traza; }
    
    // Obtener mensaje de error
    std::string getMensajeError() const { return mensajeError; }
    
    // Limpiar estado
    void limpiar();
    
private:
    Gramatica* gramatica;
    TablaLL1* tabla;
    Lexer* lexer;
    
    std::vector<PasoParser> traza;
    std::string mensajeError;
    
    // Pila del parser
    std::stack<std::string> pila;
    
    // Mapeo de tokens a nombres de terminales en la gramática
    std::map<TokenType, std::string> mapeoTokens;
    
    void inicializarMapeo();
    std::string pilaToString() const;
    std::string entradaToString(const std::vector<TokenLL1>& tokens, size_t inicio) const;
    std::string tokenToTerminal(const TokenLL1& tok) const;
};

#endif // PARSERLL1_H