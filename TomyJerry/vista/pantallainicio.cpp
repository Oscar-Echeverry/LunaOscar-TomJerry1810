#include "pantallainicio.h"
#include "ui_inicio.h"

PantallaInicio::PantallaInicio(QWidget *parent)
    : QFrame(parent), ui(new Ui::PantallaInicio)
{
    ui->setupUi(this);
    connect(ui->botonJugar, &QPushButton::clicked, this, &PantallaInicio::jugar);
}

PantallaInicio::~PantallaInicio() { delete ui; }