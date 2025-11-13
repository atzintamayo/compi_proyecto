#include "Gramatica.h"
#include <sstream>
#include <algorithm>
#include <cctype>

std::string Produccion::toString() const {
    std::string resultado = izquierda.nombre + " -> ";
    
    if (derecha.empty()) {
        resultado += "ε";
    } else {
        for (size_t i = 0; i < derecha.size(); i++) {
            resultado += derecha[i].nombre;
            if (i < derecha.size() - 1) {
                resultado += " ";
            }
        }
    }
    
    return resultado;
}

Gramatica::Gramatica() : contadorProducciones(0) {}

Gramatica::~Gramatica() {}

void Gramatica::limpiar() {
    producciones.clear();
    noTerminales.clear();
    terminales.clear();
    simboloInicial = Simbolo();
    contadorProducciones = 0;
}

bool Gramatica::cargarDesdeTexto(const std::string& texto) {
    limpiar();
    
    std::istringstream stream(texto);
    std::string linea;
    
    while (std::getline(stream, linea)) {
        // Eliminar espacios al inicio y final
        linea.erase(0, linea.find_first_not_of(" \t\r\n"));
        linea.erase(linea.find_last_not_of(" \t\r\n") + 1);
        
        // Saltar líneas vacías y comentarios
        if (linea.empty() || linea[0] == '#') {
            continue;
        }
        
        if (!parsearProduccion(linea)) {
            limpiar();
            return false;
        }
    }
    
    return !producciones.empty();
}

bool Gramatica::parsearProduccion(const std::string& linea) {
    // Formato: A -> α | β | γ ;
    // Eliminar el punto y coma final si existe
    std::string lineaLimpia = linea;
    if (!lineaLimpia.empty() && lineaLimpia.back() == ';') {
        lineaLimpia.pop_back();
    }
    
    // Buscar el ->
    size_t pos = lineaLimpia.find("->");
    if (pos == std::string::npos) {
        return false;
    }
    
    // Extraer lado izquierdo
    std::string izqStr = lineaLimpia.substr(0, pos);
    izqStr.erase(0, izqStr.find_first_not_of(" \t"));
    izqStr.erase(izqStr.find_last_not_of(" \t") + 1);
    
    if (izqStr.empty()) {
        return false;
    }
    
    Simbolo izquierda(izqStr, NO_TERMINAL);
    noTerminales.insert(izquierda);
    
    // Si es la primera producción, es el símbolo inicial
    if (simboloInicial.nombre.empty()) {
        simboloInicial = izquierda;
    }
    
    // Extraer lado derecho
    std::string derStr = lineaLimpia.substr(pos + 2);
    derStr.erase(0, derStr.find_first_not_of(" \t"));
    derStr.erase(derStr.find_last_not_of(" \t") + 1);
    
    // Dividir por |
    std::istringstream derechaStream(derStr);
    std::string alternativa;
    
    while (std::getline(derechaStream, alternativa, '|')) {
        alternativa.erase(0, alternativa.find_first_not_of(" \t"));
        alternativa.erase(alternativa.find_last_not_of(" \t") + 1);
        
        std::vector<Simbolo> derecha = tokenizarDerecha(alternativa);
        agregarProduccion(izquierda, derecha);
    }
    
    return true;
}

std::vector<Simbolo> Gramatica::tokenizarDerecha(const std::string& derecha) {
    std::vector<Simbolo> simbolos;
    
    // Caso especial: epsilon
    if (derecha == "epsilon" || derecha == "ε" || derecha.empty()) {
        return simbolos; // Producción vacía representa epsilon
    }
    
    std::istringstream stream(derecha);
    std::string token;
    
    while (stream >> token) {
        TipoSimbolo tipo = identificarTipo(token);
        Simbolo sim(token, tipo);
        
        simbolos.push_back(sim);
        
        if (tipo == TERMINAL) {
            terminales.insert(sim);
        } else {
            noTerminales.insert(sim);
        }
    }
    
    return simbolos;
}

TipoSimbolo Gramatica::identificarTipo(const std::string& nombre) {
    // Convención: 
    // - Mayúscula inicial o todo mayúsculas = No terminal
    // - Minúscula o símbolos = Terminal
    
    if (nombre.empty()) return TERMINAL;
    
    // Si empieza con mayúscula, es no terminal
    if (std::isupper(nombre[0])) {
        return NO_TERMINAL;
    }
    
    return TERMINAL;
}

void Gramatica::agregarProduccion(const Simbolo& izq, const std::vector<Simbolo>& der) {
    Produccion prod(izq, der, contadorProducciones++);
    producciones.push_back(prod);
}

std::vector<Produccion> Gramatica::produccionesDe(const Simbolo& noTerminal) const {
    std::vector<Produccion> resultado;
    
    for (const auto& prod : producciones) {
        if (prod.izquierda == noTerminal) {
            resultado.push_back(prod);
        }
    }
    
    return resultado;
}

bool Gramatica::esNoTerminal(const std::string& nombre) const {
    Simbolo sim(nombre, NO_TERMINAL);
    return noTerminales.find(sim) != noTerminales.end();
}

bool Gramatica::esTerminal(const std::string& nombre) const {
    Simbolo sim(nombre, TERMINAL);
    return terminales.find(sim) != terminales.end();
}

bool Gramatica::esEpsilon(const std::string& nombre) const {
    return nombre == "epsilon" || nombre == "ε" || nombre.empty();
}

std::string Gramatica::toString() const {
    std::string resultado;
    
    for (const auto& prod : producciones) {
        resultado += prod.toString() + "\n";
    }
    
    return resultado;
}