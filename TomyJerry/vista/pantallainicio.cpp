#include "pantallainicio.h"
#include "ui_inicio.h"

#include <QPushButton>
#include <QSignalBlocker>

PantallaInicio::PantallaInicio(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::paginaInicio)
{
    ui->setupUi(this);
    establecerDificultad(Dificultad::Normal);
    connect(ui->selectorDificultad, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int indice) { emit dificultadCambiada(Dificultad(indice)); });
    connect(ui->activarSonido, &QCheckBox::toggled, this, &PantallaInicio::sonidoCambiado);

    connect(ui->botonJugar,
            &QPushButton::clicked,
            this,
            &PantallaInicio::jugar);
}

PantallaInicio::~PantallaInicio()
{
    delete ui;
}

void PantallaInicio::establecerDificultad(Dificultad valor)
{
    const QSignalBlocker bloqueo(ui->selectorDificultad);
    ui->selectorDificultad->setCurrentIndex(int(valor));
    ui->selectorDificultad->setToolTip(valor == Dificultad::Facil
                                           ? "Tom es lento, ve menos y abandona pronto la búsqueda. Peligros más suaves."
                                           : valor == Dificultad::Normal
                                                 ? "Tom persigue a ritmo normal; usa los obstáculos y la carrera para escapar."
                                                 : "Tom corre más, ve más lejos e intenta interceptarte. Peligros más rápidos.");
    ui->controlesInicio->setText(QString(
                                     "← → Caminar · Espacio Saltar · Mayús Correr · Esc Pausa\n"
                                     "Cada queso mejora el salto durante 5 segundos. Tres vidas.\n"
                                     "Tom te da %1 segundos de ventaja al comenzar.")
                                     .arg(configuracionTom(valor).ventajaInicial, 0, 'f', 0));
}

void PantallaInicio::establecerSonido(bool activo)
{
    const QSignalBlocker bloqueo(ui->activarSonido);
    ui->activarSonido->setChecked(activo);
}
