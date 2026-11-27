#ifndef PANTALLAPAUSA_H
#define PANTALLAPAUSA_H
#include <QFrame>
#include "logica/dificultad.h"

namespace Ui { class PantallaPausa; }

class PantallaPausa : public QFrame
{
    Q_OBJECT
public:
    explicit PantallaPausa(QWidget *parent = nullptr);
    ~PantallaPausa() override;
    void establecerNivel(int nivel, bool elegirDificultad = false);
    void establecerDificultad(Dificultad valor);
    void establecerSonido(bool activo);

signals:
    void continuar();
    void reiniciar();
    void nuevaPartida();
    void dificultadCambiada(Dificultad valor);
    void sonidoCambiado(bool activo);

private:
    Ui::PantallaPausa *ui;
};
#endif