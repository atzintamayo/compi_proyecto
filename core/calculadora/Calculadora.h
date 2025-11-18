#ifndef CALCULADORA_H
#define CALCULADORA_H

#include <string>

class Calculadora {
public:
    Calculadora();
    ~Calculadora();

    // Evalúa una expresión en notación infija y devuelve el resultado.
    // Lanza std::runtime_error en caso de error de análisis/evaluación.
    double evaluar(const std::string& expresion);

    // Devuelve la representación en notación postfija (RPN) de la expresión.
    // Lanza std::runtime_error si la expresión es inválida.
    std::string obtenerNotacionPostfija(const std::string& expresion);

private:
    // (implementación en .cpp; la interfaz pública se mantiene simple)
};

#endif // CALCULADORA_H
