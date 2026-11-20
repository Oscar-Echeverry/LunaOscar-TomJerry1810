#ifndef IA_H
#define IA_H
#include "../logica/vector2.h"
#include <array>

using namespace std;

class Tom;
class Jerry;
class Nivel;
class AprendizajeTom;

struct Observacion {
    bool visible = false;
    double distancia = 0;
    Vector2 posicionJerry;
    int zonaJerry = -1;
};

enum class TipoDecision { PATRULLAR, PERSEGUIR, INTERCEPTAR, ATACAR };

class AprendizajeTom
{
public:
    void registrarVisita(int zona);
    int zonaMasFrecuente() const;
    void reiniciar();
    const array<int, 4> &obtenerVisitas() const;

private:
    array<int, 4> visitasPorZona{};
};

class PercepcionTom
{
public:
    Observacion percibir(const Tom &tom, const Jerry &jugador) const;
    void establecerNivel(const Nivel *valor);

private:
    double radioVision = 1120;
    const Nivel *nivel = nullptr;
};

class RazonamientoTom
{
public:
    TipoDecision decidir(const Observacion &observacion, const AprendizajeTom &memoria) const;

private:
    double distanciaAtaque = 32;
};

class AccionTom
{
public:
    void ejecutar(TipoDecision decision, const Observacion &observacion,
                  int zonaPreferida, Tom &tom, double dt);
    void ejecutarAtaque(Tom &tom, Jerry &jugador);
    void establecerNivel(const Nivel *valor);
    void reiniciar();

private:
    const Nivel *nivel = nullptr;
    double ultimaPosicionVista = 180;
    double tiempoBusqueda = 0;
};

class AgenteTom
{
public:
    void actualizar(Tom &tom, Jerry &jugador, double dt);
    void registrarNuevaZona(const Observacion &observacion);
    void establecerNivel(const Nivel *nivel);
    void reiniciar(bool nuevaPartida);
    const AprendizajeTom &obtenerAprendizaje() const;
    TipoDecision obtenerDecision() const;

private:
    PercepcionTom percepcion;
    RazonamientoTom razonamiento;
    AccionTom accion;
    AprendizajeTom aprendizaje;
    int ultimaZonaVisible = -1;
    TipoDecision decision = TipoDecision::PATRULLAR;
};
#endif