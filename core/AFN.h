#ifndef AFN_H
#define AFN_H

#include "Estado.h"
#include "Conjunto.h"
#include <stack>

class AFD;

class AFN {
public:
    int idAFN;
    Conjunto<Estado*> estados;
    Estado* edoInicial;
    Conjunto<char> alfabeto;
    Conjunto<Estado*> edosAcept;
    static int contadorAFN;
    
    AFN();
    ~AFN();
    
    // Crear AFN básicos
    AFN* CrearBasico(char c);
    AFN* CrearBasico(char c1, char c2);
    
    // Operaciones de Thompson
    AFN* unirAFN(AFN* F2);
    AFN* concatenarAFN(AFN* F2);
    AFN* CerraduraPositiva();
    AFN* CerraduraKleene();
    
    // Operaciones de cerradura epsilon y movimiento
    Conjunto<Estado*> CerraduraEpsilon(Estado* e);
    Conjunto<Estado*> CerraduraEpsilon(Conjunto<Estado*> C);
    Conjunto<Estado*> Mover(Estado* e, char c);
    Conjunto<Estado*> Mover(Conjunto<Estado*> e, char c);
    Conjunto<Estado*> IrA(Estado* e, char c);
    Conjunto<Estado*> IrA(Conjunto<Estado*> e, char c);
    
    // Conversión a AFD
    AFD* ConvertirAFD();
    
    // Utilidades
    void imprimirAFN();
};

#endif // AFN_H