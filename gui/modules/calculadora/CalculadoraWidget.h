#ifndef CALCULADORAWIDGET_H
#define CALCULADORAWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "Calculadora.h"

class CalculadoraWidget : public QWidget {
    Q_OBJECT

public:
    explicit CalculadoraWidget(QWidget *parent = nullptr);
    ~CalculadoraWidget();

private slots:
    void onCalcularClicked();  // Cambiado para coincidir con .cpp

private:
    Calculadora* calculadora;
    
    // No necesitas declarar los widgets como miembros si usas findChild
    // Pero es mejor práctica declararlos:
    QLineEdit* inputField;
    QLabel* resultadoLabel;
    QLabel* rpnLabel;
};

#endif // CALCULADORAWIDGET_H
