#ifndef CREARAFNDIALOG_H
#define CREARAFNDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QStackedWidget>

#include "AFN.h"
#include "AFNManager.h"

class CrearAFNDialog : public QDialog {
    Q_OBJECT

public:
    explicit CrearAFNDialog(QWidget *parent = nullptr, AFNManager* manager = nullptr);
    ~CrearAFNDialog();
    
    AFN* getAFNCreado() const { return afnResultado; }
    QString getNombre() const;
    QString getDescripcion() const;

private slots:
    void onTipoOperacionCambiado(int index);
    void onAceptar();
    void onCancelar();
    void actualizarListasAFN();

private:
    // Operación seleccionada
    QComboBox* comboTipoOperacion;
    QStackedWidget* stackedWidget;
    
    // Widgets para diferentes operaciones
    // Básico (char)
    QWidget* widgetBasicoChar;
    QLineEdit* txtCaracter;
    
    // Básico (rango)
    QWidget* widgetBasicoRango;
    QLineEdit* txtCaracterInicio;
    QLineEdit* txtCaracterFin;
    
    // Unir
    QWidget* widgetUnir;
    QListWidget* listaUnir1;
    QListWidget* listaUnir2;
    
    // Concatenar
    QWidget* widgetConcatenar;
    QListWidget* listaConcatenar1;
    QListWidget* listaConcatenar2;
    
    // Kleene
    QWidget* widgetKleene;
    QListWidget* listaKleene;
    
    // Positiva
    QWidget* widgetPositiva;
    QListWidget* listaPositiva;
    
    // Información general
    QLineEdit* txtNombre;
    QTextEdit* txtDescripcion;
    
    // Botones
    QPushButton* btnAceptar;
    QPushButton* btnCancelar;
    
    // Resultado
    AFN* afnResultado;
    
    // Gestor externo (referencia)
    AFNManager* afnManager;
    
    // Métodos auxiliares
    void setupUI();
    void setupBasicoChar();
    void setupBasicoRango();
    void setupUnir();
    void setupConcatenar();
    void setupKleene();
    void setupPositiva();
    
    AFN* obtenerAFNSeleccionado(QListWidget* lista);
};

#endif // CREARAFNDIALOG_H