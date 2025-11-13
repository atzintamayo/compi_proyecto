#ifndef TABLALL1_H
#define TABLALL1_H

#include "Gramatica.h"
#include "ConjuntosLL1.h"
#include <map>
#include <string>
#include <vector>

// Entrada de la tabla: [NoTerminal, Terminal] -> Producción
struct EntradaTabla {
    Produccion produccion;
    bool valida;
    
    EntradaTabla() : valida(false) {}
    EntradaTabla(const Produccion& prod) : produccion(prod), valida(true) {}
};

// Clase para construir y manejar la tabla LL(1)
class TablaLL1 {
public:
    TablaLL1(Gramatica* gramatica, ConjuntosLL1* conjuntos);
    ~TablaLL1();
    
    // Construir la tabla
    bool construir();
    
    // Obtener entrada de la tabla
    EntradaTabla getEntrada(const std::string& noTerminal, const std::string& terminal) const;
    
    // Verificar si hay conflictos
    bool tieneConflictos() const { return hayConflictos; }
    std::vector<std::string> getConflictos() const { return conflictos; }
    
    // Obtener terminales y no terminales ordenados
    std::vector<std::string> getTerminales() const;
    std::vector<std::string> getNoTerminales() const;
    
    // Convertir a string para mostrar
    std::string toString() const;
    
private:
    Gramatica* gramatica;
    ConjuntosLL1* conjuntos;
    
    // Tabla: [NoTerminal][Terminal] -> Producción
    std::map<std::string, std::map<std::string, EntradaTabla>> tabla;
    
    bool hayConflictos;
    std::vector<std::string> conflictos;
    
    // Agregar entrada a la tabla
    bool agregarEntrada(const std::string& noTerminal, 
                       const std::string& terminal, 
                       const Produccion& produccion);
};

#endif // TABLALL1_H