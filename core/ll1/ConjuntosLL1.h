#ifndef CONJUNTOSLL1_H
#define CONJUNTOSLL1_H

#include "Gramatica.h"
#include <map>
#include <set>
#include <string>

// Clase para calcular conjuntos FIRST y FOLLOW
class ConjuntosLL1 {
public:
    ConjuntosLL1(Gramatica* gramatica);
    ~ConjuntosLL1();
    
    // Calcular FIRST y FOLLOW
    void calcular();
    
    // Obtener FIRST de un símbolo
    std::set<std::string> getFirst(const Simbolo& simbolo) const;
    
    // Obtener FIRST de una cadena de símbolos
    std::set<std::string> getFirst(const std::vector<Simbolo>& cadena) const;
    
    // Obtener FOLLOW de un no terminal
    std::set<std::string> getFollow(const Simbolo& noTerminal) const;
    
    // Verificar si un símbolo deriva epsilon
    bool derivaEpsilon(const Simbolo& simbolo) const;
    
    // Convertir a string para mostrar
    std::string firstToString() const;
    std::string followToString() const;
    
private:
    Gramatica* gramatica;
    
    // Conjuntos FIRST y FOLLOW
    std::map<std::string, std::set<std::string>> conjuntoFirst;
    std::map<std::string, std::set<std::string>> conjuntoFollow;
    
    // Símbolos que derivan epsilon
    std::set<std::string> derivanEpsilon;
    
    // Métodos auxiliares
    void calcularFirst();
    void calcularFollow();
    void identificarDerivacionEpsilon();
    
    bool agregarFirst(const std::string& simbolo, const std::string& terminal);
    bool agregarFollow(const std::string& simbolo, const std::string& terminal);
};

#endif // CONJUNTOSLL1_H