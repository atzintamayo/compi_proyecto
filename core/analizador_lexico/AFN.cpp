#include "AFN.h"
#include "AFD.h"
#include <iostream>
#include <queue>
#include <map>

int AFN::contadorAFN = 0;

AFN::AFN() {
    idAFN = contadorAFN++;
    estados.clear();
    edoInicial = nullptr;
    alfabeto.clear();
    edosAcept.clear();
}

AFN::~AFN() {
    for (auto e : estados) {
        delete e;
    }
}

AFN* AFN::CrearBasico(char c) {
    Estado* e1 = new Estado();
    Estado* e2 = new Estado();
    
    estados.add(e1);
    estados.add(e2);
    edoInicial = e1;
    
    e1->transiciones.add(new Transition(c, e2));
    e2->edoAcept = true;
    
    alfabeto.add(c);
    edosAcept.add(e2);
    
    return this;
}

AFN* AFN::CrearBasico(char c1, char c2) {
    Estado* e1 = new Estado();
    Estado* e2 = new Estado();
    
    estados.add(e1);
    estados.add(e2);
    edoInicial = e1;
    
    e1->transiciones.add(new Transition(c1, c2, e2));
    e2->edoAcept = true;
    edosAcept.add(e2);
    
    for(int i = c1; i <= c2; i++) {
        alfabeto.add((char)i);
    }
    
    return this;
}

AFN* AFN::unirAFN(AFN* F2) {
    Estado* e1 = new Estado();
    Estado* e2 = new Estado();
    
    e1->transiciones.add(new Transition(EPSILON, edoInicial));
    e1->transiciones.add(new Transition(EPSILON, F2->edoInicial));
    
    // Conectar estados de aceptación al nuevo estado final
    // CRÍTICO: NO modificar edoAcept de los estados originales
    // porque ConvertirAFD necesita ver sus tokens
    for(Estado* e : edosAcept) {
        e->transiciones.add(new Transition(EPSILON, e2));
        // NO hacer: e->edoAcept = false;
    }
    
    for(Estado* e : F2->edosAcept) {
        e->transiciones.add(new Transition(EPSILON, e2));
        // NO hacer: e->edoAcept = false;
    }
    
    // El nuevo estado de aceptación unificado
    e2->edoAcept = true;
    e2->token = -1;
    
    edoInicial = e1;
    
    estados.Union(F2->estados);
    estados.add(e1);
    estados.add(e2);
    
    // Actualizar conjunto de estados de aceptación
    edosAcept.Union(F2->edosAcept);
    edosAcept.add(e2);
    
    alfabeto.Union(F2->alfabeto);
    
    F2->estados.clear();
    delete F2;
    
    return this;
}

AFN* AFN::concatenarAFN(AFN* F2) {
    // Los estados de aceptación de F2 deben mantener sus tokens
    for(Estado* e : edosAcept) {
        for(Transition* t : F2->edoInicial->transiciones) {
            e->transiciones.add(new Transition(t->simboloInf, t->simboloSup, t->edoDestino));
        }
        e->edoAcept = false;
    }
    
    edosAcept.clear();
    edosAcept.Union(F2->edosAcept); // Los tokens ya están en F2->edosAcept
    alfabeto.Union(F2->alfabeto);
    
    F2->estados.Quitar(F2->edoInicial);
    delete F2->edoInicial;
    
    estados.Union(F2->estados);
    
    F2->estados.clear();
    delete F2;
    
    return this;
}

AFN* AFN::CerraduraPositiva() {
    Estado* e1 = new Estado();
    Estado* e2 = new Estado();
    int tokenOriginal = -1;
    for (Estado* e : edosAcept) if (e->token != -1) { tokenOriginal = e->token; break; }

    for (Estado* e : edosAcept) {
        e->transiciones.add(new Transition(EPSILON, edoInicial));
        e->transiciones.add(new Transition(EPSILON, e2));
        e->edoAcept = false;
    }

    e1->transiciones.add(new Transition(EPSILON, edoInicial));
    edoInicial = e1;

    e2->edoAcept = true;            // <-- AÑADE ESTO
    e2->token   = tokenOriginal;    // ya lo tenías

    edosAcept.clear();
    edosAcept.add(e2);
    estados.add(e1); estados.add(e2);
    return this;
}

AFN* AFN::CerraduraKleene() {
    Estado* e1 = new Estado();
    Estado* e2 = new Estado();
    int tokenOriginal = -1;
    for (Estado* e : edosAcept) if (e->token != -1) { tokenOriginal = e->token; break; }

    for (Estado* e : edosAcept) {
        e->transiciones.add(new Transition(EPSILON, edoInicial));
        e->transiciones.add(new Transition(EPSILON, e2));
        e->edoAcept = false;
    }

    e1->transiciones.add(new Transition(EPSILON, edoInicial));
    e1->transiciones.add(new Transition(EPSILON, e2)); // cero ocurrencias
    edoInicial = e1;

    e2->edoAcept = true;            // <-- AÑADE ESTO
    e2->token   = tokenOriginal;

    edosAcept.clear();
    edosAcept.add(e2);
    estados.add(e1); estados.add(e2);
    return this;
}

Conjunto<Estado*> AFN::CerraduraEpsilon(Estado* e) {
    Conjunto<Estado*> c;
    std::stack<Estado*> p;
    
    c.clear();
    p.push(e);
    
    while(!p.empty()) {
        Estado* e2 = p.top();
        p.pop();
        
        if(!c.contiene(e2)) {
            c.add(e2);
            
            for(Transition* t : e2->transiciones) {
                if(t->simboloInf == EPSILON && t->simboloSup == EPSILON) {
                    p.push(t->edoDestino);
                }
            }
        }
    }
    
    return c;
}

Conjunto<Estado*> AFN::CerraduraEpsilon(Conjunto<Estado*> C) {
    Conjunto<Estado*> R;
    R.clear();
    
    for(Estado* e : C) {
        R.Union(CerraduraEpsilon(e));
    }
    
    return R;
}

Conjunto<Estado*> AFN::Mover(Estado* e, char c) {
    Conjunto<Estado*> R;
    R.clear();
    
    for(Transition* t : e->transiciones) {
        if(t->simboloInf <= c && c <= t->simboloSup && c != EPSILON) {
            R.add(t->edoDestino);
        }
    }
    
    return R;
}

Conjunto<Estado*> AFN::Mover(Conjunto<Estado*> e, char c) {
    Conjunto<Estado*> R;
    R.clear();
    
    for(Estado* estado : e) {
        for(Transition* t : estado->transiciones) {
            if(t->simboloInf <= c && c <= t->simboloSup && c != EPSILON) {
                R.add(t->edoDestino);
            }
        }
    }
    
    return R;
}

Conjunto<Estado*> AFN::IrA(Estado* e, char c) {
    return CerraduraEpsilon(Mover(e, c));
}

Conjunto<Estado*> AFN::IrA(Conjunto<Estado*> e, char c) {
    return CerraduraEpsilon(Mover(e, c));
}

AFD* AFN::ConvertirAFD() {
    std::map<Conjunto<Estado*>, int> conjuntoAId;
    std::queue<ElemSj> Q;
    int numElemSj = 0;
    
    // Crear conjunto inicial con cerradura epsilon del estado inicial
    ElemSj SjInicial;
    SjInicial.S = CerraduraEpsilon(edoInicial);
    SjInicial.id = numElemSj++;
    
    conjuntoAId[SjInicial.S] = SjInicial.id;
    Q.push(SjInicial);
    
    // Mapa temporal para construir transiciones
    std::map<int, std::map<char, int>> transiciones;
    std::map<int, bool> esAceptacion;
    std::map<int, int> tokenEstado;
    
    // DEBUG: Guardar composición de conjuntos
    std::map<int, Conjunto<Estado*>> conjuntoComposicion;
    
    // Procesar la cola
    while(!Q.empty()) {
        ElemSj SjAct = Q.front();
        Q.pop();
        
        conjuntoComposicion[SjAct.id] = SjAct.S;
        
        // Verificar si es estado de aceptación
        // ESTRATEGIA: Buscar el PRIMER token válido (>= 0), ignorar completamente los -1
        int mejorToken = -1;
        bool tieneEstadoAceptacion = false;
        
        // DEBUG
        std::cout << "Procesando conjunto " << SjAct.id << ": {";
        for(Estado* e : SjAct.S) {
            std::cout << e->idEdo;
            if(e->edoAcept) std::cout << "*(" << e->token << ")";
            std::cout << " ";
        }
        std::cout << "}" << std::endl;
        
        for(Estado* e : SjAct.S) {
            if(e->edoAcept) {
                tieneEstadoAceptacion = true;
                // Si encontramos un token válido, usarlo
                if(e->token >= 0) {
                    mejorToken = e->token;
                    std::cout << "  -> Token seleccionado: " << mejorToken << " del estado " << e->idEdo << std::endl;
                    break; // Usar el primer token válido encontrado
                }
            }
        }
        
        if(tieneEstadoAceptacion) {
            esAceptacion[SjAct.id] = true;
            tokenEstado[SjAct.id] = mejorToken;
            if(mejorToken == -1) {
                std::cout << "  -> ADVERTENCIA: Solo estados con token=-1" << std::endl;
            }
        }
        
        // Para cada símbolo del alfabeto
        for(char c : alfabeto) {
            if(c == EPSILON) continue; // No procesar epsilon en AFD
            
            Conjunto<Estado*> U = IrA(SjAct.S, c);
            
            if(!U.empty()) {
                // Verificar si este conjunto ya existe
                if(conjuntoAId.find(U) == conjuntoAId.end()) {
                    // Nuevo conjunto
                    ElemSj SjNuevo;
                    SjNuevo.S = U;
                    SjNuevo.id = numElemSj++;
                    conjuntoAId[U] = SjNuevo.id;
                    Q.push(SjNuevo);
                }
                
                // Agregar transición
                transiciones[SjAct.id][c] = conjuntoAId[U];
            }
        }
    }
    
    // Crear el AFD
    AFD* afdResultado = new AFD(numElemSj, alfabeto);
    afdResultado->edoInicial = 0; // El primer conjunto es el estado inicial
    
    // Llenar las transiciones del AFD
    for(int i = 0; i < numElemSj; i++) {
        // Marcar estados de aceptación
        if(esAceptacion.find(i) != esAceptacion.end()) {
            afdResultado->EdosAFD[i]->esAceptacion = true;
            afdResultado->EdosAFD[i]->token = tokenEstado[i];
        }
        
        // Copiar transiciones
        for(char c : alfabeto) {
            if(c == EPSILON) continue;
            
            if(transiciones[i].find(c) != transiciones[i].end()) {
                afdResultado->EdosAFD[i]->TransAFD[(unsigned char)c] = transiciones[i][c];
            }
        }
    }
    
    std::cout << "AFN convertido a AFD con " << numElemSj << " estados." << std::endl;
    
    return afdResultado;
}

void AFN::imprimirAFN() {
    std::cout << "\n=== AFN #" << idAFN << " ===" << std::endl;
    std::cout << "Estados: " << estados.size() << std::endl;
    std::cout << "Estado inicial: " << (edoInicial ? edoInicial->idEdo : -1) << std::endl;
    std::cout << "Estados de aceptación: ";
    for(Estado* e : edosAcept) {
        std::cout << e->idEdo << "(token:" << e->token << ") ";
    }
    std::cout << std::endl;
    
    std::cout << "Alfabeto: ";
    for(char c : alfabeto) {
        if(c == EPSILON) std::cout << "ε ";
        else std::cout << c << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\nTransiciones:" << std::endl;
    for(Estado* e : estados) {
        for(Transition* t : e->transiciones) {
            std::cout << "  " << e->idEdo << " --";
            if(t->simboloInf == EPSILON) {
                std::cout << "ε";
            } else if(t->simboloInf == t->simboloSup) {
                std::cout << t->simboloInf;
            } else {
                std::cout << "[" << t->simboloInf << "-" << t->simboloSup << "]";
            }
            std::cout << "--> " << t->edoDestino->idEdo << std::endl;
        }
    }
}