#ifndef ELEMTABLALR0_H
#define ELEMTABLALR0_H

#include <string>

// Representa una celda de la tabla LR(0) (ACTION/GOTO)
class ElemTablaLR0 {
public:
    bool EsDesplazamiento;   // true = desplazamiento/IrA, false = reducción
    int NumEstadoDestino;    // Estado destino (si EsDesplazamiento) o número de producción (si reducción)
    
    // Constructores
    ElemTablaLR0();
    ElemTablaLR0(bool esDesplazamiento, int numEstadoDestino);
    
    // Convenciones especiales:
    // - Para aceptación: EsDesplazamiento=false, NumEstadoDestino=-2
    // - Para celda vacía/error: EsDesplazamiento=true, NumEstadoDestino=-1
    
    bool esAceptacion() const;
    bool esVacio() const;
    
    std::string toString() const;
};

#endif // ELEMTABLALR0_H