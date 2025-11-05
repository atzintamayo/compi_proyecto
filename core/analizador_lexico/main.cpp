#include "AFN.h"
#include "AFD.h"
#include <iostream>
#include <string>

using namespace std;

void pruebaBasico() {
    cout << "\n========== PRUEBA 1: Crear AFN Básico (carácter 'a') ==========" << endl;
    AFN* afn1 = new AFN();
    afn1->CrearBasico('a');
    afn1->imprimirAFN();
    delete afn1;
}

void pruebaRango() {
    cout << "\n========== PRUEBA 2: Crear AFN Básico (rango 'a'-'z') ==========" << endl;
    AFN* afn2 = new AFN();
    afn2->CrearBasico('a', 'z');
    afn2->imprimirAFN();
    delete afn2;
}

void pruebaUnion() {
    cout << "\n========== PRUEBA 3: Unión de AFNs ('a' | 'b') ==========" << endl;
    AFN* afn3a = new AFN();
    afn3a->CrearBasico('a');
    
    AFN* afn3b = new AFN();
    afn3b->CrearBasico('b');
    
    afn3a->unirAFN(afn3b);
    afn3a->imprimirAFN();
    delete afn3a;
}

void pruebaConcatenacion() {
    cout << "\n========== PRUEBA 4: Concatenación de AFNs ('a' . 'b') ==========" << endl;
    AFN* afn4a = new AFN();
    afn4a->CrearBasico('a');
    
    AFN* afn4b = new AFN();
    afn4b->CrearBasico('b');
    
    afn4a->concatenarAFN(afn4b);
    afn4a->imprimirAFN();
    delete afn4a;
}

void pruebaCerraduraPositiva() {
    cout << "\n========== PRUEBA 5: Cerradura Positiva ('a'+) ==========" << endl;
    AFN* afn5 = new AFN();
    afn5->CrearBasico('a');
    afn5->CerraduraPositiva();
    afn5->imprimirAFN();
    delete afn5;
}

void pruebaCerraduraKleene() {
    cout << "\n========== PRUEBA 6: Cerradura de Kleene ('a'*) ==========" << endl;
    AFN* afn6 = new AFN();
    afn6->CrearBasico('a');
    afn6->CerraduraKleene();
    afn6->imprimirAFN();
    delete afn6;
}

void pruebaCompleja() {
    cout << "\n========== PRUEBA 7: Expresión compleja (a|b)*c ==========" << endl;
    
    // Crear 'a'
    AFN* afn_a = new AFN();
    afn_a->CrearBasico('a');
    
    // Crear 'b'
    AFN* afn_b = new AFN();
    afn_b->CrearBasico('b');
    
    // (a|b)
    afn_a->unirAFN(afn_b);
    
    // (a|b)*
    afn_a->CerraduraKleene();
    
    // Crear 'c'
    AFN* afn_c = new AFN();
    afn_c->CrearBasico('c');
    
    // (a|b)*c
    afn_a->concatenarAFN(afn_c);
    
    // ASIGNAR TOKEN antes de convertir
    for(Estado* e : afn_a->edosAcept) {
        e->token = 10; // Token para esta expresión
    }
    
    cout << "\nAFN resultante:" << endl;
    afn_a->imprimirAFN();
    
    // Convertir a AFD
    cout << "\nConvirtiendo a AFD..." << endl;
    AFD* afd = afn_a->ConvertirAFD();
    afd->imprimirAFD();
    
    // Probar reconocimiento
    cout << "\n--- Pruebas de reconocimiento ---" << endl;
    string cadenas[] = {"c", "ac", "bc", "aac", "abc", "bbbac", "aaabbbaaac", "a", "ab", "ca", ""};
    
    for(string cad : cadenas) {
        int token = afd->ReconocerCadena(cad);
        bool acepta = afd->AceptaCadena(cad);
        
        cout << "Cadena: \"" << cad << "\" -> ";
        if(acepta) {
            cout << "ACEPTADA (token: " << token << ")" << endl;
        } else {
            cout << "RECHAZADA" << endl;
        }
    }
    
    delete afn_a;
    delete afd;
}

void pruebaGuardarCargar() {
    cout << "\n========== PRUEBA 8: Guardar y Cargar AFD ==========" << endl;
    
    // Crear AFN simple: (a|b)+
    AFN* afn_a = new AFN();
    afn_a->CrearBasico('a');
    
    AFN* afn_b = new AFN();
    afn_b->CrearBasico('b');
    
    afn_a->unirAFN(afn_b);
    afn_a->CerraduraPositiva();
    
    // ASIGNAR TOKEN antes de convertir
    for(Estado* e : afn_a->edosAcept) {
        e->token = 100;
    }
    
    // Convertir a AFD
    AFD* afd_original = afn_a->ConvertirAFD();
    
    cout << "\nAFD original:" << endl;
    afd_original->imprimirAFD();
    
    // Guardar
    string archivo = "test_afd.txt";
    if(afd_original->GuardarAFD(archivo)) {
        cout << "\nAFD guardado exitosamente en: " << archivo << endl;
    } else {
        cout << "\nError al guardar AFD" << endl;
    }
    
    // Cargar en nuevo AFD
    AFD* afd_cargado = new AFD();
    if(afd_cargado->CargarAFD(archivo)) {
        cout << "\nAFD cargado exitosamente desde: " << archivo << endl;
        cout << "\nAFD cargado:" << endl;
        afd_cargado->imprimirAFD();
        
        // Probar que funciona igual
        cout << "\n--- Verificando funcionalidad ---" << endl;
        string cadenas[] = {"a", "b", "ab", "ba", "aaa", "bbb", "ababab", "c", ""};
        
        for(string cad : cadenas) {
            bool acepta1 = afd_original->AceptaCadena(cad);
            bool acepta2 = afd_cargado->AceptaCadena(cad);
            int token1 = afd_original->ReconocerCadena(cad);
            int token2 = afd_cargado->ReconocerCadena(cad);
            
            cout << "\"" << cad << "\" -> Original: ";
            if(acepta1) cout << "OK(token:" << token1 << ")";
            else cout << "RECHAZADA";
            
            cout << ", Cargado: ";
            if(acepta2) cout << "OK(token:" << token2 << ")";
            else cout << "RECHAZADA";
            
            cout << " -> " << (token1 == token2 ? "✓" : "✗") << endl;
        }
    } else {
        cout << "\nError al cargar AFD" << endl;
    }
    
    delete afn_a;
    delete afd_original;
    delete afd_cargado;
}

void pruebaTokens() {
    cout << "\n========== PRUEBA 9: Múltiples tokens (analizador léxico simple) ==========" << endl;
    
    // Crear AFN para 'if' (token 1)
    AFN* afn_if = new AFN();
    afn_if->CrearBasico('i');
    AFN* afn_f = new AFN();
    afn_f->CrearBasico('f');
    afn_if->concatenarAFN(afn_f);
    // ASIGNAR TOKEN ANTES de unir
    for(Estado* e : afn_if->edosAcept) {
        e->token = 1; // Token para 'if'
    }
    
    // Crear AFN para dígitos [0-9]+ (token 2)
    AFN* afn_digito = new AFN();
    afn_digito->CrearBasico('0', '9');
    afn_digito->CerraduraPositiva();
    // ASIGNAR TOKEN ANTES de unir
    for(Estado* e : afn_digito->edosAcept) {
        e->token = 2; // Token para número
    }
    
    // Unir ambos AFNs
    afn_if->unirAFN(afn_digito);
    
    // IMPORTANTE: Después de unir, asignar tokens nuevamente al estado final unificado
    // porque unirAFN crea un nuevo estado de aceptación
    // PERO los estados originales de aceptación YA tienen sus tokens asignados
    // y esos tokens se preservan en la conversión a AFD
    
    cout << "\nAFN con múltiples tokens:" << endl;
    afn_if->imprimirAFN();
    
    // Convertir a AFD
    AFD* afd = afn_if->ConvertirAFD();
    afd->imprimirAFD();
    
    // Probar reconocimiento
    cout << "\n--- Pruebas de reconocimiento ---" << endl;
    string cadenas[] = {"if", "123", "456", "0", "999", "i", "f", "abc"};
    
    for(string cad : cadenas) {
        bool acepta = afd->AceptaCadena(cad);
        int token = afd->ReconocerCadena(cad);
        
        cout << "Cadena: \"" << cad << "\" -> ";
        if(acepta && token == 1) {
            cout << "ACEPTADA (token: 1, lexema: 'if')" << endl;
        } else if(acepta && token == 2) {
            cout << "ACEPTADA (token: 2, lexema: 'numero')" << endl;
        } else if(acepta) {
            cout << "ACEPTADA (sin token específico)" << endl;
        } else {
            cout << "RECHAZADA" << endl;
        }
    }
    
    delete afn_if;
    delete afd;
}

int main() {
    cout << "╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║  IMPLEMENTACIÓN DE AUTÓMATAS FINITOS (AFN/AFD)            ║" << endl;
    cout << "║  Construcción de Thompson y Conversión AFN→AFD             ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝" << endl;
    
    pruebaBasico();
    pruebaRango();
    pruebaUnion();
    pruebaConcatenacion();
    pruebaCerraduraPositiva();
    pruebaCerraduraKleene();
    pruebaCompleja();
    pruebaGuardarCargar();
    pruebaTokens();
    
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║  TODAS LAS PRUEBAS COMPLETADAS                             ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝" << endl;
    
    return 0;
}