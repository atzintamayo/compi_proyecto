#include "TablaLL1.h"
#include <sstream>
#include <iomanip>

TablaLL1::TablaLL1(Gramatica* gram, ConjuntosLL1* conj) 
    : gramatica(gram), conjuntos(conj), hayConflictos(false) {}

TablaLL1::~TablaLL1() {}

bool TablaLL1::construir() {
    tabla.clear();
    conflictos.clear();
    hayConflictos = false;
    
    // Para cada producción A -> α
    for (const auto& prod : gramatica->getProducciones()) {
        std::string A = prod.izquierda.nombre;
        
        // Calcular FIRST(α)
        std::set<std::string> firstAlpha = conjuntos->getFirst(prod.derecha);
        
        // Para cada terminal a en FIRST(α) - {ε}
        for (const auto& a : firstAlpha) {
            if (a != "ε") {
                if (!agregarEntrada(A, a, prod)) {
                    hayConflictos = true;
                }
            }
        }
        
        // Si ε está en FIRST(α)
        if (firstAlpha.find("ε") != firstAlpha.end()) {
            // Para cada terminal b en FOLLOW(A)
            std::set<std::string> followA = conjuntos->getFollow(prod.izquierda);
            for (const auto& b : followA) {
                if (!agregarEntrada(A, b, prod)) {
                    hayConflictos = true;
                }
            }
        }
    }
    
    return !hayConflictos;
}

bool TablaLL1::agregarEntrada(const std::string& noTerminal, 
                               const std::string& terminal, 
                               const Produccion& produccion) {
    // Verificar si ya existe una entrada
    if (tabla[noTerminal][terminal].valida) {
        // Conflicto: ya hay una producción en esta celda
        std::ostringstream oss;
        oss << "Conflicto en [" << noTerminal << ", " << terminal << "]: "
            << "Ya existe " << tabla[noTerminal][terminal].produccion.toString()
            << ", intentando agregar " << produccion.toString();
        conflictos.push_back(oss.str());
        return false;
    }
    
    tabla[noTerminal][terminal] = EntradaTabla(produccion);
    return true;
}

EntradaTabla TablaLL1::getEntrada(const std::string& noTerminal, 
                                   const std::string& terminal) const {
    auto itNT = tabla.find(noTerminal);
    if (itNT != tabla.end()) {
        auto itT = itNT->second.find(terminal);
        if (itT != itNT->second.end()) {
            return itT->second;
        }
    }
    return EntradaTabla();
}

std::vector<std::string> TablaLL1::getTerminales() const {
    std::vector<std::string> terminales;
    
    for (const auto& terminal : gramatica->getTerminales()) {
        terminales.push_back(terminal.nombre);
    }
    
    // Agregar $ si no está
    bool tieneFin = false;
    for (const auto& t : terminales) {
        if (t == "$") {
            tieneFin = true;
            break;
        }
    }
    if (!tieneFin) {
        terminales.push_back("$");
    }
    
    return terminales;
}

std::vector<std::string> TablaLL1::getNoTerminales() const {
    std::vector<std::string> noTerminales;
    
    for (const auto& nt : gramatica->getNoTerminales()) {
        noTerminales.push_back(nt.nombre);
    }
    
    return noTerminales;
}

std::string TablaLL1::toString() const {
    std::ostringstream oss;
    
    std::vector<std::string> terminales = getTerminales();
    std::vector<std::string> noTerminales = getNoTerminales();
    
    // Encabezado
    oss << std::setw(15) << " ";
    for (const auto& terminal : terminales) {
        oss << std::setw(20) << terminal;
    }
    oss << "\n";
    
    oss << std::string(15 + terminales.size() * 20, '-') << "\n";
    
    // Filas
    for (const auto& noTerminal : noTerminales) {
        oss << std::setw(15) << noTerminal;
        
        for (const auto& terminal : terminales) {
            EntradaTabla entrada = getEntrada(noTerminal, terminal);
            
            if (entrada.valida) {
                std::string prod = entrada.produccion.toString();
                if (prod.length() > 18) {
                    prod = prod.substr(0, 15) + "...";
                }
                oss << std::setw(20) << prod;
            } else {
                oss << std::setw(20) << "-";
            }
        }
        
        oss << "\n";
    }
    
    return oss.str();
}