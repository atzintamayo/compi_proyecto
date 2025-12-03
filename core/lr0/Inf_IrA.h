#ifndef INF_IRA_H
#define INF_IRA_H

#include <string>

// Almacena información de una transición IrA para depuración/visualización
class Inf_IrA {
public:
    int Si;                      // Estado de origen
    int IrA_Sj;                  // Estado destino
    std::string IrA_Simbolo;     // Símbolo con el que se hace IrA
    std::string ConjuntoItems;   // Representación textual del conjunto de ítems resultante
    
    // Constructor
    Inf_IrA();
};

#endif // INF_IRA_H