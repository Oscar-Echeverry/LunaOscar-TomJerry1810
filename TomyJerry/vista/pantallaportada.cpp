#include "pantallaportada.h"
#include <QPainter>
#include <QPushButton>
#include <algorithm>
#include <stdexcept>

PantallaPortada::PantallaPortada(QWidget *parent)
    : QWidget(parent), imagen(":/Pantallas/Pantallas/Intro.jpeg"),
    botonJugar(new QPushButton(this))
{
    if (imagen.isNull())
        throw std::runtime_error("No se pudo cargar Pantallas/Intro.jpeg.");
    setObjectName("pantallaPortada");
    setAttribute(Qt::WA_OpaquePaintEvent);
    botonJugar->setObjectName("botonJugarPortada");
    botonJugar->setAccessibleName("Jugar: seleccionar nivel");
    botonJugar->setToolTip("Jugar · Enter");
    botonJugar->setCursor(Qt::PointingHandCursor);
    botonJugar->setFocusPolicy(Qt::NoFocus);
    botonJugar->setStyleSheet(
        "QPushButton { background: transparent; border: 2px solid transparent; border-radius: 18px; }"
        "QPushButton:hover { background: rgba(255, 210, 90, 25); border-color: #ffe4a2; }"
        "QPushButton:pressed { background: rgba(255, 210, 90, 55); }");
    connect(botonJugar, &QPushButton::clicked, this, &PantallaPortada::continuar);
}

QRectF PantallaPortada::rectanguloImagen() const
{
    const double escala = std::min(width() / double(imagen.width()), height() / double(imagen.height()));
    const QSizeF tamano(imagen.width() * escala, imagen.height() * escala);
    return QRectF(QPointF((width() - tamano.width()) / 2, (height() - tamano.height()) / 2), tamano);
}

QRectF PantallaPortada::rectanguloJugar() const
{
    const QRectF fondo = rectanguloImagen();
    return {fondo.left() + fondo.width() * 457 / 1283,
            fondo.top() + fondo.height() * 710 / 832,
            fondo.width() * 434 / 1283, fondo.height() * 107 / 832};
}

void PantallaPortada::resizeEvent(QResizeEvent *)
{
    botonJugar->setGeometry(rectanguloJugar().toAlignedRect());
}

void PantallaPortada::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(12, 20, 33));
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    const QRectF fondo = rectanguloImagen();
    painter.drawPixmap(fondo, imagen, imagen.rect());
    const QRectF firma(fondo.left() + 12, fondo.bottom() - 30, 100, 23);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(12, 20, 33, 190));
    painter.drawRoundedRect(firma, 5, 5);
    painter.setPen(QColor(255, 223, 146));
    painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
    painter.drawText(firma, Qt::AlignCenter, "EQUIPO 16");
}