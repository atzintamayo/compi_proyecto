#ifndef LL1WIDGET_H
#define LL1WIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>

#include "Gramatica.h"
#include "ConjuntosLL1.h"
#include "TablaLL1.h"
#include "ParserLL1.h"
#include "Lexer.h"
#include "AFD.h"
#include "Scanner.h"

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
    void onMostrarSimbolos();
    
    // Slots para análisis
    void onCalcularConjuntos();
    void onConstruirTabla();
    
    // Slots para parser
    void onParsear();
    void onLimpiarParser();
    
    // Slots para análisis completo
    void onCargarAFD();
    void onEliminarMapeo();
    void onAutoMapear();
    void onAnalizarCompleto();
    void onLimpiarCompleto();

private:
    void setupUI();
    void setupTabGramatica();
    void setupTabSimbolos();
    void setupTabConjuntos();
    void setupTabTabla();
    void setupTabParser();
    void setupTabCompleto();
    
    void mostrarSimbolos();
    void mostrarConjuntos();
    void mostrarTabla();
    void mostrarTraza();
    void mostrarTokensCompleto();
    void mostrarTrazaCompleto();
    
    // Métodos para análisis completo
    void extraerTokensDelAFD();
    void poblarTablaMapeoConTokens();
    void actualizarEstadoAnalisisCompleto();
    
    // Componentes core
    Gramatica* gramatica;
    ConjuntosLL1* conjuntos;
    TablaLL1* tabla;
    ParserLL1* parser;
    Lexer* lexer;
    
    // Para análisis completo (léxico + sintáctico)
    AFD* afdCompleto;
    Scanner* scannerCompleto;
    
    // Estructura para información de tokens del AFD
    struct TokenInfo {
        int id;
        int estadoId;
        bool existe;
    };
    std::map<int, TokenInfo> tokensDisponiblesAFD;
    std::map<int, std::string> mapeoTokenTerminal;
    
    // UI
    QTabWidget* tabWidget;
    
    // Tab 1: Gramática
    QWidget* tabGramatica;
    QTextEdit* txtGramatica;
    QPushButton* btnCargarGramatica;
    QPushButton* btnLimpiarGramatica;
    QPushButton* btnCargarEjemplo;
    QLabel* lblEstadoGramatica;
    
    // Tab 2: Símbolos
    QWidget* tabSimbolos;
    QPushButton* btnMostrarSimbolos;
    QListWidget* listaTerminales;
    QListWidget* listaNoTerminales;
    QLabel* lblSimboloInicial;
    QLabel* lblResumenSimbolos;
    
    // Tab 3: FIRST/FOLLOW
    QWidget* tabConjuntos;
    QPushButton* btnCalcularConjuntos;
    QTextEdit* txtFirst;
    QTextEdit* txtFollow;
    
    // Tab 4: Tabla LL(1)
    QWidget* tabTabla;
    QPushButton* btnConstruirTabla;
    QTableWidget* tablaLL1;
    QTextEdit* txtConflictos;
    
    // Tab 5: Parser
    QWidget* tabParser;
    QLineEdit* txtEntrada;
    QPushButton* btnParsear;
    QPushButton* btnLimpiarParser;
    QTableWidget* tablaTraza;
    QLabel* lblResultado;
    
    // Tab 6: Análisis Completo
    QWidget* tabCompleto;
    QPushButton* btnCargarAFD;
    QLabel* lblEstadoAFD;
    QLabel* lblTokensDetectados;
    QTableWidget* tablaMapeo;
    QPushButton* btnAutoMapear;
    QPushButton* btnEliminarMapeo;
    QTextEdit* txtEntradaCompleto;
    QPushButton* btnAnalizarCompleto;
    QPushButton* btnLimpiarCompleto;
    QTableWidget* tablaTokensCompleto;
    QTableWidget* tablaTrazaCompleto;
    QLabel* lblResultadoCompleto;
};

#endif // LL1WIDGET_H