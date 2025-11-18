#include "Calculadora.h"

#include <string>
#include <vector>
#include <cctype>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <stack>

namespace {

enum class TokenType {
    PLUS, MINUS, MUL, DIV, POW,
    LPAREN, RPAREN,
    FUNC, NUMBER,
    END
};

struct Token {
    TokenType type;
    double value;         // válido si type == NUMBER
    std::string name;     // válido si type == FUNC
    Token(TokenType t = TokenType::END, double v = 0.0, const std::string& n = "")
        : type(t), value(v), name(n) {}
};

static std::string toLower(const std::string &s) {
    std::string r(s);
    std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c){ return std::tolower(c); });
    return r;
}

static bool isIdentifierChar(char c) {
    return std::isalpha((unsigned char)c) || c == '_';
}

} // unnamed

// ----------------- IMPLEMENTACIÓN DE LA CALCULADORA -----------------

Calculadora::Calculadora() {}
Calculadora::~Calculadora() {}

static bool isFuncName(const std::string &s) {
    static const std::vector<std::string> funcs = {
        "sin","sen","cos","tan","asin","acos","atan","log","ln","exp","pi"
    };
    return std::find(funcs.begin(), funcs.end(), s) != funcs.end();
}

static double applyFunction(const std::string &fname, double arg) {
    if (fname == "sin" || fname == "sen") return std::sin(arg);
    if (fname == "cos") return std::cos(arg);
    if (fname == "tan") return std::tan(arg);
    if (fname == "asin") return std::asin(arg);
    if (fname == "acos") return std::acos(arg);
    if (fname == "atan") return std::atan(arg);
    if (fname == "log") { // log base 10
        if (arg <= 0.0) throw std::runtime_error("log: argumento no positivo");
        return std::log10(arg);
    }
    if (fname == "ln") {
        if (arg <= 0.0) throw std::runtime_error("ln: argumento no positivo");
        return std::log(arg);
    }
    if (fname == "exp") return std::exp(arg);
    if (fname == "pi") return M_PI;
    throw std::runtime_error("Función no implementada: " + fname);
}

// LEXER: generador de tokens desde la entrada
class Lexer {
public:
    Lexer(const std::string &s) : input(s), pos(0) {}

    Token next() {
        skipSpaces();
        if (pos >= input.size()) return Token(TokenType::END);

        char c = input[pos];

        // operadores y paréntesis
        if (c == '+') { ++pos; return Token(TokenType::PLUS); }
        if (c == '-') { ++pos; return Token(TokenType::MINUS); }
        if (c == '*') { ++pos; return Token(TokenType::MUL); }
        if (c == '/') { ++pos; return Token(TokenType::DIV); }
        if (c == '^') { ++pos; return Token(TokenType::POW); }
        if (c == '(') { ++pos; return Token(TokenType::LPAREN); }
        if (c == ')') { ++pos; return Token(TokenType::RPAREN); }

        // número (entero o flotante, posible notación científica)
        if (std::isdigit((unsigned char)c) || (c=='.' && pos+1 < input.size() && std::isdigit((unsigned char)input[pos+1]))) {
            size_t start = pos;
            bool hasDot = false;
            while (pos < input.size()) {
                char d = input[pos];
                if (std::isdigit((unsigned char)d)) { ++pos; continue; }
                if (d == '.' && !hasDot) { hasDot = true; ++pos; continue; }
                // permitir notación exponent (e.g. 1.2e-3)
                if ((d == 'e' || d == 'E') && pos+1 < input.size()) {
                    size_t save = pos;
                    ++pos;
                    if (input[pos] == '+' || input[pos] == '-') ++pos;
                    if (pos < input.size() && std::isdigit((unsigned char)input[pos])) {
                        while (pos < input.size() && std::isdigit((unsigned char)input[pos])) ++pos;
                        continue;
                    } else {
                        pos = save; // no es exponente, revertir
                    }
                }
                break;
            }
            std::string numstr = input.substr(start, pos - start);
            std::istringstream iss(numstr);
            double val;
            iss >> val;
            if (iss.fail()) throw std::runtime_error("Número inválido: " + numstr);
            return Token(TokenType::NUMBER, val);
        }

        // identificador / función
        if (isIdentifierChar(c)) {
            size_t start = pos;
            while (pos < input.size() && (isIdentifierChar(input[pos]) || std::isdigit((unsigned char)input[pos]))) ++pos;
            std::string name = input.substr(start, pos - start);
            std::string lname = toLower(name);
            if (isFuncName(lname)) return Token(TokenType::FUNC, 0.0, lname);
            throw std::runtime_error("Identificador desconocido: " + name);
        }

        throw std::runtime_error(std::string("Carácter inesperado: '") + c + "'");
    }

    // permite mirar siguiente token sin consumir (peek simple)
    Token peek() {
        size_t save = pos;
        Token t = next();
        pos = save;
        return t;
    }

private:
    void skipSpaces() {
        while (pos < input.size() && std::isspace((unsigned char)input[pos])) ++pos;
    }
    std::string input;
    size_t pos;
};

// Precedencia: mayor número = mayor precedencia
static int precedence(TokenType t) {
    switch (t) {
        case TokenType::PLUS: case TokenType::MINUS: return 1;
        case TokenType::MUL: case TokenType::DIV: return 2;
        case TokenType::POW: return 4;
        default: return 0;
    }
}
static bool isRightAssoc(TokenType t) {
    return t == TokenType::POW;
}

// Convierte infijo -> postfijo (vector<Token>)
static std::vector<Token> infixToPostfixInternal(const std::string &expr) {
    Lexer lexer(expr);
    std::vector<Token> output;
    std::vector<Token> opstack;

    // Para detectar unario: al inicio o después de operador o '(' es un lugar unario
    bool prevWasOperator = true;

    while (true) {
        Token tok = lexer.next();
        if (tok.type == TokenType::END) break;

        if (tok.type == TokenType::NUMBER) {
            output.push_back(tok);
            prevWasOperator = false;
            continue;
        }

        if (tok.type == TokenType::FUNC) {
            // empujar función a la pila de operadores
            opstack.push_back(tok);
            prevWasOperator = false; // una función seguida de '(' normalmente; pero ponemos false para evitar lo que sería un unary justo despues
            continue;
        }

        if (tok.type == TokenType::LPAREN) {
            opstack.push_back(tok);
            prevWasOperator = true;
            continue;
        }

        if (tok.type == TokenType::RPAREN) {
            bool found = false;
            while (!opstack.empty()) {
                Token top = opstack.back(); opstack.pop_back();
                if (top.type == TokenType::LPAREN) { found = true; break; }
                output.push_back(top);
            }
            if (!found) throw std::runtime_error("Paréntesis derecho sin correspondiente izquierdo");
            // si hay una función en tope, popearla a output (funcion(E) -> E func)
            if (!opstack.empty() && opstack.back().type == TokenType::FUNC) {
                output.push_back(opstack.back());
                opstack.pop_back();
            }
            prevWasOperator = false;
            continue;
        }

        // Detectar MINUS unario: si prevWasOperator == true y tok == MINUS, lo convertimos a (0 - ...)
        if (tok.type == TokenType::MINUS && prevWasOperator) {
            // insertar un 0 en salida antes del operador - para tratarlo como binario
            output.emplace_back(TokenType::NUMBER, 0.0);
            // ahora seguimos como si fuese un MINUS binario normal
        }

        // Si es operador binario
        if (tok.type == TokenType::PLUS || tok.type == TokenType::MINUS ||
            tok.type == TokenType::MUL || tok.type == TokenType::DIV ||
            tok.type == TokenType::POW) {

            while (!opstack.empty()) {
                Token top = opstack.back();
                if (top.type == TokenType::LPAREN) break;
                if (top.type == TokenType::FUNC) {
                    // funciones esperan hasta encontrar RPAREN — las dejamos
                    break;
                }
                int pTop = precedence(top.type);
                int pTok = precedence(tok.type);
                if ( (isRightAssoc(tok.type) && pTok < pTop) ||
                     (!isRightAssoc(tok.type) && pTok <= pTop) ) {
                    opstack.pop_back();
                    output.push_back(top);
                } else break;
            }
            opstack.push_back(tok);
            prevWasOperator = true;
            continue;
        }

        // token inesperado
        throw std::runtime_error("Token inesperado durante conversión a postfijo");
    }

    while (!opstack.empty()) {
        Token t = opstack.back(); opstack.pop_back();
        if (t.type == TokenType::LPAREN || t.type == TokenType::RPAREN) throw std::runtime_error("Paréntesis desbalanceados");
        output.push_back(t);
    }
    return output;
}

// Convierte vector<Token> (postfijo) a string legible RPN
static std::string rpnToString(const std::vector<Token> &rpn) {
    std::ostringstream oss;
    bool first = true;
    for (const Token &t : rpn) {
        if (!first) oss << ' ';
        first = false;
        switch (t.type) {
            case TokenType::NUMBER:
                {
                    std::ostringstream tmp;
                    tmp << t.value;
                    oss << tmp.str();
                }
                break;
            case TokenType::PLUS: oss << "+"; break;
            case TokenType::MINUS: oss << "-"; break;
            case TokenType::MUL: oss << "*"; break;
            case TokenType::DIV: oss << "/"; break;
            case TokenType::POW: oss << "^"; break;
            case TokenType::FUNC: oss << t.name; break;
            default: break;
        }
    }
    return oss.str();
}

// Evalúa RPN y devuelve resultado
static double evalRPN(const std::vector<Token> &rpn) {
    std::stack<double> st;
    for (const Token &t : rpn) {
        if (t.type == TokenType::NUMBER) {
            st.push(t.value);
            continue;
        }
        if (t.type == TokenType::PLUS || t.type == TokenType::MINUS ||
            t.type == TokenType::MUL || t.type == TokenType::DIV ||
            t.type == TokenType::POW) {

            if (st.size() < 2) throw std::runtime_error("Operador binario sin suficientes operandos");
            double b = st.top(); st.pop();
            double a = st.top(); st.pop();
            double res = 0.0;
            switch (t.type) {
                case TokenType::PLUS: res = a + b; break;
                case TokenType::MINUS: res = a - b; break;
                case TokenType::MUL: res = a * b; break;
                case TokenType::DIV:
                    if (b == 0.0) throw std::runtime_error("División por cero");
                    res = a / b; break;
                case TokenType::POW: res = std::pow(a, b); break;
                default: break;
            }
            st.push(res);
            continue;
        }
        if (t.type == TokenType::FUNC) {
            if (t.name == "pi") {
                st.push(M_PI);
            } else {
                if (st.empty()) throw std::runtime_error("Función sin argumento: " + t.name);
                double arg = st.top(); st.pop();
                double res = applyFunction(t.name, arg);
                st.push(res);
            }
            continue;
        }
        throw std::runtime_error("Token inesperado en evaluación RPN");
    }
    if (st.size() != 1) throw std::runtime_error("Expresión inválida: la evaluación no terminó en un único valor");
    return st.top();
}

// Métodos públicos

double Calculadora::evaluar(const std::string& expresion) {
    auto rpn = infixToPostfixInternal(expresion);
    return evalRPN(rpn);
}

std::string Calculadora::obtenerNotacionPostfija(const std::string& expresion) {
    auto rpn = infixToPostfixInternal(expresion);
    return rpnToString(rpn);
}
