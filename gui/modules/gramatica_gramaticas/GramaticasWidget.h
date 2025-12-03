#ifndef GRAMATICASWIDGET_H
#define GRAMATICASWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include "Gramaticas.h"  // Nota: sin la ruta completa porque ya está en includes

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

#endif // GRAMATICASWIDGET_H
