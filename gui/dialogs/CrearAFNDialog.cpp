#include "CrearAFNDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>

CrearAFNDialog::CrearAFNDialog(QWidget *parent, AFNManager* manager) : QDialog(parent) {
    afnResultado = nullptr;
    afnManager = manager;
    setupUI();
    actualizarListasAFN();
    setWindowTitle("Crear AFN");
    resize(500, 400);
}

CrearAFNDialog::~CrearAFNDialog() {}

void CrearAFNDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Tipo de operación
    mainLayout->addWidget(new QLabel("Tipo de operación:"));
    comboTipoOperacion = new QComboBox();
    comboTipoOperacion->addItem("Básico (carácter)");
    comboTipoOperacion->addItem("Básico (rango)");
    comboTipoOperacion->addItem("Kleene (*)");
    comboTipoOperacion->addItem("Positiva (+)");
    connect(comboTipoOperacion, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CrearAFNDialog::onTipoOperacionCambiado);
    mainLayout->addWidget(comboTipoOperacion);
    
    // Stacked widget
    stackedWidget = new QStackedWidget();
    setupBasicoChar();
    setupBasicoRango();
    setupKleene();
    setupPositiva();
    mainLayout->addWidget(stackedWidget);
    
    // Información general
    QFormLayout* formInfo = new QFormLayout();
    txtNombre = new QLineEdit();
    txtNombre->setPlaceholderText("Ej: AFN_letras");
    txtDescripcion = new QTextEdit();
    txtDescripcion->setMaximumHeight(60);
    txtDescripcion->setPlaceholderText("Descripción opcional");
    formInfo->addRow("Nombre:", txtNombre);
    formInfo->addRow("Descripción:", txtDescripcion);
    mainLayout->addLayout(formInfo);
    
    // Botones
    QHBoxLayout* layoutBotones = new QHBoxLayout();
    btnAceptar = new QPushButton("Crear");
    btnCancelar = new QPushButton("Cancelar");
    connect(btnAceptar, &QPushButton::clicked, this, &CrearAFNDialog::onAceptar);
    connect(btnCancelar, &QPushButton::clicked, this, &CrearAFNDialog::onCancelar);
    layoutBotones->addStretch();
    layoutBotones->addWidget(btnAceptar);
    layoutBotones->addWidget(btnCancelar);
    mainLayout->addLayout(layoutBotones);
}

void CrearAFNDialog::setupBasicoChar() {
    widgetBasicoChar = new QWidget();
    QFormLayout* layout = new QFormLayout(widgetBasicoChar);
    txtCaracter = new QLineEdit();
    txtCaracter->setMaxLength(1);
    txtCaracter->setPlaceholderText("a");
    layout->addRow("Carácter:", txtCaracter);
    stackedWidget->addWidget(widgetBasicoChar);
}

void CrearAFNDialog::setupBasicoRango() {
    widgetBasicoRango = new QWidget();
    QFormLayout* layout = new QFormLayout(widgetBasicoRango);
    txtCaracterInicio = new QLineEdit();
    txtCaracterInicio->setMaxLength(1);
    txtCaracterInicio->setPlaceholderText("a");
    txtCaracterFin = new QLineEdit();
    txtCaracterFin->setMaxLength(1);
    txtCaracterFin->setPlaceholderText("z");
    layout->addRow("Desde:", txtCaracterInicio);
    layout->addRow("Hasta:", txtCaracterFin);
    stackedWidget->addWidget(widgetBasicoRango);
}

void CrearAFNDialog::setupKleene() {
    widgetKleene = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widgetKleene);
    layout->addWidget(new QLabel("Seleccione el AFN para aplicar la cerradura de Kleene (*):"));
    listaKleene = new QListWidget();
    layout->addWidget(listaKleene);
    QLabel* infoLabel = new QLabel("Seleccione un AFN de la lista para aplicar la cerradura de Kleene.\nSi no hay AFNs disponibles, cree uno primero usando la opción Básico.");
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    stackedWidget->addWidget(widgetKleene);
}

void CrearAFNDialog::setupPositiva() {
    widgetPositiva = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widgetPositiva);
    layout->addWidget(new QLabel("Seleccione el AFN para aplicar la cerradura positiva (+):"));
    listaPositiva = new QListWidget();
    layout->addWidget(listaPositiva);
    QLabel* infoLabel = new QLabel("Seleccione un AFN de la lista para aplicar la cerradura positiva.\nSi no hay AFNs disponibles, cree uno primero usando la opción Básico.");
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    stackedWidget->addWidget(widgetPositiva);
}

void CrearAFNDialog::onTipoOperacionCambiado(int index) {
    stackedWidget->setCurrentIndex(index);
}

void CrearAFNDialog::onAceptar() {
    if (txtNombre->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Debe ingresar un nombre para el AFN");
        return;
    }
    
    int operacion = comboTipoOperacion->currentIndex();
    AFN* nuevoAFN = new AFN();
    
    switch(operacion) {
        case 0: // Básico char
            if (txtCaracter->text().isEmpty()) {
                QMessageBox::warning(this, "Error", "Ingrese un carácter");
                delete nuevoAFN;
                return;
            }
            nuevoAFN->CrearBasico(txtCaracter->text().at(0).toLatin1());
            break;
            
        case 1: // Básico rango
            if (txtCaracterInicio->text().isEmpty() || txtCaracterFin->text().isEmpty()) {
                QMessageBox::warning(this, "Error", "Ingrese rango completo");
                delete nuevoAFN;
                return;
            }
            nuevoAFN->CrearBasico(txtCaracterInicio->text().at(0).toLatin1(),
                                  txtCaracterFin->text().at(0).toLatin1());
            break;
            
        case 2: { // Kleene
            QListWidgetItem* item = listaKleene->currentItem();
            if (!item || !afnManager) {
                QMessageBox::warning(this, "Error", "Seleccione un AFN de la lista");
                delete nuevoAFN;
                return;
            }
            // Buscar el AFN por nombre
            auto afns = afnManager->listarAFNs();
            AFN* afnBase = nullptr;
            for (const auto& info : afns) {
                if (info.nombre == item->text().toStdString()) {
                    afnBase = info.afn;
                    break;
                }
            }
            if (!afnBase) {
                QMessageBox::warning(this, "Error", "AFN seleccionado no encontrado");
                delete nuevoAFN;
                return;
            }
            *nuevoAFN = *afnBase;
            nuevoAFN->CerraduraKleene();
            break;
        }
        case 3: { // Positiva
            QListWidgetItem* item = listaPositiva->currentItem();
            if (!item || !afnManager) {
                QMessageBox::warning(this, "Error", "Seleccione un AFN de la lista");
                delete nuevoAFN;
                return;
            }
            // Buscar el AFN por nombre
            auto afns = afnManager->listarAFNs();
            AFN* afnBase = nullptr;
            for (const auto& info : afns) {
                if (info.nombre == item->text().toStdString()) {
                    afnBase = info.afn;
                    break;
                }
            }
            if (!afnBase) {
                QMessageBox::warning(this, "Error", "AFN seleccionado no encontrado");
                delete nuevoAFN;
                return;
            }
            *nuevoAFN = *afnBase;
            nuevoAFN->CerraduraPositiva();
            break;
        }
    }
    
    afnResultado = nuevoAFN;
    accept();
}

void CrearAFNDialog::onCancelar() {
    reject();
}

void CrearAFNDialog::actualizarListasAFN() {
    if (!afnManager) return;
    
    // Limpiar listas existentes
    listaKleene->clear();
    listaPositiva->clear();
    
    // Obtener AFNs existentes y añadirlos a ambas listas
    auto afns = afnManager->listarAFNs();
    for (const auto& info : afns) {
        QString nombre = QString::fromStdString(info.nombre);
        listaKleene->addItem(nombre);
        listaPositiva->addItem(nombre);
    }
}

QString CrearAFNDialog::getNombre() const {
    return txtNombre->text();
}

QString CrearAFNDialog::getDescripcion() const {
    return txtDescripcion->toPlainText();
}