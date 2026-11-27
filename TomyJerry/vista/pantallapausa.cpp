#include "pantallapausa.h"
#include "ui_pausa.h"
#include "logica/niveles.h"
#include <QSignalBlocker>

PantallaPausa::PantallaPausa(QWidget *parent)
    : QFrame(parent), ui(new Ui::PantallaPausa)
{
    ui->setupUi(this);
    connect(ui->selectorDificultad, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int indice) {
                emit dificultadCambiada(indice == 1 ? Dificultad::Dificil : Dificultad::Normal);
            });
    connect(ui->activarSonido, &QCheckBox::toggled, this, &PantallaPausa::sonidoCambiado);
    connect(ui->botonContinuar, &QPushButton::clicked, this, &PantallaPausa::continuar);
    connect(ui->botonReiniciarPausa, &QPushButton::clicked, this, &PantallaPausa::reiniciar);
    connect(ui->botonNuevaPartidaPausa, &QPushButton::clicked, this, &PantallaPausa::nuevaPartida);
}

PantallaPausa::~PantallaPausa() { delete ui; }

void PantallaPausa::establecerNivel(int nivel, bool elegirDificultad)
{
    ui->etiquetaDificultad->setVisible(nivel == 3);
    ui->selectorDificultad->setVisible(nivel == 3);
    ui->botonReiniciarPausa->setVisible(!elegirDificultad);
    ui->botonNuevaPartidaPausa->setVisible(!elegirDificultad);
    ui->botonContinuar->setText(elegirDificultad ? "Comenzar nivel 3 · Enter" : "Continuar · Esc");
    const bool tienda = nivel == 2;
    ui->tituloPausa->setText(elegirDificultad ? "Nivel 3 · Elige la dificultad"
                                           : tienda ? "Una pausa en la tienda" : "Una pausa en la plaza");
    ui->descripcionPausa->setText(tienda
                                      ? QString("Atrapa el florero con Espacio, llévalo al mostrador y pulsa E.\n"
                                                "La entrega suma %1 puntos al marcador acumulado.\n"
                                                "Esquiva a Tom y los objetos que caen. Tienes 60 segundos.")
                                            .arg(NivelTienda::puntosEntregaFlorero)
                                      : nivel == 3
                                            ? "Empuja las cajas y entrega el mensaje al cabildo. Rojo: vigilado. Verde: rodeo.\nTom recuerda seis zonas; los muros bloquean su visión.\nNormal: decide cada 0,5 s. Difícil: cada 0,25 s e intenta interceptarte.\nFlechas / WASD: mover. E: empujar / entregar. Sin límite de tiempo."
                                            : "El escenario y las habilidades están detenidos.\nReúne 6 quesos y la llave; después pulsa E junto a la tienda.\nMayús: carrera +40 %. Cada pulsación renueva el impulso de 2 s, sin recarga.");
}

void PantallaPausa::establecerDificultad(Dificultad valor)
{
    const QSignalBlocker bloqueo(ui->selectorDificultad);
    ui->selectorDificultad->setCurrentIndex(valor == Dificultad::Dificil ? 1 : 0);
    ui->selectorDificultad->setToolTip(valor == Dificultad::Dificil
        ? "Tom decide cada 0,25 s e intenta interceptarte."
        : "Tom decide cada 0,5 s y persigue a ritmo normal.");
}

void PantallaPausa::establecerSonido(bool activo)
{
    const QSignalBlocker bloqueo(ui->activarSonido);
    ui->activarSonido->setChecked(activo);
}
