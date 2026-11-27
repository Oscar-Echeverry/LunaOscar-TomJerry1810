#include "juego.h"
#include "logica/niveles.h"
#include "logica/configuracion.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

Juego::Juego() : jugador(std::make_unique<Jerry>())
{
    niveles.push_back(std::make_unique<NivelCalles>());
    niveles.push_back(std::make_unique<NivelTienda>());
    niveles.push_back(std::make_unique<NivelCabildo>());
    obtenerNivel().cargar();
}

void Juego::iniciar() { nuevaPartida(); }
void Juego::mostrarSeleccionNivel()
{
    if (estado == EstadoJuego::Portada)
        estado = EstadoJuego::Presentacion;
}
void Juego::volverAPortada()
{
    if (estado == EstadoJuego::Presentacion)
        estado = EstadoJuego::Portada;
}
void Juego::iniciarNivel(int numero)
{
    if (numero < 1 || numero > int(niveles.size()))
        throw std::invalid_argument("Selecciona un nivel entre 1 y 3.");
    nuevaPartida();
    while (obtenerNivel().obtenerNumero() < numero)
        cambiarNivel();
    actualizarCamara();
}
void Juego::reiniciar()
{
    if (nivelActual == 1 && inicioNivelTienda) {
        obtenerNivel().reiniciar();
        *jugador = *inicioNivelTienda;
        jugador->establecerPosicion({120, Configuracion::sueloY});
        jugador->establecerVelocidad({});
        jugador->establecerDireccion(0);
        jugador->establecerFlorero(false);
        jugador->restablecerVidas();
        estado = EstadoJuego::Jugando;
        pausado = false;
        camaraX = 0;
        return;
    }
    if (nivelActual == 2) {
        obtenerNivel().reiniciar();
        *jugador = inicioNivelCabildo ? *inicioNivelCabildo : Jerry();
        jugador->establecerPosicion(static_cast<NivelCabildo &>(obtenerNivel()).obtenerInicio());
        jugador->establecerEnSuelo(true);
        obtenerJugador().restablecerVidas();
        estado = EstadoJuego::Jugando;
        pausado = false;
        camaraX = 0;
        return;
    }
    prepararIntento(false);
}
void Juego::nuevaPartida() { partidaGuardada.reset(); prepararIntento(true); }

void Juego::prepararIntento(bool nuevaPartida)
{
    sonidosPendientes.clear();
    nivelActual = 0;
    inicioNivelTienda.reset();
    inicioNivelCabildo.reset();
    *jugador = Jerry();
    obtenerNivel().reiniciar();
    if (nuevaPartida)
        for (auto &nivel : niveles) {
            nivel->establecerDificultad(Dificultad::Normal);
            nivel->reiniciarMemoria();
        }
    estado = EstadoJuego::Jugando;
    pausado = false;
    camaraX = 0;
}

void Juego::actualizar(double dt)
{
    if (!std::isfinite(dt) || dt < 0)
        throw std::invalid_argument("El tiempo de actualización debe ser finito y no negativo.");
    if (estado != EstadoJuego::Jugando || dt <= 0)
        return;
    const int vidas = jugador->obtenerVidas();
    const int quesos = jugador->obtenerQuesos();
    const int monedas = jugador->obtenerMonedas();
    const bool llave = jugador->tieneLaLlave();
    const int plaza = obtenerNivel().obtenerNumero() == 3
                          ? static_cast<NivelCabildo &>(obtenerNivel()).obtenerPantalla() : -1;
    obtenerNivel().actualizar(dt, *jugador);
    if (jugador->obtenerVidas() < vidas)
        sonidosPendientes.push_back(EventoSonido::Dano);
    if (jugador->obtenerQuesos() > quesos)
        sonidosPendientes.push_back(EventoSonido::Queso);
    if (jugador->obtenerMonedas() > monedas)
        sonidosPendientes.push_back(EventoSonido::Moneda);
    if (!llave && jugador->tieneLaLlave())
        sonidosPendientes.push_back(EventoSonido::Recoger);
    if (plaza >= 0 && static_cast<NivelCabildo &>(obtenerNivel()).obtenerPantalla() != plaza)
        sonidosPendientes.push_back(EventoSonido::Avanzar);
    if (obtenerNivel().verificarDerrota(*jugador))
        estado = EstadoJuego::Derrota;
    actualizarCamara();
}

void Juego::actualizarCamara()
{
    if (const auto *tienda = dynamic_cast<const NivelTienda *>(&obtenerNivel())) {
        if (tienda->enConversacion()) {
            camaraX = std::clamp(tienda->obtenerPuertaX() - Configuracion::anchoVista * 0.55,
                                0.0, tienda->obtenerAncho() - Configuracion::anchoVista);
            return;
        }
    }
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
    if (obtenerNivel().obtenerNumero() == 2) {
        jugador->establecerPosicion({120, Configuracion::sueloY});
        jugador->establecerVelocidad({});
        jugador->restablecerVidas();
        jugador->establecerFlorero(false);
        jugador->establecerDireccion(0);
        camaraX = 0;
        inicioNivelTienda = std::make_unique<Jerry>(*jugador);
    } else if (obtenerNivel().obtenerNumero() == 3) {
        jugador->desactivarProteccion();
        jugador->establecerPosicion(static_cast<NivelCabildo &>(obtenerNivel()).obtenerInicio());
        jugador->establecerVelocidad({});
        jugador->establecerDireccion(0);
        jugador->establecerFlorero(false);
        jugador->establecerAgachado(false);
        jugador->establecerCarreraTienda(false);
        jugador->establecerEnSuelo(true);
        camaraX = 0;
        inicioNivelCabildo = std::make_unique<Jerry>(*jugador);
        estado = EstadoJuego::SeleccionDificultad;
        pausado = true;
    }
}

void Juego::pasarNivel()
{
    obtenerNivel().completar();
    cambiarNivel();
    sonidosPendientes.push_back(estado == EstadoJuego::Victoria
                                    ? EventoSonido::Victoria : EventoSonido::Avanzar);
}

void Juego::intentarEntrar()
{
    if (estado != EstadoJuego::Jugando
        || (jugador->estaHerido() && obtenerNivel().obtenerNumero() != 3))
        return;
    if (obtenerNivel().obtenerNumero() == 2) {
        auto &tienda = static_cast<NivelTienda &>(obtenerNivel());
        if (tienda.enConversacion()) {
            tienda.avanzarDialogo();
            actualizarCamara();
            return;
        }
        if (tienda.entregarOrero(*jugador)) {
            tienda.completar();
            jugador->establecerDireccion(0);
            estado = EstadoJuego::Invitacion;
            sonidosPendientes.push_back(EventoSonido::Recoger);
        } else if (jugador->tieneElFlorero()) {
            obtenerNivel().mostrarMensaje("Acércate al mostrador, a la derecha, y pulsa E para entregar el florero.");
        }
        return;
    }
    if (obtenerNivel().obtenerNumero() == 3) {
        auto &cabildo = static_cast<NivelCabildo &>(obtenerNivel());
        const int empujes = cabildo.obtenerEmpujes();
        if (cabildo.interactuar(*jugador))
            pasarNivel();
        else if (cabildo.obtenerEmpujes() > empujes)
            sonidosPendientes.push_back(EventoSonido::Empujar);
        return;
    }
    if (obtenerNivel().verificarVictoria(*jugador)) {
        pasarNivel();
        jugador->establecerDireccion(0);
    } else if (std::abs(jugador->obtenerPosicion().x - obtenerNivel().obtenerPuertaX()) <= 100) {
        obtenerNivel().mostrarMensaje(QString("Para entrar necesitas 6 quesos (%1/6) y la llave (%2).")
                                          .arg(jugador->obtenerQuesos()).arg(jugador->tieneLaLlave() ? "sí" : "no"));
    }
}

void Juego::continuarAlCabildo()
{
    if (estado != EstadoJuego::Invitacion)
        return;
    estado = EstadoJuego::Jugando;
    pasarNivel();
    obtenerNivel().mostrarMensaje("¡Estás invitado al cabildo! Lleva el mensaje y abre el paso empujando las cajas con E.");
}

void Juego::presionarEspacio()
{
    if (estado != EstadoJuego::Jugando)
        return;
    if (obtenerNivel().obtenerNumero() == 2)
    {
        const bool teniaFlorero = jugador->tieneElFlorero();
        static_cast<NivelTienda &>(obtenerNivel()).intentarAtrapar(*jugador);
        if (!teniaFlorero && jugador->tieneElFlorero())
            sonidosPendientes.push_back(EventoSonido::Recoger);
    }
    else if (obtenerNivel().obtenerNumero() != 3)
        saltar();
}

void Juego::activarBurbuja()
{
    if (estado != EstadoJuego::Jugando || !jugador->estaVivo()
        || obtenerNivel().obtenerNumero() != 2)
        return;
    jugador->activarProteccion(5);
    obtenerNivel().mostrarMensaje("Burbuja de seguridad activa durante 5 segundos.");
}

void Juego::pausar()
{
    if (estado == EstadoJuego::SeleccionDificultad) {
        pausado = false;
        estado = EstadoJuego::Jugando;
        return;
    }
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

void Juego::establecerDireccion(int direccion, int vertical)
{
    if (const auto *tienda = dynamic_cast<const NivelTienda *>(&obtenerNivel()))
        if (tienda->enConversacion()) {
            jugador->establecerDireccion(0);
            return;
        }
    if (obtenerNivel().obtenerNumero() == 3)
        static_cast<NivelCabildo &>(obtenerNivel()).establecerDireccion(direccion, vertical);
    else
        jugador->establecerDireccion(direccion);
}
void Juego::saltar()
{
    if (estado != EstadoJuego::Jugando || obtenerNivel().obtenerNumero() != 1)
        return;
    const bool enSuelo = jugador->estaEnSuelo();
    jugador->saltar(obtenerNivel().obtenerFisica());
    if (enSuelo && !jugador->estaEnSuelo())
        sonidosPendientes.push_back(EventoSonido::Salto);
}
void Juego::correr()
{
    if (estado == EstadoJuego::Jugando && obtenerNivel().obtenerNumero() == 1)
        jugador->activarAgilidad();
}
EstadoJuego Juego::obtenerEstado() const { return estado; }
Jerry &Juego::obtenerJugador() { return *jugador; }
const Jerry &Juego::obtenerJugador() const { return *jugador; }
Nivel &Juego::obtenerNivel() { return *niveles[size_t(nivelActual)]; }
const Nivel &Juego::obtenerNivel() const { return *niveles[size_t(nivelActual)]; }
double Juego::obtenerCamaraX() const { return camaraX; }

void Juego::establecerDificultad(Dificultad valor)
{
    configuracionTom(valor);
    if (obtenerNivel().obtenerNumero() == 3)
        obtenerNivel().establecerDificultad(valor);
}

Dificultad Juego::obtenerDificultad() const
{
    return obtenerNivel().obtenerDificultad();
}

vector<EventoSonido> Juego::consumirSonidos()
{
    return std::exchange(sonidosPendientes, {});
}
