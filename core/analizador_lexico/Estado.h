#ifndef ESTADO_H
#define ESTADO_H

#include "Conjunto.h"

// Símbolo especial para epsilon
const char EPSILON = '$';

class Estado;

class Transition {
public:
    char simboloInf;
    char simboloSup;
    Estado* edoDestino;
    
    Transition();
    Transition(char c, Estado* e);
    Transition(char cInf, char cSup, Estado* e);
};

class Estado {
public:
    int idEdo;
    bool edoAcept;
    int token;
    Conjunto<Transition*> transiciones;
    static int NumEdos;
    
    Estado();
    ~Estado();
    
    Conjunto<Estado*> tieneTransition(char c);
};

#endif // ESTADO_H