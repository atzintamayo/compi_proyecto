#ifndef GRAMATICA_LR0_H
#define GRAMATICA_LR0_H

#include <string>
#include <vector>
#include <set>

// Namespace para evitar conflictos
namespace LR0 {

// Tipos de símbolos para LR0
enum class TipoSimboloLR0 {
    TERMINAL,
    NO_TERMINAL
};

// Representa un símbolo de la gramática LR0
class SimboloLR0 {
public:
    std::string nombre;
    TipoSimboloLR0 tipo;
    
    SimboloLR0() : nombre(""), tipo(TipoSimboloLR0::TERMINAL) {}
    
    SimboloLR0(const std::string& nom, TipoSimboloLR0 t) 
        : nombre(nom), tipo(t) {}
    
    bool operator==(const SimboloLR0& otro) const {
        return nombre == otro.nombre && tipo == otro.tipo;
    }
    
    bool operator<(const SimboloLR0& otro) const {
        if (tipo != otro.tipo) return tipo < otro.tipo;
        return nombre < otro.nombre;
    }
};

// Representa una producción: A -> α
class ProduccionLR0 {
public:
    SimboloLR0 izquierda;
    std::vector<SimboloLR0> derecha;
    
    ProduccionLR0() {}
    
    std::string toString() const {
        std::string result = izquierda.nombre + " -> ";
        
        if (derecha.empty()) {
            result += "ε";
        } else {
            for (size_t i = 0; i < derecha.size(); i++) {
                result += derecha[i].nombre;
                if (i < derecha.size() - 1) {
                    result += " ";
                }
            }
        }
        
        return result;
    }
};

// Gramática completa para LR0
class GramaticaLR0 {
private:
    std::set<SimboloLR0> terminales;
    std::set<SimboloLR0> noTerminales;
    std::vector<ProduccionLR0> producciones;
    
public:
    GramaticaLR0() {}
    
    // Agregar una producción
    void agregarProduccion(const SimboloLR0& izq, const std::vector<SimboloLR0>& der) {
        ProduccionLR0 p;
        p.izquierda = izq;
        p.derecha = der;
        
        // Agregar automáticamente el no terminal de la izquierda
        noTerminales.insert(izq);
        
        // Agregar automáticamente los símbolos de la derecha
        for (const auto& simb : der) {
            if (simb.tipo == TipoSimboloLR0::TERMINAL) {
                terminales.insert(simb);
            } else {
                noTerminales.insert(simb);
            }
        }
        
        producciones.push_back(p);
    }
    
    // Limpiar la gramática
    void limpiar() {
        terminales.clear();
        noTerminales.clear();
        producciones.clear();
    }
    
    // Getters
    const std::vector<ProduccionLR0>& getProducciones() const {
        return producciones;
    }
    
    std::vector<SimboloLR0> getTerminales() const {
        return std::vector<SimboloLR0>(terminales.begin(), terminales.end());
    }
    
    std::vector<SimboloLR0> getNoTerminales() const {
        return std::vector<SimboloLR0>(noTerminales.begin(), noTerminales.end());
    }
    
    std::string toString() const {
        std::string result;
        for (size_t i = 0; i < producciones.size(); i++) {
            result += std::to_string(i) + ": " + producciones[i].toString() + "\n";
        }
        return result;
    }
};

} // namespace LR0

#endif // GRAMATICA_LR0_H