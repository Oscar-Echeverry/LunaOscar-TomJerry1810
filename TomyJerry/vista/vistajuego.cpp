#include "vistajuego.h"
#include "juego.h"
#include "logica/configuracion.h"
#include "logica/niveles.h"
#include <QPainter>
#include <QFont>
#include <QLinearGradient>
#include <QPainterPath>
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
    relojIntro.start();
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
    if (nivel.obtenerNumero() == 2) {
        painter.drawPixmap(QRectF(0, Configuracion::fondoTiendaY, recursos.obtenerFondoTienda().width(),
                                  Configuracion::altoFondoTienda),
                           recursos.obtenerFondoTienda(), recursos.obtenerFondoTienda().rect());
        return;
    }
    if (nivel.obtenerNumero() == 3) {
        const auto &cabildo = static_cast<const NivelCabildo &>(nivel);
        const QPixmap &fondo = recursos.obtenerFondoCabildo(cabildo.obtenerPantalla());
        painter.drawPixmap(QRectF(0, 0, 1120, 630), fondo, fondo.rect());
        return;
    }
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

void VistaJuego::dibujarEscenaCabildo(QPainter &painter)
{
    const auto *cabildo = dynamic_cast<const NivelCabildo *>(&juego->obtenerNivel());
    if (!cabildo)
        return;

    for (const QRectF &valla : cabildo->obtenerVallas()) {
        painter.setPen(QPen(QColor(60, 38, 22), 2));
        painter.setBrush(QColor(153, 102, 55));
        painter.drawRoundedRect(valla, 2, 2);
        painter.setPen(QPen(QColor(223, 174, 100), 2));
        for (double x = valla.left() + 8; x < valla.right(); x += 22)
            painter.drawLine(QPointF(x, valla.top()), QPointF(x, valla.bottom()));
    }

    auto dibujarRuta = [&painter](const QVector<Vector2> &ruta, const QColor &color,
                                  Qt::PenStyle estilo) {
        if (ruta.size() < 2)
            return;
        QPainterPath camino;
        camino.moveTo(ruta.first().x, ruta.first().y);
        for (int i = 1; i < ruta.size(); ++i)
            camino.lineTo(ruta[i].x, ruta[i].y);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(24, 38, 49, 165), 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(camino);
        painter.setPen(QPen(color, 4, estilo, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPath(camino);
        double distancia = 0;
        for (int i = 1; i < ruta.size(); ++i) {
            distancia += ruta[i - 1].distancia(ruta[i]);
            if (distancia < 75)
                continue;
            distancia = 0;
            const QPointF fin(ruta[i].x, ruta[i].y);
            QPointF direccion(ruta[i].x - ruta[i - 1].x, ruta[i].y - ruta[i - 1].y);
            const double largo = std::hypot(direccion.x(), direccion.y());
            if (largo < 0.001)
                continue;
            direccion /= largo;
            const QPointF lado(-direccion.y(), direccion.x());
            painter.setPen(QPen(color, 3, Qt::SolidLine, Qt::RoundCap));
            painter.drawLine(fin - direccion * 9 + lado * 5, fin);
            painter.drawLine(fin - direccion * 9 - lado * 5, fin);
        }
    };
    dibujarRuta(cabildo->obtenerRutaCorta(), QColor(255, 113, 91), Qt::SolidLine);
    dibujarRuta(cabildo->obtenerRutaAlternativa(), QColor(103, 239, 175), Qt::DashLine);

    const int pantalla = cabildo->obtenerPantalla();
    const Vector2 puerta = cabildo->obtenerCabildo();
    painter.setBrush(QColor(255, 217, 92, 85));
    painter.setPen(QPen(QColor(255, 227, 136), 2));
    painter.drawEllipse(QPointF(puerta.x, puerta.y), 31, 12);
    const QRectF letrero(puerta.x - 100, puerta.y - 55, 200, 27);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(24, 38, 49, 235));
    painter.drawRoundedRect(letrero, 7, 7);
    painter.setPen(QColor(255, 230, 153));
    usarFuente(painter, 12);
    painter.drawText(letrero, Qt::AlignCenter,
                     pantalla == 4 ? "CABILDO · E: ENTREGAR" : "ARCO A LA SIGUIENTE PLAZA ↑");

    painter.setPen(Qt::NoPen);
    const QVector<Vector2> &rastro = cabildo->obtenerRastroJerry();
    for (int i = 0; i < rastro.size(); ++i) {
        painter.setBrush(QColor(54, 101, 117, 20 + 65 * i / std::max(1, int(rastro.size()))));
        painter.drawEllipse(QPointF(rastro[i].x, rastro[i].y - 8), 3, 2);
    }

}

void VistaJuego::dibujarCajaCabildo(QPainter &painter, const QRectF &caja)
{
    painter.setPen(QPen(QColor(255, 221, 121), 2));
    painter.setBrush(QColor(255, 208, 92, 35));
    painter.drawRoundedRect(caja.adjusted(-3, -3, 3, 3), 4, 4);
    const QPixmap &imagen = recursos.fotograma("caja_001", 0);
    painter.drawPixmap(caja, imagen, imagen.rect());
    const QRectF tecla(caja.center().x() - 8, caja.top() - 22, 16, 17);
    painter.setBrush(QColor(24, 38, 49, 235));
    painter.drawRoundedRect(tecla, 3, 3);
    painter.setPen(QColor(255, 245, 205));
    usarFuente(painter, 12);
    painter.drawText(tecla, Qt::AlignCenter, "E");
}

void VistaJuego::dibujarEscenaTienda(QPainter &painter)
{
    const auto *tienda = dynamic_cast<const NivelTienda *>(&juego->obtenerNivel());
    if (!tienda)
        return;
    const double tiempo = tienda->obtenerTiempo();
    const Vector2 llorente = tienda->obtenerLlorente().obtenerPosicion();
    const QString dialogo = tienda->dialogoCriollos();
    dibujarFigura(painter, recursos.fotograma(
                      tienda->enConversacion() && dialogo.startsWith("Criollos: Don")
                          ? "llorente_quieto" : "llorente_enojado",
                      tiempo, 5), llorente.x, llorente.y, 170, false);
    const QPixmap &fondo = recursos.obtenerFondoTienda();
    const QRectF frente(3 * Configuracion::anchoVista + 760 * 1120.0 / 1672,
                        Configuracion::fondoTiendaY + 505 * 630.0 / 941,
                        675 * 1120.0 / 1672, 140 * 630.0 / 941);
    const QRectF origen(frente.x(), frente.y() - Configuracion::fondoTiendaY,
                        frente.width(), frente.height());
    painter.drawPixmap(frente, fondo, origen);
    const bool llevandoFlorero = juego->obtenerJugador().tieneElFlorero();
    const double mostradorX = tienda->obtenerPuertaX();
    painter.setPen(QPen(QColor(255, 222, 124), 2));
    painter.setBrush(QColor(255, 208, 82, llevandoFlorero ? 85 : 35));
    painter.drawEllipse(QRectF(mostradorX - 78, Configuracion::sueloY - 9, 156, 18));
    const QRectF entrega(mostradorX - 125, 414, 250, 35);
    painter.setBrush(QColor(33, 30, 22, 235));
    painter.drawRoundedRect(entrega, 7, 7);
    painter.setPen(QColor(255, 232, 164));
    usarFuente(painter, 14);
    painter.drawText(entrega, Qt::AlignCenter, "ENTREGAR FLORERO · E");
    const double flechaY = 513 + std::sin(tiempo * 4) * 4;
    painter.setPen(QPen(QColor(255, 223, 118), 5, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(mostradorX, flechaY - 17), QPointF(mostradorX, flechaY + 9));
    painter.drawLine(QPointF(mostradorX - 9, flechaY), QPointF(mostradorX, flechaY + 9));
    painter.drawLine(QPointF(mostradorX + 9, flechaY), QPointF(mostradorX, flechaY + 9));
    if (tienda->enConversacion()) {
        const QRectF conversacion(juego->obtenerCamaraX() + 70, 150, 980, 110);
        painter.setPen(QPen(QColor(241, 186, 76), 2));
        painter.setBrush(QColor(24, 38, 49, 240));
        painter.drawRoundedRect(conversacion, 12, 12);
        painter.setPen(QColor(255, 244, 214));
        usarFuente(painter, 19);
        painter.drawText(conversacion.adjusted(20, 12, -20, -30),
                         Qt::AlignVCenter | Qt::TextWordWrap, dialogo);
        painter.setPen(QColor(241, 186, 76));
        usarFuente(painter, 12);
        painter.drawText(conversacion.adjusted(20, 78, -20, -8), Qt::AlignRight | Qt::AlignVCenter,
                         "E · Continuar diálogo");
        return;
    }
    if (tienda->tieneOrero())
        dibujarFlorero(painter, tienda->posicionOreroX(), tienda->posicionOreroY() + 16, 32);
    for (const Proyectil &proyectil : tienda->obtenerObjetosCaida()) {
        const Vector2 objeto = proyectil.obtenerPosicion();
        dibujarFigura(painter, recursos.fotograma("tintero", tiempo, 1),
                      objeto.x, objeto.y + 14, 30);
    }
}
void VistaJuego::dibujarFlorero(QPainter &painter, double x, double baseY, double alto)
{
    painter.save();
    painter.translate(x, baseY);
    painter.scale(alto / 32.0, alto / 32.0);
    QPainterPath cuerpo;
    cuerpo.moveTo(-5, -27);
    cuerpo.lineTo(5, -27);
    cuerpo.cubicTo(5, -20, 12, -18, 13, -11);
    cuerpo.cubicTo(15, -3, 10, 0, 0, 0);
    cuerpo.cubicTo(-10, 0, -15, -3, -13, -11);
    cuerpo.cubicTo(-12, -18, -5, -20, -5, -27);
    cuerpo.closeSubpath();
    QLinearGradient esmalte(-12, -12, 12, -8);
    esmalte.setColorAt(0, QColor(105, 44, 25));
    esmalte.setColorAt(0.5, QColor(190, 91, 43));
    esmalte.setColorAt(1, QColor(113, 48, 28));
    painter.setPen(QPen(QColor(74, 34, 24), 1.5));
    painter.setBrush(esmalte);
    painter.drawPath(cuerpo);
    painter.setBrush(QColor(128, 53, 31));
    painter.drawRoundedRect(QRectF(-6, -30, 12, 5), 2, 2);
    painter.setPen(QPen(QColor(230, 162, 93), 1.2));
    painter.drawArc(QRectF(-10, -12, 20, 15), 20 * 16, 130 * 16);
    painter.restore();
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

    const double altura = juego->obtenerNivel().obtenerNumero() == 3
                              ? 48 : 102 - (Configuracion::sueloY - posicion.y) * 0.18;

    dibujarFigura(painter,
                  recursos.fotograma(accion,
                                     juego->obtenerNivel().obtenerTiempo(), 11),
                  posicion.x, posicion.y, altura, tom.miraHaciaDerecha());
    if (const auto *cabildo = dynamic_cast<const NivelCabildo *>(&juego->obtenerNivel())) {
        if (cabildo->tomEstaAturdido()) {
            painter.setPen(QColor(255, 229, 110));
            usarFuente(painter, 13);
            painter.drawText(QRectF(posicion.x - 45, posicion.y - 68, 90, 20),
                             Qt::AlignCenter, "★ 1 s ★");
        }
    }
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
    } else if (juego->obtenerNivel().obtenerNumero() == 3
               && std::hypot(velocidad.x, velocidad.y) > 1) {
        accion = "jerry_caminar";
    } else if (!jerry.estaEnSuelo()) {
        accion = "jerry_saltar";

        const double impulso =
            420 * (jerry.obtenerTiempoSaltoMejorado() > 0
                       ? sqrt(1.2) : 1.0);

        reloj = clamp((velocidad.y + impulso) /
                          (2 * impulso) * 4.99, 0.0, 4.99);

        cuadros = 1;
    } else if (jerry.estaAgachado()) {
        accion = abs(velocidad.x) > 1 ? "jerry_caminar" : "jerry_quieto";
        altura = 48;
    } else if (abs(velocidad.x) > 1) {
        accion = jerry.obtenerTiempoAgilidad() > 0
                     ? "jerry_correr" : "jerry_caminar";
    }

    if (juego->obtenerNivel().obtenerNumero() == 3)
        altura = 44;

    const bool enPlaza = juego->obtenerNivel().obtenerNumero() == 3;
    if (!enPlaza && jerry.obtenerProteccion() > 0 &&
        int(jerry.obtenerProteccion() * 12) % 2 == 0)
        painter.setOpacity(0.35);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(31, 24, 18, 55));

    painter.drawEllipse(QRectF(
        posicion.x - 18,
        (jerry.estaEnSuelo() ? posicion.y : Configuracion::sueloY) - 3,
        36, 7));

    dibujarFigura(painter, recursos.fotograma(accion, reloj, cuadros),
                  posicion.x, juego->obtenerNivel().obtenerNumero() == 3
                      ? posicion.y : min(posicion.y, Configuracion::sueloY),
                  altura, jerry.miraHaciaDerecha());

    if (jerry.tieneElFlorero())
        dibujarFlorero(painter, posicion.x + (jerry.miraHaciaDerecha() ? 19 : -19),
                       posicion.y - (jerry.estaAgachado() ? 14 : 24), 25);
    if (juego->obtenerNivel().obtenerNumero() == 3) {
        painter.setPen(QPen(QColor(110, 77, 42), 1));
        painter.setBrush(QColor(248, 226, 170));
        painter.drawRoundedRect(QRectF(posicion.x + 12, posicion.y - 38, 20, 13), 2, 2);
        painter.drawLine(QPointF(posicion.x + 15, posicion.y - 34),
                         QPointF(posicion.x + 28, posicion.y - 34));
        painter.drawLine(QPointF(posicion.x + 15, posicion.y - 30),
                         QPointF(posicion.x + 25, posicion.y - 30));
    }

    painter.setOpacity(1);

    if (juego->obtenerNivel().obtenerNumero() != 2
        && jerry.obtenerTiempoSaltoMejorado() > 0 && !jerry.estaHerido()) {
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

void VistaJuego::dibujarActoresCabildo(QPainter &painter)
{
    const auto &cabildo = static_cast<const NivelCabildo &>(juego->obtenerNivel());
    struct Actor {
        double piesY;
        int tipo;
        int indice;
    };
    QVector<Actor> actores;
    const QVector<Habitante> &habitantes = juego->obtenerNivel().obtenerHabitantes();
    for (int i = 0; i < habitantes.size(); ++i)
        actores.append({habitantes[i].piesY, 0, i});
    actores.append({juego->obtenerNivel().obtenerTom().obtenerPosicion().y, 1, 0});
    actores.append({juego->obtenerJugador().obtenerPosicion().y, 2, 0});
    const auto &decorados = cabildo.obtenerEscenografia();
    for (int i = 0; i < decorados.size(); ++i)
        actores.append({decorados[i].profundidad, 3, i});
    const auto &cajas = cabildo.obtenerCajas();
    for (int i = 0; i < cajas.size(); ++i)
        actores.append({cajas[i].rectangulo.bottom(), 4, i});
    std::stable_sort(actores.begin(), actores.end(), [](const Actor &a, const Actor &b) {
        return a.piesY < b.piesY;
    });

    for (const Actor &actor : actores) {
        if (actor.tipo == 0) {
            const Habitante &habitante = habitantes[actor.indice];
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(31, 24, 18, 55));
            painter.drawEllipse(QRectF(habitante.x - 13, habitante.piesY - 2, 26, 5));
            dibujarFigura(painter, recursos.fotograma(habitante.animacion,
                                                      juego->obtenerNivel().obtenerTiempo(), 5), habitante.x,
                          habitante.piesY, habitante.altura, habitante.direccion > 0);
        } else if (actor.tipo == 1) {
            dibujarTom(painter);
        } else if (actor.tipo == 2) {
            dibujarJerry(painter);
        } else if (actor.tipo == 3) {
            const QRectF &destino = decorados[actor.indice].imagen;
            const QPixmap &fondo = recursos.obtenerFondoCabildo(cabildo.obtenerPantalla());
            const QRectF origen(destino.x() * fondo.width() / 1120.0,
                                destino.y() * fondo.height() / 630.0,
                                destino.width() * fondo.width() / 1120.0,
                                destino.height() * fondo.height() / 630.0);
            painter.drawPixmap(destino, fondo, origen);
        } else {
            dibujarCajaCabildo(painter, cajas[actor.indice].rectangulo);
        }
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

    const bool esCabildo = juego->obtenerNivel().obtenerNumero() == 3;
    const int altoEscena = esCabildo ? Configuracion::altoFondo : Configuracion::altoVista;
    const double escala = min(
        width() / double(Configuracion::anchoVista),
        height() / double(altoEscena));

    painter.translate(
        (width() - Configuracion::anchoVista * escala) / 2,
        (height() - altoEscena * escala) / 2);

    painter.scale(escala, escala);

    painter.setClipRect(0, 0,
                        Configuracion::anchoVista,
                        altoEscena);

    painter.save();

    painter.translate(-juego->obtenerCamaraX(),
                      esCabildo ? 0 : -Configuracion::inicioVista);

    const Nivel &nivel = juego->obtenerNivel();
    dibujarFondo(painter);
    if (nivel.obtenerNumero() == 2)
        dibujarEscenaTienda(painter);
    else if (nivel.obtenerNumero() == 3)
        dibujarEscenaCabildo(painter);

    if (nivel.obtenerNumero() != 3) {
        for (const Habitante &habitante : nivel.obtenerHabitantes()) {
            dibujarFigura(painter, recursos.fotograma(habitante.animacion,
                                                      nivel.obtenerTiempo(), 5), habitante.x, habitante.piesY,
                          habitante.altura, habitante.direccion > 0);
        }
    }

    if (nivel.obtenerNumero() != 3 && nivel.obtenerTom().obtenerPosicion().y <=
                                          Configuracion::sueloY - 18)
        dibujarTom(painter);

    if (nivel.obtenerNumero() != 3) {
        dibujarPlataformas(painter);
        dibujarObjetos(painter);
    }

    if (nivel.obtenerNumero() != 3 && nivel.obtenerTom().obtenerPosicion().y >
                                          Configuracion::sueloY - 18)
        dibujarTom(painter);
    if (nivel.obtenerNumero() == 3)
        dibujarActoresCabildo(painter);
    else
        dibujarJerry(painter);

    painter.restore();

    if (nivel.obtenerNumero() == 2 && juego->obtenerEstado() == EstadoJuego::Jugando
        && juego->obtenerJugador().tieneElFlorero()) {
        const double distancia = nivel.obtenerPuertaX() - juego->obtenerJugador().obtenerPosicion().x;
        const QRectF indicacion(760, 16, 340, 60);
        painter.setPen(QPen(QColor(241, 186, 76), 2));
        painter.setBrush(QColor(24, 38, 49, 240));
        painter.drawRoundedRect(indicacion, 10, 10);
        painter.setPen(QColor(255, 232, 164));
        usarFuente(painter, 18);
        painter.drawText(indicacion.adjusted(10, 5, -10, -27), Qt::AlignCenter,
                         std::abs(distancia) <= 78 ? "E · Entregar el florero"
                             : distancia > 0 ? "Mostrador →" : "← Mostrador");
        usarFuente(painter, 12);
        painter.drawText(indicacion.adjusted(10, 33, -10, -6), Qt::AlignCenter,
                         "Entrega el florero en la zona iluminada");
    }

    if (juego->obtenerEstado() == EstadoJuego::Presentacion) {
        painter.fillRect(QRectF(0, 0, Configuracion::anchoVista, altoEscena), QColor(12, 23, 31, 170));
        dibujarFigura(painter, recursos.fotograma("tom_correr", relojIntro.elapsed() / 1000.0, 8),
                      103, altoEscena - 14, 190, true);
        dibujarFigura(painter, recursos.fotograma("jerry_correr", relojIntro.elapsed() / 1000.0, 8),
                      1017, altoEscena - 14, 130, false);
    } else if (juego->obtenerEstado() != EstadoJuego::Jugando)
        painter.fillRect(
            QRectF(0, 0,
                   Configuracion::anchoVista,
                   altoEscena),
            QColor(12, 23, 31, 145));
}
