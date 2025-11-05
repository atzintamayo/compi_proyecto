#include "Scanner.h"
#include <sstream>
#include <algorithm>

Scanner::Scanner(AFD* automata) {
    afd = automata;
    posicion = 0;
    omitirEspacios = true;
    tokens.clear();
    errores.clear();
}

Scanner::~Scanner() {
    // No eliminar afd, es manejado externamente
}

void Scanner::setTexto(std::string txt) {
    texto = txt;
    posicion = 0;
    tokens.clear();
    errores.clear();
}

void Scanner::setOmitirEspacios(bool omitir) {
    omitirEspacios = omitir;
}

bool Scanner::esEspacio(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

std::string Scanner::obtenerContexto(int pos, int radio) {
    int ini = std::max(0, pos - radio);
    int fin = std::min((int)texto.length(), pos + radio);
    
    std::string ctx = texto.substr(ini, fin - ini);
    // Reemplazar newlines por espacios para visualización
    std::replace(ctx.begin(), ctx.end(), '\n', ' ');
    std::replace(ctx.begin(), ctx.end(), '\r', ' ');
    
    return ctx;
}

Token Scanner::maximalMunch(int inicio) {
    if (inicio >= (int)texto.length()) {
        return Token(-1, "", inicio, inicio);
    }
    
    // Verificar si es espacio
    if (esEspacio(texto[inicio])) {
        int fin = inicio;
        while (fin < (int)texto.length() && esEspacio(texto[fin])) {
            fin++;
        }
        return Token(TOKEN_ESP, texto.substr(inicio, fin - inicio), inicio, fin - 1);
    }
    
    // Maximal Munch: buscar el lexema más largo aceptado
    int edoActual = afd->edoInicial;
    int ultimaAceptacion = -1;
    int posUltimaAceptacion = inicio;
    int tokenUltimaAceptacion = -1;
    
    int pos = inicio;
    
    while (pos < (int)texto.length()) {
        char c = texto[pos];
        
        // Intentar transición
        if (edoActual >= 0 && edoActual < afd->NumEdos) {
            int siguiente = afd->EdosAFD[edoActual]->TransAFD[(unsigned char)c];
            
            if (siguiente == -1) {
                // No hay transición, terminar
                break;
            }
            
            edoActual = siguiente;
            pos++;
            
            // Verificar si es estado de aceptación
            if (afd->EdosAFD[edoActual]->esAceptacion) {
                ultimaAceptacion = edoActual;
                posUltimaAceptacion = pos;
                tokenUltimaAceptacion = afd->EdosAFD[edoActual]->token;
            }
        } else {
            break;
        }
    }
    
    // Si encontramos alguna aceptación, retornar ese lexema
    if (ultimaAceptacion != -1) {
        std::string lexema = texto.substr(inicio, posUltimaAceptacion - inicio);
        return Token(tokenUltimaAceptacion, lexema, inicio, posUltimaAceptacion - 1);
    }
    
    // Error: ningún patrón reconoció el carácter
    return Token(-1, "", inicio, inicio);
}

bool Scanner::escanear() {
    tokens.clear();
    errores.clear();
    posicion = 0;
    
    if (!afd) {
        return false;
    }
    
    while (posicion < (int)texto.length()) {
        Token tok = maximalMunch(posicion);
        
        if (tok.tokenId == -1) {
            // Error léxico
            char c = texto[posicion];
            std::string ctx = obtenerContexto(posicion);
            errores.push_back(ErrorLexico(c, posicion, ctx));
            posicion++; // Saltar el carácter erróneo
        } else {
            // Token válido
            if (tok.tokenId == TOKEN_ESP && omitirEspacios) {
                // Omitir espacios si está configurado
            } else {
                tokens.push_back(tok);
            }
            posicion = tok.fin + 1;
        }
    }
    
    return errores.empty();
}

const std::vector<Token>& Scanner::getTokens() const {
    return tokens;
}

const std::vector<ErrorLexico>& Scanner::getErrores() const {
    return errores;
}

bool Scanner::tieneErrores() const {
    return !errores.empty();
}

std::string Scanner::toString() const {
    std::ostringstream oss;
    
    oss << "=== TOKENS ===" << std::endl;
    for (const auto& tok : tokens) {
        oss << "(token: " << tok.tokenId 
            << ", lexema: \"" << tok.lexema 
            << "\", pos: [" << tok.inicio << ", " << tok.fin << "])" 
            << std::endl;
    }
    
    if (!errores.empty()) {
        oss << "\n=== ERRORES LÉXICOS ===" << std::endl;
        for (const auto& err : errores) {
            oss << "Error en posición " << err.posicion 
                << ": carácter inesperado '" << err.caracter << "'" << std::endl;
            oss << "  Contexto: \"" << err.contexto << "\"" << std::endl;
        }
    }
    
    return oss.str();
}

void Scanner::limpiar() {
    tokens.clear();
    errores.clear();
    posicion = 0;
    texto.clear();
}