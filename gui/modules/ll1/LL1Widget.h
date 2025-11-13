#ifndef LL1WIDGET_H
#define LL1WIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>

#include "Gramatica.h"
#include "ConjuntosLL1.h"
#include "TablaLL1.h"
#include "ParserLL1.h"
#include "Lexer.h"

class LL1Widget : public QWidget {
    Q_OBJECT

public:
    explicit LL1Widget(QWidget *parent = nullptr);
    ~LL1Widget();

private slots:
    // Slots para gramática
    void onCargarGramatica();
    void onLimpiarGramatica();
    void onCargarEjemplo();
    
    // Slots para análisis
    void onCalcularConjuntos();
    void onConstruirTabla();
    
    // Slots para parser
    void onParsear();
    void onLimpiarParser();

private:
    void setupUI();
    void setupTabGramatica();
    void setupTabConjuntos();
    void setupTabTabla();
    void setupTabParser();
    
    void mostrarConjuntos();
    void mostrarTabla();
    void mostrarTraza();
    
    // Componentes core
    Gramatica* gramatica;
    ConjuntosLL1* conjuntos;
    TablaLL1* tabla;
    ParserLL1* parser;
    Lexer* lexer;
    
    // UI
    QTabWidget* tabWidget;
    
    // Tab 1: Gramática
    QWidget* tabGramatica;
    QTextEdit* txtGramatica;
    QPushButton* btnCargarGramatica;
    QPushButton* btnLimpiarGramatica;
    QPushButton* btnCargarEjemplo;
    QLabel* lblEstadoGramatica;
    
    // Tab 2: FIRST/FOLLOW
    QWidget* tabConjuntos;
    QPushButton* btnCalcularConjuntos;
    QTextEdit* txtFirst;
    QTextEdit* txtFollow;
    
    // Tab 3: Tabla LL(1)
    QWidget* tabTabla;
    QPushButton* btnConstruirTabla;
    QTableWidget* tablaLL1;
    QTextEdit* txtConflictos;
    
    // Tab 4: Parser
    QWidget* tabParser;
    QLineEdit* txtEntrada;
    QPushButton* btnParsear;
    QPushButton* btnLimpiarParser;
    QTableWidget* tablaTraza;
    QLabel* lblResultado;
};

#endif // LL1WIDGET_H