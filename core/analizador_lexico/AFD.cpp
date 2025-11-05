#include "AFD.h"
#include <iostream>
#include <fstream>

// ElemSj
ElemSj::ElemSj() {
    id = -1;
    S.clear();
}

// EdoAFD
EdoAFD::EdoAFD() {
    id = -1;
    esAceptacion = false;
    token = -1;
    for(int i = 0; i <= 256; i++) {
        TransAFD[i] = -1;
    }
}

EdoAFD::EdoAFD(int idEdo) {
    id = idEdo;
    esAceptacion = false;
    token = -1;
    for(int i = 0; i <= 256; i++) {
        TransAFD[i] = -1;
    }
}

// AFD
AFD::AFD() {
    NumEdos = 0;
    edoInicial = 0;
    alfabeto.clear();
}

AFD::AFD(int n) {
    NumEdos = n;
    edoInicial = 0;
    alfabeto.clear();
    EdosAFD.resize(n);
    for(int i = 0; i < n; i++) {
        EdosAFD[i] = new EdoAFD(i);
    }
}

AFD::AFD(int n, Conjunto<char> alf) {
    NumEdos = n;
    edoInicial = 0;
    alfabeto.clear();
    alfabeto.Union(alf);
    EdosAFD.resize(n);
    for(int i = 0; i < n; i++) {
        EdosAFD[i] = new EdoAFD(i);
    }
}

AFD::~AFD() {
    for(auto e : EdosAFD) {
        delete e;
    }
}

bool AFD::GuardarAFD(std::string NombArch) {
    std::ofstream archivo(NombArch);
    if(!archivo.is_open()) {
        return false;
    }
    
    // Guardar número de estados
    archivo << NumEdos << std::endl;
    
    // Guardar estado inicial
    archivo << edoInicial << std::endl;
    
    // Guardar alfabeto
    archivo << alfabeto.size() << " ";
    for(char c : alfabeto) {
        archivo << (int)c << " ";
    }
    archivo << std::endl;
    
    // Guardar cada estado
    for(int i = 0; i < NumEdos; i++) {
        archivo << EdosAFD[i]->id << " "
                << EdosAFD[i]->esAceptacion << " "
                << EdosAFD[i]->token << std::endl;
        
        // Guardar transiciones
        for(char c : alfabeto) {
            archivo << EdosAFD[i]->TransAFD[(unsigned char)c] << " ";
        }
        archivo << std::endl;
    }
    
    archivo.close();
    return true;
}

bool AFD::CargarAFD(std::string NombArch) {
    std::ifstream archivo(NombArch);
    if(!archivo.is_open()) {
        return false;
    }
    
    // Limpiar AFD actual
    for(auto e : EdosAFD) {
        delete e;
    }
    EdosAFD.clear();
    alfabeto.clear();
    
    // Cargar número de estados
    archivo >> NumEdos;
    
    // Cargar estado inicial
    archivo >> edoInicial;
    
    // Cargar alfabeto
    int tamAlf;
    archivo >> tamAlf;
    for(int i = 0; i < tamAlf; i++) {
        int c;
        archivo >> c;
        alfabeto.add((char)c);
    }
    
    // Cargar estados
    EdosAFD.resize(NumEdos);
    for(int i = 0; i < NumEdos; i++) {
        EdosAFD[i] = new EdoAFD();
        archivo >> EdosAFD[i]->id
                >> EdosAFD[i]->esAceptacion
                >> EdosAFD[i]->token;
        
        // Cargar transiciones
        for(char c : alfabeto) {
            archivo >> EdosAFD[i]->TransAFD[(unsigned char)c];
        }
    }
    
    archivo.close();
    return true;
}

int AFD::ReconocerCadena(std::string cadena) {
    int edoActual = edoInicial;
    
    for(char c : cadena) {
        if(edoActual < 0 || edoActual >= NumEdos) {
            return -2; // Error: estado inválido
        }
        
        int siguiente = EdosAFD[edoActual]->TransAFD[(unsigned char)c];
        
        if(siguiente == -1) {
            return -2; // Error: no hay transición (cambié a -2)
        }
        
        edoActual = siguiente;
    }
    
    // Verificar si el estado final es de aceptación
    if(edoActual >= 0 && edoActual < NumEdos && EdosAFD[edoActual]->esAceptacion) {
        return EdosAFD[edoActual]->token; // Puede ser -1 si no tiene token
    }
    
    return -2; // No es estado de aceptación (cambié a -2)
}

bool AFD::AceptaCadena(std::string cadena) {
    int edoActual = edoInicial;
    
    for(char c : cadena) {
        if(edoActual < 0 || edoActual >= NumEdos) {
            return false;
        }
        
        int siguiente = EdosAFD[edoActual]->TransAFD[(unsigned char)c];
        
        if(siguiente == -1) {
            return false;
        }
        
        edoActual = siguiente;
    }
    
    // Verificar si el estado final es de aceptación
    return (edoActual >= 0 && edoActual < NumEdos && EdosAFD[edoActual]->esAceptacion);
}

void AFD::imprimirAFD() {
    std::cout << "\n=== AFD ===" << std::endl;
    std::cout << "Número de estados: " << NumEdos << std::endl;
    std::cout << "Estado inicial: " << edoInicial << std::endl;
    
    std::cout << "Alfabeto: ";
    for(char c : alfabeto) {
        if(c == EPSILON) std::cout << "ε ";
        else std::cout << c << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\nEstados de aceptación:" << std::endl;
    for(int i = 0; i < NumEdos; i++) {
        if(EdosAFD[i]->esAceptacion) {
            std::cout << "  Estado " << i << " (token: " << EdosAFD[i]->token << ")" << std::endl;
        }
    }
    
    std::cout << "\nTabla de transiciones:" << std::endl;
    std::cout << "Estado\t";
    for(char c : alfabeto) {
        if(c == EPSILON) std::cout << "ε\t";
        else std::cout << c << "\t";
    }
    std::cout << std::endl;
    
    for(int i = 0; i < NumEdos; i++) {
        std::cout << i;
        if(EdosAFD[i]->esAceptacion) std::cout << "*";
        std::cout << "\t";
        
        for(char c : alfabeto) {
            int dest = EdosAFD[i]->TransAFD[(unsigned char)c];
            if(dest == -1) std::cout << "-\t";
            else std::cout << dest << "\t";
        }
        std::cout << std::endl;
    }
}