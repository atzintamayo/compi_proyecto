#include "GramaticasWidget.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QHeaderView>

GramaticasWidget::GramaticasWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    btnLoad = new QPushButton("Cargar archivo .txt");
    table = new QTableWidget();
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"Token", "Regex", "FirstSymbols", "Followpos"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(btnLoad);
    layout->addWidget(table);

    connect(btnLoad, &QPushButton::clicked, this, &GramaticasWidget::onLoadFile);
}

void GramaticasWidget::onLoadFile() {
    QString file = QFileDialog::getOpenFileName(this, "Seleccionar archivo", "", "Texto (*.txt)");
    if (file.isEmpty()) return;

    try {
        auto data = analyzer.analyzeFile(file.toStdString());
        fillTable(data);
    } catch (std::exception &ex) {
        table->setRowCount(0);
    }
}

void GramaticasWidget::fillTable(const std::vector<RegexResult> &data) {
    table->setRowCount(0);

    for (int r = 0; r < (int)data.size(); r++) {
        table->insertRow(r);

        table->setItem(r, 0, new QTableWidgetItem(QString::fromStdString(data[r].tokenName)));
        table->setItem(r, 1, new QTableWidgetItem(QString::fromStdString(data[r].regex)));

        // First Symbols
        QString first;
        for (char c : data[r].firstSymbols)
            first += c, first += ' ';
        table->setItem(r, 2, new QTableWidgetItem(first));

        // Followpos
        QString fp;
        for (auto &p : data[r].followpos) {
            fp += QString::number(p.first) + " -> {";
            for (int i : p.second)
                fp += QString::number(i) + ",";
            fp += "}  ";
        }
        table->setItem(r, 3, new QTableWidgetItem(fp));
    }
}

