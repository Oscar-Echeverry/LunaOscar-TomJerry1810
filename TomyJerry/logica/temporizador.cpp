#include "temporizador.h"
#include <algorithm>
using namespace std;

void Temporizador::iniciar(double segundos)
{
    tiempoRestante = max(0.0, segundos);
    activo = tiempoRestante > 0;
}

void Temporizador::actualizar(double dt)
{
    if (activo) {
        tiempoRestante = max(0.0, tiempoRestante - dt);
        activo = tiempoRestante > 0;
    }
}

bool Temporizador::termino() const
{
    return tiempoRestante == 0;
}

double Temporizador::obtenerTiempoRestante() const
{
    return tiempoRestante;
}