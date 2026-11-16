#include "pantallapausa.h"
#include "ui_pausa.h"

PantallaPausa::PantallaPausa(QWidget *parent)
    : QFrame(parent), ui(new Ui::PantallaPausa)
{
    ui->setupUi(this);
    connect(ui->botonContinuar, &QPushButton::clicked, this, &PantallaPausa::continuar);
    connect(ui->botonReiniciarPausa, &QPushButton::clicked, this, &PantallaPausa::reiniciar);
    connect(ui->botonNuevaPartidaPausa, &QPushButton::clicked, this, &PantallaPausa::nuevaPartida);
}

PantallaPausa::~PantallaPausa() { delete ui; }
