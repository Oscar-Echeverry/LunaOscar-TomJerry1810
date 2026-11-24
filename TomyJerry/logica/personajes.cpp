#include "personajes.h"
#include "fisicajuego.h"
#include "objetos.h"
#include "niveles.h"
#include "configuracion.h"
#include <algorithm>
#include <cmath>

Personaje::Personaje(Vector2 posicion, double ancho, double alto, int vidas, double velocidadBase)
    : Entidad(posicion, ancho, alto), vidas(vidas), velocidadBase(velocidadBase)
{
}

void Personaje::mover(const Vector2 &direccion, double dt)
{
    Vector2 velocidad = obtenerVelocidad();
    Vector2 posicion = obtenerPosicion();
    velocidad.x = direccion.x * velocidadBase;
    posicion.x += velocidad.x * dt;
    establecerVelocidad(velocidad);
    establecerPosicion(posicion);
    if (direccion.x != 0)
        miraDerecha = direccion.x > 0;
}

void Personaje::recibirDaño(int cantidad) { vidas = std::max(0, vidas - std::max(0, cantidad)); }
bool Personaje::estaVivo() const { return vidas > 0; }
bool Personaje::estaEnSuelo() const { return enSuelo; }
void Personaje::establecerEnSuelo(bool valor) { enSuelo = valor; }
int Personaje::obtenerVidas() const { return vidas; }
void Personaje::establecerVidas(int cantidad) { vidas = std::max(0, cantidad); }
bool Personaje::miraHaciaDerecha() const { return miraDerecha; }
double Personaje::obtenerVelocidadBase() const { return velocidadBase; }

Jerry::Jerry() : Personaje({180, Configuracion::sueloY}, 30, 59, 3, 240) {}

void Jerry::actualizar(double dt)
{
    actualizarEfectos(dt);
    if (!estaVivo() || estaHerido()) {
        Vector2 velocidad = obtenerVelocidad();
        velocidad.x = 0;
        establecerVelocidad(velocidad);
        return;
    }
    const double factor = (tieneFlorero ? 0.75 : 1.0)
                          * ((tiempoAgilidad > 0 || carreraTienda) ? 1.6 : 1.0);
    mover({direccion * factor, 0}, dt);
}

void Jerry::saltar(const FisicaJuego &fisica)
{
    if (estaVivo() && !estaHerido() && !tieneFlorero) {
        fisica.iniciarSalto(*this, impulsoSalto * (tiempoSaltoMejorado > 0 ? std::sqrt(1.2) : 1.0));
    }
}

void Jerry::recoger(Objeto &objeto) { objeto.aplicarEfecto(*this); }

void Jerry::activarAgilidad()
{
    if (tiempoAgilidad == 0 && tiempoRecarga == 0 && !estaHerido()) {
        tiempoAgilidad = 2;
        tiempoRecarga = 6;
    }
}

void Jerry::actualizarEfectos(double dt)
{
    proteccion = std::max(0.0, proteccion - dt);
    tiempoHerida = std::max(0.0, tiempoHerida - dt);
    tiempoSaltoMejorado = std::max(0.0, tiempoSaltoMejorado - dt);
    if (tiempoAgilidad > 0)
        tiempoAgilidad = std::max(0.0, tiempoAgilidad - dt);
    else
        tiempoRecarga = std::max(0.0, tiempoRecarga - dt);
}

bool Jerry::puedeSalir() const
{
    return quesos >= 6 && tieneLlave && estaVivo() && !estaHerido();
}

void Jerry::recibirDaño(int cantidad)
{
    if (proteccion > 0 || estaHerido() || !estaVivo() || cantidad <= 0)
        return;
    Personaje::recibirDaño(cantidad);
    tiempoHerida = 0.9;
    establecerVelocidad({});
}

void Jerry::perderVida() { recibirDaño(1); }
void Jerry::restablecerVidas()
{
    establecerVidas(3);
    tiempoHerida = 0;
}
void Jerry::activarProteccion(double segundos) { proteccion = std::max(proteccion, std::max(0.0, segundos)); }
void Jerry::activarSaltoMejorado(double segundos) { tiempoSaltoMejorado = segundos; }
void Jerry::establecerDireccion(int valor) { direccion = std::clamp(valor, -1, 1); }

void Jerry::reaparecer(Vector2 punto)
{
    establecerPosicion(punto);
    establecerVelocidad({});
    establecerEnSuelo(true);
    tiempoHerida = 0;
    proteccion = 1.5;
}

void Jerry::sumarQueso(int cantidad) { quesos += cantidad; }
void Jerry::sumarMoneda(int valor) { ++monedas; puntos += valor; }
void Jerry::establecerLlave(bool valor) { tieneLlave = valor; }
void Jerry::establecerFlorero(bool valor) { tieneFlorero = valor; }
void Jerry::establecerAgachado(bool valor) { agachado = valor; }
bool Jerry::estaAgachado() const { return agachado; }
void Jerry::establecerCarreraTienda(bool valor) { carreraTienda = valor; }
QRectF Jerry::rectanguloColision() const
{
    if (!agachado)
        return Entidad::rectanguloColision();
    const Vector2 posicion = obtenerPosicion();
    return QRectF(posicion.x - 15, posicion.y - 30, 30, 30);
}
int Jerry::obtenerQuesos() const { return quesos; }
int Jerry::obtenerMonedas() const { return monedas; }
int Jerry::obtenerPuntos() const { return puntos; }
bool Jerry::tieneLaLlave() const { return tieneLlave; }
bool Jerry::tieneElFlorero() const { return tieneFlorero; }
double Jerry::obtenerTiempoAgilidad() const { return tiempoAgilidad; }
double Jerry::obtenerTiempoRecarga() const { return tiempoRecarga; }
double Jerry::obtenerTiempoSaltoMejorado() const { return tiempoSaltoMejorado; }
double Jerry::obtenerProteccion() const { return proteccion; }
double Jerry::obtenerTiempoHerida() const { return tiempoHerida; }
bool Jerry::estaHerido() const { return tiempoHerida > 0; }

Tom::Tom() : Personaje({30, Configuracion::sueloY}, 46, 82, 1, 150) {}

void Tom::actualizar(double dt)
{
    tiempoPreparacion = std::max(0.0, tiempoPreparacion - dt);
    tiempoRetirada = std::max(0.0, tiempoRetirada - dt);
}

void Tom::actualizarIA(double dt, Jerry &jugador)
{
    actualizar(dt);
    agente.actualizar(*this, jugador, dt);
}

void Tom::perseguir(const Vector2 &objetivo, double dt)
{
    const double diferencia = objetivo.x - obtenerPosicion().x;
    const double distancia = std::abs(diferencia);
    if (tiempoPreparacion > 0) {
        if (distancia > 120)
            moverEnCalle(objetivo.x - std::copysign(120.0, diferencia), 180, dt);
        else
            establecerVelocidad({});
        return;
    }
    const double recuperacion = std::clamp((distancia - 320) * 0.15, 0.0, 50.0);
    moverEnCalle(objetivo.x, velocidadPersecucion + recuperacion, dt);
}

void Tom::patrullar(double dt)
{
    const Vector2 posicion = obtenerPosicion();
    if (posicion.x > centroPatrulla + 170)
        direccionPatrulla = -1;
    if (posicion.x < centroPatrulla - 170)
        direccionPatrulla = 1;
    moverEnCalle(posicion.x + direccionPatrulla * 100, obtenerVelocidadBase(), dt);
}

void Tom::interceptar(int zona, double dt)
{
    if (nivel)
        moverEnCalle((std::clamp(zona, 0, 3) + 0.5) * nivel->obtenerAncho() / 4,
                     velocidadPersecucion * 0.9, dt);
}

void Tom::atacar(Jerry &jugador)
{
    if (tiempoPreparacion == 0 && colisiona(jugador))
        jugador.recibirDaño(dañoContacto);
}

void Tom::moverEnCalle(double destino, double rapidez, double dt)
{
    if (!nivel || dt <= 0)
        return;
    Vector2 posicion = obtenerPosicion();
    const Vector2 anterior = posicion;
    const int direccion = destino >= posicion.x ? 1 : -1;
    const QVector<QRectF> solidos = nivel->obtenerSolidos();
    double destinoPies = Configuracion::sueloY;
    for (const QRectF &solido : solidos) {
        const bool cerca = direccion > 0
                               ? posicion.x > solido.left() - 165 && posicion.x < solido.right() + 58
                               : posicion.x < solido.right() + 165 && posicion.x > solido.left() - 58;
        if (cerca)
            destinoPies = std::min(destinoPies, solido.top() - 7);
    }
    posicion.y += std::clamp(destinoPies - posicion.y, -320 * dt, 320 * dt);
    if (destinoPies < Configuracion::sueloY)
        rapidez = std::min(rapidez, velocidadPersecucion * 0.9);
    const double desplazamiento = std::clamp(destino - posicion.x, -rapidez * dt, rapidez * dt);
    posicion.x = std::clamp(posicion.x + desplazamiento, 30.0, nivel->obtenerAncho() - 30);
    establecerPosicion(posicion);
    for (const QRectF &solido : solidos) {
        if (rectanguloColision().intersects(solido)) {
            posicion.x = anterior.x < solido.center().x() ? solido.left() - 23 : solido.right() + 23;
            establecerPosicion(posicion);
        }
    }
    if (posicion.x <= 30 || posicion.x >= nivel->obtenerAncho() - 30) {
        direccionPatrulla = -direccion;
        centroPatrulla = posicion.x + direccionPatrulla * 95;
    }
    const Vector2 velocidad{(posicion.x - anterior.x) / dt, (posicion.y - anterior.y) / dt};
    establecerVelocidad(velocidad);
    if (std::abs(velocidad.x) > 0.1)
        miraDerecha = velocidad.x > 0;
}

void Tom::establecerNivel(const Nivel *valor) { nivel = valor; agente.establecerNivel(valor); }

void Tom::reiniciar(bool nuevaPartida)
{
    establecerPosicion({30, Configuracion::sueloY});
    establecerVelocidad({});
    centroPatrulla = 100;
    direccionPatrulla = 1;
    tiempoPreparacion = 3;
    tiempoRetirada = 0;
    miraDerecha = true;
    agente.reiniciar(nuevaPartida);
}

void Tom::retirarse(double segundos) { tiempoRetirada = segundos; }
bool Tom::estaRetirandose() const { return tiempoRetirada > 0; }
const AgenteTom &Tom::obtenerAgente() const { return agente; }

Llorente::Llorente() : Personaje({}, 40, 90, 1, 0) {}
void Llorente::actualizar(double) {}
void Llorente::interactuar(Jerry &jugador)
{
    if (jugador.tieneElFlorero())
        recibirFlorero(jugador);
    else
        negarFlorero();
}
void Llorente::negarFlorero() { enojarse(); }
void Llorente::entregarFlorero(Jerry &jugador) { jugador.establecerFlorero(true); }
void Llorente::recibirFlorero(Jerry &jugador)
{
    floreroEntregado = jugador.tieneElFlorero();
    jugador.establecerFlorero(false);
}
void Llorente::enojarse() { enojado = true; }