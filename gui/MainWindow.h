#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QAction>
#include <QLabel>

// Forward declarations de los módulos
class AnalizadorLexicoWidget;
class CalculadoraWidget;
class LL1Widget;
class LR0Widget;
class GramaticasWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void mostrarAnalizadorLexico();
    void mostrarCalculadora();
    void mostrarLL1();
    void mostrarLR0();
    void mostrarGramatica();
    void mostrarAcercaDe();

private:
    void setupUI();
    void crearMenus();
    void crearWidgetInicial();
    
    // Contenedor principal
    QStackedWidget* stackedWidget;
    
    // Widget inicial (pantalla de bienvenida)
    QWidget* widgetInicial;
    
    // Módulos
    AnalizadorLexicoWidget* analizadorLexicoWidget;
    CalculadoraWidget* calculadoraWidget;
    LL1Widget* ll1Widget;
    LR0Widget* lr0Widget;
    GramaticasWidget* gramaticasWidget;
    // Agregar más módulos aquí según sea necesario
    
    // Menús
    QMenu* menuHerramientas;
    QMenu* menuAyuda;
    
    // Acciones
    QAction* accionAnalizadorLexico;
    QAction* accionCalculadora;
    QAction* accionLL1;
    QAction* accionLR0;
    QAction* accionGramatica;
    QAction* accionAcercaDe;
};

#endif // MAINWINDOW_H
