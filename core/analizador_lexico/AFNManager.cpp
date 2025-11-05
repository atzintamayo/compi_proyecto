#include "AFNManager.h"
#include <iostream>

AFNManager::AFNManager() {
    contadorId = 1;
    afns.clear();
}

AFNManager::~AFNManager() {
    limpiarTodos();
}

int AFNManager::agregarAFN(AFN* afn, std::string nombre, std::string descripcion) {
    int id = contadorId++;
    afns.insert({id, AFNInfo(id, afn, nombre, descripcion)});
    return id;
}

AFN* AFNManager::obtenerAFN(int id) {
    auto it = afns.find(id);
    if (it != afns.end()) {
        return it->second.afn;
    }
    return nullptr;
}

AFNInfo* AFNManager::obtenerInfo(int id) {
    auto it = afns.find(id);
    if (it != afns.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool AFNManager::eliminarAFN(int id) {
    auto it = afns.find(id);
    if (it != afns.end()) {
        delete it->second.afn;
        afns.erase(it);
        return true;
    }
    return false;
}

std::vector<AFNInfo> AFNManager::listarAFNs() const {
    std::vector<AFNInfo> lista;
    for (const auto& par : afns) {
        lista.push_back(par.second);
    }
    return lista;
}

AFD* AFNManager::crearAFDDesdeSeleccion(std::vector<int> idsSeleccionados) {
    if (idsSeleccionados.empty()) {
        return nullptr;
    }
    
    // Si solo hay uno, convertir directamente
    if (idsSeleccionados.size() == 1) {
        AFN* afn = obtenerAFN(idsSeleccionados[0]);
        if (afn) {
            return afn->ConvertirAFD();
        }
        return nullptr;
    }
    
    // Si hay múltiples, crear una copia del primero y unir el resto
    AFN* afnCombinado = nullptr;
    
    for (size_t i = 0; i < idsSeleccionados.size(); i++) {
        AFN* afnOriginal = obtenerAFN(idsSeleccionados[i]);
        if (!afnOriginal) continue;
        
        // Crear copia del AFN (simplificado: crear nuevo y copiar estructura)
        // NOTA: Esto requeriría un método clone() en AFN
        // Por ahora, asumimos que el usuario no necesita los AFNs originales después
        
        if (i == 0) {
            afnCombinado = afnOriginal;
        } else {
            afnCombinado = afnCombinado->unirAFN(afnOriginal);
        }
    }
    
    if (afnCombinado) {
        return afnCombinado->ConvertirAFD();
    }
    
    return nullptr;
}

void AFNManager::limpiarTodos() {
    for (auto& par : afns) {
        delete par.second.afn;
    }
    afns.clear();
}

int AFNManager::cantidadAFNs() const {
    return afns.size();
}

bool AFNManager::existe(int id) const {
    return afns.find(id) != afns.end();
}