#ifndef AFDTABLEWIDGET_H
#define AFDTABLEWIDGET_H

#include <QTableWidget>
#include "AFD.h"

class AFDTableWidget : public QTableWidget {
    Q_OBJECT

public:
    explicit AFDTableWidget(QWidget *parent = nullptr);
    void mostrarAFD(AFD* afd);
    void limpiar();
};

#endif