#ifndef ASIGNARTOKENDIALOG_H
#define ASIGNARTOKENDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>

class AsignarTokenDialog : public QDialog {
    Q_OBJECT

public:
    explicit AsignarTokenDialog(QWidget *parent = nullptr);
    int getTokenId() const;

private:
    QSpinBox* spinTokenId;
    QPushButton* btnAceptar;
    QPushButton* btnCancelar;
};

#endif