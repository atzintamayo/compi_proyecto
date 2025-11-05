#ifndef AFN_MANAGER_H
#define AFN_MANAGER_H

#include "AFN.h"
#include "AFD.h"
#include <map>
#include <string>
#include <vector>

// Estructura para almacenar información de un AFN
struct AFNInfo {
    int id;
    AFN* afn;
    std::string nombre;
    std::string descripcion;
    
    AFNInfo(int i, AFN* a, std::string nom, std::string desc = "") 
        : id(i), afn(a), nombre(nom), descripcion(desc) {}
};

// Gestor centralizado de AFNs
class AFNManager {
private:
    std::map<int, AFNInfo> afns;
    int contadorId;
    
public:
    AFNManager();
    ~AFNManager();
    
    // Agregar AFN al repositorio
    int agregarAFN(AFN* afn, std::string nombre, std::string descripcion = "");
    
    // Obtener AFN por ID
    AFN* obtenerAFN(int id);
    AFNInfo* obtenerInfo(int id);
    
    // Eliminar AFN
    bool eliminarAFN(int id);
    
    // Listar todos los AFNs
    std::vector<AFNInfo> listarAFNs() const;
    
    // Combinar múltiples AFNs seleccionados para crear AFD
    AFD* crearAFDDesdeSeleccion(std::vector<int> idsSeleccionados);
    
    // Limpiar todos
    void limpiarTodos();
    
    // Utilidades
    int cantidadAFNs() const;
    bool existe(int id) const;
};

#endif // AFN_MANAGER_H