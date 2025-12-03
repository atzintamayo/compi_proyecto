#include "ElemTablaLR0.h"
#include <sstream>

ElemTablaLR0::ElemTablaLR0() 
    : EsDesplazamiento(true), NumEstadoDestino(-1) {}

ElemTablaLR0::ElemTablaLR0(bool esDesplazamiento, int numEstadoDestino)
    : EsDesplazamiento(esDesplazamiento), NumEstadoDestino(numEstadoDestino) {}

bool ElemTablaLR0::esAceptacion() const {
    return !EsDesplazamiento && NumEstadoDestino == -2;
}

bool ElemTablaLR0::esVacio() const {
    return EsDesplazamiento && NumEstadoDestino == -1;
}

std::string ElemTablaLR0::toString() const {
    if (esVacio()) {
        return "";
    }
    if (esAceptacion()) {
        return "acc";
    }
    
    std::ostringstream oss;
    if (EsDesplazamiento) {
        oss << "d" << NumEstadoDestino;
    } else {
        oss << "r" << NumEstadoDestino;
    }
    return oss.str();
}