#include "ConjuntosLL1.h"
#include <sstream>

ConjuntosLL1::ConjuntosLL1(Gramatica* gram) : gramatica(gram) {}

ConjuntosLL1::~ConjuntosLL1() {}

void ConjuntosLL1::calcular() {
    conjuntoFirst.clear();
    conjuntoFollow.clear();
    derivanEpsilon.clear();
    
    identificarDerivacionEpsilon();
    calcularFirst();
    calcularFollow();
}

void ConjuntosLL1::identificarDerivacionEpsilon() {
    bool cambio = true;
    
    while (cambio) {
        cambio = false;
        
        for (const auto& prod : gramatica->getProducciones()) {
            // Si el lado izquierdo ya deriva epsilon, continuar
            if (derivanEpsilon.find(prod.izquierda.nombre) != derivanEpsilon.end()) {
                continue;
            }
            
            // Si el lado derecho está vacío (epsilon)
            if (prod.derecha.empty()) {
                derivanEpsilon.insert(prod.izquierda.nombre);
                cambio = true;
                continue;
            }
            
            // Si todos los símbolos del lado derecho derivan epsilon
            bool todosDerivanEpsilon = true;
            for (const auto& sim : prod.derecha) {
                if (sim.tipo == TERMINAL || 
                    derivanEpsilon.find(sim.nombre) == derivanEpsilon.end()) {
                    todosDerivanEpsilon = false;
                    break;
                }
            }
            
            if (todosDerivanEpsilon) {
                derivanEpsilon.insert(prod.izquierda.nombre);
                cambio = true;
            }
        }
    }
}

bool ConjuntosLL1::derivaEpsilon(const Simbolo& simbolo) const {
    if (simbolo.tipo == TERMINAL) return false;
    return derivanEpsilon.find(simbolo.nombre) != derivanEpsilon.end();
}

void ConjuntosLL1::calcularFirst() {
    // Inicializar FIRST de terminales
    for (const auto& terminal : gramatica->getTerminales()) {
        conjuntoFirst[terminal.nombre].insert(terminal.nombre);
    }
    
    // Algoritmo iterativo de punto fijo
    bool cambio = true;
    
    while (cambio) {
        cambio = false;
        
        for (const auto& prod : gramatica->getProducciones()) {
            std::string A = prod.izquierda.nombre;
            
            // Si A -> ε
            if (prod.derecha.empty()) {
                if (agregarFirst(A, "ε")) {
                    cambio = true;
                }
                continue;
            }
            
            // A -> X1 X2 ... Xn
            bool todosDerivanEpsilon = true;
            
            for (const auto& X : prod.derecha) {
                // Agregar FIRST(Xi) - {ε} a FIRST(A)
                for (const auto& simbolo : getFirst(X)) {
                    if (simbolo != "ε") {
                        if (agregarFirst(A, simbolo)) {
                            cambio = true;
                        }
                    }
                }
                
                // Si Xi no deriva ε, parar
                if (!derivaEpsilon(X)) {
                    todosDerivanEpsilon = false;
                    break;
                }
            }
            
            // Si todos derivan ε, agregar ε a FIRST(A)
            if (todosDerivanEpsilon) {
                if (agregarFirst(A, "ε")) {
                    cambio = true;
                }
            }
        }
    }
}

void ConjuntosLL1::calcularFollow() {
    // FOLLOW del símbolo inicial contiene $
    Simbolo inicial = gramatica->getSimboloInicial();
    conjuntoFollow[inicial.nombre].insert("$");
    
    // Algoritmo iterativo de punto fijo
    bool cambio = true;
    
    while (cambio) {
        cambio = false;
        
        for (const auto& prod : gramatica->getProducciones()) {
            std::string A = prod.izquierda.nombre;
            
            // Para cada símbolo en el lado derecho
            for (size_t i = 0; i < prod.derecha.size(); i++) {
                const Simbolo& B = prod.derecha[i];
                
                // Solo calculamos FOLLOW para no terminales
                if (B.tipo == TERMINAL) continue;
                
                // β = X(i+1) ... Xn
                std::vector<Simbolo> beta(prod.derecha.begin() + i + 1, prod.derecha.end());
                
                // Agregar FIRST(β) - {ε} a FOLLOW(B)
                std::set<std::string> firstBeta = getFirst(beta);
                for (const auto& simbolo : firstBeta) {
                    if (simbolo != "ε") {
                        if (agregarFollow(B.nombre, simbolo)) {
                            cambio = true;
                        }
                    }
                }
                
                // Si β deriva ε (o está vacío), agregar FOLLOW(A) a FOLLOW(B)
                bool betaDerivaEpsilon = beta.empty() || 
                    (firstBeta.find("ε") != firstBeta.end());
                
                if (betaDerivaEpsilon) {
                    for (const auto& simbolo : conjuntoFollow[A]) {
                        if (agregarFollow(B.nombre, simbolo)) {
                            cambio = true;
                        }
                    }
                }
            }
        }
    }
}

bool ConjuntosLL1::agregarFirst(const std::string& simbolo, const std::string& terminal) {
    auto& conjunto = conjuntoFirst[simbolo];
    if (conjunto.find(terminal) == conjunto.end()) {
        conjunto.insert(terminal);
        return true;
    }
    return false;
}

bool ConjuntosLL1::agregarFollow(const std::string& simbolo, const std::string& terminal) {
    auto& conjunto = conjuntoFollow[simbolo];
    if (conjunto.find(terminal) == conjunto.end()) {
        conjunto.insert(terminal);
        return true;
    }
    return false;
}

std::set<std::string> ConjuntosLL1::getFirst(const Simbolo& simbolo) const {
    auto it = conjuntoFirst.find(simbolo.nombre);
    if (it != conjuntoFirst.end()) {
        return it->second;
    }
    return std::set<std::string>();
}

std::set<std::string> ConjuntosLL1::getFirst(const std::vector<Simbolo>& cadena) const {
    std::set<std::string> resultado;
    
    if (cadena.empty()) {
        resultado.insert("ε");
        return resultado;
    }
    
    bool todosDerivanEpsilon = true;
    
    for (const auto& X : cadena) {
        std::set<std::string> firstX = getFirst(X);
        
        // Agregar FIRST(X) - {ε}
        for (const auto& sim : firstX) {
            if (sim != "ε") {
                resultado.insert(sim);
            }
        }
        
        // Si X no deriva ε, parar
        if (!derivaEpsilon(X)) {
            todosDerivanEpsilon = false;
            break;
        }
    }
    
    if (todosDerivanEpsilon) {
        resultado.insert("ε");
    }
    
    return resultado;
}

std::set<std::string> ConjuntosLL1::getFollow(const Simbolo& noTerminal) const {
    auto it = conjuntoFollow.find(noTerminal.nombre);
    if (it != conjuntoFollow.end()) {
        return it->second;
    }
    return std::set<std::string>();
}

std::string ConjuntosLL1::firstToString() const {
    std::ostringstream oss;
    
    for (const auto& par : conjuntoFirst) {
        oss << "FIRST(" << par.first << ") = { ";
        bool primero = true;
        for (const auto& sim : par.second) {
            if (!primero) oss << ", ";
            oss << sim;
            primero = false;
        }
        oss << " }\n";
    }
    
    return oss.str();
}

std::string ConjuntosLL1::followToString() const {
    std::ostringstream oss;
    
    for (const auto& par : conjuntoFollow) {
        oss << "FOLLOW(" << par.first << ") = { ";
        bool primero = true;
        for (const auto& sim : par.second) {
            if (!primero) oss << ", ";
            oss << sim;
            primero = false;
        }
        oss << " }\n";
    }
    
    return oss.str();
}