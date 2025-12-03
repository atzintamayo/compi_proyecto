#include "LR0Widget.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QScrollArea>
#include <sstream>

using namespace LR0;

LR0Widget::LR0Widget(GramaticaLR0* gram, QWidget* parent)
    : QWidget(parent), gramaticaPropia(false) {
    
    if (gram == nullptr) {
        gramatica = new GramaticaLR0();
        gramaticaPropia = true;
        crearGramaticaEjemplo();
    } else {
        gramatica = gram;
        gramaticaPropia = false;
    }
    
    analizador = new AnalizadorLR0(gramatica);
    
    // Inicializar widgets
    textEditGramatica = new QTextEdit(this);
    textEditGramatica->setPlaceholderText(
        "Ingrese la gramática en el formato:\n"
        "S -> A B\n"
        "A -> a | ε\n"
        "B -> b\n\n"
        "O use la gramática de ejemplo precargada"
    );
    
    lineEditSigma = new QLineEdit(this);
    lineEditSigma->setPlaceholderText("Ingrese la cadena a analizar (ej: a b)");
    
    btnCargarGramatica = new QPushButton("Cargar Gramática", this);
    btnCrearTabla = new QPushButton("Crear Tabla LR(0)", this);
    btnAnalizar = new QPushButton("Analizar Cadena", this);
    btnLimpiar = new QPushButton("Limpiar", this);
    
    listaNoTerminales = new QListWidget(this);
    listaTerminales = new QListWidget(this);
    
    tablaLR0 = new QTableWidget(this);
    tablaLexico = new QTableWidget(this);
    tablaEjecucion = new QTableWidget(this);
    tablaEstados = new QTableWidget(this);
    tablaProducciones = new QTableWidget(this);
    
    // Configurar layout
    configurarLayout();
    
    // Conectar señales
    connect(btnCargarGramatica, &QPushButton::clicked, this, &LR0Widget::onCargarGramatica);
    connect(btnCrearTabla, &QPushButton::clicked, this, &LR0Widget::onCrearTablaLR0);
    connect(btnAnalizar, &QPushButton::clicked, this, &LR0Widget::onAnalizarLR0);
    connect(btnLimpiar, &QPushButton::clicked, this, &LR0Widget::onLimpiarTodo);
    
    // Aplicar estilos
    aplicarEstiloTabla(tablaLR0);
    aplicarEstiloTabla(tablaLexico);
    aplicarEstiloTabla(tablaEjecucion);
    aplicarEstiloTabla(tablaEstados);
    aplicarEstiloTabla(tablaProducciones);
    
    // Mostrar gramática de ejemplo en el texto
    if (gramaticaPropia) {
        textEditGramatica->setPlainText(
            "# Gramática de ejemplo (E -> E + T | T)\n"
            "E' -> E\n"
            "E -> E + T\n"
            "E -> T\n"
            "T -> T * F\n"
            "T -> F\n"
            "F -> ( E )\n"
            "F -> id\n"
        );
        
        // Configurar un ejemplo de entrada
        lineEditSigma->setText("id + id * id");
        
        // Mostrar producciones de ejemplo
        actualizarTablaProducciones();
        actualizarListasSimbolos();
    }
}

LR0Widget::~LR0Widget() {
    delete analizador;
    if (gramaticaPropia) {
        delete gramatica;
    }
}

void LR0Widget::crearGramaticaEjemplo() {
    // Crear una gramática de ejemplo: E -> E + T | T, T -> T * F | F, F -> ( E ) | id
    // Ampliada: E' -> E

    gramatica->agregarProduccion(
        SimboloLR0("E'", TipoSimboloLR0::NO_TERMINAL),
        {SimboloLR0("E", TipoSimboloLR0::NO_TERMINAL)}
    );
        // 1: E -> E + T
    gramatica->agregarProduccion(
        SimboloLR0("E", TipoSimboloLR0::NO_TERMINAL),
        {
            SimboloLR0("E", TipoSimboloLR0::NO_TERMINAL),
            SimboloLR0("+", TipoSimboloLR0::TERMINAL),
            SimboloLR0("T", TipoSimboloLR0::NO_TERMINAL)
        }
    );
    
    // 2: E -> T
    gramatica->agregarProduccion(
        SimboloLR0("E", TipoSimboloLR0::NO_TERMINAL),
        {SimboloLR0("T", TipoSimboloLR0::NO_TERMINAL)}
    );
    
    // 3: T -> T * F
    gramatica->agregarProduccion(
        SimboloLR0("T", TipoSimboloLR0::NO_TERMINAL),
        {
            SimboloLR0("T", TipoSimboloLR0::NO_TERMINAL),
            SimboloLR0("*", TipoSimboloLR0::TERMINAL),
            SimboloLR0("F", TipoSimboloLR0::NO_TERMINAL)
        }
    );
    
    // 4: T -> F
    gramatica->agregarProduccion(
        SimboloLR0("T", TipoSimboloLR0::NO_TERMINAL),
        {SimboloLR0("F", TipoSimboloLR0::NO_TERMINAL)}
    );
    
    // 5: F -> ( E )
    gramatica->agregarProduccion(
        SimboloLR0("F", TipoSimboloLR0::NO_TERMINAL),
        {
            SimboloLR0("(", TipoSimboloLR0::TERMINAL),
            SimboloLR0("E", TipoSimboloLR0::NO_TERMINAL),
            SimboloLR0(")", TipoSimboloLR0::TERMINAL)
        }
    );
    
    // 6: F -> id
    gramatica->agregarProduccion(
        SimboloLR0("F", TipoSimboloLR0::NO_TERMINAL),
        {SimboloLR0("id", TipoSimboloLR0::TERMINAL)}
    );

}

void LR0Widget::configurarLayout() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // === SECCIÓN SUPERIOR: Entrada de gramática ===
    QGroupBox* groupGramatica = new QGroupBox("Gramática", this);
    QVBoxLayout* layoutGramatica = new QVBoxLayout(groupGramatica);
    
    QLabel* labelInfo = new QLabel(
        "Formato: NoTerminal -> simbolo1 simbolo2 ...\n"
        "La primera producción debe ser la ampliada (ej: E' -> E)", this);
    labelInfo->setStyleSheet("color: gray; font-size: 10px;");
    layoutGramatica->addWidget(labelInfo);
    
    textEditGramatica->setMaximumHeight(120);
    layoutGramatica->addWidget(textEditGramatica);
    
    QHBoxLayout* layoutBotonesGram = new QHBoxLayout();
    layoutBotonesGram->addWidget(btnCargarGramatica);
    layoutBotonesGram->addWidget(btnCrearTabla);
    layoutBotonesGram->addStretch();
    layoutGramatica->addLayout(layoutBotonesGram);
    
    mainLayout->addWidget(groupGramatica);
    
    // === SECCIÓN: Controles de análisis ===
    QGroupBox* groupControles = new QGroupBox("Análisis de Cadena", this);
    QVBoxLayout* layoutControles = new QVBoxLayout(groupControles);
    
    QHBoxLayout* layoutSigma = new QHBoxLayout();
    layoutSigma->addWidget(new QLabel("Cadena (Σ):", this));
    layoutSigma->addWidget(lineEditSigma);
    layoutControles->addLayout(layoutSigma);
    
    QHBoxLayout* layoutBotones = new QHBoxLayout();
    layoutBotones->addWidget(btnAnalizar);
    layoutBotones->addWidget(btnLimpiar);
    layoutBotones->addStretch();
    layoutControles->addLayout(layoutBotones);
    
    mainLayout->addWidget(groupControles);
    
    // === SECCIÓN MEDIA: Símbolos y Tabla LR(0) ===
    QSplitter* splitterMedia = new QSplitter(Qt::Horizontal, this);
    
    // Panel izquierdo: Símbolos y Producciones
    QTabWidget* tabIzquierda = new QTabWidget(this);
    
    // Tab: Símbolos
    QWidget* tabSimbolos = new QWidget(this);
    QVBoxLayout* layoutSimbolos = new QVBoxLayout(tabSimbolos);
    
    QGroupBox* groupTerminales = new QGroupBox("Terminales (Vt)", this);
    QVBoxLayout* layoutTerminales = new QVBoxLayout(groupTerminales);
    layoutTerminales->addWidget(listaTerminales);
    
    QGroupBox* groupNoTerminales = new QGroupBox("No Terminales (Vn)", this);
    QVBoxLayout* layoutNoTerminales = new QVBoxLayout(groupNoTerminales);
    layoutNoTerminales->addWidget(listaNoTerminales);
    
    layoutSimbolos->addWidget(groupTerminales);
    layoutSimbolos->addWidget(groupNoTerminales);
    tabIzquierda->addTab(tabSimbolos, "Símbolos");
    
    // Tab: Producciones
    QWidget* tabProducciones = new QWidget(this);
    QVBoxLayout* layoutProds = new QVBoxLayout(tabProducciones);
    layoutProds->addWidget(tablaProducciones);
    tabIzquierda->addTab(tabProducciones, "Producciones");
    
    // Panel derecho: Tabla LR(0)
    QGroupBox* groupTablaLR0 = new QGroupBox("Tabla LR(0)", this);
    QVBoxLayout* layoutTablaLR0 = new QVBoxLayout(groupTablaLR0);
    layoutTablaLR0->addWidget(tablaLR0);
    
    splitterMedia->addWidget(tabIzquierda);
    splitterMedia->addWidget(groupTablaLR0);
    splitterMedia->setStretchFactor(0, 1);
    splitterMedia->setStretchFactor(1, 3);
    
    mainLayout->addWidget(splitterMedia, 2);
    
    // === SECCIÓN INFERIOR: Tablas de análisis ===
    QTabWidget* tabWidget = new QTabWidget(this);
    
    // Tab: Estados LR(0)
    QWidget* tabEstados = new QWidget(this);
    QVBoxLayout* layoutEstados = new QVBoxLayout(tabEstados);
    layoutEstados->addWidget(tablaEstados);
    tabWidget->addTab(tabEstados, "Estados LR(0)");
    
    // Tab: Análisis Léxico
    QWidget* tabLexico = new QWidget(this);
    QVBoxLayout* layoutLexico = new QVBoxLayout(tabLexico);
    layoutLexico->addWidget(tablaLexico);
    tabWidget->addTab(tabLexico, "Tokens");
    
    // Tab: Ejecución LR(0)
    QWidget* tabEjecucion = new QWidget(this);
    QVBoxLayout* layoutEjecucion = new QVBoxLayout(tabEjecucion);
    layoutEjecucion->addWidget(tablaEjecucion);
    tabWidget->addTab(tabEjecucion, "Traza de Ejecución");
    
    mainLayout->addWidget(tabWidget, 2);
    
    setLayout(mainLayout);
}

void LR0Widget::onCargarGramatica() {
    try {
        QString textoGramatica = textEditGramatica->toPlainText().trimmed();
        
        if (textoGramatica.isEmpty()) {
            QMessageBox::warning(this, "Advertencia", 
                "Por favor ingrese una gramática");
            return;
        }
        
        if (!gramaticaPropia) {
            gramatica = new GramaticaLR0();
            gramaticaPropia = true;
            delete analizador;
            analizador = new AnalizadorLR0(gramatica);
        } else {
            gramatica->limpiar();
        }
        
        QStringList lineas = textoGramatica.split('\n', Qt::SkipEmptyParts);
        
        for (const QString& linea : lineas) {
            QString lineaLimpia = linea.trimmed();
            if (lineaLimpia.isEmpty() || lineaLimpia.startsWith('#')) {
                continue;
            }
            
            QStringList partes = lineaLimpia.split("->", Qt::SkipEmptyParts);
            if (partes.size() != 2) continue;
            
            QString izq = partes[0].trimmed();
            QString der = partes[1].trimmed();
            
            QStringList alternativas = der.split('|', Qt::SkipEmptyParts);
            
            for (const QString& alt : alternativas) {
                QStringList simbolos = alt.trimmed().split(' ', Qt::SkipEmptyParts);
                
                SimboloLR0 simboloIzq(izq.toStdString(), TipoSimboloLR0::NO_TERMINAL);
                std::vector<SimboloLR0> simbolosDer;
                
                for (const QString& simb : simbolos) {
                    std::string simboloStr = simb.toStdString();
                    
                    bool esTerminal = false;
                    if (simb.length() == 1 && (simb[0].isLower() || !simb[0].isLetter())) {
                        esTerminal = true;
                    } else if (simb == "id" || simb == "num" || simb == "ε") {
                        esTerminal = true;
                    }
                    
                    if (esTerminal) {
                        simbolosDer.push_back(SimboloLR0(simboloStr, TipoSimboloLR0::TERMINAL));
                    } else {
                        simbolosDer.push_back(SimboloLR0(simboloStr, TipoSimboloLR0::NO_TERMINAL));
                    }
                }
                
                gramatica->agregarProduccion(simboloIzq, simbolosDer);
            }
        }
        
        actualizarListasSimbolos();
        actualizarTablaProducciones();
        
        QMessageBox::information(this, "Éxito", 
            QString("Gramática cargada exitosamente.\n"
                    "Terminales: %1\n"
                    "No Terminales: %2\n"
                    "Producciones: %3")
                    .arg(gramatica->getTerminales().size())
                    .arg(gramatica->getNoTerminales().size())
                    .arg(gramatica->getProducciones().size()));
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Error al cargar gramática: %1").arg(e.what()));
    }
}

void LR0Widget::onCrearTablaLR0() {
    try {
        if (gramatica->getProducciones().empty()) {
            QMessageBox::warning(this, "Advertencia", 
                "Primero debe cargar una gramática");
            return;
        }
        
        // Analizar gramática y preparar símbolos
        analizador->AnalizarGramatica_Y_PrepararSimbolos();
        
        // Crear tabla LR(0)
        analizador->CrearTablaLR0();
        
        // Actualizar visualización
        actualizarTablaLR0();
        actualizarTablaEstados();
        
        QMessageBox::information(this, "Éxito", 
            QString("Tabla LR(0) creada exitosamente.\n"
                    "Estados: %1\n"
                    "Transiciones: %2")
                    .arg(analizador->RenglonesLR0.size())
                    .arg(analizador->ResultIrA.size()));
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Error al crear tabla LR(0): %1").arg(e.what()));
    }
}

void LR0Widget::onAnalizarLR0() {
    try {
        QString sigma = lineEditSigma->text().trimmed();
        
        if (sigma.isEmpty()) {
            QMessageBox::warning(this, "Advertencia", 
                "Por favor ingrese una cadena a analizar");
            return;
        }
        
        if (analizador->RenglonesLR0.empty()) {
            QMessageBox::warning(this, "Advertencia", 
                "Primero debe crear la tabla LR(0)");
            return;
        }
        
        // Tokenizar la cadena (simple: separar por espacios)
        std::vector<std::string> tokens = tokenizarCadena(sigma);
        
        // Mostrar tokens
        mostrarLexico(tokens);
        
        // Analizar con LR(0)
        std::vector<AnalizadorLR0::PasoLR0> pasos = 
            analizador->AnalizarCadenaLR0(tokens);
        
        mostrarEjecucion(pasos);
        
        // Verificar si fue aceptada
        if (!pasos.empty()) {
            const auto& ultimoPaso = pasos.back();
            if (ultimoPaso.accion.find("ACEPTAR") != std::string::npos) {
                QMessageBox::information(this, "✓ Éxito", 
                    "¡Cadena ACEPTADA por la gramática!");
            } else if (ultimoPaso.accion.find("ERROR") != std::string::npos) {
                QMessageBox::warning(this, "✗ Rechazada", 
                    "Cadena RECHAZADA por la gramática");
            }
        }
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Error en análisis LR(0): %1").arg(e.what()));
    }
}

void LR0Widget::onLimpiarTodo() {
    lineEditSigma->clear();
    tablaLexico->setRowCount(0);
    tablaEjecucion->setRowCount(0);
}

std::vector<std::string> LR0Widget::tokenizarCadena(const QString& cadena) {
    std::vector<std::string> tokens;
    
    // Tokenización simple: separar por espacios
    QStringList partes = cadena.split(' ', Qt::SkipEmptyParts);
    
    for (const QString& parte : partes) {
        tokens.push_back(parte.toStdString());
    }
    
    return tokens;
}

void LR0Widget::actualizarListasSimbolos() {
    listaTerminales->clear();
    listaNoTerminales->clear();
    
    for (const auto& t : gramatica->getTerminales()) {
        listaTerminales->addItem(QString::fromStdString(t.nombre));
    }
    
    for (const auto& nt : gramatica->getNoTerminales()) {
        listaNoTerminales->addItem(QString::fromStdString(nt.nombre));
    }
}

void LR0Widget::actualizarTablaProducciones() {
    const auto& producciones = gramatica->getProducciones();
    
    tablaProducciones->setColumnCount(2);
    tablaProducciones->setHorizontalHeaderLabels({"#", "Producción"});
    tablaProducciones->setRowCount(producciones.size());
    
    for (size_t i = 0; i < producciones.size(); i++) {
        // Número de producción
        QTableWidgetItem* itemNum = new QTableWidgetItem(QString::number(i));
        itemNum->setTextAlignment(Qt::AlignCenter);
        tablaProducciones->setItem(i, 0, itemNum);
        
        // Producción
        QString prodStr = QString::fromStdString(producciones[i].toString());
        QTableWidgetItem* itemProd = new QTableWidgetItem(prodStr);
        tablaProducciones->setItem(i, 1, itemProd);
    }
    
    tablaProducciones->resizeColumnsToContents();
    tablaProducciones->horizontalHeader()->setStretchLastSection(true);
}

void LR0Widget::actualizarTablaLR0() {
    if (analizador->RenglonesLR0.empty() || analizador->Vt_Vn.empty()) {
        return;
    }
    
    int numEstados = analizador->RenglonesLR0.size();
    int numSimbolos = analizador->TotSimbolos;
    
    // Configurar tabla
    tablaLR0->setRowCount(numEstados);
    tablaLR0->setColumnCount(numSimbolos + 1);
    
    // Headers
    QStringList headers;
    headers << "Estado";
    for (const auto& simb : analizador->Vt_Vn) {
        headers << QString::fromStdString(simb);
    }
    tablaLR0->setHorizontalHeaderLabels(headers);
    
    // Llenar tabla
    for (int i = 0; i < numEstados; i++) {
        const auto& renglon = analizador->RenglonesLR0[i];
        
        // Columna de estado
        QTableWidgetItem* itemEstado = new QTableWidgetItem(
            QString::number(renglon.IdEdo));
        itemEstado->setTextAlignment(Qt::AlignCenter);
        itemEstado->setBackground(QColor(220, 220, 220));
        itemEstado->setFont(QFont("Arial", 10, QFont::Bold));
        tablaLR0->setItem(i, 0, itemEstado);
        
        // Columnas de acciones
        for (int j = 0; j < numSimbolos; j++) {
            const auto& accion = renglon.Acciones[j];
            QString texto = QString::fromStdString(accion.toString());
            
            QTableWidgetItem* item = new QTableWidgetItem(texto);
            item->setTextAlignment(Qt::AlignCenter);
            
            // Colorear según tipo de acción
            if (!accion.esVacio()) {
                if (accion.esAceptacion()) {
                    item->setBackground(QColor(144, 238, 144)); // Verde claro
                    item->setForeground(QColor(0, 100, 0)); // Verde oscuro
                    item->setFont(QFont("Arial", 10, QFont::Bold));
                } else if (accion.EsDesplazamiento) {
                    item->setBackground(QColor(173, 216, 230)); // Azul claro
                    item->setForeground(QColor(0, 0, 139)); // Azul oscuro
                } else {
                    item->setBackground(QColor(255, 218, 185)); // Naranja claro
                    item->setForeground(QColor(139, 69, 0)); // Naranja oscuro
                }
            }
            
            tablaLR0->setItem(i, j + 1, item);
        }
    }
    
    // Ajustar tamaño de columnas
    tablaLR0->resizeColumnsToContents();
    tablaLR0->horizontalHeader()->setStretchLastSection(false);
}

void LR0Widget::actualizarTablaEstados() {
    if (analizador->ColeccionCanonica.empty()) {
        return;
    }
    
    tablaEstados->setColumnCount(2);
    tablaEstados->setHorizontalHeaderLabels({"Estado", "Ítems"});
    tablaEstados->setRowCount(analizador->ColeccionCanonica.size());
    
    for (size_t i = 0; i < analizador->ColeccionCanonica.size(); i++) {
        const auto& estado = analizador->ColeccionCanonica[i];
        
        // Columna estado
        QTableWidgetItem* itemEstado = new QTableWidgetItem(
            QString("S%1").arg(estado.j));
        itemEstado->setTextAlignment(Qt::AlignCenter);
        itemEstado->setBackground(QColor(220, 220, 220));
        itemEstado->setFont(QFont("Arial", 10, QFont::Bold));
        tablaEstados->setItem(i, 0, itemEstado);
        
        // Columna ítems
        QString itemsStr = QString::fromStdString(
            analizador->conjuntoItemsToString(estado.Sj));
        
        QTableWidgetItem* itemItems = new QTableWidgetItem(itemsStr);
        itemItems->setFont(QFont("Courier New", 9));
        
        // Colorear ítems iniciales o de aceptación
        if (estado.j == 0) {
            itemItems->setBackground(QColor(200, 255, 200)); // Verde claro
        }
        
        // Detectar estados de aceptación
        for (const auto& item : estado.Sj) {
            if (analizador->itemTienePuntoAlFinal(item) && item.NumRegla == 0) {
                itemItems->setBackground(QColor(144, 238, 144)); // Verde
                itemItems->setForeground(QColor(0, 100, 0));
                break;
            }
        }
        
        tablaEstados->setItem(i, 1, itemItems);
    }
    
    tablaEstados->resizeColumnsToContents();
    tablaEstados->horizontalHeader()->setStretchLastSection(true);
    tablaEstados->resizeRowsToContents();
}

void LR0Widget::mostrarLexico(const std::vector<std::string>& tokens) {
    tablaLexico->setColumnCount(2);
    tablaLexico->setHorizontalHeaderLabels({"Lexema", "Token"});
    tablaLexico->setRowCount(tokens.size());
    
    for (size_t i = 0; i < tokens.size(); i++) {
        // Lexema
        QTableWidgetItem* itemLexema = new QTableWidgetItem(
            QString::fromStdString(tokens[i]));
        itemLexema->setTextAlignment(Qt::AlignCenter);
        tablaLexico->setItem(i, 0, itemLexema);
        
        // Token (identificar tipo)
        QString tipoToken;
        if (tokens[i] == "$") {
            tipoToken = "FIN";
        } else if (tokens[i] == "id" || tokens[i] == "num") {
            tipoToken = "IDENTIFICADOR";
        } else {
            // Buscar en terminales
            bool encontrado = false;
            for (const auto& t : gramatica->getTerminales()) {
                if (t.nombre == tokens[i]) {
                    tipoToken = QString::fromStdString(t.nombre).toUpper();
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado) {
                tipoToken = "DESCONOCIDO";
            }
        }
        
        QTableWidgetItem* itemToken = new QTableWidgetItem(tipoToken);
        itemToken->setTextAlignment(Qt::AlignCenter);
        
        // Colorear según tipo
        if (tipoToken == "FIN") {
            itemToken->setBackground(QColor(255, 200, 200));
        } else if (tipoToken == "DESCONOCIDO") {
            itemToken->setBackground(QColor(255, 100, 100));
            itemToken->setForeground(Qt::white);
        } else {
            itemToken->setBackground(QColor(200, 230, 255));
        }
        
        tablaLexico->setItem(i, 1, itemToken);
    }
    
    tablaLexico->resizeColumnsToContents();
    tablaLexico->horizontalHeader()->setStretchLastSection(true);
}

void LR0Widget::mostrarEjecucion(const std::vector<AnalizadorLR0::PasoLR0>& pasos) {
    tablaEjecucion->setColumnCount(3);
    tablaEjecucion->setHorizontalHeaderLabels({"Pila", "Entrada", "Acción"});
    tablaEjecucion->setRowCount(pasos.size());
    
    for (size_t i = 0; i < pasos.size(); i++) {
        const auto& paso = pasos[i];
        
        // Columna Pila
        QTableWidgetItem* itemPila = new QTableWidgetItem(
            QString::fromStdString(paso.pila));
        itemPila->setFont(QFont("Courier New", 9));
        tablaEjecucion->setItem(i, 0, itemPila);
        
        // Columna Entrada
        QTableWidgetItem* itemEntrada = new QTableWidgetItem(
            QString::fromStdString(paso.cadena));
        itemEntrada->setFont(QFont("Courier New", 9));
        itemEntrada->setTextAlignment(Qt::AlignCenter);
        tablaEjecucion->setItem(i, 1, itemEntrada);
        
        // Columna Acción
        QString accion = QString::fromStdString(paso.accion);
        QTableWidgetItem* itemAccion = new QTableWidgetItem(accion);
        itemAccion->setFont(QFont("Arial", 9));
        itemAccion->setTextAlignment(Qt::AlignCenter);
        
        // Colorear según tipo de acción
        if (accion.contains("ACEPTAR")) {
            itemAccion->setBackground(QColor(144, 238, 144)); // Verde
            itemAccion->setForeground(QColor(0, 100, 0));
            itemAccion->setFont(QFont("Arial", 10, QFont::Bold));
            
            // Colorear toda la fila
            itemPila->setBackground(QColor(200, 255, 200));
            itemEntrada->setBackground(QColor(200, 255, 200));
        } else if (accion.contains("ERROR")) {
            itemAccion->setBackground(QColor(255, 100, 100)); // Rojo
            itemAccion->setForeground(Qt::white);
            itemAccion->setFont(QFont("Arial", 9, QFont::Bold));
            
            // Colorear toda la fila
            itemPila->setBackground(QColor(255, 200, 200));
            itemEntrada->setBackground(QColor(255, 200, 200));
        } else if (accion.startsWith('d')) {
            itemAccion->setBackground(QColor(173, 216, 230)); // Azul claro
            itemAccion->setForeground(QColor(0, 0, 139));
        } else if (accion.startsWith('r')) {
            itemAccion->setBackground(QColor(255, 218, 185)); // Naranja claro
            itemAccion->setForeground(QColor(139, 69, 0));
        } else if (accion == "Inicio") {
            itemAccion->setBackground(QColor(220, 220, 220));
            itemAccion->setFont(QFont("Arial", 9, QFont::Courier));
        }
        
        tablaEjecucion->setItem(i, 2, itemAccion);
    }
    
    tablaEjecucion->resizeColumnsToContents();
    tablaEjecucion->horizontalHeader()->setStretchLastSection(true);
    
    // Scroll automático al último paso
    tablaEjecucion->scrollToBottom();
}

void LR0Widget::aplicarEstiloTabla(QTableWidget* tabla) {
    tabla->setAlternatingRowColors(true);
    tabla->setStyleSheet(
        "QTableWidget {"
        "    gridline-color: #d0d0d0;"
        "    background-color: white;"
        "    alternate-background-color: #f5f5f5;"
        "}"
        "QTableWidget::item {"
        "    padding: 5px;"
        "}"
        "QHeaderView::section {"
        "    background-color: #4a90e2;"
        "    color: white;"
        "    padding: 8px;"
        "    border: 1px solid #3a7bc8;"
        "    font-weight: bold;"
        "    font-size: 11px;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #b3d9ff;"
        "}"
    );
    
    tabla->verticalHeader()->setVisible(false);
    tabla->setSelectionBehavior(QTableWidget::SelectRows);
    tabla->setSelectionMode(QTableWidget::SingleSelection);
    tabla->setEditTriggers(QTableWidget::NoEditTriggers);
}

void LR0Widget::colorearCeldasTablaLR0() {
    // Este método es llamado opcionalmente para resaltar conflictos
    // Por ahora, la coloración ya se hace en actualizarTablaLR0()
    // Puedes implementar aquí detección de conflictos shift/reduce o reduce/reduce
    
    for (int i = 0; i < tablaLR0->rowCount(); i++) {
        for (int j = 1; j < tablaLR0->columnCount(); j++) {
            QTableWidgetItem* item = tablaLR0->item(i, j);
            if (!item) continue;
            
            QString texto = item->text();
            
            // Detectar conflictos (múltiples acciones en una celda)
            if (texto.contains('/') || texto.contains(',')) {
                item->setBackground(QColor(255, 100, 100)); // Rojo para conflictos
                item->setForeground(Qt::white);
                item->setFont(QFont("Arial", 9, QFont::Bold));
                item->setToolTip("⚠️ Conflicto detectado");
            }
        }
    }
}


























