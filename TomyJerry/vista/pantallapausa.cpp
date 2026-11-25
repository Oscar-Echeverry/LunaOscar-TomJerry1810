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

void PantallaPausa::establecerNivel(int nivel)
{
    const bool tienda = nivel == 2;
    ui->tituloPausa->setText(tienda ? "Una pausa en la tienda" : "Una pausa en la plaza");
    ui->descripcionPausa->setText(tienda
        ? "El escenario está detenido. Atrapa el florero con Espacio, llévalo al mostrador y pulsa E.\nEsquiva a Tom y los objetos que caen. Tienes 60 segundos."
        : "El escenario y las habilidades están detenidos.\nReúne 6 quesos y la llave; después pulsa E junto a la tienda.\nMantén Mayús para correr: +60 % durante 2 s; recarga en 6 s.");
}
