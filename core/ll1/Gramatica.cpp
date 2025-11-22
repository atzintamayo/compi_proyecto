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
    
    // PASO 1: Parsear todas las producciones y construir conjunto de LHS (No Terminales)
    std::set<std::string> lhs; // Lado izquierdo
    std::set<std::string> rhs; // Lado derecho
    std::vector<std::tuple<std::string, std::vector<std::string>>> produccionesTemp;
    
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
        
        // Eliminar ; final si existe
        if (!linea.empty() && linea.back() == ';') {
            linea.pop_back();
        }
        
        // Buscar ->
        size_t pos = linea.find("->");
        if (pos == std::string::npos) {
            continue;
        }
        
        // Extraer LHS
        std::string izqStr = linea.substr(0, pos);
        izqStr.erase(0, izqStr.find_first_not_of(" \t"));
        izqStr.erase(izqStr.find_last_not_of(" \t") + 1);
        
        if (izqStr.empty()) continue;
        
        lhs.insert(izqStr); // Marcar como No Terminal
        
        // Extraer RHS
        std::string derStr = linea.substr(pos + 2);
        derStr.erase(0, derStr.find_first_not_of(" \t"));
        derStr.erase(derStr.find_last_not_of(" \t") + 1);
        
        // Dividir por |
        std::istringstream derechaStream(derStr);
        std::string alternativa;
        
        while (std::getline(derechaStream, alternativa, '|')) {
            alternativa.erase(0, alternativa.find_first_not_of(" \t"));
            alternativa.erase(alternativa.find_last_not_of(" \t") + 1);
            
            std::vector<std::string> simbolos;
            
            // Tokenizar RHS
            if (alternativa != "epsilon" && alternativa != "ε" && alternativa != "e" && !alternativa.empty()) {
                std::istringstream ss(alternativa);
                std::string token;
                while (ss >> token) {
                    simbolos.push_back(token);
                    rhs.insert(token); // Agregar a RHS
                }
            }
            
            produccionesTemp.push_back({izqStr, simbolos});
        }
    }
    
    // PASO 2: Clasificar símbolos
    // No Terminales = LHS
    for (const auto& nt : lhs) {
        noTerminales.insert(Simbolo(nt, NO_TERMINAL));
    }
    
    // Terminales = RHS - LHS - {epsilon, ε, e}
    for (const auto& simbolo : rhs) {
        if (lhs.find(simbolo) == lhs.end() &&  // No está en LHS
            simbolo != "epsilon" && simbolo != "ε" && simbolo != "e") {
            terminales.insert(Simbolo(simbolo, TERMINAL));
        }
    }
    
    // PASO 3: Crear producciones con tipo correcto
    bool primeraProduccion = true;
    for (const auto& [izq, der] : produccionesTemp) {
        Simbolo izquierda(izq, NO_TERMINAL);
        
        if (primeraProduccion) {
            simboloInicial = izquierda;
            primeraProduccion = false;
        }
        
        std::vector<Simbolo> derecha;
        for (const auto& sim : der) {
            TipoSimbolo tipo = (lhs.find(sim) != lhs.end()) ? NO_TERMINAL : TERMINAL;
            derecha.push_back(Simbolo(sim, tipo));
        }
        
        agregarProduccion(izquierda, derecha);
    }
    
    return !producciones.empty();
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
        // El tipo se determina después en cargarDesdeTexto
        simbolos.push_back(Simbolo(token, TERMINAL));
    }
    
    return simbolos;
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