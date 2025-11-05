#include "AFNTableWidget.h"
#include <QHeaderView>

AFNTableWidget::AFNTableWidget(QWidget *parent) : QTableWidget(parent) {
    setColumnCount(4);
    setHorizontalHeaderLabels({"Origen", "Símbolo/Rango", "Destino", "¿Aceptación?"});
    horizontalHeader()->setStretchLastSection(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

void AFNTableWidget::mostrarAFN(AFN* afn) {
    if (!afn) return;
    
    limpiar();
    
    int row = 0;
    for (Estado* e : afn->estados) {
        for (Transition* t : e->transiciones) {
            insertRow(row);
            
            setItem(row, 0, new QTableWidgetItem(QString::number(e->idEdo)));
            
            QString simbolo;
            if (t->simboloInf == EPSILON) {
                simbolo = "ε";
            } else if (t->simboloInf == t->simboloSup) {
                simbolo = QString(t->simboloInf);
            } else {
                simbolo = QString("[%1-%2]").arg(t->simboloInf).arg(t->simboloSup);
            }
            setItem(row, 1, new QTableWidgetItem(simbolo));
            
            setItem(row, 2, new QTableWidgetItem(QString::number(t->edoDestino->idEdo)));
            
            QString acepta = afn->edosAcept.contiene(t->edoDestino) ? 
                QString("Sí (token:%1)").arg(t->edoDestino->token) : "No";
            setItem(row, 3, new QTableWidgetItem(acepta));
            
            row++;
        }
    }
    
    resizeColumnsToContents();
}

void AFNTableWidget::limpiar() {
    setRowCount(0);
}