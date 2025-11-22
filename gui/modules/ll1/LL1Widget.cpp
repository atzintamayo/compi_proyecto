#include "LL1Widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QSpinBox>
#include <QComboBox>
#include <QGridLayout>

LL1Widget::LL1Widget(QWidget *parent) : QWidget(parent) {
    gramatica = new Gramatica();
    lexer = new Lexer();
    conjuntos = nullptr;
    tabla = nullptr;
    parser = nullptr;
    
    afdCompleto = nullptr;
    scannerCompleto = nullptr;
    
    setupUI();
}

LL1Widget::~LL1Widget() {
    delete gramatica;
    delete lexer;
    if (conjuntos) delete conjuntos;
    if (tabla) delete tabla;
    if (parser) delete parser;
    if (afdCompleto) delete afdCompleto;
    if (scannerCompleto) delete scannerCompleto;
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
    setupTabSimbolos();
    setupTabConjuntos();
    setupTabTabla();
    setupTabParser();
    setupTabCompleto();
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

void LL1Widget::setupTabSimbolos() {
    tabSimbolos = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tabSimbolos);
    
    btnMostrarSimbolos = new QPushButton("Mostrar Símbolos de la Gramática");
    btnMostrarSimbolos->setEnabled(false);
    connect(btnMostrarSimbolos, &QPushButton::clicked, this, &LL1Widget::onMostrarSimbolos);
    layout->addWidget(btnMostrarSimbolos);
    
    lblResumenSimbolos = new QLabel("Cargue una gramática primero");
    lblResumenSimbolos->setStyleSheet("font-weight: bold; padding: 10px;");
    lblResumenSimbolos->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblResumenSimbolos);

    
    // Símbolo inicial
    QGroupBox* grupoInicial = new QGroupBox("Símbolo Inicial");
    QVBoxLayout* layoutInicial = new QVBoxLayout();
    lblSimboloInicial = new QLabel("-");
    QFont fontInicial;
    fontInicial.setPointSize(16);
    fontInicial.setBold(true);
    lblSimboloInicial->setFont(fontInicial);
    lblSimboloInicial->setAlignment(Qt::AlignCenter);
    lblSimboloInicial->setStyleSheet("color: #2196F3; padding: 20px;");
    layoutInicial->addWidget(lblSimboloInicial);
    grupoInicial->setLayout(layoutInicial);
    layout->addWidget(grupoInicial);
    
    // Layout horizontal para terminales y no terminales
    QHBoxLayout* layoutListas = new QHBoxLayout();
    
    // No Terminales
    QGroupBox* grupoNoTerminales = new QGroupBox("No Terminales (Variables)");
    QVBoxLayout* layoutNT = new QVBoxLayout();
    
    QLabel* infoNT = new QLabel("Símbolos que se expanden en producciones.\nEmpiezan con mayúscula.");
    infoNT->setWordWrap(true);
    infoNT->setStyleSheet("color: gray; font-style: italic; font-size: 10px;");
    layoutNT->addWidget(infoNT);
    
    listaNoTerminales = new QListWidget();
    listaNoTerminales->setStyleSheet("QListWidget::item { padding: 5px; }");
    layoutNT->addWidget(listaNoTerminales);
    
    grupoNoTerminales->setLayout(layoutNT);
    layoutListas->addWidget(grupoNoTerminales);
    
    // Terminales
    QGroupBox* grupoTerminales = new QGroupBox("Terminales (Tokens)");
    QVBoxLayout* layoutT = new QVBoxLayout();
    
    QLabel* infoT = new QLabel("Símbolos del alfabeto que NO se expanden.\nAparecen en la entrada del parser.");
    infoT->setWordWrap(true);
    infoT->setStyleSheet("color: gray; font-style: italic; font-size: 10px;");
    layoutT->addWidget(infoT);
    
    listaTerminales = new QListWidget();
    listaTerminales->setStyleSheet("QListWidget::item { padding: 5px; }");
    layoutT->addWidget(listaTerminales);
    
    grupoTerminales->setLayout(layoutT);
    layoutListas->addWidget(grupoTerminales);
    
    layout->addLayout(layoutListas);
    
    tabWidget->addTab(tabSimbolos, "2. Símbolos");
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
    
    tabWidget->addTab(tabConjuntos, "3. FIRST/FOLLOW");
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
    
    tabWidget->addTab(tabTabla, "4. Tabla LL(1)");
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
    
    tabWidget->addTab(tabParser, "5. Parser");
}

void LL1Widget::setupTabCompleto() {
    tabCompleto = new QWidget();

    // Layout principal (solo para margen general)
    QVBoxLayout* mainLayout = new QVBoxLayout(tabCompleto);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Grid que imita tu CSS:
    //  div1: fila 0, columnas 0-1 (span 2)
    //  div2: fila 1, col 0
    //  div3: fila 1, col 1
    //  div4: fila 2, col 0
    //  div5: fila 2, col 1
    QGridLayout* grid = new QGridLayout();
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(12);
    grid->setContentsMargins(0, 0, 0, 0);

    // ========== Sección 1: Cargar AFD (div1) ==========
    QGroupBox* grupoAFD = new QGroupBox("1. Cargar AFD del Analizador Léxico");
    QVBoxLayout* layoutAFD = new QVBoxLayout();

    btnCargarAFD = new QPushButton("Cargar AFD (.afd / .txt)");
    connect(btnCargarAFD, &QPushButton::clicked, this, &LL1Widget::onCargarAFD);
    layoutAFD->addWidget(btnCargarAFD);

    lblEstadoAFD = new QLabel("Estado: Sin AFD cargado");
    lblEstadoAFD->setStyleSheet("font-weight: bold;");
    layoutAFD->addWidget(lblEstadoAFD);

    lblTokensDetectados = new QLabel("Tokens detectados: 0");
    lblTokensDetectados->setStyleSheet("color: gray;");
    layoutAFD->addWidget(lblTokensDetectados);

    grupoAFD->setLayout(layoutAFD);

    // ========== Sección 2: Mapeo Token → Terminal (div2) ==========
    QGroupBox* grupoMapeo = new QGroupBox("2. Mapeo: Token ID → Terminal de Gramática");
    QVBoxLayout* layoutMapeo = new QVBoxLayout();

    QLabel* infoMapeo = new QLabel(
        "Configure qué token del AFD corresponde a cada terminal de la gramática.\n"
        "Los tokens se detectan automáticamente del AFD. Use el ComboBox para asignar terminales."
    );
    infoMapeo->setWordWrap(true);
    infoMapeo->setStyleSheet("color: gray; font-style: italic;");
    layoutMapeo->addWidget(infoMapeo);

    tablaMapeo = new QTableWidget();
    tablaMapeo->setColumnCount(3);
    tablaMapeo->setHorizontalHeaderLabels({"Token ID", "Estado AFD", "Terminal"});
    tablaMapeo->horizontalHeader()->setStretchLastSection(true);
    tablaMapeo->setColumnWidth(0, 80);
    tablaMapeo->setColumnWidth(1, 100);
    layoutMapeo->addWidget(tablaMapeo);

    QHBoxLayout* layoutBotonesMapeo = new QHBoxLayout();
    btnAutoMapear = new QPushButton("🔄 Auto-mapear por nombre");
    btnAutoMapear->setEnabled(false);
    btnAutoMapear->setToolTip("Mapea automáticamente tokens cuyos nombres coinciden con terminales");
    btnEliminarMapeo = new QPushButton("Limpiar Mapeos");
    btnEliminarMapeo->setEnabled(false);

    connect(btnAutoMapear, &QPushButton::clicked, this, &LL1Widget::onAutoMapear);
    connect(btnEliminarMapeo, &QPushButton::clicked, this, &LL1Widget::onEliminarMapeo);

    layoutBotonesMapeo->addWidget(btnAutoMapear);
    layoutBotonesMapeo->addWidget(btnEliminarMapeo);
    layoutMapeo->addLayout(layoutBotonesMapeo);

    grupoMapeo->setLayout(layoutMapeo);

    // ========== Sección 3: Análisis Léxico + Sintáctico (div3) ==========
    QGroupBox* grupoAnalisis = new QGroupBox("3. Análisis Léxico + Sintáctico");
    QVBoxLayout* layoutAnalisis = new QVBoxLayout();

    QLabel* lblTexto = new QLabel("Texto de entrada:");
    layoutAnalisis->addWidget(lblTexto);

    txtEntradaCompleto = new QTextEdit();
    txtEntradaCompleto->setMaximumHeight(80);   // Igual que antes
    txtEntradaCompleto->setPlaceholderText("Ingrese el texto a analizar...");
    layoutAnalisis->addWidget(txtEntradaCompleto);

    QHBoxLayout* layoutBotonesAnalisis = new QHBoxLayout();
    btnAnalizarCompleto = new QPushButton("Analizar");
    btnAnalizarCompleto->setEnabled(false);
    btnLimpiarCompleto = new QPushButton("Limpiar");

    connect(btnAnalizarCompleto, &QPushButton::clicked, this, &LL1Widget::onAnalizarCompleto);
    connect(btnLimpiarCompleto, &QPushButton::clicked, this, &LL1Widget::onLimpiarCompleto);

    layoutBotonesAnalisis->addWidget(btnAnalizarCompleto);
    layoutBotonesAnalisis->addWidget(btnLimpiarCompleto);
    layoutAnalisis->addLayout(layoutBotonesAnalisis);

    lblResultadoCompleto = new QLabel("Resultado: -");
    lblResultadoCompleto->setStyleSheet("font-weight: bold; font-size: 14px;");
    layoutAnalisis->addWidget(lblResultadoCompleto);

    grupoAnalisis->setLayout(layoutAnalisis);

    // ========== Sección 4: Tokens reconocidos (div4) ==========
    QGroupBox* grupoTokens = new QGroupBox("Tokens Reconocidos");
    QVBoxLayout* layoutTokens = new QVBoxLayout();

    tablaTokensCompleto = new QTableWidget();
    tablaTokensCompleto->setColumnCount(4);
    tablaTokensCompleto->setHorizontalHeaderLabels({"Token ID", "Lexema", "Terminal", "Posición"});
    tablaTokensCompleto->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaTokensCompleto->horizontalHeader()->setStretchLastSection(true);

    // Antes tenía setMaximumHeight(120). Lo cambiamos a mínimo para que pueda crecer en el grid.
    tablaTokensCompleto->setMinimumHeight(120);

    layoutTokens->addWidget(tablaTokensCompleto);
    grupoTokens->setLayout(layoutTokens);

    // ========== Sección 5: Traza del parser (div5) ==========
    QGroupBox* grupoTraza = new QGroupBox("Traza del Parser LL(1)");
    QVBoxLayout* layoutTraza = new QVBoxLayout();

    tablaTrazaCompleto = new QTableWidget();
    tablaTrazaCompleto->setColumnCount(3);
    tablaTrazaCompleto->setHorizontalHeaderLabels({"Pila", "Entrada", "Acción"});
    tablaTrazaCompleto->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaTrazaCompleto->horizontalHeader()->setStretchLastSection(true);
    layoutTraza->addWidget(tablaTrazaCompleto);

    grupoTraza->setLayout(layoutTraza);

    // ========== Colocar cada grupo en el grid ==========
    // div1: fila 0, columnas 0-1
    grid->addWidget(grupoAFD,   0, 0, 1, 2);
    // div2: fila 1, col 0
    grid->addWidget(grupoMapeo, 1, 0, 1, 1);
    // div3: fila 1, col 1
    grid->addWidget(grupoAnalisis, 1, 1, 1, 1);
    // div4: fila 2, col 0
    grid->addWidget(grupoTokens, 2, 0, 1, 1);
    // div5: fila 2, col 1
    grid->addWidget(grupoTraza, 2, 1, 1, 1);

    // Estirar columnas y filas para que se parezca al grid CSS
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    grid->setRowStretch(0, 0);  // Header (AFD) menos alto
    grid->setRowStretch(1, 1);  // Mapeo / Análisis
    grid->setRowStretch(2, 2);  // Tokens / Traza (más espacio)

    mainLayout->addLayout(grid);

    tabWidget->addTab(tabCompleto, "6. Análisis Completo");
}

// ==================== SLOTS ANÁLISIS COMPLETO ====================

void LL1Widget::onCargarAFD() {
    QString archivo = QFileDialog::getOpenFileName(this, 
        "Cargar AFD", "", "Archivos AFD (*.afd *.txt);;Todos los archivos (*)");
    
    if (archivo.isEmpty()) return;
    
    if (afdCompleto) delete afdCompleto;
    afdCompleto = new AFD();
    
    if (afdCompleto->CargarAFD(archivo.toStdString())) {
        lblEstadoAFD->setText(QString("Estado: AFD cargado ✓ (%1 estados)")
            .arg(afdCompleto->NumEdos));
        lblEstadoAFD->setStyleSheet("font-weight: bold; color: green;");
        
        if (scannerCompleto) delete scannerCompleto;
        scannerCompleto = new Scanner(afdCompleto);
        
        // Extraer tokens automáticamente
        extraerTokensDelAFD();
        
        // Poblar tabla con tokens detectados
        poblarTablaMapeoConTokens();
        
        btnAutoMapear->setEnabled(true);
        btnEliminarMapeo->setEnabled(true);
        
        QMessageBox::information(this, "Éxito", 
            QString("AFD cargado correctamente\n%1 token(s) de aceptación detectados")
            .arg(tokensDisponiblesAFD.size()));
    } else {
        lblEstadoAFD->setText("Estado: Error al cargar AFD ✗");
        lblEstadoAFD->setStyleSheet("font-weight: bold; color: red;");
        delete afdCompleto;
        afdCompleto = nullptr;
        QMessageBox::critical(this, "Error", "No se pudo cargar el AFD");
    }
}

void LL1Widget::extraerTokensDelAFD() {
    tokensDisponiblesAFD.clear();
    
    if (!afdCompleto) return;
    
    // Recorrer todos los estados del AFD
    for (int i = 0; i < afdCompleto->NumEdos; i++) {
        EdoAFD* estado = afdCompleto->EdosAFD[i];
        
        if (estado->esAceptacion && estado->token >= 0) {
            TokenInfo info;
            info.id = estado->token;
            info.estadoId = estado->id;
            info.existe = true;
            
            tokensDisponiblesAFD[estado->token] = info;
        }
    }
    
    lblTokensDetectados->setText(QString("Tokens detectados: %1")
        .arg(tokensDisponiblesAFD.size()));
    lblTokensDetectados->setStyleSheet("color: green; font-weight: bold;");
}

void LL1Widget::poblarTablaMapeoConTokens() {
    tablaMapeo->setRowCount(0);
    mapeoTokenTerminal.clear();
    
    if (!gramatica || gramatica->getTerminales().empty()) {
        QMessageBox::warning(this, "Advertencia", 
            "Cargue primero una gramática para ver los terminales disponibles");
        return;
    }
    
    int row = 0;
    for (const auto& [tokenId, info] : tokensDisponiblesAFD) {
        tablaMapeo->insertRow(row);
        
        // Columna 0: Token ID (no editable, con fondo gris)
        QTableWidgetItem* itemId = new QTableWidgetItem(QString::number(tokenId));
        itemId->setFlags(itemId->flags() & ~Qt::ItemIsEditable);

        // Fondo gris claro para marcar que es de solo lectura
        itemId->setBackground(QColor(240, 240, 240));
        // Texto oscuro para que se vea bien tanto en tema claro como oscuro
        itemId->setForeground(QColor(0, 0, 0));

        itemId->setTextAlignment(Qt::AlignCenter);
        tablaMapeo->setItem(row, 0, itemId);


        
        // Columna 1: Estado AFD (informativo)
        QTableWidgetItem* itemEstado = new QTableWidgetItem(QString("Estado %1").arg(info.estadoId));
        itemEstado->setFlags(itemEstado->flags() & ~Qt::ItemIsEditable);
        itemEstado->setBackground(QColor(245, 245, 245));
        itemEstado->setForeground(QColor(100, 100, 100));
        tablaMapeo->setItem(row, 1, itemEstado);
        
        // Columna 2: ComboBox con terminales
        QComboBox* combo = new QComboBox();
        combo->addItem("-- Sin mapear --");
        
        for (const auto& t : gramatica->getTerminales()) {
            combo->addItem(QString::fromStdString(t.nombre));
        }
        
        // Conectar cambio de combo
        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [this, tokenId, combo]() {
            if (combo->currentIndex() > 0) {
                mapeoTokenTerminal[tokenId] = combo->currentText().toStdString();
            } else {
                mapeoTokenTerminal.erase(tokenId);
            }
            actualizarEstadoAnalisisCompleto();
        });
        
        tablaMapeo->setCellWidget(row, 2, combo);
        row++;
    }
    
    tablaMapeo->resizeColumnsToContents();
}

void LL1Widget::actualizarEstadoAnalisisCompleto() {
    // Habilitar análisis si hay tabla LL(1) y al menos un mapeo
    bool listo = tabla && !mapeoTokenTerminal.empty();
    btnAnalizarCompleto->setEnabled(listo);
}

void LL1Widget::onAutoMapear() {
    if (tokensDisponiblesAFD.empty()) {
        QMessageBox::warning(this, "Advertencia", "No hay tokens cargados del AFD");
        return;
    }
    
    if (!gramatica || gramatica->getTerminales().empty()) {
        QMessageBox::warning(this, "Advertencia", "No hay terminales en la gramática");
        return;
    }
    
    int mapeados = 0;
    
    // Auto-mapear token 10 a 'simb' (caso común)
    // Puedes personalizar esto según convención
    if (tokensDisponiblesAFD.find(10) != tokensDisponiblesAFD.end()) {
        for (const auto& t : gramatica->getTerminales()) {
            if (t.nombre == "simb") {
                // Buscar fila con token 10
                for (int row = 0; row < tablaMapeo->rowCount(); row++) {
                    if (tablaMapeo->item(row, 0)->text().toInt() == 10) {
                        QComboBox* combo = qobject_cast<QComboBox*>(
                            tablaMapeo->cellWidget(row, 2));
                        if (combo) {
                            int idx = combo->findText("simb");
                            if (idx >= 0) {
                                combo->setCurrentIndex(idx);
                                mapeados++;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    
    if (mapeados > 0) {
        QMessageBox::information(this, "Auto-mapeo",
            QString("Se mapearon automáticamente %1 token(s)\n\n"
                    "Nota: Para mapeos personalizados, use los ComboBox manualmente.")
            .arg(mapeados));
    } else {
        QMessageBox::information(this, "Auto-mapeo",
            "No se encontraron coincidencias automáticas.\n"
            "Configure los mapeos manualmente usando los ComboBox.");
    }
}

void LL1Widget::onEliminarMapeo() {
    // Limpiar todos los mapeos
    mapeoTokenTerminal.clear();
    
    // Resetear todos los combos a "-- Sin mapear --"
    for (int row = 0; row < tablaMapeo->rowCount(); row++) {
        QComboBox* combo = qobject_cast<QComboBox*>(tablaMapeo->cellWidget(row, 2));
        if (combo) {
            combo->setCurrentIndex(0);
        }
    }
    
    actualizarEstadoAnalisisCompleto();
    
    QMessageBox::information(this, "Mapeos limpiados", 
        "Todos los mapeos han sido eliminados.");
}

void LL1Widget::onAnalizarCompleto() {
    if (!scannerCompleto || !afdCompleto) {
        QMessageBox::warning(this, "Advertencia", "Primero cargue un AFD");
        return;
    }
    
    if (!parser || !tabla) {
        QMessageBox::warning(this, "Advertencia", "Primero construya la tabla LL(1)");
        return;
    }
    
    if (mapeoTokenTerminal.empty()) {
        QMessageBox::warning(this, "Advertencia", "Configure al menos un mapeo Token→Terminal");
        return;
    }
    
    QString texto = txtEntradaCompleto->toPlainText().trimmed();
    if (texto.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "Ingrese texto para analizar");
        return;
    }
    
    // Paso 1: Análisis léxico
    scannerCompleto->setTexto(texto.toStdString());
    bool lexicoExito = scannerCompleto->escanear();
    
    if (!lexicoExito) {
        lblResultadoCompleto->setText("Resultado: ✗ ERROR LÉXICO");
        lblResultadoCompleto->setStyleSheet("font-weight: bold; font-size: 14px; color: red;");
        QMessageBox::critical(this, "Error Léxico", 
            QString("Se encontraron %1 errores léxicos").arg(scannerCompleto->getErrores().size()));
        mostrarTokensCompleto();
        return;
    }
    
    // Paso 2: Convertir tokens a TokenLL1
    const auto& tokensScanner = scannerCompleto->getTokens();
    std::vector<TokenLL1> tokensLL1;
    
    for (const auto& tok : tokensScanner) {
        auto it = mapeoTokenTerminal.find(tok.tokenId);
        if (it != mapeoTokenTerminal.end()) {
            std::string terminal = it->second;
            
            // Mapear a TokenType del lexer LL1
            TokenType tipo = TOKEN_SIMB;  // Por defecto
            if (terminal == "or") tipo = TOKEN_OR;
            else if (terminal == "conc") tipo = TOKEN_CONC;
            else if (terminal == "CarPos") tipo = TOKEN_MAS;
            else if (terminal == "CerrKleen") tipo = TOKEN_ASTERISCO;
            else if (terminal == "Opc") tipo = TOKEN_INTERROGACION;
            else if (terminal == "Parizq") tipo = TOKEN_PARIZQ;
            else if (terminal == "ParDer") tipo = TOKEN_PARDER;
            else if (terminal == "Corchizq") tipo = TOKEN_CORCHIZQ;
            else if (terminal == "CorchDer") tipo = TOKEN_CORCHDER;
            else if (terminal == "guion") tipo = TOKEN_GUION;
            else if (terminal == "simb") tipo = TOKEN_SIMB;
            
            tokensLL1.push_back(TokenLL1(tipo, tok.lexema, tok.inicio));
        }
    }
    
    // Agregar token de fin
    tokensLL1.push_back(TokenLL1(TOKEN_FIN, "", texto.length()));
    
    // Paso 3: Parsear
    bool sintacticoExito = parser->parsear(tokensLL1);
    
    if (sintacticoExito) {
        lblResultadoCompleto->setText("Resultado: ✓ ACEPTADO");
        lblResultadoCompleto->setStyleSheet("font-weight: bold; font-size: 14px; color: green;");
    } else {
        lblResultadoCompleto->setText("Resultado: ✗ ERROR SINTÁCTICO - " + 
            QString::fromStdString(parser->getMensajeError()));
        lblResultadoCompleto->setStyleSheet("font-weight: bold; font-size: 14px; color: red;");
    }
    
    mostrarTokensCompleto();
    mostrarTrazaCompleto();
}

void LL1Widget::onLimpiarCompleto() {
    txtEntradaCompleto->clear();
    tablaTokensCompleto->setRowCount(0);
    tablaTrazaCompleto->setRowCount(0);
    lblResultadoCompleto->setText("Resultado: -");
    lblResultadoCompleto->setStyleSheet("font-weight: bold; font-size: 14px;");
}

void LL1Widget::mostrarTokensCompleto() {
    if (!scannerCompleto) return;
    
    const auto& tokens = scannerCompleto->getTokens();
    
    tablaTokensCompleto->setRowCount(tokens.size());
    
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& tok = tokens[i];
        
        tablaTokensCompleto->setItem(i, 0, new QTableWidgetItem(QString::number(tok.tokenId)));
        tablaTokensCompleto->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(tok.lexema)));
        
        // Buscar terminal mapeado
        QString terminal = "-";
        auto it = mapeoTokenTerminal.find(tok.tokenId);
        if (it != mapeoTokenTerminal.end()) {
            terminal = QString::fromStdString(it->second);
        }
        tablaTokensCompleto->setItem(i, 2, new QTableWidgetItem(terminal));
        
        tablaTokensCompleto->setItem(i, 3, new QTableWidgetItem(
            QString("%1-%2").arg(tok.inicio).arg(tok.fin)));
    }
    
    tablaTokensCompleto->resizeColumnsToContents();
}

void LL1Widget::mostrarTrazaCompleto() {
    if (!parser) return;
    
    const auto& traza = parser->getTraza();
    
    tablaTrazaCompleto->setRowCount(traza.size());
    
    for (size_t i = 0; i < traza.size(); i++) {
        const auto& paso = traza[i];
        
        QTableWidgetItem* itemPila = new QTableWidgetItem(QString::fromStdString(paso.pila));
        QTableWidgetItem* itemEntrada = new QTableWidgetItem(QString::fromStdString(paso.entrada));
        QTableWidgetItem* itemAccion = new QTableWidgetItem(QString::fromStdString(paso.accion));
        
        if (paso.error) {
            itemAccion->setBackground(QColor(255, 200, 200));
            itemAccion->setForeground(QColor(200, 0, 0));
        }
        
        tablaTrazaCompleto->setItem(i, 0, itemPila);
        tablaTrazaCompleto->setItem(i, 1, itemEntrada);
        tablaTrazaCompleto->setItem(i, 2, itemAccion);
    }
    
    tablaTrazaCompleto->resizeColumnsToContents();
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
        btnMostrarSimbolos->setEnabled(true);
        
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
    btnMostrarSimbolos->setEnabled(false);
    btnConstruirTabla->setEnabled(false);
    btnParsear->setEnabled(false);
    
    // Limpiar tab de símbolos
    listaTerminales->clear();
    listaNoTerminales->clear();
    lblSimboloInicial->setText("-");
    lblResumenSimbolos->setText("Cargue una gramática primero");
    
    txtFirst->clear();
    txtFollow->clear();
    tablaLL1->clear();
    txtConflictos->clear();
    tablaTraza->setRowCount(0);
}

void LL1Widget::onMostrarSimbolos() {
    mostrarSimbolos();
}

void LL1Widget::mostrarSimbolos() {
    listaTerminales->clear();
    listaNoTerminales->clear();
    
    // Mostrar símbolo inicial
    Simbolo inicial = gramatica->getSimboloInicial();
    if (!inicial.nombre.empty()) {
        lblSimboloInicial->setText(QString::fromStdString(inicial.nombre));
    } else {
        lblSimboloInicial->setText("-");
    }
    
    // Contar símbolos
    int numTerminales = gramatica->getTerminales().size();
    int numNoTerminales = gramatica->getNoTerminales().size();
    
    lblResumenSimbolos->setText(QString("Total: %1 Terminales + %2 No Terminales = %3 Símbolos")
        .arg(numTerminales)
        .arg(numNoTerminales)
        .arg(numTerminales + numNoTerminales));
    
    // Llenar lista de no terminales
    for (const auto& nt : gramatica->getNoTerminales()) {
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(nt.nombre));
        item->setForeground(QColor(41, 128, 185));  // Azul
        QFont font = item->font();
        font.setBold(true);
        font.setPointSize(11);
        item->setFont(font);
        
        // Marcar el símbolo inicial con estrella
        if (nt.nombre == inicial.nombre) {
            item->setText("⭐ " + QString::fromStdString(nt.nombre) + " (inicial)");
            item->setBackground(QColor(230, 247, 255));
        }
        
        listaNoTerminales->addItem(item);
    }
    
    // Llenar lista de terminales
    for (const auto& t : gramatica->getTerminales()) {
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(t.nombre));
        item->setForeground(QColor(46, 125, 50));  // Verde
        QFont font = item->font();
        font.setPointSize(11);
        item->setFont(font);
        
        listaTerminales->addItem(item);
    }
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