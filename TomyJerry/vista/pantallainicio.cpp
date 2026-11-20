#include "pantallainicio.h"
#include "ui_inicio.h"

#include <QPushButton>

PantallaInicio::PantallaInicio(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::paginaInicio)
{
    ui->setupUi(this);

    connect(ui->botonJugar,
            &QPushButton::clicked,
            this,
            &PantallaInicio::jugar);
}

PantallaInicio::~PantallaInicio()
{
    delete ui;
}
