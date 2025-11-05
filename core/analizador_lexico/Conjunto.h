#ifndef CONJUNTO_H
#define CONJUNTO_H

#include <set>
#include <algorithm>

template <typename T>
class Conjunto {
private:
    std::set<T> elementos;

public:
    Conjunto() {}
    
    void clear() {
        elementos.clear();
    }
    
    void add(const T& elemento) {
        elementos.insert(elemento);
    }
    
    void Union(const Conjunto<T>& otro) {
        for (const auto& elem : otro.elementos) {
            elementos.insert(elem);
        }
    }
    
    void Quitar(const T& elemento) {
        elementos.erase(elemento);
    }
    
    bool contiene(const T& elemento) const {
        return elementos.find(elemento) != elementos.end();
    }
    
    int size() const {
        return elementos.size();
    }
    
    bool empty() const {
        return elementos.empty();
    }
    
    // Iteradores para poder usar en for-each
    typename std::set<T>::iterator begin() {
        return elementos.begin();
    }
    
    typename std::set<T>::iterator end() {
        return elementos.end();
    }
    
    typename std::set<T>::const_iterator begin() const {
        return elementos.begin();
    }
    
    typename std::set<T>::const_iterator end() const {
        return elementos.end();
    }
    
    // Operador de comparación para usar en std::set
    bool operator<(const Conjunto<T>& otro) const {
        return elementos < otro.elementos;
    }
    
    bool operator==(const Conjunto<T>& otro) const {
        return elementos == otro.elementos;
    }
};

#endif // CONJUNTO_H