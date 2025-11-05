#include "AsignarTokenDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

AsignarTokenDialog::AsignarTokenDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Asignar Token ID");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    layout->addWidget(new QLabel("Ingrese el ID del token:"));
    spinTokenId = new QSpinBox();
    spinTokenId->setRange(0, 9999);
    spinTokenId->setValue(1);
    layout->addWidget(spinTokenId);
    
    QHBoxLayout* layoutBotones = new QHBoxLayout();
    btnAceptar = new QPushButton("Aceptar");
    btnCancelar = new QPushButton("Cancelar");
    connect(btnAceptar, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancelar, &QPushButton::clicked, this, &QDialog::reject);
    layoutBotones->addStretch();
    layoutBotones->addWidget(btnAceptar);
    layoutBotones->addWidget(btnCancelar);
    layout->addLayout(layoutBotones);
    
    resize(300, 150);
}

int AsignarTokenDialog::getTokenId() const {
    return spinTokenId->value();
}