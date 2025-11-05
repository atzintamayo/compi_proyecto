#include "AFDTableWidget.h"
#include <QHeaderView>

AFDTableWidget::AFDTableWidget(QWidget *parent) : QTableWidget(parent) {
    horizontalHeader()->setStretchLastSection(false);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

void AFDTableWidget::mostrarAFD(AFD* afd) {
    if (!afd) return;
    
    limpiar();
    
    // Configurar columnas: Estado + cada símbolo del alfabeto + Aceptación
    std::vector<char> simbolos;
    for (char c : afd->alfabeto) {
        if (c != EPSILON) simbolos.push_back(c);
    }
    
    setColumnCount(simbolos.size() + 2);
    
    QStringList headers;
    headers << "Estado";
    for (char c : simbolos) {
        headers << QString(c);
    }
    headers << "Aceptación";
    setHorizontalHeaderLabels(headers);
    
    // Llenar filas
    setRowCount(afd->NumEdos);
    for (int i = 0; i < afd->NumEdos; i++) {
        // Nombre del estado
        QString nombre = QString::number(i);
        if (afd->EdosAFD[i]->esAceptacion) nombre += "*";
        setItem(i, 0, new QTableWidgetItem(nombre));
        
        // Transiciones
        int col = 1;
        for (char c : simbolos) {
            int destino = afd->EdosAFD[i]->TransAFD[(unsigned char)c];
            QString valor = (destino == -1) ? "-" : QString::number(destino);
            setItem(i, col, new QTableWidgetItem(valor));
            col++;
        }
        
        // Aceptación y token
        QString acepta = afd->EdosAFD[i]->esAceptacion ? 
            QString("Sí (token:%1)").arg(afd->EdosAFD[i]->token) : "No";
        setItem(i, col, new QTableWidgetItem(acepta));
    }
    
    resizeColumnsToContents();
}

void AFDTableWidget::limpiar() {
    setRowCount(0);
    setColumnCount(0);
}