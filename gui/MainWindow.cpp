#include "MainWindow.h"
#include "modules/analizador_lexico/AnalizadorLexicoWidget.h"
#include "modules/calculadora/CalculadoraWidget.h"

#include <QVBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Inicializar widgets como nullptr
    analizadorLexicoWidget = nullptr;
    calculadoraWidget = nullptr;
    
    setupUI();
    crearMenus();
    
    setWindowTitle("Compilador Tools - Suite de Herramientas");
    resize(1200, 800);
}

MainWindow::~MainWindow() {
    // Los widgets se eliminan automáticamente por Qt (son hijos de stackedWidget)
}

void MainWindow::setupUI() {
    // Crear contenedor principal con QStackedWidget
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    
    // Crear widget inicial (pantalla de bienvenida)
    crearWidgetInicial();
    stackedWidget->addWidget(widgetInicial);
}

void MainWindow::crearWidgetInicial() {
    widgetInicial = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widgetInicial);
    
    // Título
    QLabel* titulo = new QLabel("Compilador Tools");
    QFont fuenteTitulo;
    fuenteTitulo.setPointSize(32);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);
    titulo->setAlignment(Qt::AlignCenter);
    layout->addWidget(titulo);
    
    // Subtítulo
    QLabel* subtitulo = new QLabel("Suite de Herramientas para Compiladores");
    QFont fuenteSubtitulo;
    fuenteSubtitulo.setPointSize(16);
    subtitulo->setFont(fuenteSubtitulo);
    subtitulo->setAlignment(Qt::AlignCenter);
    subtitulo->setStyleSheet("color: gray;");
    layout->addWidget(subtitulo);
    
    layout->addStretch();
    
    // Instrucciones
    QLabel* instrucciones = new QLabel(
        "Seleccione una herramienta del menú superior para comenzar:\n\n"
        "• Analizador Léxico - Crear y gestionar AFN/AFD\n"
        "• Calculadora - Evaluación con recursividad por la izquierda\n"
        "• Gramática - (Próximamente)\n"
    );
    instrucciones->setAlignment(Qt::AlignCenter);
    instrucciones->setWordWrap(true);
    QFont fuenteInstrucciones;
    fuenteInstrucciones.setPointSize(12);
    instrucciones->setFont(fuenteInstrucciones);
    layout->addWidget(instrucciones);
    
    layout->addStretch();
    
    // Versión
    QLabel* version = new QLabel("Versión 1.0");
    version->setAlignment(Qt::AlignCenter);
    version->setStyleSheet("color: lightgray;");
    layout->addWidget(version);
}

void MainWindow::crearMenus() {
    // Menú Herramientas
    menuHerramientas = menuBar()->addMenu("&Herramientas");
    
    accionAnalizadorLexico = new QAction("&Analizador Léxico", this);
    accionAnalizadorLexico->setShortcut(QKeySequence("Ctrl+1"));
    accionAnalizadorLexico->setStatusTip("Abrir el módulo de análisis léxico (AFN/AFD)");
    connect(accionAnalizadorLexico, &QAction::triggered, this, &MainWindow::mostrarAnalizadorLexico);
    menuHerramientas->addAction(accionAnalizadorLexico);
    
    accionCalculadora = new QAction("&Calculadora", this);
    accionCalculadora->setShortcut(QKeySequence("Ctrl+2"));
    accionCalculadora->setStatusTip("Abrir la calculadora con recursividad");
    connect(accionCalculadora, &QAction::triggered, this, &MainWindow::mostrarCalculadora);
    menuHerramientas->addAction(accionCalculadora);
    
    accionGramatica = new QAction("&Gramática", this);
    accionGramatica->setShortcut(QKeySequence("Ctrl+3"));
    accionGramatica->setStatusTip("Abrir el módulo de gramáticas");
    accionGramatica->setEnabled(false); // Deshabilitado hasta implementar
    connect(accionGramatica, &QAction::triggered, this, &MainWindow::mostrarGramatica);
    menuHerramientas->addAction(accionGramatica);
    
    menuHerramientas->addSeparator();
    
    QAction* accionInicio = new QAction("&Inicio", this);
    accionInicio->setShortcut(QKeySequence("Ctrl+H"));
    connect(accionInicio, &QAction::triggered, [this]() {
        stackedWidget->setCurrentWidget(widgetInicial);
    });
    menuHerramientas->addAction(accionInicio);
    
    // Menú Ayuda
    menuAyuda = menuBar()->addMenu("A&yuda");
    
    accionAcercaDe = new QAction("&Acerca de", this);
    connect(accionAcercaDe, &QAction::triggered, this, &MainWindow::mostrarAcercaDe);
    menuAyuda->addAction(accionAcercaDe);
}

void MainWindow::mostrarAnalizadorLexico() {
    // Crear el widget solo la primera vez (lazy loading)
    if (!analizadorLexicoWidget) {
        analizadorLexicoWidget = new AnalizadorLexicoWidget(this);
        stackedWidget->addWidget(analizadorLexicoWidget);
    }
    
    stackedWidget->setCurrentWidget(analizadorLexicoWidget);
}

void MainWindow::mostrarCalculadora() {
    // Crear el widget solo la primera vez (lazy loading)
    if (!calculadoraWidget) {
        calculadoraWidget = new CalculadoraWidget(this);
        stackedWidget->addWidget(calculadoraWidget);
    }
    
    stackedWidget->setCurrentWidget(calculadoraWidget);
}

void MainWindow::mostrarGramatica() {
    QMessageBox::information(this, "Próximamente", 
        "El módulo de Gramática estará disponible próximamente.");
}

void MainWindow::mostrarAcercaDe() {
    QMessageBox::about(this, "Acerca de Compilador Tools",
        "<h2>Compilador Tools</h2>"
        "<p>Suite de herramientas para el curso de Compiladores</p>"
        "<p><b>Versión:</b> 1.0</p>"
        "<p><b>Módulos disponibles:</b></p>"
        "<ul>"
        "<li>Analizador Léxico (AFN/AFD/Scanner)</li>"
        "<li>Calculadora con recursividad</li>"
        "<li>Gramática de gramáticas (próximamente)</li>"
        "</ul>"
    );
}