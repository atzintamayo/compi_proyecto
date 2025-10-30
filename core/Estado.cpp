#include "Estado.h"

int Estado::NumEdos = 0;

// Constructores de Transition
Transition::Transition() {
    edoDestino = nullptr;
    simboloInf = 0;
    simboloSup = 0;
}

Transition::Transition(char c, Estado* e) {
    simboloInf = simboloSup = c;
    edoDestino = e;
}

Transition::Transition(char cInf, char cSup, Estado* e) {
    simboloInf = cInf;
    simboloSup = cSup;
    edoDestino = e;
}

// Constructor y destructor de Estado
Estado::Estado() {
    idEdo = NumEdos++;
    edoAcept = false;
    token = -1;
    transiciones.clear();
}

Estado::~Estado() {
    for (auto t : transiciones) {
        delete t;
    }
}

// Método para verificar si tiene transición con un carácter
Conjunto<Estado*> Estado::tieneTransition(char c) {
    Conjunto<Estado*> r;
    r.clear();
    
    for (Transition* t : transiciones) {
        if (t->simboloInf <= c && c <= t->simboloSup) {
            r.add(t->edoDestino);
        }
    }
    
    return r;
}