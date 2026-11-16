#ifndef ENTIDAD_H
#define ENTIDAD_H
#include "vector2.h"
#include <QRectF>

class Entidad
{
public:
    Entidad(Vector2 posicion = {}, double ancho = 0, double alto = 0);
    virtual ~Entidad() = default;
    virtual void actualizar(double dt) = 0;
    bool colisiona(const Entidad &otra) const;
    QRectF rectanguloColision() const;
    const Vector2 &obtenerPosicion() const;
    const Vector2 &obtenerVelocidad() const;
    void establecerPosicion(Vector2 valor);
    void establecerVelocidad(Vector2 valor);

private:
    Vector2 posicion;
    Vector2 velocidad;
    double ancho;
    double alto;
};
#endif
