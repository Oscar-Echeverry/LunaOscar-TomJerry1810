#include "pantallainicio.h"
#include "ui_inicio.h"
#include "logica/dificultad.h"

#include <QPushButton>
#include <QSignalBlocker>

PantallaInicio::PantallaInicio(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::paginaInicio)
{
    ui->setupUi(this);
    connect(ui->selectorNivel, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int indice) {
        const QStringList descripciones = {
            "Reúne seis quesos y la llave para entrar a la tienda de Llorente.",
            "Entra a la tienda, atrapa el florero y entrégalo en el mostrador.",
            "Abre el paso empujando cajas y lleva el mensaje de los criollos al cabildo."
        };
        ui->descripcionInicio->setText(descripciones.value(indice));
        actualizarControles();
    });
    actualizarControles();
    connect(ui->activarSonido, &QCheckBox::toggled, this, &PantallaInicio::sonidoCambiado);

    connect(ui->botonJugar,
            &QPushButton::clicked,
            this,
            &PantallaInicio::jugar);
}

int PantallaInicio::obtenerNivelSeleccionado() const
{
    return ui->selectorNivel->currentIndex() + 1;
}

PantallaInicio::~PantallaInicio()
{
    delete ui;
}

void PantallaInicio::actualizarControles()
{
    ui->controlesInicio->setText(obtenerNivelSeleccionado() == 3
        ? "Flechas / WASD Mover · E Empujar / Entregar · Esc Pausa"
        : obtenerNivelSeleccionado() == 2
            ? "← → Mover · Espacio Atrapar · E Entregar · Mayús Correr +40 % · Esc Pausa"
            : QString("← → Caminar · Espacio Saltar · Mayús Correr +40 % · Esc Pausa\n"
                      "Tom te da %1 segundos de ventaja al comenzar.")
                          .arg(configuracionTom(Dificultad::Normal).ventajaInicial, 0, 'f', 0));
}

void PantallaInicio::establecerSonido(bool activo)
{
    const QSignalBlocker bloqueo(ui->activarSonido);
    ui->activarSonido->setChecked(activo);
}
