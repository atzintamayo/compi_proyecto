#include "LL1Widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>

LL1Widget::LL1Widget(QWidget *parent) : QWidget(parent) {
    gramatica = new Gramatica();
    lexer = new Lexer();
    conjuntos = nullptr;
    tabla = nullptr;
    parser = nullptr;
    
    setupUI();
}

LL1Widget::~LL1Widget() {
    delete gramatica;
    delete lexer;
    if (conjuntos) delete conjuntos;
    if (tabla) delete tabla;
    if (parser) delete parser;
}

void LL1Widget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Título
    QLabel* titulo = new QLabel("Analizador Sintáctico LL(1)");
    QFont font = titulo->font();
    font.setPointSize(16);
    font.setBold(true);
    titulo->setFont(font);
    titulo->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titulo);
    
    tabWidget = new QTabWidget();
    mainLayout->addWidget(tabWidget);
    
    setupTabGramatica();
    setupTabConjuntos();
    setupTabTabla();
    setupTabParser();
}

void LL1Widget::setupTabGramatica() {
    tabGramatica = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tabGramatica);
    
    QGroupBox* grupo = new QGroupBox("Editor de Gramática");
    QVBoxLayout* layoutGrupo = new QVBoxLayout();
    
    QLabel* info = new QLabel(
        "Formato: NoTerminal -> simbolo1 simbolo2 | alternativa2 ;\n"
        "Use 'epsilon' o 'ε' para producciones vacías.\n"
        "Los no terminales empiezan con mayúscula."
    );
    info->setWordWrap(true);
    info->setStyleSheet("color: gray; font-style: italic;");
    layoutGrupo->addWidget(info);
    
    txtGramatica = new QTextEdit();
    txtGramatica->setFont(QFont("Courier", 11));
    txtGramatica->setPlaceholderText("Ingrese la gramática aquí...");
    layoutGrupo->addWidget(txtGramatica);
    
    QHBoxLayout* layoutBotones = new QHBoxLayout();
    btnCargarGramatica = new QPushButton("Cargar Gramática");
    btnCargarEjemplo = new QPushButton("Cargar Ejemplo ER");
    btnLimpiarGramatica = new QPushButton("Limpiar");
    
    connect(btnCargarGramatica, &QPushButton::clicked, this, &LL1Widget::onCargarGramatica);
    connect(btnCargarEjemplo, &QPushButton::clicked, this, &LL1Widget::onCargarEjemplo);
    connect(btnLimpiarGramatica, &QPushButton::clicked, this, &LL1Widget::onLimpiarGramatica);
    
    layoutBotones->addWidget(btnCargarGramatica);
    layoutBotones->addWidget(btnCargarEjemplo);
    layoutBotones->addWidget(btnLimpiarGramatica);
    layoutGrupo->addLayout(layoutBotones);
    
    lblEstadoGramatica = new QLabel("Estado: Sin gramática cargada");
    lblEstadoGramatica->setStyleSheet("font-weight: bold;");
    layoutGrupo->addWidget(lblEstadoGramatica);
    
    grupo->setLayout(layoutGrupo);
    layout->addWidget(grupo);
    
    tabWidget->addTab(tabGramatica, "1. Gramática");
}

void LL1Widget::setupTabConjuntos() {
    tabConjuntos = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tabConjuntos);
    
    btnCalcularConjuntos = new QPushButton("Calcular FIRST y FOLLOW");
    btnCalcularConjuntos->setEnabled(false);
    connect(btnCalcularConjuntos, &QPushButton::clicked, this, &LL1Widget::onCalcularConjuntos);
    layout->addWidget(btnCalcularConjuntos);
    
    QHBoxLayout* layoutConjuntos = new QHBoxLayout();
    
    QGroupBox* grupoFirst = new QGroupBox("Conjuntos FIRST");
    QVBoxLayout* layoutFirst = new QVBoxLayout();
    txtFirst = new QTextEdit();
    txtFirst->setReadOnly(true);
    txtFirst->setFont(QFont("Courier", 10));
    layoutFirst->addWidget(txtFirst);
    grupoFirst->setLayout(layoutFirst);
    layoutConjuntos->addWidget(grupoFirst);
    
    QGroupBox* grupoFollow = new QGroupBox("Conjuntos FOLLOW");
    QVBoxLayout* layoutFollow = new QVBoxLayout();
    txtFollow = new QTextEdit();
    txtFollow->setReadOnly(true);
    txtFollow->setFont(QFont("Courier", 10));
    layoutFollow->addWidget(txtFollow);
    grupoFollow->setLayout(layoutFollow);
    layoutConjuntos->addWidget(grupoFollow);
    
    layout->addLayout(layoutConjuntos);
    
    tabWidget->addTab(tabConjuntos, "2. FIRST/FOLLOW");
}

void LL1Widget::setupTabTabla() {
    tabTabla = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tabTabla);
    
    btnConstruirTabla = new QPushButton("Construir Tabla LL(1)");
    btnConstruirTabla->setEnabled(false);
    connect(btnConstruirTabla, &QPushButton::clicked, this, &LL1Widget::onConstruirTabla);
    layout->addWidget(btnConstruirTabla);
    
    QGroupBox* grupoTabla = new QGroupBox("Tabla LL(1)");
    QVBoxLayout* layoutTabla = new QVBoxLayout();
    
    tablaLL1 = new QTableWidget();
    tablaLL1->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaLL1->horizontalHeader()->setStretchLastSection(true);
    layoutTabla->addWidget(tablaLL1);
    
    grupoTabla->setLayout(layoutTabla);
    layout->addWidget(grupoTabla);
    
    QGroupBox* grupoConflictos = new QGroupBox("Conflictos");
    QVBoxLayout* layoutConflictos = new QVBoxLayout();
    txtConflictos = new QTextEdit();
    txtConflictos->setReadOnly(true);
    txtConflictos->setMaximumHeight(100);
    layoutConflictos->addWidget(txtConflictos);
    grupoConflictos->setLayout(layoutConflictos);
    layout->addWidget(grupoConflictos);
    
    tabWidget->addTab(tabTabla, "3. Tabla LL(1)");
}

void LL1Widget::setupTabParser() {
    tabParser = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tabParser);
    
    QGroupBox* grupoEntrada = new QGroupBox("Cadena de Entrada");
    QVBoxLayout* layoutEntrada = new QVBoxLayout();
    
    txtEntrada = new QLineEdit();
    txtEntrada->setPlaceholderText("Ejemplo: a|b*");
    txtEntrada->setFont(QFont("Courier", 12));
    layoutEntrada->addWidget(txtEntrada);
    
    QHBoxLayout* layoutBotones = new QHBoxLayout();
    btnParsear = new QPushButton("Parsear");
    btnParsear->setEnabled(false);
    btnLimpiarParser = new QPushButton("Limpiar");
    
    connect(btnParsear, &QPushButton::clicked, this, &LL1Widget::onParsear);
    connect(btnLimpiarParser, &QPushButton::clicked, this, &LL1Widget::onLimpiarParser);
    connect(txtEntrada, &QLineEdit::returnPressed, this, &LL1Widget::onParsear);
    
    layoutBotones->addWidget(btnParsear);
    layoutBotones->addWidget(btnLimpiarParser);
    layoutEntrada->addLayout(layoutBotones);
    
    grupoEntrada->setLayout(layoutEntrada);
    layout->addWidget(grupoEntrada);
    
    lblResultado = new QLabel("Resultado: -");
    lblResultado->setStyleSheet("font-weight: bold; font-size: 14px;");
    layout->addWidget(lblResultado);
    
    QGroupBox* grupoTraza = new QGroupBox("Traza del Parser");
    QVBoxLayout* layoutTraza = new QVBoxLayout();
    
    tablaTraza = new QTableWidget();
    tablaTraza->setColumnCount(3);
    tablaTraza->setHorizontalHeaderLabels({"Pila", "Entrada", "Acción"});
    tablaTraza->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaTraza->horizontalHeader()->setStretchLastSection(true);
    layoutTraza->addWidget(tablaTraza);
    
    grupoTraza->setLayout(layoutTraza);
    layout->addWidget(grupoTraza);
    
    tabWidget->addTab(tabParser, "4. Parser");
}

// ==================== SLOTS ====================

void LL1Widget::onCargarGramatica() {
    QString texto = txtGramatica->toPlainText();
    
    if (texto.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "Ingrese una gramática");
        return;
    }
    
    if (gramatica->cargarDesdeTexto(texto.toStdString())) {
        lblEstadoGramatica->setText("Estado: Gramática cargada correctamente ✓");
        lblEstadoGramatica->setStyleSheet("font-weight: bold; color: green;");
        
        btnCalcularConjuntos->setEnabled(true);
        
        // Limpiar análisis previos
        if (conjuntos) { delete conjuntos; conjuntos = nullptr; }
        if (tabla) { delete tabla; tabla = nullptr; }
        if (parser) { delete parser; parser = nullptr; }
        
        txtFirst->clear();
        txtFollow->clear();
        tablaLL1->clear();
        tablaLL1->setRowCount(0);
        tablaLL1->setColumnCount(0);
        txtConflictos->clear();
        
        btnConstruirTabla->setEnabled(false);
        btnParsear->setEnabled(false);
        
        QMessageBox::information(this, "Éxito", 
            QString("Gramática cargada:\n- %1 producciones\n- %2 no terminales\n- %3 terminales")
            .arg(gramatica->getProducciones().size())
            .arg(gramatica->getNoTerminales().size())
            .arg(gramatica->getTerminales().size()));
    } else {
        lblEstadoGramatica->setText("Estado: Error al cargar gramática ✗");
        lblEstadoGramatica->setStyleSheet("font-weight: bold; color: red;");
        QMessageBox::critical(this, "Error", "No se pudo cargar la gramática. Verifique el formato.");
    }
}

void LL1Widget::onCargarEjemplo() {
    QString ejemplo = 
        "E -> T Ep ;\n"
        "Ep -> or T Ep | epsilon ;\n"
        "T -> C Tp ;\n"
        "Tp -> conc C Tp | epsilon ;\n"
        "C -> F Cp ;\n"
        "Cp -> CarPos Cp | CerrKleen Cp | Opc Cp | epsilon ;\n"
        "F -> Parizq E ParDer | simb | Corchizq simb guion simb CorchDer ;";
    
    txtGramatica->setPlainText(ejemplo);
}

void LL1Widget::onLimpiarGramatica() {
    txtGramatica->clear();
    gramatica->limpiar();
    lblEstadoGramatica->setText("Estado: Sin gramática cargada");
    lblEstadoGramatica->setStyleSheet("font-weight: bold;");
    
    btnCalcularConjuntos->setEnabled(false);
    btnConstruirTabla->setEnabled(false);
    btnParsear->setEnabled(false);
    
    txtFirst->clear();
    txtFollow->clear();
    tablaLL1->clear();
    txtConflictos->clear();
    tablaTraza->setRowCount(0);
}

void LL1Widget::onCalcularConjuntos() {
    if (conjuntos) delete conjuntos;
    conjuntos = new ConjuntosLL1(gramatica);
    
    conjuntos->calcular();
    mostrarConjuntos();
    
    btnConstruirTabla->setEnabled(true);
    
    QMessageBox::information(this, "Éxito", "Conjuntos FIRST y FOLLOW calculados");
}

void LL1Widget::onConstruirTabla() {
    if (!conjuntos) {
        QMessageBox::warning(this, "Advertencia", "Primero calcule FIRST y FOLLOW");
        return;
    }
    
    if (tabla) delete tabla;
    tabla = new TablaLL1(gramatica, conjuntos);
    
    bool exito = tabla->construir();
    mostrarTabla();
    
    if (exito) {
        QMessageBox::information(this, "Éxito", "Tabla LL(1) construida sin conflictos ✓");
        
        // Crear parser
        if (parser) delete parser;
        parser = new ParserLL1(gramatica, tabla, lexer);
        
        btnParsear->setEnabled(true);
    } else {
        QMessageBox::warning(this, "Conflictos", 
            QString("Tabla LL(1) construida con %1 conflicto(s).\nLa gramática NO es LL(1).")
            .arg(tabla->getConflictos().size()));
    }
}

void LL1Widget::onParsear() {
    if (!parser) {
        QMessageBox::warning(this, "Advertencia", "Primero construya la tabla LL(1)");
        return;
    }
    
    QString entrada = txtEntrada->text().trimmed();
    if (entrada.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "Ingrese una cadena para parsear");
        return;
    }
    
    // Tokenizar entrada
    lexer->setTexto(entrada.toStdString());
    std::vector<TokenLL1> tokens = lexer->tokenizar();
    
    // Parsear
    bool exito = parser->parsear(tokens);
    
    // Mostrar resultado
    if (exito) {
        lblResultado->setText("Resultado: ✓ ACEPTADO");
        lblResultado->setStyleSheet("font-weight: bold; font-size: 14px; color: green;");
    } else {
        lblResultado->setText("Resultado: ✗ RECHAZADO - " + 
            QString::fromStdString(parser->getMensajeError()));
        lblResultado->setStyleSheet("font-weight: bold; font-size: 14px; color: red;");
    }
    
    mostrarTraza();
}

void LL1Widget::onLimpiarParser() {
    txtEntrada->clear();
    tablaTraza->setRowCount(0);
    lblResultado->setText("Resultado: -");
    lblResultado->setStyleSheet("font-weight: bold; font-size: 14px;");
}

// ==================== MÉTODOS AUXILIARES ====================

void LL1Widget::mostrarConjuntos() {
    if (!conjuntos) return;
    
    txtFirst->setPlainText(QString::fromStdString(conjuntos->firstToString()));
    txtFollow->setPlainText(QString::fromStdString(conjuntos->followToString()));
}

void LL1Widget::mostrarTabla() {
    if (!tabla) return;
    
    std::vector<std::string> terminales = tabla->getTerminales();
    std::vector<std::string> noTerminales = tabla->getNoTerminales();
    
    tablaLL1->clear();
    tablaLL1->setRowCount(noTerminales.size());
    tablaLL1->setColumnCount(terminales.size());
    
    // Encabezados
    QStringList headers;
    for (const auto& t : terminales) {
        headers << QString::fromStdString(t);
    }
    tablaLL1->setHorizontalHeaderLabels(headers);
    
    QStringList vertHeaders;
    for (const auto& nt : noTerminales) {
        vertHeaders << QString::fromStdString(nt);
    }
    tablaLL1->setVerticalHeaderLabels(vertHeaders);
    
    // Llenar tabla
    for (size_t i = 0; i < noTerminales.size(); i++) {
        for (size_t j = 0; j < terminales.size(); j++) {
            EntradaTabla entrada = tabla->getEntrada(noTerminales[i], terminales[j]);
            
            QString texto = "-";
            if (entrada.valida) {
                texto = QString::fromStdString(entrada.produccion.toString());
            }
            
            tablaLL1->setItem(i, j, new QTableWidgetItem(texto));
        }
    }
    
    tablaLL1->resizeColumnsToContents();
    
    // Mostrar conflictos
    if (tabla->tieneConflictos()) {
        QString conflictos;
        for (const auto& conf : tabla->getConflictos()) {
            conflictos += QString::fromStdString(conf) + "\n";
        }
        txtConflictos->setPlainText(conflictos);
    } else {
        txtConflictos->setPlainText("✓ No hay conflictos");
    }
}

void LL1Widget::mostrarTraza() {
    if (!parser) return;
    
    const auto& traza = parser->getTraza();
    
    tablaTraza->setRowCount(traza.size());
    
    for (size_t i = 0; i < traza.size(); i++) {
        const auto& paso = traza[i];
        
        QTableWidgetItem* itemPila = new QTableWidgetItem(QString::fromStdString(paso.pila));
        QTableWidgetItem* itemEntrada = new QTableWidgetItem(QString::fromStdString(paso.entrada));
        QTableWidgetItem* itemAccion = new QTableWidgetItem(QString::fromStdString(paso.accion));
        
        if (paso.error) {
            itemAccion->setBackground(QColor(255, 200, 200));
            itemAccion->setForeground(QColor(200, 0, 0));
        }
        
        tablaTraza->setItem(i, 0, itemPila);
        tablaTraza->setItem(i, 1, itemEntrada);
        tablaTraza->setItem(i, 2, itemAccion);
    }
    
    tablaTraza->resizeColumnsToContents();
}