#ifndef PERSONAJES_H
#define PERSONAJES_H
#include "entidad.h"
#include "../ia/ia.h"
#include "dificultad.h"

class FisicaJuego;
class Objeto;
class Jerry;
class Nivel;

class Personaje : public Entidad
{
public:
    Personaje(Vector2 posicion, double ancho, double alto, int vidas, double velocidadBase);
    void mover(const Vector2 &direccion, double dt);
    virtual void recibirDaño(int cantidad);
    bool estaVivo() const;
    bool estaEnSuelo() const;
    void establecerEnSuelo(bool valor);
    int obtenerVidas() const;
    bool miraHaciaDerecha() const;

protected:
    double obtenerVelocidadBase() const;
    void establecerVidas(int cantidad);
    bool miraDerecha = true;

private:
    int vidas;
    double velocidadBase;
    bool enSuelo = true;
};

class Jerry : public Personaje
{
public:
    Jerry();
    void actualizar(double dt) override;
    void saltar(const FisicaJuego &fisica);
    void recoger(Objeto &objeto);
    void activarAgilidad();
    void actualizarEfectos(double dt);
    bool puedeSalir() const;
    void recibirDaño(int cantidad) override;
    void perderVida();
    void activarProteccion(double segundos);
    void desactivarProteccion();
    void restablecerVidas();
    void activarSaltoMejorado(double segundos);
    void establecerDireccion(int valor);
    void reaparecer(Vector2 punto);
    void sumarQueso(int cantidad);
    void sumarMoneda(int valor);
    void sumarPuntos(int cantidad);
    void establecerLlave(bool valor);
    void establecerFlorero(bool valor);
    void establecerAgachado(bool valor);
    bool estaAgachado() const;
    void establecerCarreraTienda(bool valor);
    QRectF rectanguloColision() const override;
    int obtenerQuesos() const;
    int obtenerMonedas() const;
    int obtenerPuntos() const;
    bool tieneLaLlave() const;
    bool tieneElFlorero() const;
    double obtenerTiempoAgilidad() const;
    double obtenerTiempoSaltoMejorado() const;
    double obtenerProteccion() const;
    double obtenerTiempoHerida() const;
    bool estaHerido() const;

private:
    int quesos = 0;
    int monedas = 0;
    int puntos = 0;
    int direccion = 0;
    bool tieneLlave = false;
    bool tieneFlorero = false;
    bool agachado = false;
    bool carreraTienda = false;
    double impulsoSalto = -420;
    double tiempoAgilidad = 0;
    double tiempoSaltoMejorado = 0;
    double proteccion = 0;
    double tiempoHerida = 0;
};

class Tom : public Personaje
{
public:
    Tom();
    void actualizar(double dt) override;
    void actualizarIA(double dt, Jerry &jugador);
    void perseguir(const Vector2 &objetivo, double dt);
    void patrullar(double dt);
    void interceptar(int zona, double dt);
    void atacar(Jerry &jugador);
    void moverEnCalle(double destino, double rapidez, double dt);
    void establecerNivel(const Nivel *valor);
    void reiniciar(bool nuevaPartida);
    void retirarse(double segundos);
    bool estaRetirandose() const;
    const AgenteTom &obtenerAgente() const;
    void establecerDificultad(Dificultad valor);
    Dificultad obtenerDificultad() const;
private:
    double velocidadPersecucion = 250;
    int dañoContacto = 1;
    AgenteTom agente;
    const Nivel *nivel = nullptr;
    double centroPatrulla = 100;
    int direccionPatrulla = 1;
    double tiempoPreparacion = 3;
    double tiempoRetirada = 0;
    Dificultad dificultad = Dificultad::Normal;
};

class Llorente : public Personaje
{
public:
    Llorente();
    void actualizar(double dt) override;
    void interactuar(Jerry &jugador);
    void negarFlorero();
    void entregarFlorero(Jerry &jugador);
    void recibirFlorero(Jerry &jugador);
    void enojarse();
private:
    bool enojado = false;
    bool floreroEntregado = false;
};
#endif