#include "ia.h"
#include "../logica/personajes.h"
#include "../logica/niveles.h"
#include "../logica/configuracion.h"

#include <algorithm>
#include <cmath>

using namespace std;

void AprendizajeTom::registrarVisita(int zona)
{
    if (zona >= 0 && zona < int(visitasPorZona.size()))
        ++visitasPorZona[size_t(zona)];
}

int AprendizajeTom::zonaMasFrecuente() const
{
    const auto mayor = max_element(visitasPorZona.begin(), visitasPorZona.end());
    return *mayor == 0 ? -1 : int(distance(visitasPorZona.begin(), mayor));
}

void AprendizajeTom::reiniciar()
{
    visitasPorZona.fill(0);
}

const array<int, 6> &AprendizajeTom::obtenerVisitas() const
{
    return visitasPorZona;
}

Observacion PercepcionTom::percibir(const Tom &tom, const Jerry &jugador) const
{
    Observacion resultado;
    const Vector2 posicionTom = tom.obtenerPosicion();

    if (const auto *cabildo = dynamic_cast<const NivelCabildo *>(nivel)) {
        resultado.distancia = posicionTom.distancia(jugador.obtenerPosicion());
        resultado.visible = jugador.estaVivo()
                            && resultado.distancia < configuracionTom(tom.obtenerDificultad()).radioVision
                            && cabildo->hayLineaVision(posicionTom, jugador.obtenerPosicion());
        if (resultado.visible) {
            resultado.posicionJerry = jugador.obtenerPosicion();
            resultado.velocidadJerry = jugador.obtenerVelocidad();
            resultado.zonaJerry = cabildo->zonaEnPlaza(resultado.posicionJerry);
        }
        return resultado;
    }

    resultado.distancia = abs(jugador.obtenerPosicion().x - posicionTom.x);
    resultado.visible = resultado.distancia < configuracionTom(tom.obtenerDificultad()).visionCalle
                        && !jugador.estaHerido() && jugador.estaVivo();

    if (nivel) {
        for (const QRectF &caja : nivel->obtenerPlataformas()) {
            if (posicionTom.y <= caja.top() || jugador.obtenerPosicion().y <= caja.top())
                continue;

            if ((posicionTom.x < caja.left() && jugador.obtenerPosicion().x > caja.right())
                || (jugador.obtenerPosicion().x < caja.left() && posicionTom.x > caja.right()))
                resultado.visible = false;
        }
    }

    if (resultado.visible) {
        resultado.posicionJerry = jugador.obtenerPosicion();
        resultado.velocidadJerry = jugador.obtenerVelocidad();
        const double ancho = nivel ? nivel->obtenerAncho() : Configuracion::anchoMundo;
        resultado.zonaJerry = clamp(int(resultado.posicionJerry.x * 6 / ancho), 0, 5);
    }

    return resultado;
}

void PercepcionTom::establecerNivel(const Nivel *valor)
{
    nivel = valor;
}

TipoDecision RazonamientoTom::decidir(const Observacion &observacion, const AprendizajeTom &memoria,
                                     Dificultad dificultad) const
{
    if (observacion.visible) {
        if (observacion.distancia < distanciaAtaque)
            return TipoDecision::ATACAR;
        if (dificultad == Dificultad::Dificil
            && hypot(observacion.velocidadJerry.x, observacion.velocidadJerry.y) > 1)
            return TipoDecision::INTERCEPTAR;
        return TipoDecision::PERSEGUIR;
    }

    return memoria.zonaMasFrecuente() >= 0 ? TipoDecision::INTERCEPTAR : TipoDecision::PATRULLAR;
}

void AccionTom::ejecutar(TipoDecision decision, const Observacion &observacion,
                         int zonaPreferida, Tom &tom, double dt)
{
    if (!nivel)
        return;

    if (tom.estaRetirandose()) {
        tom.establecerVelocidad({});
        return;
    }

    if (observacion.visible) {
        ultimaPosicionVista = observacion.posicionJerry.x;
        tiempoBusqueda = configuracionTom(tom.obtenerDificultad()).tiempoBusqueda;
    }

    switch (decision) {
    case TipoDecision::PERSEGUIR:
    case TipoDecision::ATACAR:
        if (observacion.visible) {
            Vector2 objetivo = observacion.posicionJerry;
            objetivo.x += observacion.velocidadJerry.x
                          * configuracionTom(tom.obtenerDificultad()).anticipacion;
            tom.perseguir(objetivo, dt);
        }
        break;

    case TipoDecision::INTERCEPTAR:
        if (observacion.visible) {
            tom.perseguir({observacion.posicionJerry.x + observacion.velocidadJerry.x
                              * configuracionTom(tom.obtenerDificultad()).anticipacion,
                           observacion.posicionJerry.y}, dt);
            break;
        }
        if (tiempoBusqueda > 0) {
            tiempoBusqueda = max(0.0, tiempoBusqueda - dt);

            if (abs(ultimaPosicionVista - tom.obtenerPosicion().x) >= 12) {
                tom.perseguir({ultimaPosicionVista, Configuracion::sueloY}, dt);
                break;
            }
        }

        if (zonaPreferida >= 0)
            tom.interceptar(zonaPreferida, dt);
        else
            tom.patrullar(dt);
        break;

    case TipoDecision::PATRULLAR:
        tom.patrullar(dt);
        break;
    }
}

void AccionTom::ejecutarAtaque(Tom &tom, Jerry &jugador)
{
    if (!tom.estaRetirandose() && tom.obtenerPosicion().y > Configuracion::sueloY - 18
        && tom.colisiona(jugador))
        tom.atacar(jugador);
}

void AccionTom::establecerNivel(const Nivel *valor)
{
    nivel = valor;
}

void AccionTom::reiniciar()
{
    ultimaPosicionVista = 180;
    tiempoBusqueda = 0;
}

void AgenteTom::actualizar(Tom &tom, Jerry &jugador, double dt)
{
    const Observacion observacion = percepcion.percibir(tom, jugador);
    observarYDecidir(observacion, tom.obtenerDificultad(), dt);

    accion.ejecutar(decision, observacion, aprendizaje.zonaMasFrecuente(), tom, dt);
    accion.ejecutarAtaque(tom, jugador);
}

void AgenteTom::registrarNuevaZona(const Observacion &observacion)
{
    if (observacion.visible && observacion.zonaJerry != ultimaZonaVisible) {
        aprendizaje.registrarVisita(observacion.zonaJerry);
        ultimaZonaVisible = observacion.zonaJerry;
    }
}

bool AgenteTom::observarYDecidir(const Observacion &observacion, Dificultad dificultad, double dt)
{
    registrarNuevaZona(observacion);
    esperaDecision -= dt;
    if (esperaDecision > 0.000001)
        return false;
    decision = razonamiento.decidir(observacion, aprendizaje, dificultad);
    esperaDecision = configuracionTom(dificultad).intervaloDecision;
    return true;
}

void AgenteTom::establecerNivel(const Nivel *nivel)
{
    percepcion.establecerNivel(nivel);
    accion.establecerNivel(nivel);
}

void AgenteTom::reiniciar(bool nuevaPartida)
{
    if (nuevaPartida)
        aprendizaje.reiniciar();

    ultimaZonaVisible = -1;
    decision = TipoDecision::PATRULLAR;
    esperaDecision = 0;
    accion.reiniciar();
}

const AprendizajeTom &AgenteTom::obtenerAprendizaje() const
{
    return aprendizaje;
}

TipoDecision AgenteTom::obtenerDecision() const
{
    return decision;
}
