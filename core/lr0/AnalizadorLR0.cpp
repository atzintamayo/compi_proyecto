#include "AnalizadorLR0.h"
#include <sstream>
#include <queue>
#include <algorithm>

AnalizadorLR0::AnalizadorLR0(LR0::GramaticaLR0* gram)  // ← Cambiar tipo
    : gramatica(gram), idAutGram(0), TotSimbolos(0), NumRenglonesIrA(0) {
    if (gramatica) {
        Gram = gramatica->toString();
    }
}

AnalizadorLR0::~AnalizadorLR0() {
    // La gramática no se elimina aquí (es manejada externamente)
}

void AnalizadorLR0::AnalizarGramatica_Y_PrepararSimbolos() {
    Vt_Vn.clear();
    V.clear();
    
    if (!gramatica) return;
    
    // 1. Agregar terminales
    for (const auto& t : gramatica->getTerminales()) {
        Vt_Vn.push_back(t.nombre);
        V.push_back(Simb(t.nombre, true, -1));
    }
    
    // 2. Agregar símbolo $
    Vt_Vn.push_back("$");
    V.push_back(Simb("$", true, -999)); // ID especial para $
    
    // 3. Agregar no terminales
    for (const auto& nt : gramatica->getNoTerminales()) {
        Vt_Vn.push_back(nt.nombre);
        V.push_back(Simb(nt.nombre, false, -1));
    }
    
    TotSimbolos = V.size();
}

std::string AnalizadorLR0::obtenerSimboloEnPosicion(int numRegla, int pos) const {
    if (!gramatica || numRegla < 0 || numRegla >= (int)gramatica->getProducciones().size()) {
        return "";
    }
    
    const auto& produccion = gramatica->getProducciones()[numRegla];
    
    if (pos < 0 || pos >= (int)produccion.derecha.size()) {
        return "";
    }
    
    return produccion.derecha[pos].nombre;
}

bool AnalizadorLR0::itemTienePuntoAlFinal(const ItemLR0& item) const {
    if (!gramatica || item.NumRegla < 0 || 
        item.NumRegla >= (int)gramatica->getProducciones().size()) {
        return false;
    }
    
    const auto& produccion = gramatica->getProducciones()[item.NumRegla];
    return item.PosPunto >= (int)produccion.derecha.size();
}

std::string AnalizadorLR0::conjuntoItemsToString(const std::set<ItemLR0>& conjunto) const {
    if (!gramatica) return "{}";
    
    std::ostringstream oss;
    oss << "{ ";
    
    bool primero = true;
    for (const auto& item : conjunto) {
        if (!primero) oss << ", ";
        
        const auto& prod = gramatica->getProducciones()[item.NumRegla];
        oss << prod.izquierda.nombre << " -> ";
        
        for (size_t i = 0; i < prod.derecha.size(); i++) {
            if ((int)i == item.PosPunto) {
                oss << "• ";
            }
            oss << prod.derecha[i].nombre << " ";
        }
        
        if (item.PosPunto >= (int)prod.derecha.size()) {
            oss << "•";
        }
        
        primero = false;
    }
    
    oss << " }";
    return oss.str();
}

int AnalizadorLR0::buscarEstadoConConjunto(const std::set<ItemLR0>& conjunto) const {
    for (const auto& estado : ColeccionCanonica) {
        if (estado.Sj == conjunto) {
            return estado.j;
        }
    }
    return -1;
}

// Continuación de AnalizadorLR0.cpp

std::set<ItemLR0> AnalizadorLR0::Cerradura_LR0(const std::set<ItemLR0>& conjItems) {
    std::set<ItemLR0> cerradura = conjItems;
    bool cambio = true;
    
    while (cambio) {
        cambio = false;
        std::set<ItemLR0> nuevosItems;
        
        for (const auto& item : cerradura) {
            // Obtener símbolo después del punto
            std::string simboloDespuesPunto = obtenerSimboloEnPosicion(item.NumRegla, item.PosPunto);
            
            if (simboloDespuesPunto.empty()) {
                continue; // Punto al final
            }
            
            // Verificar si es no terminal
            bool esNoTerminal = false;
            for (const auto& s : V) {
                if (s.Simbolo == simboloDespuesPunto && !s.EsTerminal) {
                    esNoTerminal = true;
                    break;
                }
            }
            
            if (!esNoTerminal) {
                continue;
            }
            
            // Agregar todas las producciones que tengan ese no terminal en el lado izquierdo
            const auto& producciones = gramatica->getProducciones();
            for (size_t i = 0; i < producciones.size(); i++) {
                if (producciones[i].izquierda.nombre == simboloDespuesPunto) {
                    ItemLR0 nuevoItem(i, 0); // A -> • α
                    
                    if (cerradura.find(nuevoItem) == cerradura.end()) {
                        nuevosItems.insert(nuevoItem);
                        cambio = true;
                    }
                }
            }
        }
        
        // Agregar nuevos ítems a la cerradura
        for (const auto& item : nuevosItems) {
            cerradura.insert(item);
        }
    }
    
    return cerradura;
}

std::set<ItemLR0> AnalizadorLR0::IrA_LR0(const std::set<ItemLR0>& conjunto, const std::string& simbolo) {
    std::set<ItemLR0> resultado;
    
    // Para cada ítem en el conjunto
    for (const auto& item : conjunto) {
        // Obtener símbolo después del punto
        std::string simboloDespuesPunto = obtenerSimboloEnPosicion(item.NumRegla, item.PosPunto);
        
        // Si coincide con el símbolo X, crear nuevo ítem moviendo el punto
        if (simboloDespuesPunto == simbolo) {
            ItemLR0 nuevoItem(item.NumRegla, item.PosPunto + 1);
            resultado.insert(nuevoItem);
        }
    }
    
    // Aplicar cerradura al resultado
    if (!resultado.empty()) {
        resultado = Cerradura_LR0(resultado);
    }
    
    return resultado;
}

// Continuación de AnalizadorLR0.cpp

void AnalizadorLR0::CrearTablaLR0() {
    RenglonesLR0.clear();
    ResultIrA.clear();
    ColeccionCanonica.clear();
    
    if (!gramatica || gramatica->getProducciones().empty()) {
        return;
    }
    
    // 1. Crear ítem inicial [S' -> • S]
    // Asumimos que la producción 0 es S' -> S (gramática ampliada)
    ItemLR0 itemInicial(0, 0);
    std::set<ItemLR0> conjInicial;
    conjInicial.insert(itemInicial);
    
    // 2. Aplicar cerradura para obtener S0
    std::set<ItemLR0> S0 = Cerradura_LR0(conjInicial);
    
    // 3. Inicializar colección canónica y cola
    std::queue<LR0_Conj_Sj> Q;
    
    LR0_Conj_Sj estado0;
    estado0.j = 0;
    estado0.Sj = S0;
    
    ColeccionCanonica.push_back(estado0);
    Q.push(estado0);
    
    // 4. Algoritmo de construcción de la colección canónica
    while (!Q.empty()) {
        LR0_Conj_Sj estadoActual = Q.front();
        Q.pop();
        
        // Crear renglón para este estado
        RenglonLR0 reng(estadoActual.j, TotSimbolos);
        
        // Para cada símbolo en Vt_Vn
        for (int i = 0; i < TotSimbolos; i++) {
            std::string simbolo = V[i].Simbolo;
            
            // Calcular IrA
            std::set<ItemLR0> SjAux = IrA_LR0(estadoActual.Sj, simbolo);
            
            if (SjAux.empty()) {
                continue; // No hay transición con este símbolo
            }
            
            // Buscar si este conjunto ya existe
            int estadoDestino = buscarEstadoConConjunto(SjAux);
            
            if (estadoDestino == -1) {
                // Nuevo estado
                estadoDestino = ColeccionCanonica.size();
                
                LR0_Conj_Sj nuevoEstado;
                nuevoEstado.j = estadoDestino;
                nuevoEstado.Sj = SjAux;
                
                ColeccionCanonica.push_back(nuevoEstado);
                Q.push(nuevoEstado);
            }
            
            // Registrar transición en ResultIrA
            Inf_IrA info;
            info.Si = estadoActual.j;
            info.IrA_Sj = estadoDestino;
            info.IrA_Simbolo = simbolo;
            info.ConjuntoItems = conjuntoItemsToString(SjAux);
            ResultIrA.push_back(info);
            
            // Actualizar tabla: desplazamiento/IrA
            reng.Acciones[i] = ElemTablaLR0(true, estadoDestino);
        }
        
        // Analizar ítems para reducciones y aceptación
        for (const auto& item : estadoActual.Sj) {
            if (!itemTienePuntoAlFinal(item)) {
                continue; // No es reducción
            }
            
            const auto& prod = gramatica->getProducciones()[item.NumRegla];
            
            // Verificar si es S' -> S • (aceptación)
            if (item.NumRegla == 0 && prod.izquierda.nombre.find("'") != std::string::npos) {
                // Marcar aceptación en columna de $
                int colDolar = -1;
                for (int i = 0; i < TotSimbolos; i++) {
                    if (V[i].Simbolo == "$") {
                        colDolar = i;
                        break;
                    }
                }
                
                if (colDolar != -1) {
                    reng.Acciones[colDolar] = ElemTablaLR0(false, -2); // Aceptación
                }
            } else {
                // Reducción A -> α •
                // En LR(0) puro: marcar reducción en TODAS las columnas de terminales
                for (int i = 0; i < TotSimbolos; i++) {
                    if (V[i].EsTerminal) {
                        // Solo marcar si la celda está vacía (para evitar conflictos)
                        if (reng.Acciones[i].esVacio()) {
                            reng.Acciones[i] = ElemTablaLR0(false, item.NumRegla);
                        }
                    }
                }
            }
        }
        
        RenglonesLR0.push_back(reng);
    }
    
    NumRenglonesIrA = ResultIrA.size();
}

// Continuación de AnalizadorLR0.cpp

std::vector<AnalizadorLR0::PasoLR0> AnalizadorLR0::AnalizarCadenaLR0(
    const std::vector<std::string>& tokens) {
    
    std::vector<PasoLR0> pasos;
    
    if (RenglonesLR0.empty()) {
        pasos.push_back(PasoLR0("", "", "ERROR: Tabla LR(0) no construida"));
        return pasos;
    }
    
    // Agregar $ al final de los tokens si no está
    std::vector<std::string> entrada = tokens;
    if (entrada.empty() || entrada.back() != "$") {
        entrada.push_back("$");
    }
    
    // Pila de estados
    std::vector<int> pilaEstados;
    pilaEstados.push_back(0); // Estado inicial
    
    // Pila de símbolos (para visualización)
    std::vector<std::string> pilaSimbolos;
    pilaSimbolos.push_back("$");
    
    size_t indiceEntrada = 0;
    
    // Paso inicial
    std::ostringstream ossPila, ossEntrada;
    ossPila << "0";
    for (size_t i = 0; i < entrada.size(); i++) {
        ossEntrada << entrada[i];
        if (i < entrada.size() - 1) ossEntrada << " ";
    }
    pasos.push_back(PasoLR0(ossPila.str(), ossEntrada.str(), "Inicio"));
    
    // Algoritmo LR(0)
    while (true) {
        int estadoActual = pilaEstados.back();
        std::string tokenActual = entrada[indiceEntrada];
        
        // Buscar índice del token en V
        int indiceToken = -1;
        for (int i = 0; i < TotSimbolos; i++) {
            if (V[i].Simbolo == tokenActual) {
                indiceToken = i;
                break;
            }
        }
        
        if (indiceToken == -1) {
            // Token no reconocido
            std::ostringstream ossAccion;
            ossAccion << "ERROR: Token '" << tokenActual << "' no reconocido";
            
            ossPila.str("");
            for (size_t i = 0; i < pilaEstados.size(); i++) {
                ossPila << pilaEstados[i];
                if (i < pilaSimbolos.size()) {
                    ossPila << " " << pilaSimbolos[i];
                }
                if (i < pilaEstados.size() - 1) ossPila << " ";
            }
            
            ossEntrada.str("");
            for (size_t i = indiceEntrada; i < entrada.size(); i++) {
                ossEntrada << entrada[i];
                if (i < entrada.size() - 1) ossEntrada << " ";
            }
            
            pasos.push_back(PasoLR0(ossPila.str(), ossEntrada.str(), ossAccion.str()));
            break;
        }
        
        // Obtener acción de la tabla
        ElemTablaLR0 accion = RenglonesLR0[estadoActual].Acciones[indiceToken];
        
        if (accion.esVacio()) {
            // Error: no hay acción
            std::ostringstream ossAccion;
            ossAccion << "ERROR: No hay acción en M[" << estadoActual << ", " << tokenActual << "]";
            
            ossPila.str("");
            for (size_t i = 0; i < pilaEstados.size(); i++) {
                ossPila << pilaEstados[i];
                if (i < pilaSimbolos.size()) {
                    ossPila << " " << pilaSimbolos[i];
                }
                if (i < pilaEstados.size() - 1) ossPila << " ";
            }
            
            ossEntrada.str("");
            for (size_t i = indiceEntrada; i < entrada.size(); i++) {
                ossEntrada << entrada[i];
                if (i < entrada.size() - 1) ossEntrada << " ";
            }
            
            pasos.push_back(PasoLR0(ossPila.str(), ossEntrada.str(), ossAccion.str()));
            break;
        }
        
        if (accion.esAceptacion()) {
            // ¡Aceptar!
            ossPila.str("");
            for (size_t i = 0; i < pilaEstados.size(); i++) {
                ossPila << pilaEstados[i];
                if (i < pilaSimbolos.size()) {
                    ossPila << " " << pilaSimbolos[i];
                }
                if (i < pilaEstados.size() - 1) ossPila << " ";
            }
            
            pasos.push_back(PasoLR0(ossPila.str(), "$", "✓ ACEPTAR"));
            break;
        }
        
        if (accion.EsDesplazamiento) {
            // Desplazamiento
            int estadoDestino = accion.NumEstadoDestino;
            
            pilaSimbolos.push_back(tokenActual);
            pilaEstados.push_back(estadoDestino);
            indiceEntrada++;
            
            std::ostringstream ossAccion;
            ossAccion << "d" << estadoDestino;
            
            ossPila.str("");
            for (size_t i = 0; i < pilaEstados.size(); i++) {
                ossPila << pilaEstados[i];
                if (i < pilaSimbolos.size()) {
                    ossPila << " " << pilaSimbolos[i];
                }
                if (i < pilaEstados.size() - 1) ossPila << " ";
            }
            
            ossEntrada.str("");
            for (size_t i = indiceEntrada; i < entrada.size(); i++) {
                ossEntrada << entrada[i];
                if (i < entrada.size() - 1) ossEntrada << " ";
            }
            
            pasos.push_back(PasoLR0(ossPila.str(), ossEntrada.str(), ossAccion.str()));
            
        } else {
            // Reducción
            int numProduccion = accion.NumEstadoDestino;
            
            if (numProduccion < 0 || numProduccion >= (int)gramatica->getProducciones().size()) {
                pasos.push_back(PasoLR0("", "", "ERROR: Producción inválida"));
                break;
            }
            
            const auto& prod = gramatica->getProducciones()[numProduccion];
            int longitudDerecha = prod.derecha.size();
            
            // Desapilar 2*|β| elementos (estados y símbolos)
            for (int i = 0; i < longitudDerecha; i++) {
                if (!pilaEstados.empty()) pilaEstados.pop_back();
                if (!pilaSimbolos.empty()) pilaSimbolos.pop_back();
            }
            
            // Estado en el tope después de desapilar
            int estadoTope = pilaEstados.back();
            
            // Buscar índice del no terminal A
            int indiceA = -1;
            for (int i = 0; i < TotSimbolos; i++) {
                if (V[i].Simbolo == prod.izquierda.nombre) {
                    indiceA = i;
                    break;
                }
            }
            
            if (indiceA == -1) {
                pasos.push_back(PasoLR0("", "", "ERROR: No terminal no encontrado"));
                break;
            }
            
            // IrA(estadoTope, A)
            ElemTablaLR0 accionIrA = RenglonesLR0[estadoTope].Acciones[indiceA];
            
            if (accionIrA.esVacio()) {
                pasos.push_back(PasoLR0("", "", "ERROR: No hay IrA para no terminal"));
                break;
            }
            
            int estadoIrA = accionIrA.NumEstadoDestino;
            
            pilaSimbolos.push_back(prod.izquierda.nombre);
            pilaEstados.push_back(estadoIrA);
            
            std::ostringstream ossAccion;
            ossAccion << "r" << numProduccion << " (" << prod.toString() << ")";
            
            ossPila.str("");
            for (size_t i = 0; i < pilaEstados.size(); i++) {
                ossPila << pilaEstados[i];
                if (i < pilaSimbolos.size()) {
                    ossPila << " " << pilaSimbolos[i];
                }
                if (i < pilaEstados.size() - 1) ossPila << " ";
            }
            
            ossEntrada.str("");
            for (size_t i = indiceEntrada; i < entrada.size(); i++) {
                ossEntrada << entrada[i];
                if (i < entrada.size() - 1) ossEntrada << " ";
            }
            
            pasos.push_back(PasoLR0(ossPila.str(), ossEntrada.str(), ossAccion.str()));
        }
    }
    
    return pasos;
}