#ifndef VISTAJUEGO_H
#define VISTAJUEGO_H
#include <QWidget>
#include <QElapsedTimer>
#include "recursosgraficos.h"

class Juego;
class QPainter;

class VistaJuego : public QWidget
{
    Q_OBJECT
public:
    explicit VistaJuego(QWidget *parent = nullptr);
    void establecerJuego(const Juego *valor);
    const QStringList &obtenerErroresRecursos() const;
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    void dibujarFondo(QPainter &painter);
    void dibujarEscenaTienda(QPainter &painter);
    void dibujarEscenaCabildo(QPainter &painter);
    void dibujarPlataformas(QPainter &painter);
    void dibujarObjetos(QPainter &painter);
    void dibujarTom(QPainter &painter);
    void dibujarJerry(QPainter &painter);
    void dibujarActoresCabildo(QPainter &painter);
    void dibujarCajaCabildo(QPainter &painter, const QRectF &caja);
    void dibujarFlorero(QPainter &painter, double x, double baseY, double alto);
    void dibujarFigura(QPainter &painter, const QPixmap &imagen, double x, double piesY,
                       double altura, bool derecha = true);
    const Juego *juego = nullptr;
    RecursosGraficos recursos;
    QElapsedTimer relojIntro;
};
#endif