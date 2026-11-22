#include "juego.h"
#include "logica/niveles.h"
#include "logica/configuracion.h"
#include <algorithm>
#include <cmath>

Juego::Juego() : jugador(std::make_unique<Jerry>())
{
    niveles.push_back(std::make_unique<NivelCalles>());
    niveles.push_back(std::make_unique<NivelTienda>());
    niveles.push_back(std::make_unique<NivelCabildo>());
    obtenerNivel().cargar();
}

void Juego::iniciar() { nuevaPartida(); }
void Juego::reiniciar() { prepararIntento(false); }
void Juego::nuevaPartida() { partidaGuardada.reset(); prepararIntento(true); }

void Juego::prepararIntento(bool nuevaPartida)
{
    nivelActual = 0;
    *jugador = Jerry();
    obtenerNivel().reiniciar();
    if (nuevaPartida)
        obtenerNivel().reiniciarMemoria();
    estado = EstadoJuego::Jugando;
    pausado = false;
    camaraX = 0;
}

void Juego::actualizar(double dt)
{
    if (estado != EstadoJuego::Jugando || dt <= 0)
        return;
    obtenerNivel().actualizar(dt, *jugador);
    if (obtenerNivel().verificarDerrota(*jugador))
        estado = EstadoJuego::Derrota;
    actualizarCamara();
}

void Juego::actualizarCamara()
{
    camaraX = std::clamp(jugador->obtenerPosicion().x - Configuracion::anchoVista * 0.42,
                         0.0, obtenerNivel().obtenerAncho() - Configuracion::anchoVista);
}

void Juego::cambiarNivel()
{
    const int siguiente = nivelActual + 1;
    if (siguiente >= int(niveles.size()) || !niveles[size_t(siguiente)]->disponible()) {
        estado = EstadoJuego::Victoria;
        return;
    }
    nivelActual = siguiente;
    obtenerNivel().cargar();
}

void Juego::pasarNivel()
{
    obtenerNivel().completar();
    cambiarNivel();
}

void Juego::intentarEntrar()
{
    if (estado != EstadoJuego::Jugando || jugador->estaHerido())
        return;
    if (obtenerNivel().verificarVictoria(*jugador)) {
        pasarNivel();
        jugador->establecerDireccion(0);
    } else if (std::abs(jugador->obtenerPosicion().x - obtenerNivel().obtenerPuertaX()) <= 100) {
        obtenerNivel().mostrarMensaje(QString("Para entrar necesitas 6 quesos (%1/6) y la llave (%2).")
                                          .arg(jugador->obtenerQuesos()).arg(jugador->tieneLaLlave() ? "sí" : "no"));
    }
}

void Juego::pausar()
{
    if (estado == EstadoJuego::Jugando || estado == EstadoJuego::Pausa) {
        pausado = !pausado;
        estado = pausado ? EstadoJuego::Pausa : EstadoJuego::Jugando;
        jugador->establecerDireccion(0);
    }
}

void Juego::guardar()
{
    if (estado != EstadoJuego::Jugando && estado != EstadoJuego::Pausa)
        return;
    partidaGuardada = std::make_unique<PartidaGuardada>();
    partidaGuardada->jugador = *jugador;
    partidaGuardada->tom = obtenerNivel().obtenerTom();
    partidaGuardada->tiempo = obtenerNivel().obtenerTiempo();
    partidaGuardada->puntoSeguro = obtenerNivel().obtenerPuntoSeguro();
    for (const auto &objeto : obtenerNivel().obtenerObjetos())
        partidaGuardada->objetosActivos.push_back(objeto->estaActivo());
}

void Juego::cargar()
{
    if (!partidaGuardada)
        return;
    nivelActual = 0;
    obtenerNivel().reiniciar();
    *jugador = partidaGuardada->jugador;
    jugador->establecerDireccion(0);
    obtenerNivel().obtenerTom() = partidaGuardada->tom;
    obtenerNivel().obtenerTom().establecerNivel(&obtenerNivel());
    obtenerNivel().restaurarEscenario(partidaGuardada->tiempo, partidaGuardada->puntoSeguro,
                                      partidaGuardada->objetosActivos);
    estado = EstadoJuego::Jugando;
    pausado = false;
    actualizarCamara();
}

void Juego::establecerDireccion(int direccion) { jugador->establecerDireccion(direccion); }
void Juego::saltar() { if (estado == EstadoJuego::Jugando) jugador->saltar(obtenerNivel().obtenerFisica()); }
void Juego::correr() { if (estado == EstadoJuego::Jugando) jugador->activarAgilidad(); }
EstadoJuego Juego::obtenerEstado() const { return estado; }
Jerry &Juego::obtenerJugador() { return *jugador; }
const Jerry &Juego::obtenerJugador() const { return *jugador; }
Nivel &Juego::obtenerNivel() { return *niveles[size_t(nivelActual)]; }
const Nivel &Juego::obtenerNivel() const { return *niveles[size_t(nivelActual)]; }
double Juego::obtenerCamaraX() const { return camaraX; }