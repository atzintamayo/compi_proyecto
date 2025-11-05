#ifndef AFNTABLEWIDGET_H
#define AFNTABLEWIDGET_H

#include <QTableWidget>
#include "AFN.h"

class AFNTableWidget : public QTableWidget {
    Q_OBJECT

public:
    explicit AFNTableWidget(QWidget *parent = nullptr);
    void mostrarAFN(AFN* afn);
    void limpiar();
};

#endif