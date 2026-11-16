#include "entidad.h"

Entidad::Entidad(Vector2 posicion, double ancho, double alto)
    : posicion(posicion), ancho(ancho), alto(alto)
{
}

bool Entidad::colisiona(const Entidad &otra) const
{
    return rectanguloColision().intersects(otra.rectanguloColision());
}

QRectF Entidad::rectanguloColision() const
{
    return QRectF(posicion.x - ancho / 2, posicion.y - alto, ancho, alto);
}

const Vector2 &Entidad::obtenerPosicion() const { return posicion; }
const Vector2 &Entidad::obtenerVelocidad() const { return velocidad; }
void Entidad::establecerPosicion(Vector2 valor) { posicion = valor; }
void Entidad::establecerVelocidad(Vector2 valor) { velocidad = valor; }