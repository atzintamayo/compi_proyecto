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
    comboTipoOperacion->addItem("Básico (carácter)");      // 0
    comboTipoOperacion->addItem("Básico (rango)");         // 1
    comboTipoOperacion->addItem("Unir (OR)");              // 2
    comboTipoOperacion->addItem("Concatenar");             // 3
    comboTipoOperacion->addItem("Kleene (*)");             // 4
    comboTipoOperacion->addItem("Positiva (+)");           // 5
    connect(comboTipoOperacion, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CrearAFNDialog::onTipoOperacionCambiado);
    mainLayout->addWidget(comboTipoOperacion);
    
    // Stacked widget
    stackedWidget = new QStackedWidget();
    setupBasicoChar();
    setupBasicoRango();
    setupUnir();
    setupConcatenar();
    setupKleene();
    setupPositiva();
    mainLayout->addWidget(stackedWidget);
    
    // Información general
    QFormLayout* formInfo = new QFormLayout();
    txtNombre = new QLineEdit();
    txtNombre->setPlaceholderText("Ej: AFN_alfanumerico");
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

void CrearAFNDialog::setupUnir() {
    widgetUnir = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widgetUnir);
    
    layout->addWidget(new QLabel("Seleccione los dos AFNs para UNIR (OR):"));
    
    QHBoxLayout* layoutListas = new QHBoxLayout();
    
    QVBoxLayout* layoutIzq = new QVBoxLayout();
    layoutIzq->addWidget(new QLabel("AFN 1:"));
    listaUnir1 = new QListWidget();
    layoutIzq->addWidget(listaUnir1);
    layoutListas->addLayout(layoutIzq);
    
    QVBoxLayout* layoutDer = new QVBoxLayout();
    layoutDer->addWidget(new QLabel("AFN 2:"));
    listaUnir2 = new QListWidget();
    layoutDer->addWidget(listaUnir2);
    layoutListas->addLayout(layoutDer);
    
    layout->addLayout(layoutListas);
    
    QLabel* infoLabel = new QLabel("La operación UNIR (OR) crea un nuevo AFN que acepta cualquiera de los dos lenguajes.\nEjemplo: [a-z] OR [0-9] acepta letras o dígitos.");
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    
    stackedWidget->addWidget(widgetUnir);
}

void CrearAFNDialog::setupConcatenar() {
    widgetConcatenar = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widgetConcatenar);
    
    layout->addWidget(new QLabel("Seleccione los dos AFNs para CONCATENAR:"));
    
    QHBoxLayout* layoutListas = new QHBoxLayout();
    
    QVBoxLayout* layoutIzq = new QVBoxLayout();
    layoutIzq->addWidget(new QLabel("AFN 1 (primero):"));
    listaConcatenar1 = new QListWidget();
    layoutIzq->addWidget(listaConcatenar1);
    layoutListas->addLayout(layoutIzq);
    
    QVBoxLayout* layoutDer = new QVBoxLayout();
    layoutDer->addWidget(new QLabel("AFN 2 (después):"));
    listaConcatenar2 = new QListWidget();
    layoutDer->addWidget(listaConcatenar2);
    layoutListas->addLayout(layoutDer);
    
    layout->addLayout(layoutListas);
    
    QLabel* infoLabel = new QLabel("La operación CONCATENAR une dos AFNs en secuencia.\nEjemplo: [a-z] concatenado con [0-9] acepta una letra seguida de un dígito.");
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);
    
    stackedWidget->addWidget(widgetConcatenar);
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
            
        case 2: { // Unir (OR)
            QListWidgetItem* item1 = listaUnir1->currentItem();
            QListWidgetItem* item2 = listaUnir2->currentItem();
            
            if (!item1 || !item2 || !afnManager) {
                QMessageBox::warning(this, "Error", "Seleccione dos AFNs de las listas");
                delete nuevoAFN;
                return;
            }
            
            int id1 = item1->data(Qt::UserRole).toInt();
            int id2 = item2->data(Qt::UserRole).toInt();
            
            AFN* afn1 = afnManager->obtenerAFN(id1);
            AFN* afn2 = afnManager->obtenerAFN(id2);
            
            if (!afn1 || !afn2) {
                QMessageBox::warning(this, "Error", "AFNs seleccionados no encontrados");
                delete nuevoAFN;
                return;
            }
            
            // Copiar el primer AFN y unir con el segundo
            *nuevoAFN = *afn1;
            nuevoAFN->unirAFN(afn2);  // Método correcto: unirAFN (no Unir)
            break;
        }
        
        case 3: { // Concatenar
            QListWidgetItem* item1 = listaConcatenar1->currentItem();
            QListWidgetItem* item2 = listaConcatenar2->currentItem();
            
            if (!item1 || !item2 || !afnManager) {
                QMessageBox::warning(this, "Error", "Seleccione dos AFNs de las listas");
                delete nuevoAFN;
                return;
            }
            
            int id1 = item1->data(Qt::UserRole).toInt();
            int id2 = item2->data(Qt::UserRole).toInt();
            
            AFN* afn1 = afnManager->obtenerAFN(id1);
            AFN* afn2 = afnManager->obtenerAFN(id2);
            
            if (!afn1 || !afn2) {
                QMessageBox::warning(this, "Error", "AFNs seleccionados no encontrados");
                delete nuevoAFN;
                return;
            }
            
            // Copiar el primer AFN y concatenar con el segundo
            *nuevoAFN = *afn1;
            nuevoAFN->concatenarAFN(afn2);  // Método correcto: concatenarAFN (no Concatenar)
            break;
        }
        
        case 4: { // Kleene
            QListWidgetItem* item = listaKleene->currentItem();
            if (!item || !afnManager) {
                QMessageBox::warning(this, "Error", "Seleccione un AFN de la lista");
                delete nuevoAFN;
                return;
            }
            
            int idAFN = item->data(Qt::UserRole).toInt();
            AFN* afnBase = afnManager->obtenerAFN(idAFN);
            
            if (!afnBase) {
                QMessageBox::warning(this, "Error", "AFN seleccionado no encontrado");
                delete nuevoAFN;
                return;
            }
            *nuevoAFN = *afnBase;
            nuevoAFN->CerraduraKleene();
            break;
        }
        
        case 5: { // Positiva
            QListWidgetItem* item = listaPositiva->currentItem();
            if (!item || !afnManager) {
                QMessageBox::warning(this, "Error", "Seleccione un AFN de la lista");
                delete nuevoAFN;
                return;
            }
            
            int idAFN = item->data(Qt::UserRole).toInt();
            AFN* afnBase = afnManager->obtenerAFN(idAFN);
            
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
    
    // Limpiar todas las listas
    listaUnir1->clear();
    listaUnir2->clear();
    listaConcatenar1->clear();
    listaConcatenar2->clear();
    listaKleene->clear();
    listaPositiva->clear();
    
    // Obtener AFNs existentes y añadirlos a todas las listas
    auto afns = afnManager->listarAFNs();
    for (const auto& info : afns) {
        QString texto = QString("ID %1: %2").arg(info.id).arg(QString::fromStdString(info.nombre));
        
        // Crear items para cada lista
        QListWidgetItem* itemUnir1 = new QListWidgetItem(texto);
        itemUnir1->setData(Qt::UserRole, info.id);
        listaUnir1->addItem(itemUnir1);
        
        QListWidgetItem* itemUnir2 = new QListWidgetItem(texto);
        itemUnir2->setData(Qt::UserRole, info.id);
        listaUnir2->addItem(itemUnir2);
        
        QListWidgetItem* itemConcat1 = new QListWidgetItem(texto);
        itemConcat1->setData(Qt::UserRole, info.id);
        listaConcatenar1->addItem(itemConcat1);
        
        QListWidgetItem* itemConcat2 = new QListWidgetItem(texto);
        itemConcat2->setData(Qt::UserRole, info.id);
        listaConcatenar2->addItem(itemConcat2);
        
        QListWidgetItem* itemKleene = new QListWidgetItem(texto);
        itemKleene->setData(Qt::UserRole, info.id);
        listaKleene->addItem(itemKleene);
        
        QListWidgetItem* itemPositiva = new QListWidgetItem(texto);
        itemPositiva->setData(Qt::UserRole, info.id);
        listaPositiva->addItem(itemPositiva);
    }
}

QString CrearAFNDialog::getNombre() const {
    return txtNombre->text();
}

QString CrearAFNDialog::getDescripcion() const {
    return txtDescripcion->toPlainText();
}

AFN* CrearAFNDialog::obtenerAFNSeleccionado(QListWidget* lista) {
    QListWidgetItem* item = lista->currentItem();
    if (!item || !afnManager) return nullptr;
    
    int id = item->data(Qt::UserRole).toInt();
    return afnManager->obtenerAFN(id);
}