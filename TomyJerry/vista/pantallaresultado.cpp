#include "pantallaresultado.h"
#include "ui_resultado.h"

PantallaResultado::PantallaResultado(QWidget *parent)
    : QFrame(parent), ui(new Ui::PantallaResultado)
{
    ui->setupUi(this);
    connect(ui->botonRepetir, &QPushButton::clicked, this, &PantallaResultado::repetir);
    connect(ui->botonNuevaPartidaResultado, &QPushButton::clicked, this, &PantallaResultado::nuevaPartida);
    connect(ui->botonSalirResultado, &QPushButton::clicked, this, &PantallaResultado::salir);
    ui->creditosResultado->hide();
    ui->botonSalirResultado->hide();
}

PantallaResultado::~PantallaResultado() { delete ui; }

void PantallaResultado::mostrarResultado(bool victoria, int quesos, int puntos, int nivel)
{
    const bool juegoFinalizado = victoria && nivel == 3;
    ui->tituloResultado->setText(victoria
                                     ? (juegoFinalizado ? QStringLiteral("¡JUEGO FINALIZADO!") : QString("¡NIVEL %1 COMPLETADO!").arg(nivel))
                                     : (nivel == 2 ? QStringLiteral("¡SE ACABÓ EL TIEMPO O LAS VIDAS!") : QStringLiteral("¡Tom te ha alcanzado!")));
    if (nivel == 3) {
        ui->detalleResultado->setText(victoria
                                          ? "¡Muchas gracias por jugar!\nCompletaste la aventura de Tom y Jerry: Caos en 1810."
                                          : "Te quedaste sin vidas antes de entregar el mensaje.\nEsquiva los puestos y usa las cajas para frenar a Tom.");
    } else if (nivel == 2) {
        ui->detalleResultado->setText(victoria
                                          ? "Jerry entregó el florero de Llorente.\n¡El cabildo lo invita a continuar!"
                                          : "No lograste entregar el florero a tiempo.\nEsquiva a Tom y calcula cada caída.");
    } else {
        ui->detalleResultado->setText(victoria
                                          ? "Jerry reunió los seis quesos, encontró la llave\ny llegó a la tienda de Llorente."
                                          : QString("Te quedaste sin vidas. Reuniste %1 de los 6 quesos.\nSalta sobre los barriles y usa la carrera para escapar.").arg(quesos));
    }
    ui->puntosResultado->setText(QString("PUNTOS: %1").arg(puntos));
    ui->creditosResultado->setVisible(juegoFinalizado);
    ui->botonSalirResultado->setVisible(juegoFinalizado);
    ui->botonRepetir->setVisible(!juegoFinalizado);
    ui->botonNuevaPartidaResultado->setText(juegoFinalizado ? "Jugar de nuevo · N" : "Nueva partida · N");
    ui->botonRepetir->setText(victoria ? "Volver a jugar · R" : "Reintentar · R");
}
