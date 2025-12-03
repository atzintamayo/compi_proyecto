#include "RenglonLR0.h"

RenglonLR0::RenglonLR0() : IdEdo(-1) {}

RenglonLR0::RenglonLR0(int idEdoLR, int numSimbolos) 
    : IdEdo(idEdoLR), Acciones(numSimbolos) {
    // Inicializa todas las acciones como vacías (constructor por defecto de ElemTablaLR0)
}