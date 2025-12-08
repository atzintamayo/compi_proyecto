#include "CalculadoraWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QFont>
#include <QGroupBox>
#include <QTextEdit>

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
    
    // Subtítulo
    QLabel *subtitulo = new QLabel("Recursive Descent Parser + Notación Postfija (RPN)");
    subtitulo->setStyleSheet("QLabel { color: #666; font-style: italic; }");
    subtitulo->setAlignment(Qt::AlignCenter);
    subtitulo->setWordWrap(true);
    mainLayout->addWidget(subtitulo);
    
    // Grupo de entrada
    QGroupBox *grupoEntrada = new QGroupBox("Expresión Matemática");
    auto *entradaLayout = new QVBoxLayout();
    
    auto *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(new QLabel("Expresión:"));
    inputField = new QLineEdit();
    inputField->setPlaceholderText("Ej: 2+3*4, sin(90), (5+3)^2, -5+10");
    inputField->setFont(QFont("Monospace", 11));
    inputLayout->addWidget(inputField);
    
    entradaLayout->addLayout(inputLayout);
    
    // Botón calcular
    QPushButton *btnCalcular = new QPushButton("🧮 Calcular");
    btnCalcular->setMinimumHeight(40);
    btnCalcular->setStyleSheet(
        "QPushButton {"
        "  background-color: #4CAF50;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 5px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #3d8b40;"
        "}"
    );
    entradaLayout->addWidget(btnCalcular);
    
    grupoEntrada->setLayout(entradaLayout);
    mainLayout->addWidget(grupoEntrada);
    
    // Grupo de resultados
    QGroupBox *grupoResultados = new QGroupBox("Resultados");
    grupoResultados->setStyleSheet(
        "QGroupBox {"
        "  border: 2px solid #4CAF50;"
        "  border-radius: 8px;"
        "  margin-top: 10px;"
        "  padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 10px;"
        "  padding: 0 5px;"
        "  color: #4CAF50;"
        "  font-weight: bold;"
        "}"
    );
    auto *resultLayout = new QVBoxLayout();
    
    // Resultado numérico
    resultadoLabel = new QLabel("Resultado: -");
    resultadoLabel->setStyleSheet(
        "QLabel { "
        "  padding: 15px; "
        "  background-color: #e8f5e9; "
        "  border: 2px solid #4CAF50; "
        "  border-radius: 5px; "
        "  font-size: 16px;"
        "}"
    );
    QFont fuenteResultado;
    fuenteResultado.setPointSize(13);
    fuenteResultado.setBold(true);
    resultadoLabel->setFont(fuenteResultado);
    
    // Notación postfija
    rpnLabel = new QLabel("Notación Postfija (RPN): -");
    rpnLabel->setStyleSheet(
        "QLabel { "
        "  padding: 12px; "
        "  background-color: #f5f5f5; "
        "  border: 1px solid #ccc; "
        "  border-radius: 5px; "
        "  font-family: 'Courier New', monospace;"
        "  font-size: 12px;"
        "}"
    );
    rpnLabel->setWordWrap(true);
    
    resultLayout->addWidget(resultadoLabel);
    resultLayout->addWidget(rpnLabel);
    
    grupoResultados->setLayout(resultLayout);
    mainLayout->addWidget(grupoResultados);
    
    // Grupo de gramática original
    QGroupBox *grupoGramaticaOriginal = new QGroupBox("Gramática Original (con recursión izquierda)");
    grupoGramaticaOriginal->setStyleSheet(
        "QGroupBox {"
        "  border: 1px solid #ff6f00;"
        "  border-radius: 5px;"
        "  margin-top: 10px;"
        "  padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "  color: #ff6f00;"
        "  font-weight: bold;"
        "}"
    );
    auto *gramaticaOrigLayout = new QVBoxLayout();
    
    QTextEdit *gramaticaOrig = new QTextEdit();
    gramaticaOrig->setReadOnly(true);
    gramaticaOrig->setMaximumHeight(100);
    gramaticaOrig->setStyleSheet("QTextEdit { background-color: #fff3e0; border: none; }");
    gramaticaOrig->setHtml(
        "<div style='font-family: monospace; font-size: 12px;'>"
        "<b style='color: #e65100;'>Gramática con recursión por la izquierda:</b><br>"
        "<span style='color: #d32f2f;'>E → E + T | E - T | T</span><br>"
        "<span style='color: #1976d2;'>T → T * P | T / P | P</span><br>"
        "<span style='color: #388e3c;'>P → P ^ F | F</span><br>"
        "<span style='color: #7b1fa2;'>F → (E) | FUNC(E) | NUMBER</span><br>"
        "<span style='color: #666;'>FUNC → sin | cos | tan | asin | acos | atan | log | ln | exp</span>"
        "</div>"
    );
    gramaticaOrigLayout->addWidget(gramaticaOrig);
    grupoGramaticaOriginal->setLayout(gramaticaOrigLayout);
    mainLayout->addWidget(grupoGramaticaOriginal);
    
    // Grupo de gramática transformada
    QGroupBox *grupoGramaticaTrans = new QGroupBox("Gramática Transformada (sin recursión izquierda)");
    grupoGramaticaTrans->setStyleSheet(
        "QGroupBox {"
        "  border: 1px solid #2e7d32;"
        "  border-radius: 5px;"
        "  margin-top: 10px;"
        "  padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "  color: #2e7d32;"
        "  font-weight: bold;"
        "}"
    );
    auto *gramaticaTransLayout = new QVBoxLayout();
    
    QTextEdit *gramaticaTrans = new QTextEdit();
    gramaticaTrans->setReadOnly(true);
    gramaticaTrans->setMaximumHeight(110);
    gramaticaTrans->setStyleSheet("QTextEdit { background-color: #e8f5e9; border: none; }");
    gramaticaTrans->setHtml(
        "<div style='font-family: monospace; font-size: 11px;'>"
        "<b style='color: #1b5e20;'>Transformación para eliminar recursión izquierda:</b><br>"
        "<span style='color: #d32f2f;'>E  → T E'</span><br>"
        "<span style='color: #d32f2f;'>E' → + T E' | - T E' | ε</span><br>"
        "<span style='color: #1976d2;'>T  → P T'</span><br>"
        "<span style='color: #1976d2;'>T' → * P T' | / P T' | ε</span><br>"
        "<span style='color: #388e3c;'>P  → F P'</span><br>"
        "<span style='color: #388e3c;'>P' → ^ F P' | ε</span> <i style='color: #666;'>(nota: ^ es asociativa derecha)</i><br>"
        "<span style='color: #7b1fa2;'>F  → (E) | FUNC(E) | NUMBER</span>"
        "</div>"
    );
    gramaticaTransLayout->addWidget(gramaticaTrans);
    grupoGramaticaTrans->setLayout(gramaticaTransLayout);
    mainLayout->addWidget(grupoGramaticaTrans);
    
    // Información de funciones
    QGroupBox *grupoFunciones = new QGroupBox("Funciones y Operadores Soportados");
    auto *funcionesLayout = new QVBoxLayout();
    
    QLabel *infoFunciones = new QLabel(
        "<b>Operadores:</b><br>"
        "• <code>+</code> <code>-</code> (suma, resta) - Recursión izquierda en E<br>"
        "• <code>*</code> <code>/</code> (multiplicación, división) - Recursión izquierda en T<br>"
        "• <code>^</code> (potencia) - Recursión izquierda en P, <i>pero asociativa derecha</i><br><br>"
        "<b>Funciones trigonométricas (entrada en grados):</b><br>"
        "• sin, cos, tan, asin (arcoseno), acos (arcocoseno), atan (arctangente)<br><br>"
        "<b>Logaritmos y exponencial:</b><br>"
        "• log (base 10), ln (natural), exp<br><br>"
        "<b>Otras:</b><br>"
        "• pi (constante π), sqrt (raíz cuadrada), abs (valor absoluto)<br><br>"
        "<b>Características:</b><br>"
        "• Decimales: 3.14<br>"
        "• Científica: 1.5e-3<br>"
        "• Unarios: -5, +3<br>"
        "• Precedencia: <code>( )</code> FUNC > <code>^</code> > <code>* /</code> > <code>+ -</code><br>"
        "• Asociatividad: potencia (derecha), resto (izquierda)"
    );
    infoFunciones->setStyleSheet("QLabel { padding: 10px; color: #444; }");
    infoFunciones->setWordWrap(true);
    funcionesLayout->addWidget(infoFunciones);
    
    grupoFunciones->setLayout(funcionesLayout);
    mainLayout->addWidget(grupoFunciones);
    
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
        resultadoLabel->setStyleSheet(
            "QLabel { "
            "  padding: 15px; "
            "  background-color: #f5f5f5; "
            "  border: 2px solid #ccc; "
            "  border-radius: 5px; "
            "  font-size: 16px;"
            "}"
        );
        rpnLabel->setText("Notación Postfija (RPN): -");
        return;
    }

    try {
        std::string infija = expr.toStdString();

        // Obtener notación postfija
        std::string rpn = calculadora->obtenerNotacionPostfija(infija);
        
        // Evaluar expresión
        double resultado = calculadora->evaluar(infija);

        // Mostrar resultado exitoso
        resultadoLabel->setText(
            QString("✓ Resultado: <span style='color: #2e7d32; font-size: 18px;'>%1</span>")
            .arg(resultado, 0, 'g', 12)
        );
        resultadoLabel->setStyleSheet(
            "QLabel { "
            "  padding: 15px; "
            "  background-color: #e8f5e9; "
            "  border: 2px solid #4CAF50; "
            "  border-radius: 5px; "
            "  font-size: 16px;"
            "}"
        );
        
        rpnLabel->setText(
            QString("📋 Notación Postfija (RPN): <span style='color: #1565c0; font-weight: bold;'>%1</span>")
            .arg(QString::fromStdString(rpn))
        );

    } catch (const std::exception& e) {
        resultadoLabel->setText("✗ <span style='color: #c62828; font-weight: bold;'>Error</span>");
        resultadoLabel->setStyleSheet(
            "QLabel { "
            "  padding: 15px; "
            "  background-color: #ffebee; "
            "  border: 2px solid #c62828; "
            "  border-radius: 5px; "
            "  font-size: 16px;"
            "}"
        );
        
        rpnLabel->setText(
            QString("⚠️ <b>Error:</b> <span style='color: #c62828;'>%1</span>")
            .arg(QString::fromUtf8(e.what()))
        );
    }
}
