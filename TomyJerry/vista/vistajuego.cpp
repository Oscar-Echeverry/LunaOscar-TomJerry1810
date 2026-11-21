#include "vistajuego.h"
#include "juego.h"
#include "logica/configuracion.h"
#include <QPainter>
#include <QFont>
#include <algorithm>
#include <cmath>

using namespace std;

namespace {

void usarFuente(QPainter &painter, int tamaño)
{
    QFont fuente("Segoe UI");
    fuente.setPixelSize(tamaño);
    fuente.setBold(true);
    painter.setFont(fuente);
}

}

VistaJuego::VistaJuego(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::NoFocus);
    setMinimumSize(0, 300);
}

void VistaJuego::establecerJuego(const Juego *valor)
{
    juego = valor;
    update();
}

const QStringList &VistaJuego::obtenerErroresRecursos() const
{
    return recursos.obtenerErrores();
}

void VistaJuego::dibujarFigura(QPainter &painter, const QPixmap &imagen, double x,
                               double piesY, double altura, bool derecha)
{
    if (imagen.isNull())
        return;

    const double ancho = altura * imagen.width() / imagen.height();

    painter.save();
    painter.translate(x, piesY);

    if (!derecha)
        painter.scale(-1, 1);

    painter.drawPixmap(QRectF(-ancho / 2, -altura, ancho, altura), imagen, imagen.rect());
    painter.restore();
}

void VistaJuego::dibujarFondo(QPainter &painter)
{
    const Nivel &nivel = juego->obtenerNivel();
    const Jerry &jerry = juego->obtenerJugador();

    painter.drawPixmap(0, 0, recursos.obtenerFondo());
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 217, 126, 45));

    painter.drawEllipse(QRectF(nivel.obtenerPuertaX() - 64,
                               Configuracion::sueloY - 7, 128, 14));

    if (abs(jerry.obtenerPosicion().x - nivel.obtenerPuertaX()) < 280) {
        const QRectF cartel(nivel.obtenerPuertaX() - 125,
                            Configuracion::sueloY - 151, 250, 34);

        painter.setBrush(QColor(24, 38, 49));
        painter.drawRoundedRect(cartel, 10, 10);
        painter.setPen(QColor(241, 186, 76));

        usarFuente(painter, 15);

        painter.drawText(cartel, Qt::AlignCenter,
                         jerry.puedeSalir()
                             ? "E · Entrar a la tienda"
                             : "Tienda · 6 quesos + llave");
    }
}

void VistaJuego::dibujarPlataformas(QPainter &painter)
{
    for (const QRectF &plataforma : juego->obtenerNivel().obtenerPlataformas()) {
        for (int i = 0; i < 3; ++i) {
            dibujarFigura(painter, recursos.fotograma("caja_001", 0),
                          plataforma.x() + plataforma.width() / 3 * (i + 0.5),
                          Configuracion::sueloY, 64);
        }
    }

    for (const auto &obstaculo : juego->obtenerNivel().obtenerObstaculos()) {
        const Vector2 posicion = obstaculo->obtenerPosicion();

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(31, 24, 18, 50));

        painter.drawEllipse(QRectF(posicion.x - 20,
                                   posicion.y - 3, 40, 7));

        dibujarFigura(painter, recursos.fotograma("barril_001", 0),
                      posicion.x, posicion.y, 43);
    }
}

void VistaJuego::dibujarObjetos(QPainter &painter)
{
    const double tiempo = juego->obtenerNivel().obtenerTiempo();

    for (const auto &objeto : juego->obtenerNivel().obtenerObjetos()) {
        if (!objeto->estaActivo())
            continue;

        const Vector2 posicion = objeto->obtenerPosicion();
        const QString nombre = objeto->obtenerRecurso();

        const double altura = nombre == "llave_001" ? 36 :
                                  nombre == "moneda" ? 25 : 30;

        const double rebote = sin(tiempo * 3 + posicion.x * 0.01) * 3;

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 210, 90, 42));

        painter.drawEllipse(QPointF(posicion.x,
                                    posicion.y - altura / 2), 23, 23);

        dibujarFigura(painter, recursos.fotograma(nombre, tiempo, 7),
                      posicion.x, posicion.y + rebote, altura);
    }
}

void VistaJuego::dibujarTom(QPainter &painter)
{
    const Tom &tom = juego->obtenerNivel().obtenerTom();

    const Vector2 posicion = tom.obtenerPosicion();
    const Vector2 velocidad = tom.obtenerVelocidad();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(31, 24, 18, 55));

    painter.drawEllipse(QRectF(posicion.x - 27,
                               posicion.y - 3, 54, 8));

    const QString accion =
        abs(velocidad.x) + abs(velocidad.y) < 1 ? "tom_quieto" :
            abs(velocidad.x) > 170 ? "tom_correr" : "tom_caminar";

    const double altura = 102 - (Configuracion::sueloY - posicion.y) * 0.18;

    dibujarFigura(painter,
                  recursos.fotograma(accion,
                                     juego->obtenerNivel().obtenerTiempo(), 11),
                  posicion.x, posicion.y, altura, tom.miraHaciaDerecha());
}

void VistaJuego::dibujarJerry(QPainter &painter)
{
    const Jerry &jerry = juego->obtenerJugador();

    const Vector2 posicion = jerry.obtenerPosicion();
    const Vector2 velocidad = jerry.obtenerVelocidad();

    QString accion = "jerry_quieto";
    double altura = 78;
    double reloj = juego->obtenerNivel().obtenerTiempo();
    double cuadros = 11;

    if (jerry.estaHerido()) {
        const double transcurrido = 0.9 - jerry.obtenerTiempoHerida();

        if (transcurrido < 0.25) {
            accion = "jerry_herido";
            reloj = transcurrido;
            cuadros = 16;
        } else {
            accion = "jerry_caer";
            reloj = min(3.0, (transcurrido - 0.25) * 6);
            cuadros = 1;
            altura = reloj >= 2 ? 38 : 72;
        }
    } else if (!jerry.estaEnSuelo()) {
        accion = "jerry_saltar";

        const double impulso =
            420 * (jerry.obtenerTiempoSaltoMejorado() > 0
                       ? sqrt(1.2) : 1.0);

        reloj = clamp((velocidad.y + impulso) /
                          (2 * impulso) * 4.99, 0.0, 4.99);

        cuadros = 1;
    } else if (abs(velocidad.x) > 1) {
        accion = jerry.obtenerTiempoAgilidad() > 0
                     ? "jerry_correr" : "jerry_caminar";
    }

    if (jerry.obtenerProteccion() > 0 &&
        int(jerry.obtenerProteccion() * 12) % 2 == 0)
        painter.setOpacity(0.35);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(31, 24, 18, 55));

    painter.drawEllipse(QRectF(
        posicion.x - 18,
        (jerry.estaEnSuelo() ? posicion.y : Configuracion::sueloY) - 3,
        36, 7));

    dibujarFigura(painter, recursos.fotograma(accion, reloj, cuadros),
                  posicion.x, min(posicion.y, Configuracion::sueloY),
                  altura, jerry.miraHaciaDerecha());

    painter.setOpacity(1);

    if (jerry.obtenerTiempoSaltoMejorado() > 0 && !jerry.estaHerido()) {
        painter.setBrush(QColor(24, 38, 49, 215));

        painter.drawRoundedRect(QRectF(posicion.x - 46,
                                       posicion.y - 101, 92, 18), 5, 5);

        painter.setPen(QColor(241, 186, 76));
        usarFuente(painter, 12);

        painter.drawText(QRectF(posicion.x - 40,
                                posicion.y - 101, 80, 18),
                         Qt::AlignCenter, "SALTO +20 %");
    }
}

void VistaJuego::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(rect(), QColor(24, 38, 49));

    if (!juego)
        return;

    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::SmoothPixmapTransform);

    const double escala = min(
        width() / double(Configuracion::anchoVista),
        height() / double(Configuracion::altoVista));

    painter.translate(
        (width() - Configuracion::anchoVista * escala) / 2,
        (height() - Configuracion::altoVista * escala) / 2);

    painter.scale(escala, escala);

    painter.setClipRect(0, 0,
                        Configuracion::anchoVista,
                        Configuracion::altoVista);

    painter.save();

    painter.translate(-juego->obtenerCamaraX(),
                      -Configuracion::inicioVista);

    dibujarFondo(painter);

    const Nivel &nivel = juego->obtenerNivel();

    for (const Habitante &habitante : nivel.obtenerHabitantes()) {
        dibujarFigura(
            painter,
            recursos.fotograma(habitante.animacion,
                               nivel.obtenerTiempo(), 5),
            habitante.x,
            habitante.piesY,
            habitante.altura,
            habitante.direccion > 0);
    }

    if (nivel.obtenerTom().obtenerPosicion().y <=
        Configuracion::sueloY - 18)
        dibujarTom(painter);

    dibujarPlataformas(painter);
    dibujarObjetos(painter);

    if (nivel.obtenerTom().obtenerPosicion().y >
        Configuracion::sueloY - 18)
        dibujarTom(painter);

    dibujarJerry(painter);

    painter.restore();

    if (juego->obtenerEstado() != EstadoJuego::Jugando)
        painter.fillRect(
            QRectF(0, 0,
                   Configuracion::anchoVista,
                   Configuracion::altoVista),
            QColor(12, 23, 31, 145));
}