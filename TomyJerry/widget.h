#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QSet>
#include "juego.h"
#include "sonido/sonidos.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class PantallaPortada;

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
    void resizeEvent(QResizeEvent *event) override;

private:
    void avanzarTiempo();
    void actualizarInterfaz();
    void iniciarIntento(bool nuevaPartida);
    void cambiarPausa();
    void continuarAlCabildo();
    void mostrarSeleccionNivel();
    void limpiarTeclas();
    void cambiarDificultad(Dificultad valor);
    void cambiarSonido(bool activo);
    void mostrarError(const std::exception &error);
    std::unique_ptr<Ui::Widget> ui;
    Juego juego;
    Sonidos sonidos;
    QString errorAudio;
    QTimer temporizador;
    QElapsedTimer reloj;
    QSet<int> teclas;
    double acumulador = 0;
    QWidget *paginaInvitacion = nullptr;
    PantallaPortada *portada = nullptr;
};

#endif