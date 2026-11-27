#ifndef FISICAJUEGO_H
#define FISICAJUEGO_H
#include "entidad.h"

class Personaje;

class FisicaJuego
{
public:
    explicit FisicaJuego(double gravedad = 900, double coeficienteFriccion = 0.25);
    void aplicarParabola(Entidad &entidad, double dt, bool moverHorizontal = true) const;
    void aplicarOscilacion(Entidad &entidad, const Vector2 &centro,
                           const Vector2 &eje, double amplitud, double frecuencia,
                           double tiempo) const;
    void aplicarOscilacion(Entidad &entidad, const Vector2 &centro,
                           const Vector2 &eje, double amplitud, double frecuencia,
                           double tiempo, double fase) const;
    void aplicarFriccion(Entidad &entidad, double dt) const;
    void aplicarGravedad(Entidad &entidad, double dt) const;
    void iniciarSalto(Personaje &personaje, double impulso) const;

private:
    double gravedad;
    double coeficienteFriccion;
};
#endif