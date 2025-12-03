#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "../../core/gramatica_gramaticas/Gramaticas.h"

class GramaticasWidget : public QWidget {
    Q_OBJECT
public:
    explicit GramaticasWidget(QWidget *parent = nullptr);

private slots:
    void onLoadFile();

private:
    QTableWidget *table;
    QPushButton *btnLoad;
    GrammarAnalyzer analyzer;

    void fillTable(const std::vector<RegexResult> &data);
};
