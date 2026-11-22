#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QSet>
#include "juego.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    void avanzarTiempo();
    void actualizarInterfaz();
    void iniciarIntento(bool nuevaPartida);
    void cambiarPausa();
    void limpiarTeclas();
    Ui::Widget *ui;
    Juego juego;
    QTimer temporizador;
    QElapsedTimer reloj;
    QSet<int> teclas;
    double acumulador = 0;
};

#endif