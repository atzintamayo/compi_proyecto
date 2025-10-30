#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QCheckBox>
#include <QLabel>

#include "AFNManager.h"
#include "AFD.h"
#include "Scanner.h"

class AFNTableWidget;
class AFDTableWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slots para AFN
    void onCrearAFN();
    void onEliminarAFN();
    void onMarcarAceptacion();
    void onAsignarToken();
    void onAFNSeleccionado();
    void onVerAFN();
    
    // Slots para AFD
    void onConvertirAFD();
    void onGuardarAFD();
    void onCargarAFD();
    
    // Slots para Scanner
    void onEscanear();
    void onCargarArchivo();
    void onLimpiarScanner();

private:
    // Core
    AFNManager* afnManager;
    AFD* afdActual;
    Scanner* scanner;
    
    // UI Components
    QTabWidget* tabWidget;
    
    // Tab 1: Gestión de AFNs
    QWidget* tabAFN;
    QListWidget* listaAFNs;
    QPushButton* btnCrearAFN;
    QPushButton* btnEliminarAFN;
    QPushButton* btnMarcarAceptacion;
    QPushButton* btnAsignarToken;
    QPushButton* btnVerAFN;
    AFNTableWidget* tablaAFN;
    
    // Tab 2: AFD
    QWidget* tabAFD;
    QListWidget* listaAFNsParaAFD;
    QPushButton* btnConvertirAFD;
    QPushButton* btnGuardarAFD;
    QPushButton* btnCargarAFD;
    AFDTableWidget* tablaAFD;
    QLabel* labelInfoAFD;
    
    // Tab 3: Scanner
    QWidget* tabScanner;
    QTextEdit* textoEntrada;
    QPushButton* btnCargarArchivo;
    QPushButton* btnEscanear;
    QPushButton* btnLimpiarScanner;
    QCheckBox* chkOmitirEspacios;
    QTableWidget* tablaTokens;
    QTextEdit* textoErrores;
    
    // Métodos auxiliares
    void setupUI();
    void setupTabAFN();
    void setupTabAFD();
    void setupTabScanner();
    
    void actualizarListaAFNs();
    void actualizarListaAFNsParaAFD();
    void mostrarAFNEnTabla(int idAFN);
    void mostrarAFDEnTabla();
    void mostrarTokensEnTabla();
    void mostrarErrores();
};

#endif // MAINWINDOW_H