#ifndef GRAMATICA_H
#define GRAMATICA_H

#include <string>
#include <vector>
#include <map>
#include <set>

// Tipos de símbolos
enum TipoSimbolo {
    TERMINAL,
    NO_TERMINAL
};

// Estructura para un símbolo de la gramática
struct Simbolo {
    std::string nombre;
    TipoSimbolo tipo;
    
    Simbolo() : nombre(""), tipo(TERMINAL) {}
    Simbolo(const std::string& n, TipoSimbolo t) : nombre(n), tipo(t) {}
    
    bool operator<(const Simbolo& otro) const {
        if (tipo != otro.tipo) return tipo < otro.tipo;
        return nombre < otro.nombre;
    }
    
    bool operator==(const Simbolo& otro) const {
        return nombre == otro.nombre && tipo == otro.tipo;
    }
};

// Estructura para una producción: A -> α
struct Produccion {
    Simbolo izquierda;              // A (no terminal)
    std::vector<Simbolo> derecha;   // α (secuencia de símbolos)
    int numero;                     // Número de producción
    
    Produccion() : numero(0) {}
    Produccion(const Simbolo& izq, const std::vector<Simbolo>& der, int num = 0)
        : izquierda(izq), derecha(der), numero(num) {}
    
    std::string toString() const;
};

// Clase principal para manejar gramáticas
class Gramatica {
public:
    Gramatica();
    ~Gramatica();
    
    // Cargar gramática desde texto
    bool cargarDesdeTexto(const std::string& texto);
    
    // Agregar producción manualmente
    void agregarProduccion(const Simbolo& izq, const std::vector<Simbolo>& der);
    
    // Obtener información
    std::vector<Produccion> getProducciones() const { return producciones; }
    std::set<Simbolo> getNoTerminales() const { return noTerminales; }
    std::set<Simbolo> getTerminales() const { return terminales; }
    Simbolo getSimboloInicial() const { return simboloInicial; }
    
    // Buscar producciones de un no terminal
    std::vector<Produccion> produccionesDe(const Simbolo& noTerminal) const;
    
    // Validaciones
    bool esNoTerminal(const std::string& nombre) const;
    bool esTerminal(const std::string& nombre) const;
    bool esEpsilon(const std::string& nombre) const;
    
    // Limpiar
    void limpiar();
    
    // Convertir a string
    std::string toString() const;
    
private:
    std::vector<Produccion> producciones;
    std::set<Simbolo> noTerminales;
    std::set<Simbolo> terminales;
    Simbolo simboloInicial;
    int contadorProducciones;
    
    // Parsear una línea de producción
    bool parsearProduccion(const std::string& linea);
    
    // Identificar si un símbolo es terminal o no terminal
    TipoSimbolo identificarTipo(const std::string& nombre);
    
    // Tokenizar el lado derecho de una producción
    std::vector<Simbolo> tokenizarDerecha(const std::string& derecha);
};

#endif // GRAMATICA_H