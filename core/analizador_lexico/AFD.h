#ifndef AFD_H
#define AFD_H

#include "Estado.h"
#include "Conjunto.h"
#include <string>
#include <vector>

// Clase auxiliar para mapear conjuntos de estados a IDs
class ElemSj {
public:
    int id;
    Conjunto<Estado*> S;
    
    ElemSj();
    
    bool operator<(const ElemSj& otro) const {
        return S < otro.S;
    }
    
    bool operator==(const ElemSj& otro) const {
        return S == otro.S;
    }
};

// Clase para estado de AFD
class EdoAFD {
public:
    int TransAFD[257];
    int id;
    bool esAceptacion;
    int token;
    
    EdoAFD();
    EdoAFD(int idEdo);
};

// Clase AFD
class AFD {
public:
    std::vector<EdoAFD*> EdosAFD;
    Conjunto<char> alfabeto;
    int NumEdos;
    int edoInicial;
    
    AFD();
    AFD(int n);
    AFD(int n, Conjunto<char> alf);
    ~AFD();
    
    bool GuardarAFD(std::string NombArch);
    bool CargarAFD(std::string NombArch);
    
    // Reconocimiento de cadenas
    int ReconocerCadena(std::string cadena);
    bool AceptaCadena(std::string cadena);  // Versión booleana
    
    void imprimirAFD();
};

#endif // AFD_H