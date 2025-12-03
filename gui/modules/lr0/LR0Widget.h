#ifndef LR0WIDGET_H
#define LR0WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSplitter>
#include <QTabWidget>
#include "../../core/lr0/AnalizadorLR0.h"
#include "../../core/lr0/GramaticaLR0.h"  // ← Cambiar nombre

class LR0Widget : public QWidget {
    Q_OBJECT
    
public:
    explicit LR0Widget(LR0::GramaticaLR0* gram = nullptr, QWidget* parent = nullptr);  // ← Cambiar tipo
    ~LR0Widget();
    
private slots:
    void onCargarGramatica();
    void onCrearTablaLR0();
    void onAnalizarLR0();
    void onLimpiarTodo();
    
private:
    QTextEdit* textEditGramatica;
    QLineEdit* lineEditSigma;
    QPushButton* btnCargarGramatica;
    QPushButton* btnCrearTabla;
    QPushButton* btnAnalizar;
    QPushButton* btnLimpiar;
    
    QListWidget* listaNoTerminales;
    QListWidget* listaTerminales;
    
    QTableWidget* tablaLR0;
    QTableWidget* tablaLexico;
    QTableWidget* tablaEjecucion;
    QTableWidget* tablaEstados;
    QTableWidget* tablaProducciones;
    
    AnalizadorLR0* analizador;
    LR0::GramaticaLR0* gramatica;  // ← Cambiar tipo
    bool gramaticaPropia;
    
    void configurarLayout();
    void crearGramaticaEjemplo();
    void actualizarTablaLR0();
    void actualizarListasSimbolos();
    void actualizarTablaEstados();
    void actualizarTablaProducciones();
    void mostrarLexico(const std::vector<std::string>& tokens);
    void mostrarEjecucion(const std::vector<AnalizadorLR0::PasoLR0>& pasos);
    
    std::vector<std::string> tokenizarCadena(const QString& cadena);
    
    void aplicarEstiloTabla(QTableWidget* tabla);
    void colorearCeldasTablaLR0();
};

#endif // LR0WIDGET_H