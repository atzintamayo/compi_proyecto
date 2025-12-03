#ifndef ITEMLR0_H
#define ITEMLR0_H

#include <string>

// Representa un ítem LR(0): A → α • β
class ItemLR0 {
public:
    int NumRegla;   // Índice de la producción en la gramática
    int PosPunto;   // Posición del punto en el lado derecho (>=0)
    
    // Constructores
    ItemLR0();
    ItemLR0(int numRegla, int posPunto);
    
    // Operadores para usar en std::set
    bool operator==(const ItemLR0& otro) const;
    bool operator<(const ItemLR0& otro) const;
    
    // Utilidad para mostrar el ítem
    std::string toString() const;
};

#endif // ITEMLR0_H