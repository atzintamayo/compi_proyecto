#include "AnalizadorLexicoWidget.h"
#include "dialogs/CrearAFNDialog.h"
#include "dialogs/AsignarTokenDialog.h"
#include "widgets/AFNTableWidget.h"
#include "widgets/AFDTableWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>

AnalizadorLexicoWidget::AnalizadorLexicoWidget(QWidget *parent) : QWidget(parent) {
    afnManager = new AFNManager();
    afdActual = nullptr;
    scanner = nullptr;
    
    setupUI();
}

AnalizadorLexicoWidget::~AnalizadorLexicoWidget() {
    delete afnManager;
    if (afdActual) delete afdActual;
    if (scanner) delete scanner;
}

void AnalizadorLexicoWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    tabWidget = new QTabWidget(this);
    mainLayout->addWidget(tabWidget);
    
    setupTabAFN();
    setupTabAFD();
    setupTabScanner();
}

void AnalizadorLexicoWidget::setupTabAFN() {
    tabAFN = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tabAFN);
    
    // Grupo: Lista de AFNs
    QGroupBox* grupoLista = new QGroupBox("AFNs Creados");
    QVBoxLayout* layoutLista = new QVBoxLayout();
    
    listaAFNs = new QListWidget();
    listaAFNs->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(listaAFNs, &QListWidget::itemSelectionChanged, this, &AnalizadorLexicoWidget::onAFNSeleccionado);
    layoutLista->addWidget(listaAFNs);
    
    // Botones de operaciones
    QHBoxLayout* layoutBotones = new QHBoxLayout();
    btnCrearAFN = new QPushButton("Crear AFN");
    btnEliminarAFN = new QPushButton("Eliminar");
    btnMarcarAceptacion = new QPushButton("Marcar Aceptación");
    btnAsignarToken = new QPushButton("Asignar Token");
    btnVerAFN = new QPushButton("Ver Detalles");
    
    btnEliminarAFN->setEnabled(false);
    btnMarcarAceptacion->setEnabled(false);
    btnAsignarToken->setEnabled(false);
    btnVerAFN->setEnabled(false);
    
    connect(btnCrearAFN, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onCrearAFN);
    connect(btnEliminarAFN, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onEliminarAFN);
    connect(btnMarcarAceptacion, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onMarcarAceptacion);
    connect(btnAsignarToken, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onAsignarToken);
    connect(btnVerAFN, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onVerAFN);
    
    layoutBotones->addWidget(btnCrearAFN);
    layoutBotones->addWidget(btnEliminarAFN);
    layoutBotones->addWidget(btnMarcarAceptacion);
    layoutBotones->addWidget(btnAsignarToken);
    layoutBotones->addWidget(btnVerAFN);
    layoutLista->addLayout(layoutBotones);
    
    grupoLista->setLayout(layoutLista);
    layout->addWidget(grupoLista);
    
    // Tabla para visualizar AFN
    QGroupBox* grupoTabla = new QGroupBox("Visualización del AFN");
    QVBoxLayout* layoutTabla = new QVBoxLayout();
    tablaAFN = new AFNTableWidget();
    layoutTabla->addWidget(tablaAFN);
    grupoTabla->setLayout(layoutTabla);
    layout->addWidget(grupoTabla);
    
    tabWidget->addTab(tabAFN, "1. Gestión de AFNs");
}

void AnalizadorLexicoWidget::setupTabAFD() {
    tabAFD = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tabAFD);
    
    // Grupo: Selección de AFNs
    QGroupBox* grupoSeleccion = new QGroupBox("Seleccionar AFNs para combinar");
    QVBoxLayout* layoutSel = new QVBoxLayout();
    
    listaAFNsParaAFD = new QListWidget();
    listaAFNsParaAFD->setSelectionMode(QAbstractItemView::MultiSelection);
    layoutSel->addWidget(listaAFNsParaAFD);
    
    QHBoxLayout* layoutBotonesAFD = new QHBoxLayout();
    btnConvertirAFD = new QPushButton("Convertir a AFD");
    btnGuardarAFD = new QPushButton("Guardar AFD");
    btnCargarAFD = new QPushButton("Cargar AFD");
    
    btnGuardarAFD->setEnabled(false);
    
    connect(btnConvertirAFD, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onConvertirAFD);
    connect(btnGuardarAFD, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onGuardarAFD);
    connect(btnCargarAFD, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onCargarAFD);
    
    layoutBotonesAFD->addWidget(btnConvertirAFD);
    layoutBotonesAFD->addWidget(btnGuardarAFD);
    layoutBotonesAFD->addWidget(btnCargarAFD);
    layoutSel->addLayout(layoutBotonesAFD);
    
    grupoSeleccion->setLayout(layoutSel);
    layout->addWidget(grupoSeleccion);
    
    // Info del AFD
    labelInfoAFD = new QLabel("No hay AFD generado");
    layout->addWidget(labelInfoAFD);
    
    // Tabla AFD
    QGroupBox* grupoTablaAFD = new QGroupBox("Tabla de Transiciones del AFD");
    QVBoxLayout* layoutTablaAFD = new QVBoxLayout();
    tablaAFD = new AFDTableWidget();
    layoutTablaAFD->addWidget(tablaAFD);
    grupoTablaAFD->setLayout(layoutTablaAFD);
    layout->addWidget(grupoTablaAFD);
    
    tabWidget->addTab(tabAFD, "2. Conversión AFN→AFD");
}

void AnalizadorLexicoWidget::setupTabScanner() {
    tabScanner = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(tabScanner);
    
    // Entrada
    QGroupBox* grupoEntrada = new QGroupBox("Texto de Entrada");
    QVBoxLayout* layoutEntrada = new QVBoxLayout();
    
    textoEntrada = new QTextEdit();
    textoEntrada->setPlaceholderText("Ingrese el texto a analizar...");
    layoutEntrada->addWidget(textoEntrada);
    
    QHBoxLayout* layoutBotonesScanner = new QHBoxLayout();
    btnCargarArchivo = new QPushButton("Cargar desde Archivo");
    btnEscanear = new QPushButton("Escanear");
    btnLimpiarScanner = new QPushButton("Limpiar");
    chkOmitirEspacios = new QCheckBox("Omitir espacios (token ESP)");
    chkOmitirEspacios->setChecked(true);
    
    btnEscanear->setEnabled(false);
    
    connect(btnCargarArchivo, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onCargarArchivo);
    connect(btnEscanear, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onEscanear);
    connect(btnLimpiarScanner, &QPushButton::clicked, this, &AnalizadorLexicoWidget::onLimpiarScanner);
    
    layoutBotonesScanner->addWidget(btnCargarArchivo);
    layoutBotonesScanner->addWidget(btnEscanear);
    layoutBotonesScanner->addWidget(btnLimpiarScanner);
    layoutBotonesScanner->addWidget(chkOmitirEspacios);
    layoutBotonesScanner->addStretch();
    layoutEntrada->addLayout(layoutBotonesScanner);
    
    grupoEntrada->setLayout(layoutEntrada);
    layout->addWidget(grupoEntrada);
    
    // Tokens
    QGroupBox* grupoTokens = new QGroupBox("Tokens Reconocidos");
    QVBoxLayout* layoutTokens = new QVBoxLayout();
    
    tablaTokens = new QTableWidget();
    tablaTokens->setColumnCount(4);
    tablaTokens->setHorizontalHeaderLabels({"Token ID", "Lexema", "Inicio", "Fin"});
    tablaTokens->horizontalHeader()->setStretchLastSection(true);
    tablaTokens->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layoutTokens->addWidget(tablaTokens);
    
    grupoTokens->setLayout(layoutTokens);
    layout->addWidget(grupoTokens);
    
    // Errores
    QGroupBox* grupoErrores = new QGroupBox("Errores Léxicos");
    QVBoxLayout* layoutErrores = new QVBoxLayout();
    
    textoErrores = new QTextEdit();
    textoErrores->setReadOnly(true);
    textoErrores->setMaximumHeight(150);
    layoutErrores->addWidget(textoErrores);
    
    grupoErrores->setLayout(layoutErrores);
    layout->addWidget(grupoErrores);
    
    tabWidget->addTab(tabScanner, "3. Scanner / Análisis Léxico");
}

// ==================== SLOTS AFN ====================

void AnalizadorLexicoWidget::onCrearAFN() {
    CrearAFNDialog dialog(this, afnManager);
    if (dialog.exec() == QDialog::Accepted) {
        AFN* nuevoAFN = dialog.getAFNCreado();
        QString nombre = dialog.getNombre();
        QString descripcion = dialog.getDescripcion();
        
        if (nuevoAFN) {
            int id = afnManager->agregarAFN(nuevoAFN, nombre.toStdString(), descripcion.toStdString());
            actualizarListaAFNs();
            actualizarListaAFNsParaAFD();
            
            QMessageBox::information(this, "Éxito", 
                QString("AFN '%1' creado con ID: %2").arg(nombre).arg(id));
        }
    }
}

void AnalizadorLexicoWidget::onEliminarAFN() {
    QListWidgetItem* item = listaAFNs->currentItem();
    if (!item) return;
    
    int id = item->data(Qt::UserRole).toInt();
    
    auto respuesta = QMessageBox::question(this, "Confirmar", 
        "¿Está seguro de eliminar este AFN?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (respuesta == QMessageBox::Yes) {
        if (afnManager->eliminarAFN(id)) {
            actualizarListaAFNs();
            actualizarListaAFNsParaAFD();
            tablaAFN->limpiar();
            QMessageBox::information(this, "Éxito", "AFN eliminado correctamente");
        }
    }
}

void AnalizadorLexicoWidget::onMarcarAceptacion() {
    QListWidgetItem* item = listaAFNs->currentItem();
    if (!item) return;
    
    int id = item->data(Qt::UserRole).toInt();
    AFN* afn = afnManager->obtenerAFN(id);
    
    if (!afn) return;
    
    QMessageBox::information(this, "Info", 
        QString("El AFN ya tiene %1 estado(s) de aceptación marcados")
        .arg(afn->edosAcept.size()));
}

void AnalizadorLexicoWidget::onAsignarToken() {
    QListWidgetItem* item = listaAFNs->currentItem();
    if (!item) return;
    
    int id = item->data(Qt::UserRole).toInt();
    AFN* afn = afnManager->obtenerAFN(id);
    
    if (!afn) return;
    
    AsignarTokenDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        int tokenId = dialog.getTokenId();
        
        // Asignar token a todos los estados de aceptación
        for (Estado* e : afn->edosAcept) {
            e->token = tokenId;
        }
        
        QMessageBox::information(this, "Éxito", 
            QString("Token %1 asignado a los estados de aceptación").arg(tokenId));
        
        mostrarAFNEnTabla(id);
    }
}

void AnalizadorLexicoWidget::onAFNSeleccionado() {
    bool haySeleccion = listaAFNs->currentItem() != nullptr;
    btnEliminarAFN->setEnabled(haySeleccion);
    btnMarcarAceptacion->setEnabled(haySeleccion);
    btnAsignarToken->setEnabled(haySeleccion);
    btnVerAFN->setEnabled(haySeleccion);
}

void AnalizadorLexicoWidget::onVerAFN() {
    QListWidgetItem* item = listaAFNs->currentItem();
    if (!item) return;
    
    int id = item->data(Qt::UserRole).toInt();
    mostrarAFNEnTabla(id);
}

// ==================== SLOTS AFD ====================

void AnalizadorLexicoWidget::onConvertirAFD() {
    QList<QListWidgetItem*> seleccionados = listaAFNsParaAFD->selectedItems();
    
    if (seleccionados.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", 
            "Seleccione al menos un AFN para convertir");
        return;
    }
    
    std::vector<int> ids;
    for (auto item : seleccionados) {
        ids.push_back(item->data(Qt::UserRole).toInt());
    }
    
    // Limpiar AFD anterior
    if (afdActual) {
        delete afdActual;
        afdActual = nullptr;
    }
    
    // Crear nuevo AFD
    afdActual = afnManager->crearAFDDesdeSeleccion(ids);
    
    if (afdActual) {
        btnGuardarAFD->setEnabled(true);
        btnEscanear->setEnabled(true);
        
        labelInfoAFD->setText(QString("AFD generado: %1 estados, alfabeto de %2 símbolos")
            .arg(afdActual->NumEdos)
            .arg(afdActual->alfabeto.size()));
        
        mostrarAFDEnTabla();
        
        // Crear scanner con el nuevo AFD
        if (scanner) delete scanner;
        scanner = new Scanner(afdActual);
        scanner->setOmitirEspacios(chkOmitirEspacios->isChecked());
        
        QMessageBox::information(this, "Éxito", "AFD generado correctamente");
    } else {
        QMessageBox::critical(this, "Error", "No se pudo generar el AFD");
    }
}

void AnalizadorLexicoWidget::onGuardarAFD() {
    if (!afdActual) {
        QMessageBox::warning(this, "Advertencia", "No hay AFD para guardar");
        return;
    }
    
    QString archivo = QFileDialog::getSaveFileName(this, 
        "Guardar AFD", "", "Archivos AFD (*.afd);;Todos los archivos (*)");
    
    if (!archivo.isEmpty()) {
        if (afdActual->GuardarAFD(archivo.toStdString())) {
            QMessageBox::information(this, "Éxito", "AFD guardado correctamente");
        } else {
            QMessageBox::critical(this, "Error", "No se pudo guardar el AFD");
        }
    }
}

void AnalizadorLexicoWidget::onCargarAFD() {
    QString archivo = QFileDialog::getOpenFileName(this, 
        "Cargar AFD", "", "Archivos AFD (*.afd);;Todos los archivos (*)");
    
    if (!archivo.isEmpty()) {
        if (afdActual) delete afdActual;
        afdActual = new AFD();
        
        if (afdActual->CargarAFD(archivo.toStdString())) {
            btnGuardarAFD->setEnabled(true);
            btnEscanear->setEnabled(true);
            
            labelInfoAFD->setText(QString("AFD cargado: %1 estados")
                .arg(afdActual->NumEdos));
            
            mostrarAFDEnTabla();
            
            // Crear scanner
            if (scanner) delete scanner;
            scanner = new Scanner(afdActual);
            scanner->setOmitirEspacios(chkOmitirEspacios->isChecked());
            
            QMessageBox::information(this, "Éxito", "AFD cargado correctamente");
        } else {
            QMessageBox::critical(this, "Error", "No se pudo cargar el AFD");
            delete afdActual;
            afdActual = nullptr;
        }
    }
}

// ==================== SLOTS SCANNER ====================

void AnalizadorLexicoWidget::onEscanear() {
    if (!scanner || !afdActual) {
        QMessageBox::warning(this, "Advertencia", 
            "Primero debe generar o cargar un AFD");
        return;
    }
    
    QString texto = textoEntrada->toPlainText();
    if (texto.isEmpty()) {
        QMessageBox::warning(this, "Advertencia", "Ingrese texto para analizar");
        return;
    }
    
    scanner->setTexto(texto.toStdString());
    scanner->setOmitirEspacios(chkOmitirEspacios->isChecked());
    
    bool exito = scanner->escanear();
    
    mostrarTokensEnTabla();
    mostrarErrores();
    
    if (exito) {
        QMessageBox::information(this, "Éxito", 
            QString("Análisis completado. %1 tokens reconocidos.")
            .arg(scanner->getTokens().size()));
    } else {
        QMessageBox::warning(this, "Errores", 
            QString("Se encontraron %1 errores léxicos")
            .arg(scanner->getErrores().size()));
    }
}

void AnalizadorLexicoWidget::onCargarArchivo() {
    QString archivo = QFileDialog::getOpenFileName(this, 
        "Abrir archivo", "", "Archivos de texto (*.txt);;Todos los archivos (*)");
    
    if (!archivo.isEmpty()) {
        QFile file(archivo);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            textoEntrada->setPlainText(in.readAll());
            file.close();
        } else {
            QMessageBox::critical(this, "Error", "No se pudo abrir el archivo");
        }
    }
}

void AnalizadorLexicoWidget::onLimpiarScanner() {
    textoEntrada->clear();
    tablaTokens->setRowCount(0);
    textoErrores->clear();
    if (scanner) scanner->limpiar();
}

// ==================== MÉTODOS AUXILIARES ====================

void AnalizadorLexicoWidget::actualizarListaAFNs() {
    listaAFNs->clear();
    auto afns = afnManager->listarAFNs();
    
    for (const auto& info : afns) {
        QString texto = QString("ID %1: %2").arg(info.id).arg(QString::fromStdString(info.nombre));
        QListWidgetItem* item = new QListWidgetItem(texto);
        item->setData(Qt::UserRole, info.id);
        listaAFNs->addItem(item);
    }
}

void AnalizadorLexicoWidget::actualizarListaAFNsParaAFD() {
    listaAFNsParaAFD->clear();
    auto afns = afnManager->listarAFNs();
    
    for (const auto& info : afns) {
        QString texto = QString("ID %1: %2").arg(info.id).arg(QString::fromStdString(info.nombre));
        QListWidgetItem* item = new QListWidgetItem(texto);
        item->setData(Qt::UserRole, info.id);
        listaAFNsParaAFD->addItem(item);
    }
}

void AnalizadorLexicoWidget::mostrarAFNEnTabla(int idAFN) {
    AFN* afn = afnManager->obtenerAFN(idAFN);
    if (afn) {
        tablaAFN->mostrarAFN(afn);
    }
}

void AnalizadorLexicoWidget::mostrarAFDEnTabla() {
    if (afdActual) {
        tablaAFD->mostrarAFD(afdActual);
    }
}

void AnalizadorLexicoWidget::mostrarTokensEnTabla() {
    if (!scanner) return;
    
    tablaTokens->setRowCount(0);
    const auto& tokens = scanner->getTokens();
    
    for (const auto& tok : tokens) {
        int row = tablaTokens->rowCount();
        tablaTokens->insertRow(row);
        
        tablaTokens->setItem(row, 0, new QTableWidgetItem(QString::number(tok.tokenId)));
        tablaTokens->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(tok.lexema)));
        tablaTokens->setItem(row, 2, new QTableWidgetItem(QString::number(tok.inicio)));
        tablaTokens->setItem(row, 3, new QTableWidgetItem(QString::number(tok.fin)));
    }
}

void AnalizadorLexicoWidget::mostrarErrores() {
    if (!scanner) return;
    
    textoErrores->clear();
    const auto& errores = scanner->getErrores();
    
    if (errores.empty()) {
        textoErrores->setPlainText("No hay errores léxicos.");
        return;
    }
    
    QString texto;
    for (const auto& err : errores) {
        texto += QString("Error en posición %1: carácter inesperado '%2'\n")
            .arg(err.posicion)
            .arg(err.caracter);
        texto += QString("  Contexto: \"%1\"\n\n")
            .arg(QString::fromStdString(err.contexto));
    }
    
    textoErrores->setPlainText(texto);
}