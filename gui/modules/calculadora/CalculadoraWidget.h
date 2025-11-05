#ifndef CALCULADORAWIDGET_H
#define CALCULADORAWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>

#include "Calculadora.h"

class CalculadoraWidget : public QWidget {
    Q_OBJECT

public:
    explicit CalculadoraWidget(QWidget *parent = nullptr);
    ~CalculadoraWidget();

private slots:
    void onCalcular();
    void onLimpiar();

private:
    void setupUI();
    
    Calculadora* calculadora;
    
    QLineEdit* txtExpresion;
    QPushButton* btnCalcular;
    QPushButton* btnLimpiar;
    QLabel* lblResultado;
    QTextEdit* txtPostfija;
};

#endif // CALCULADORAWIDGET_H