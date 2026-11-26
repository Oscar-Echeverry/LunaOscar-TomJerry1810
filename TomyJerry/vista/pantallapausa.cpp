#include "pantallapausa.h"
#include "ui_pausa.h"
#include "logica/niveles.h"
#include <QSignalBlocker>

PantallaPausa::PantallaPausa(QWidget *parent)
    : QFrame(parent), ui(new Ui::PantallaPausa)
{
    ui->setupUi(this);
    connect(ui->selectorDificultad, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int indice) { emit dificultadCambiada(Dificultad(indice)); });
    connect(ui->activarSonido, &QCheckBox::toggled, this, &PantallaPausa::sonidoCambiado);
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
                                      ? QString("Atrapa el florero con Espacio, llévalo al mostrador y pulsa E.\n"
                                                "La entrega suma %1 puntos al marcador acumulado.\n"
                                                "Esquiva a Tom y los objetos que caen. Tienes 60 segundos.")
                                            .arg(NivelTienda::puntosEntregaFlorero)
                                      : nivel == 3
                                            ? "Empuja la caja que bloquea el camino en cada plaza; después avanza al cabildo.\nRojo: ruta corta vigilada. Verde: rodeo. La dificultad cambia la persecución de Tom.\nFlechas / WASD: mover. E: empujar / entregar. M: sonido.\nGanas 100 puntos por plaza y 500 al entregar. Sin límite de tiempo."
                                            : "El escenario y las habilidades están detenidos.\nReúne 6 quesos y la llave; después pulsa E junto a la tienda.\nMayús: carrera +60 %. Cada pulsación renueva el impulso de 2 s, sin recarga.");
}

void PantallaPausa::establecerDificultad(Dificultad valor)
{
    const QSignalBlocker bloqueo(ui->selectorDificultad);
    ui->selectorDificultad->setCurrentIndex(int(valor));
    ui->selectorDificultad->setToolTip(valor == Dificultad::Facil
                                           ? "Tom lento, visión corta y peligros suaves."
                                           : valor == Dificultad::Normal ? "Persecución y peligros a ritmo normal."
                                                                         : "Tom rápido, visión larga e intercepción; peligros más rápidos.");
}

void PantallaPausa::establecerSonido(bool activo)
{
    const QSignalBlocker bloqueo(ui->activarSonido);
    ui->activarSonido->setChecked(activo);
}
