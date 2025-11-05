#ifndef CALCULADORA_H
#define CALCULADORA_H

#include <string>

class Calculadora {
public:
    Calculadora();
    ~Calculadora();
    
    double evaluar(const std::string& expresion);
    std::string obtenerNotacionPostfija(const std::string& expresion);
};

#endif // CALCULADORA_H