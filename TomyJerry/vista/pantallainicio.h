#ifndef PANTALLAINICIO_H
#define PANTALLAINICIO_H

#include <QFrame>

namespace Ui {
class paginaInicio;
}

class PantallaInicio : public QFrame
{
    Q_OBJECT

public:
    explicit PantallaInicio(QWidget *parent = nullptr);
    ~PantallaInicio() override;
    void establecerSonido(bool activo);
    int obtenerNivelSeleccionado() const;

signals:
    void jugar();
    void sonidoCambiado(bool activo);

private:
    void actualizarControles();
    Ui::paginaInicio *ui;
};

#endif