#ifndef RENGLONLR0_H
#define RENGLONLR0_H

#include "ElemTablaLR0.h"
#include <vector>

// Representa un renglón (estado) de la tabla LR(0)
class RenglonLR0 {
public:
    int IdEdo;                          // Número de estado LR(0)
    std::vector<ElemTablaLR0> Acciones; // Una entrada por cada símbolo en Vt_Vn
    
    // Constructores
    RenglonLR0();
    RenglonLR0(int idEdoLR, int numSimbolos);
};

#endif // RENGLONLR0_H