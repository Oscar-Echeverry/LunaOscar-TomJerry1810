#ifndef PANTALLAINICIO_H
#define PANTALLAINICIO_H

#include <QFrame>
#include "logica/dificultad.h"

namespace Ui {
class paginaInicio;
}

class PantallaInicio : public QFrame
{
    Q_OBJECT

public:
    explicit PantallaInicio(QWidget *parent = nullptr);
    ~PantallaInicio() override;
    void establecerDificultad(Dificultad valor);
    void establecerSonido(bool activo);

signals:
    void jugar();
    void dificultadCambiada(Dificultad valor);
    void sonidoCambiado(bool activo);

private:
    Ui::paginaInicio *ui;
};

#endif