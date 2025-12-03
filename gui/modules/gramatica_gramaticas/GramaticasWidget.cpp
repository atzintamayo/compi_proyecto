#include "GramaticasWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>
#include <QFont>
#include <QLabel>

GramaticasWidget::GramaticasWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Título
    QLabel *titulo = new QLabel("Analizador de Gramáticas - Expresiones Regulares");
    QFont fuenteTitulo;
    fuenteTitulo.setPointSize(16);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);
    titulo->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titulo);
    
    // Grupo de carga de archivo
    QGroupBox *grupoArchivo = new QGroupBox("Cargar Gramática");
    QVBoxLayout *archivoLayout = new QVBoxLayout();
    
    QLabel *instruccion = new QLabel(
        "Formato del archivo: <b>TOKEN: expresión_regular</b><br>"
        "Ejemplo:<br>"
        "• ID: [a-z]+<br>"
        "• NUM: [0-9]+<br>"
        "• OP: \\+|\\-|\\*|\\/"
    );
    instruccion->setWordWrap(true);
    instruccion->setStyleSheet("QLabel { padding: 10px; background-color: #f0f0f0; border-radius: 5px; }");
    archivoLayout->addWidget(instruccion);
    
    btnLoad = new QPushButton("📁 Cargar archivo .txt");
    btnLoad->setMinimumHeight(35);
    archivoLayout->addWidget(btnLoad);
    
    grupoArchivo->setLayout(archivoLayout);
    mainLayout->addWidget(grupoArchivo);
    
    // Grupo de resultados
    QGroupBox *grupoResultados = new QGroupBox("Resultados del Análisis");
    QVBoxLayout *resultadosLayout = new QVBoxLayout();
    
    table = new QTableWidget();
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Token", "Regex", "First Symbols", "Followpos"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    resultadosLayout->addWidget(table);
    grupoResultados->setLayout(resultadosLayout);
    mainLayout->addWidget(grupoResultados);
    
    // Conectar señal
    connect(btnLoad, &QPushButton::clicked, this, &GramaticasWidget::onLoadFile);
}

void GramaticasWidget::onLoadFile() {
    QString file = QFileDialog::getOpenFileName(
        this, 
        "Seleccionar archivo de gramática", 
        "", 
        "Archivos de texto (*.txt);;Todos los archivos (*)"
    );
    
    if (file.isEmpty()) return;

    try {
        auto data = analyzer.analyzeFile(file.toStdString());
        fillTable(data);
        
        QMessageBox::information(
            this, 
            "Éxito", 
            QString("Se analizaron %1 expresiones correctamente.").arg(data.size())
        );
    } catch (const std::exception &ex) {
        table->setRowCount(0);
        QMessageBox::critical(
            this, 
            "Error", 
            QString("Error al procesar el archivo:\n%1").arg(ex.what())
        );
    }
}

void GramaticasWidget::fillTable(const std::vector<RegexResult> &data) {
    table->setRowCount(0);

    for (int r = 0; r < (int)data.size(); r++) {
        table->insertRow(r);

        // Token
        table->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(data[r].tokenName)));

        // Regex
        table->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(data[r].regex)));

        // First Symbols
        QString first;
        for (char c : data[r].firstSymbols) {
            if (c == ' ') first += "'espacio' ";
            else if (c == '\n') first += "'\\n' ";
            else if (c == '\t') first += "'\\t' ";
            else first += QString("'%1' ").arg(c);
        }
        table->setItem(r, 2, new QTableWidgetItem(first.trimmed()));

        // Followpos
        QString fp;
        for (auto &p : data[r].followpos) {
            fp += QString("%1 → {").arg(p.first);
            bool first_pos = true;
            for (int i : p.second) {
                if (!first_pos) fp += ", ";
                fp += QString::number(i);
                first_pos = false;
            }
            fp += "}\n";
        }
        table->setItem(r, 3, new QTableWidgetItem(fp.trimmed()));
    }
    
    // Ajustar altura de filas
    for (int i = 0; i < table->rowCount(); i++) {
        table->resizeRowToContents(i);
    }
}
