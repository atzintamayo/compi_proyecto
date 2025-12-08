#include "Calculadora.h"

#include <string>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <sstream>

// ============================================================================
//  CALCULADORA CON RECURSIVE DESCENT PARSER
//  Gramática con recursión por la izquierda:
//  
//  E → E + T | E - T | T
//  T → T * P | T / P | P
//  P → P ^ F | F
//  F → (E) | FUNC(E) | NUMBER
//
//  Transformada a:
//  E  → T E'
//  E' → + T E' | - T E' | ε
//  T  → P T'
//  T' → * P T' | / P T' | ε
//  P  → F P'
//  P' → ^ F P' | ε
//  F  → (E) | FUNC(E) | NUMBER
// ============================================================================

class RecursiveParser {
private:
    std::string input;
    size_t pos;
    std::string rpnOutput;  // Notación postfija generada
    
    // ===================
    //  UTILIDADES
    // ===================
    
    void skipSpaces() {
        while (pos < input.size() && std::isspace((unsigned char)input[pos])) 
            ++pos;
    }
    
    char peek() {
        skipSpaces();
        return (pos < input.size()) ? input[pos] : '\0';
    }
    
    char get() {
        skipSpaces();
        if (pos >= input.size()) 
            throw std::runtime_error("Fin inesperado de expresión");
        return input[pos++];
    }
    
    void appendToRPN(const std::string& token) {
        if (!rpnOutput.empty()) rpnOutput += " ";
        rpnOutput += token;
    }
    
    std::string toLower(const std::string& s) {
        std::string result = s;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    // ===================
    //  GRAMÁTICA RECURSIVA
    // ===================
    
    // E → T E'
    double parseE() {
        double left = parseT();
        return parseEPrime(left);
    }
    
    // E' → + T E' | - T E' | ε
    // Elimina recursión izquierda de: E → E + T | E - T
    double parseEPrime(double left) {
        while (true) {
            char op = peek();
            if (op != '+' && op != '-') break;
            
            get(); // consumir operador
            double right = parseT();
            
            // RPN: operador después de operandos
            appendToRPN(std::string(1, op));
            
            if (op == '+') 
                left = left + right;
            else 
                left = left - right;
        }
        return left;
    }
    
    // T → P T'
    double parseT() {
        double left = parseP();
        return parseTPrime(left);
    }
    
    // T' → * P T' | / P T' | ε
    // Elimina recursión izquierda de: T → T * P | T / P
    double parseTPrime(double left) {
        while (true) {
            char op = peek();
            if (op != '*' && op != '/') break;
            
            get(); // consumir operador
            double right = parseP();
            
            // RPN: operador después de operandos
            appendToRPN(std::string(1, op));
            
            if (op == '*') {
                left = left * right;
            } else {
                if (right == 0.0) 
                    throw std::runtime_error("División por cero");
                left = left / right;
            }
        }
        return left;
    }
    
    // P → F P'
    double parseP() {
        double left = parseF();
        return parsePPrime(left);
    }
    
    // P' → ^ F P' | ε
    // Elimina recursión izquierda de: P → P ^ F
    // NOTA: ^ es asociativa DERECHA, así que usamos recursión derecha
    double parsePPrime(double base) {
        if (peek() == '^') {
            get(); // consumir '^'
            
            // Recursión DERECHA para mantener asociatividad: 2^3^4 = 2^(3^4)
            double exponent = parseP(); // Llamada recursiva a P (no a F)
            
            // RPN: operador después de operandos
            appendToRPN("^");
            
            return std::pow(base, exponent);
        }
        return base;
    }
    
    // F → (E) | FUNC(E) | NUMBER
    double parseF() {
        char c = peek();
        
        // ---------------
        // PARÉNTESIS: (E)
        // ---------------
        if (c == '(') {
            get(); // consumir '('
            double value = parseE(); // RECURSIÓN a E
            if (peek() != ')') 
                throw std::runtime_error("Se esperaba ')'");
            get(); // consumir ')'
            // Los paréntesis NO aparecen en RPN
            return value;
        }
        
        // ---------------
        // FUNCIÓN: FUNC(E) o NÚMERO
        // ---------------
        if (std::isalpha((unsigned char)c) || c == '_') {
            return parseFunctionOrConstant();
        }
        
        // ---------------
        // NÚMERO
        // ---------------
        if (std::isdigit((unsigned char)c) || c == '.') {
            double num = parseNumber();
            
            // Agregar número a RPN
            std::ostringstream oss;
            oss << num;
            appendToRPN(oss.str());
            
            return num;
        }
        
        // ---------------
        // MENOS UNARIO (extensión)
        // Lo manejamos como 0 - F
        // ---------------
        if (c == '-') {
            get(); // consumir '-'
            appendToRPN("0");
            double value = parseF(); // RECURSIÓN a F
            appendToRPN("-");
            return -value;
        }
        
        // ---------------
        // MÁS UNARIO (extensión)
        // ---------------
        if (c == '+') {
            get(); // consumir '+'
            return parseF(); // RECURSIÓN a F
        }
        
        throw std::runtime_error(std::string("Se esperaba número, función o '(', encontrado: '") + c + "'");
    }
    
    // Parsea un número (entero, decimal, científico)
    double parseNumber() {
        size_t start = pos;
        bool hasDot = false;
        
        // Parte entera y decimal
        while (pos < input.size()) {
            char c = input[pos];
            if (std::isdigit((unsigned char)c)) {
                ++pos;
            } else if (c == '.' && !hasDot) {
                hasDot = true;
                ++pos;
            } else {
                break;
            }
        }
        
        // Notación científica: 1.5e-3
        if (pos < input.size() && (input[pos] == 'e' || input[pos] == 'E')) {
            ++pos;
            if (pos < input.size() && (input[pos] == '+' || input[pos] == '-')) {
                ++pos;
            }
            if (pos < input.size() && std::isdigit((unsigned char)input[pos])) {
                while (pos < input.size() && std::isdigit((unsigned char)input[pos])) {
                    ++pos;
                }
            } else {
                throw std::runtime_error("Exponente inválido en notación científica");
            }
        }
        
        std::string numStr = input.substr(start, pos - start);
        try {
            return std::stod(numStr);
        } catch (...) {
            throw std::runtime_error("Número inválido: " + numStr);
        }
    }
    
    // Parsea funciones o constantes (pi)
    double parseFunctionOrConstant() {
        size_t start = pos;
        
        // Leer identificador
        while (pos < input.size() && 
               (std::isalnum((unsigned char)input[pos]) || input[pos] == '_')) {
            ++pos;
        }
        
        std::string name = input.substr(start, pos - start);
        std::string nameLower = toLower(name);
        
        // Constante PI (sin paréntesis)
        if (nameLower == "pi") {
            appendToRPN("pi");
            return M_PI;
        }
        
        // Funciones requieren paréntesis: FUNC(E)
        skipSpaces();
        if (peek() != '(') {
            throw std::runtime_error("La función '" + name + "' requiere paréntesis");
        }
        
        get(); // consumir '('
        double arg = parseE(); // RECURSIÓN a E
        
        if (peek() != ')') {
            throw std::runtime_error("Se esperaba ')' después del argumento de " + name);
        }
        get(); // consumir ')'
        
        // Agregar función a RPN DESPUÉS del argumento
        appendToRPN(nameLower);
        
        return applyFunction(nameLower, arg);
    }
    
    // Aplica la función matemática
    double applyFunction(const std::string& fname, double arg) {
        // Funciones trigonométricas (entrada en grados)
        double rad = arg * M_PI / 180.0;
        
        // Funciones trigonométricas directas
        if (fname == "sin" || fname == "sen") 
            return std::sin(rad);
        
        if (fname == "cos") 
            return std::cos(rad);
        
        if (fname == "tan") 
            return std::tan(rad);
        
        // Funciones trigonométricas inversas
        if (fname == "asin" || fname == "arcsin" || fname == "arcoseno") {
            if (arg < -1.0 || arg > 1.0)
                throw std::runtime_error("asin: argumento debe estar en [-1, 1]");
            return std::asin(arg) * 180.0 / M_PI; // resultado en grados
        }
        
        if (fname == "acos" || fname == "arccos" || fname == "arcocoseno") {
            if (arg < -1.0 || arg > 1.0)
                throw std::runtime_error("acos: argumento debe estar en [-1, 1]");
            return std::acos(arg) * 180.0 / M_PI; // resultado en grados
        }
        
        if (fname == "atan" || fname == "arctan" || fname == "arctangente") 
            return std::atan(arg) * 180.0 / M_PI; // resultado en grados
        
        // Logaritmos
        if (fname == "log") {
            if (arg <= 0.0) 
                throw std::runtime_error("log: argumento debe ser positivo");
            return std::log10(arg);
        }
        
        if (fname == "ln") {
            if (arg <= 0.0) 
                throw std::runtime_error("ln: argumento debe ser positivo");
            return std::log(arg);
        }
        
        // Exponencial
        if (fname == "exp") 
            return std::exp(arg);
        
        // Raíz cuadrada (extensión)
        if (fname == "sqrt" || fname == "raiz") {
            if (arg < 0.0) 
                throw std::runtime_error("sqrt: argumento no puede ser negativo");
            return std::sqrt(arg);
        }
        
        // Valor absoluto (extensión)
        if (fname == "abs") 
            return std::abs(arg);
        
        throw std::runtime_error("Función desconocida: " + fname);
    }

public:
    RecursiveParser() : pos(0) {}
    
    double parse(const std::string& expression) {
        input = expression;
        pos = 0;
        rpnOutput.clear();
        
        if (input.empty()) {
            throw std::runtime_error("Expresión vacía");
        }
        
        double result = parseE(); // Inicia desde E (la expresión raíz)
        
        skipSpaces();
        if (pos < input.size()) {
            throw std::runtime_error("Caracteres adicionales al final de la expresión");
        }
        
        return result;
    }
    
    std::string getRPN() const {
        return rpnOutput;
    }
};

// ============================================================================
//  IMPLEMENTACIÓN DE LA CLASE CALCULADORA
// ============================================================================

Calculadora::Calculadora() {}
Calculadora::~Calculadora() {}

double Calculadora::evaluar(const std::string& expresion) {
    RecursiveParser parser;
    return parser.parse(expresion);
}

std::string Calculadora::obtenerNotacionPostfija(const std::string& expresion) {
    RecursiveParser parser;
    try {
        parser.parse(expresion);
        return parser.getRPN();
    } catch (const std::exception& e) {
        throw; // Re-lanzar la excepción
    }
}
