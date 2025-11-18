#include "CalculadoraWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QString>

CalculadoraWidget::CalculadoraWidget(QWidget *parent)
    : QWidget(parent), calculadora(new Calculadora())
{
    auto *layout = new QVBoxLayout(this);

    auto *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(new QLabel("Expresión:"));
    QLineEdit *inputField = new QLineEdit();
    inputField->setObjectName("inputField");
    inputLayout->addWidget(inputField);

    layout->addLayout(inputLayout);

    QPushButton *btnCalcular = new QPushButton("Calcular");
    layout->addWidget(btnCalcular);

    auto *resultLayout = new QVBoxLayout();
    QLabel *resultadoLabel = new QLabel("Resultado: ");
    resultadoLabel->setObjectName("resultadoLabel");

    QLabel *rpnLabel = new QLabel("Postfija: ");
    rpnLabel->setObjectName("rpnLabel");

    resultLayout->addWidget(resultadoLabel);
    resultLayout->addWidget(rpnLabel);

    layout->addLayout(resultLayout);

    connect(btnCalcular, &QPushButton::clicked, this, &CalculadoraWidget::onCalcularClicked);
}

CalculadoraWidget::~CalculadoraWidget() {
    delete calculadora;
}

void CalculadoraWidget::onCalcularClicked() {
    QLineEdit *inputField = findChild<QLineEdit*>("inputField");
    QLabel *resultadoLabel = findChild<QLabel*>("resultadoLabel");
    QLabel *rpnLabel = findChild<QLabel*>("rpnLabel");

    QString expr = inputField->text();

    try {
        std::string infija = expr.toStdString();

        // ✔ core actual
        std::string rpn = calculadora->obtenerNotacionPostfija(infija);
        double resultado = calculadora->evaluar(infija);

        resultadoLabel->setText("Resultado: " + QString::number(resultado));
        rpnLabel->setText("Postfija: " + QString::fromStdString(rpn));

    } catch (const std::exception& e) {
        resultadoLabel->setText("Error: " + QString(e.what()));
        rpnLabel->setText("Postfija: -");
    }
}
