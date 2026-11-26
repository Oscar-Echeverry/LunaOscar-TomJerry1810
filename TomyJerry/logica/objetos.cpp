#include "objetos.h"
#include "personajes.h"
#include "fisicajuego.h"

Objeto::Objeto(Vector2 posicion, double alto) : Entidad(posicion, 32, alto) {}
bool Objeto::estaActivo() const { return activo; }
void Objeto::establecerActivo(bool valor) { activo = valor; }

void Objeto::recoger(Jerry &jugador)
{
    if (activo && colisiona(jugador)) {
        jugador.recoger(*this);
        activo = false;
    }
}

void Objeto::actualizar(double dt)
{
    Vector2 posicion = obtenerPosicion();
    const Vector2 velocidad = obtenerVelocidad();
    posicion.x += velocidad.x * dt;
    posicion.y += velocidad.y * dt;
    establecerPosicion(posicion);
}

Queso::Queso(Vector2 posicion) : Objeto(posicion) {}
void Queso::aplicarEfecto(Jerry &jugador)
{
    jugador.sumarQueso(valor);
    jugador.activarSaltoMejorado(5);
}
QString Queso::obtenerRecurso() const { return "queso_001"; }

Moneda::Moneda(Vector2 posicion) : Objeto(posicion, 25) {}
void Moneda::aplicarEfecto(Jerry &jugador) { jugador.sumarMoneda(valor); }
QString Moneda::obtenerRecurso() const { return "moneda"; }

Llave::Llave(Vector2 posicion) : Objeto(posicion, 36) {}
void Llave::aplicarEfecto(Jerry &jugador)
{
    obtenida = true;
    jugador.establecerLlave(obtenida);
}
QString Llave::obtenerRecurso() const { return "llave_001"; }

Florero::Florero(Vector2 posicion) : Objeto(posicion) {}
void Florero::aplicarEfecto(Jerry &jugador)
{
    obtenido = true;
    jugador.establecerFlorero(obtenido);
}
QString Florero::obtenerRecurso() const { return {}; }

Obstaculo::Obstaculo(Vector2 centro, double fase, TipoMovimiento tipo, double amplitud, double frecuencia)
    : Entidad(centro, 30, 43), tipoMovimiento(tipo), centro(centro), amplitud(amplitud),
    frecuencia(frecuencia), fase(fase)
{
}

void Obstaculo::actualizar(double dt) { tiempo += dt; }

void Obstaculo::actualizarFisica(const FisicaJuego &fisica, double dt)
{
    actualizar(dt);
    if (tipoMovimiento == TipoMovimiento::OSCILATORIO)
        fisica.aplicarOscilacion(*this, centro, eje, amplitud, frecuencia, tiempo, fase);
    else if (tipoMovimiento == TipoMovimiento::FRICCION)
        fisica.aplicarFriccion(*this, dt);
}

void Obstaculo::recibirImpulso(const Vector2 &impulso) { establecerVelocidad(impulso); }
void Obstaculo::aplicardaño(Personaje &personaje)
{
    if (rectanguloColision().adjusted(4, 5, -4, -2).intersects(personaje.rectanguloColision()))
        personaje.recibirDaño(daño);
}
const Vector2 &Obstaculo::obtenerCentro() const { return centro; }
double Obstaculo::obtenerAmplitud() const { return amplitud; }
void Obstaculo::establecerTiempo(double valor) { tiempo = valor; }
void Obstaculo::configurarOscilacion(double nuevaAmplitud, double nuevaFrecuencia)
{
    amplitud = nuevaAmplitud;
    frecuencia = nuevaFrecuencia;
}