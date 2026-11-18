#include "pantallaresultado.h"
#include "ui_resultado.h"

PantallaResultado::PantallaResultado(QWidget *parent)
    : QFrame(parent), ui(new Ui::PantallaResultado)
{
    ui->setupUi(this);
    connect(ui->botonRepetir, &QPushButton::clicked, this, &PantallaResultado::repetir);
    connect(ui->botonNuevaPartidaResultado, &QPushButton::clicked, this, &PantallaResultado::nuevaPartida);
}

PantallaResultado::~PantallaResultado() { delete ui; }

void PantallaResultado::mostrarResultado(bool victoria, int quesos, int puntos)
{
    ui->tituloResultado->setText(victoria ? "¡NIVEL 1 COMPLETADO!" : "¡Tom te ha alcanzado!");
    ui->detalleResultado->setText(victoria
                                      ? "Jerry reunió los seis quesos, encontró la llave\ny llegó a la tienda de Llorente."
                                      : QString("Te quedaste sin vidas. Reuniste %1 de los 6 quesos.\nSalta sobre los barriles y usa la carrera para escapar.").arg(quesos));
    ui->puntosResultado->setText(QString("PUNTOS: %1").arg(puntos));
    ui->botonRepetir->setText(victoria ? "Volver a jugar · R" : "Reintentar · R");
}