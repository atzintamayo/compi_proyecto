#include "ParserLL1.h"
#include <sstream>

ParserLL1::ParserLL1(Gramatica* gram, TablaLL1* tab, Lexer* lex)
    : gramatica(gram), tabla(tab), lexer(lex) {
    inicializarMapeo();
}

ParserLL1::~ParserLL1() {}

void ParserLL1::inicializarMapeo() {
    // Mapear tokens del lexer a nombres de terminales en la gramática
    mapeoTokens[TOKEN_OR] = "or";
    mapeoTokens[TOKEN_CONC] = "conc";
    mapeoTokens[TOKEN_MAS] = "CarPos";
    mapeoTokens[TOKEN_ASTERISCO] = "CerrKleen";
    mapeoTokens[TOKEN_INTERROGACION] = "Opc";
    mapeoTokens[TOKEN_PARIZQ] = "Parizq";
    mapeoTokens[TOKEN_PARDER] = "ParDer";
    mapeoTokens[TOKEN_CORCHIZQ] = "Corchizq";
    mapeoTokens[TOKEN_CORCHDER] = "CorchDer";
    mapeoTokens[TOKEN_GUION] = "guion";
    mapeoTokens[TOKEN_SIMB] = "simb";
    mapeoTokens[TOKEN_FIN] = "$";
}

void ParserLL1::limpiar() {
    while (!pila.empty()) pila.pop();
    traza.clear();
    mensajeError.clear();
}

std::string ParserLL1::tokenToTerminal(const TokenLL1& tok) const {
    auto it = mapeoTokens.find(tok.tipo);
    if (it != mapeoTokens.end()) {
        return it->second;
    }
    return "ERROR";
}

std::string ParserLL1::pilaToString() const {
    if (pila.empty()) return "vacía";
    
    std::stack<std::string> temp = pila;
    std::vector<std::string> elementos;
    
    while (!temp.empty()) {
        elementos.push_back(temp.top());
        temp.pop();
    }
    
    std::ostringstream oss;
    for (auto it = elementos.rbegin(); it != elementos.rend(); ++it) {
        oss << *it << " ";
    }
    
    return oss.str();
}

std::string ParserLL1::entradaToString(const std::vector<TokenLL1>& tokens, size_t inicio) const {
    std::ostringstream oss;
    
    for (size_t i = inicio; i < tokens.size(); i++) {
        oss << tokenToTerminal(tokens[i]) << " ";
    }
    
    return oss.str();
}

bool ParserLL1::parsear(const std::vector<TokenLL1>& tokens) {
    limpiar();
    
    if (tokens.empty()) {
        mensajeError = "No hay tokens para parsear";
        return false;
    }
    
    // Inicializar pila con $ y símbolo inicial
    pila.push("$");
    pila.push(gramatica->getSimboloInicial().nombre);
    
    size_t indiceToken = 0;
    
    // Agregar paso inicial
    traza.push_back(PasoParser(
        pilaToString(),
        entradaToString(tokens, indiceToken),
        "Inicialización"
    ));
    
    // Algoritmo del parser predictivo
    while (!pila.empty()) {
        std::string tope = pila.top();
        TokenLL1 tokenActual = tokens[indiceToken];
        std::string terminal = tokenToTerminal(tokenActual);
        
        // Caso 1: Tope es terminal
        if (gramatica->esTerminal(tope) || tope == "$") {
            if (tope == terminal) {
                // Match: consumir token y desapilar
                pila.pop();
                indiceToken++;
                
                std::ostringstream accion;
                accion << "Match: " << tope;
                
                traza.push_back(PasoParser(
                    pilaToString(),
                    entradaToString(tokens, indiceToken),
                    accion.str()
                ));
                
                // Si llegamos a $ y fin de entrada, éxito
                if (tope == "$" && terminal == "$") {
                    traza.push_back(PasoParser(
                        "vacía",
                        "",
                        "✓ Aceptado"
                    ));
                    return true;
                }
            } else {
                // Error: no coincide
                std::ostringstream error;
                error << "Error: Se esperaba '" << tope << "' pero se encontró '" 
                      << terminal << "' en posición " << tokenActual.posicion;
                mensajeError = error.str();
                
                traza.push_back(PasoParser(
                    pilaToString(),
                    entradaToString(tokens, indiceToken),
                    mensajeError,
                    true
                ));
                
                return false;
            }
        }
        // Caso 2: Tope es no terminal
        else {
            // Consultar tabla LL(1)
            EntradaTabla entrada = tabla->getEntrada(tope, terminal);
            
            if (!entrada.valida) {
                // Error: no hay producción
                std::ostringstream error;
                error << "Error: No hay producción para [" << tope << ", " 
                      << terminal << "]";
                mensajeError = error.str();
                
                traza.push_back(PasoParser(
                    pilaToString(),
                    entradaToString(tokens, indiceToken),
                    mensajeError,
                    true
                ));
                
                return false;
            }
            
            // Desapilar no terminal
            pila.pop();
            
            // Apilar lado derecho en orden inverso (si no es epsilon)
            const auto& produccion = entrada.produccion;
            if (!produccion.derecha.empty()) {
                for (auto it = produccion.derecha.rbegin(); 
                     it != produccion.derecha.rend(); ++it) {
                    pila.push(it->nombre);
                }
            }
            
            // Registrar acción
            std::ostringstream accion;
            accion << "Aplicar: " << produccion.toString();
            
            traza.push_back(PasoParser(
                pilaToString(),
                entradaToString(tokens, indiceToken),
                accion.str()
            ));
        }
    }
    
    // Si terminamos sin errores pero no llegamos a $
    if (indiceToken < tokens.size() - 1) {
        mensajeError = "Error: Quedan tokens sin procesar";
        return false;
    }
    
    return true;
}