#include "ItemLR0.h"
#include <sstream>

ItemLR0::ItemLR0() : NumRegla(-1), PosPunto(-1) {}

ItemLR0::ItemLR0(int numRegla, int posPunto) 
    : NumRegla(numRegla), PosPunto(posPunto) {}

bool ItemLR0::operator==(const ItemLR0& otro) const {
    return NumRegla == otro.NumRegla && PosPunto == otro.PosPunto;
}

bool ItemLR0::operator<(const ItemLR0& otro) const {
    if (NumRegla != otro.NumRegla) {
        return NumRegla < otro.NumRegla;
    }
    return PosPunto < otro.PosPunto;
}

std::string ItemLR0::toString() const {
    std::ostringstream oss;
    oss << "[" << NumRegla << ", " << PosPunto << "]";
    return oss.str();
}