#include "CalculadoraWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QFont>
#include <QGroupBox>

CalculadoraWidget::CalculadoraWidget(QWidget *parent)
    : QWidget(parent), calculadora(new Calculadora())
{
    // Layout principal
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Título
    QLabel *titulo = new QLabel("Calculadora con Recursión por la Izquierda");
    QFont fuenteTitulo;
    fuenteTitulo.setPointSize(16);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);
    titulo->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titulo);
    
    // Grupo de entrada
    QGroupBox *grupoEntrada = new QGroupBox("Expresión");
    auto *entradaLayout = new QVBoxLayout();
    
    auto *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(new QLabel("Expresión:"));
    inputField = new QLineEdit();
    inputField->setPlaceholderText("Ej: 2+3*4, sin(3.14/2), (5+3)^2");
    inputLayout->addWidget(inputField);
    
    entradaLayout->addLayout(inputLayout);
    
    // Botón calcular
    QPushButton *btnCalcular = new QPushButton("Calcular");
    btnCalcular->setMinimumHeight(35);
    entradaLayout->addWidget(btnCalcular);
    
    grupoEntrada->setLayout(entradaLayout);
    mainLayout->addWidget(grupoEntrada);
    
    // Grupo de resultados
        // Grupo de resultados
    QGroupBox *grupoResultados = new QGroupBox("Resultados");
    auto *resultLayout = new QVBoxLayout();

    // Estilo del groupbox (cuadro contenedor)
    grupoResultados->setStyleSheet(
        "QGroupBox {"
        "  border: 1px solid #444;"
        "  border-radius: 8px;"
        "  margin-top: 10px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 10px;"
        "  padding: 0 4px 0 4px;"
        "}"
    );

    // Resultado principal
    resultadoLabel = new QLabel("Resultado: -");
    resultadoLabel->setWordWrap(true);
    resultadoLabel->setStyleSheet(
        "QLabel {"
        "  padding: 10px 14px;"
        "  background-color: #262626;"   // gris oscuro para modo oscuro
        "  border-radius: 6px;"
        "  border: 1px solid #555;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
    );

    // RPN
    rpnLabel = new QLabel("Notación Postfija (RPN): -");
    rpnLabel->setWordWrap(true);
    rpnLabel->setStyleSheet(
        "QLabel {"
        "  margin-top: 6px;"
        "  padding: 8px 14px;"
        "  background-color: #262626;"
        "  border-radius: 6px;"
        "  border: 1px solid #555;"
        "  font-family: monospace;"
        "  font-size: 12px;"
        "}"
    );

    resultLayout->addWidget(resultadoLabel);
    resultLayout->addWidget(rpnLabel);

    grupoResultados->setLayout(resultLayout);
    mainLayout->addWidget(grupoResultados);

    // Información de funciones soportadas
    QLabel *infoFunciones = new QLabel(
        "<b>Funciones soportadas:</b><br>"
        "• Operadores: +, -, *, /, ^ (potencia)<br>"
        "• Trigonométricas: sin, cos, tan, asin, acos, atan<br>"
        "• Logaritmos: log (base 10), ln (natural)<br>"
        "• Otras: exp, pi<br>"
        "• Soporta números decimales y notación científica (ej: 1.5e-3)"
    );
    infoFunciones->setStyleSheet("QLabel { color: #666; padding: 10px; }");
    infoFunciones->setWordWrap(true);
    mainLayout->addWidget(infoFunciones);
    
    mainLayout->addStretch();
    
    // Conectar señales
    connect(btnCalcular, &QPushButton::clicked, this, &CalculadoraWidget::onCalcularClicked);
    connect(inputField, &QLineEdit::returnPressed, this, &CalculadoraWidget::onCalcularClicked);
}

CalculadoraWidget::~CalculadoraWidget() {
    delete calculadora;
}

void CalculadoraWidget::onCalcularClicked() {
    QString expr = inputField->text().trimmed();
    
    if (expr.isEmpty()) {
        resultadoLabel->setText("Resultado: -");
        rpnLabel->setText("Notación Postfija (RPN): -");
        return;
    }

    try {
        std::string infija = expr.toStdString();

        // Obtener notación postfija
        std::string rpn = calculadora->obtenerNotacionPostfija(infija);
        
        // Evaluar expresión
        double resultado = calculadora->evaluar(infija);

        // Mostrar resultados
        resultadoLabel->setText(QString("Resultado: <span style='color: green;'>%1</span>")
                                .arg(resultado, 0, 'g', 10));
        rpnLabel->setText(QString("Notación Postfija (RPN): <span style='font-family: monospace;'>%1</span>")
                         .arg(QString::fromStdString(rpn)));

    } catch (const std::exception& e) {
        resultadoLabel->setText(QString("<span style='color: red;'>Error: %1</span>")
                               .arg(QString::fromUtf8(e.what())));
        rpnLabel->setText("Notación Postfija (RPN): -");
    }
}
