#ifndef ANALIZADORLR0_H
#define ANALIZADORLR0_H

#include "ItemLR0.h"
#include "ElemTablaLR0.h"
#include "RenglonLR0.h"
#include "Inf_IrA.h"
#include "Simb.h"
#include "GramaticaLR0.h"  // ← Cambiar nombre
#include <vector>
#include <set>
#include <string>
#include <map>

// Estructura para agrupar un conjunto de ítems y su número de estado
struct LR0_Conj_Sj {
    int j;
    std::set<ItemLR0> Sj;
};

// Clase principal del Analizador LR(0)
class AnalizadorLR0 {
public:
    std::vector<Inf_IrA> ResultIrA;
    int idAutGram;
    int TotSimbolos;
    int NumRenglonesIrA;
    
    // Gramática - USAR LA CLASE DEL NAMESPACE LR0
    LR0::GramaticaLR0* gramatica;  // ← Cambiar tipo
    std::string Gram;
    
    std::vector<RenglonLR0> RenglonesLR0;
    std::vector<std::string> Vt_Vn;
    std::vector<Simb> V;
    std::vector<LR0_Conj_Sj> ColeccionCanonica;
    
    struct PasoLR0 {
        std::string pila;
        std::string cadena;
        std::string accion;
        
        PasoLR0(const std::string& p, const std::string& c, const std::string& a)
            : pila(p), cadena(c), accion(a) {}
    };
    
    // Constructor - CAMBIAR TIPO DE PARÁMETRO
    AnalizadorLR0(LR0::GramaticaLR0* gram);  // ← Cambiar tipo
    ~AnalizadorLR0();
    
    void AnalizarGramatica_Y_PrepararSimbolos();
    std::set<ItemLR0> Cerradura_LR0(const std::set<ItemLR0>& conjItems);
    std::set<ItemLR0> IrA_LR0(const std::set<ItemLR0>& conjunto, const std::string& simbolo);
    void CrearTablaLR0();
    std::vector<PasoLR0> AnalizarCadenaLR0(const std::vector<std::string>& tokens);
    
    std::string conjuntoItemsToString(const std::set<ItemLR0>& conjunto) const;
    std::string obtenerSimboloEnPosicion(int numRegla, int pos) const;
    bool itemTienePuntoAlFinal(const ItemLR0& item) const;
    
private:
    int buscarEstadoConConjunto(const std::set<ItemLR0>& conjunto) const;
};

#endif // ANALIZADORLR0_H