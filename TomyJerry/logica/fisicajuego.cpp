#include "fisicajuego.h"
#include "../logica/personajes.h"
#include <algorithm>
#include <cmath>

using namespace std;

FisicaJuego::FisicaJuego(double gravedad, double coeficienteFriccion)
    : gravedad(gravedad), coeficienteFriccion(coeficienteFriccion)
{
}

void FisicaJuego::aplicarParabola(Entidad &entidad, double dt, bool moverHorizontal) const
{
    Vector2 posicion = entidad.obtenerPosicion();
    const Vector2 velocidad = entidad.obtenerVelocidad();
    if (moverHorizontal)
        posicion.x += velocidad.x * dt;
    posicion.y += velocidad.y * dt + 0.5 * gravedad * dt * dt;
    entidad.establecerPosicion(posicion);
    aplicarGravedad(entidad, dt);
}

void FisicaJuego::aplicarGravedad(Entidad &entidad, double dt) const
{
    Vector2 velocidad = entidad.obtenerVelocidad();
    velocidad.y += gravedad * dt;
    entidad.establecerVelocidad(velocidad);
}

void FisicaJuego::aplicarOscilacion(Entidad &entidad, const Vector2 &centro,
                                    const Vector2 &eje, double amplitud, double frecuencia,
                                    double tiempo) const
{
    aplicarOscilacion(entidad, centro, eje, amplitud, frecuencia, tiempo, 0);
}

void FisicaJuego::aplicarOscilacion(Entidad &entidad, const Vector2 &centro,
                                    const Vector2 &eje, double amplitud, double frecuencia,
                                    double tiempo, double fase) const
{
    constexpr double pi = 3.14159265358979323846;
    const double desplazamiento = amplitud * sin(2 * pi * frecuencia * tiempo + fase);

    entidad.establecerPosicion({centro.x + eje.x * desplazamiento,
                                centro.y + eje.y * desplazamiento});
}

void FisicaJuego::aplicarFriccion(Entidad &entidad, double dt) const
{
    Vector2 velocidad = entidad.obtenerVelocidad();
    Vector2 posicion = entidad.obtenerPosicion();

    const double rapidez = hypot(velocidad.x, velocidad.y);

    if (rapidez == 0)
        return;

    const double nueva = max(0.0, rapidez - coeficienteFriccion * gravedad * dt);
    const double media = (rapidez + nueva) / 2;

    posicion.x += velocidad.x / rapidez * media * dt;
    posicion.y += velocidad.y / rapidez * media * dt;

    velocidad.x *= nueva / rapidez;
    velocidad.y *= nueva / rapidez;

    entidad.establecerPosicion(posicion);
    entidad.establecerVelocidad(velocidad);
}

void FisicaJuego::iniciarSalto(Personaje &personaje, double impulso) const
{
    if (!personaje.estaEnSuelo())
        return;

    Vector2 velocidad = personaje.obtenerVelocidad();
    velocidad.y = impulso;

    personaje.establecerVelocidad(velocidad);
    personaje.establecerEnSuelo(false);
}