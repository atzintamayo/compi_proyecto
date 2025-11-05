#include "CalculadoraWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

CalculadoraWidget::CalculadoraWidget(QWidget *parent) : QWidget(parent) {
    calculadora = new Calculadora();
    setupUI();
}

CalculadoraWidget::~CalculadoraWidget() {
    delete calculadora;
}

void CalculadoraWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Título
    QLabel* titulo = new QLabel("Calculadora con Recursividad por la Izquierda");
    QFont fuenteTitulo;
    fuenteTitulo.setPointSize(18);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);
    titulo->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titulo);
    
    mainLayout->addSpacing(20);
    
    // Grupo: Entrada
    QGroupBox* grupoEntrada = new QGroupBox("Expresión");
    QVBoxLayout* layoutEntrada = new QVBoxLayout();
    
    txtExpresion = new QLineEdit();
    txtExpresion->setPlaceholderText("Ejemplo: 3 + 5 * 2");
    txtExpresion->setFont(QFont("Courier", 14));
    layoutEntrada->addWidget(txtExpresion);
    
    QHBoxLayout* layoutBotones = new QHBoxLayout();
    btnCalcular = new QPushButton("Calcular");
    btnLimpiar = new QPushButton("Limpiar");
    
    connect(btnCalcular, &QPushButton::clicked, this, &CalculadoraWidget::onCalcular);
    connect(btnLimpiar, &QPushButton::clicked, this, &CalculadoraWidget::onLimpiar);
    connect(txtExpresion, &QLineEdit::returnPressed, this, &CalculadoraWidget::onCalcular);
    
    layoutBotones->addWidget(btnCalcular);
    layoutBotones->addWidget(btnLimpiar);
    layoutEntrada->addLayout(layoutBotones);
    
    grupoEntrada->setLayout(layoutEntrada);
    mainLayout->addWidget(grupoEntrada);
    
    // Grupo: Resultado
    QGroupBox* grupoResultado = new QGroupBox("Resultado");
    QVBoxLayout* layoutResultado = new QVBoxLayout();
    
    lblResultado = new QLabel("---");
    lblResultado->setAlignment(Qt::AlignCenter);
    QFont fuenteResultado;
    fuenteResultado.setPointSize(24);
    fuenteResultado.setBold(true);
    lblResultado->setFont(fuenteResultado);
    lblResultado->setStyleSheet("color: #2196F3; padding: 20px;");
    layoutResultado->addWidget(lblResultado);
    
    grupoResultado->setLayout(layoutResultado);
    mainLayout->addWidget(grupoResultado);
    
    // Grupo: Notación Postfija
    QGroupBox* grupoPostfija = new QGroupBox("Notación Postfija");
    QVBoxLayout* layoutPostfija = new QVBoxLayout();
    
    txtPostfija = new QTextEdit();
    txtPostfija->setReadOnly(true);
    txtPostfija->setMaximumHeight(100);
    txtPostfija->setFont(QFont("Courier", 12));
    txtPostfija->setPlaceholderText("La notación postfija aparecerá aquí...");
    layoutPostfija->addWidget(txtPostfija);
    
    grupoPostfija->setLayout(layoutPostfija);
    mainLayout->addWidget(grupoPostfija);
    
    mainLayout->addStretch();
    
    // Nota informativa
    QLabel* nota = new QLabel(
        "Nota: Esta calculadora evalúa expresiones usando recursividad por la izquierda.\n"
        "Soporta operadores: +, -, *, /, ^ (potencia) y paréntesis."
    );
    nota->setStyleSheet("color: gray; font-style: italic;");
    nota->setWordWrap(true);
    mainLayout->addWidget(nota);
}

void CalculadoraWidget::onCalcular() {
    QString expresion = txtExpresion->text().trimmed();
    
    if (expresion.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "Ingrese una expresión para calcular");
        return;
    }
    
    try {
        // TODO: Implementar la lógica de evaluación
        double resultado = calculadora->evaluar(expresion.toStdString());
        std::string postfija = calculadora->obtenerNotacionPostfija(expresion.toStdString());
        
        lblResultado->setText(QString::number(resultado));
        txtPostfija->setPlainText(QString::fromStdString(postfija));
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Error al evaluar la expresión:\n%1").arg(e.what()));
        lblResultado->setText("Error");
        txtPostfija->clear();
    }
}

void CalculadoraWidget::onLimpiar() {
    txtExpresion->clear();
    lblResultado->setText("---");
    txtPostfija->clear();
}