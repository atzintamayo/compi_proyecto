#ifndef SIMB_H
#define SIMB_H

#include <string>

// Representa terminales y no terminales en un solo arreglo
class Simb {
public:
    std::string Simbolo;  // Nombre del símbolo
    bool EsTerminal;      // true si es terminal ($ también es terminal)
    int IdToken;          // ID de token del léxico (opcional)
    
    // Constructores
    Simb();
    Simb(const std::string& simb, bool esTerminal, int idToken = -1);
};

#endif // SIMB_H