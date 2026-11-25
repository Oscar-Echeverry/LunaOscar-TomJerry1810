#ifndef NIVELES_H
#define NIVELES_H
#include "fisicajuego.h"
#include "objetos.h"
#include "personajes.h"
#include "temporizador.h"
#include <QVector>
#include <QPointF>
#include <QString>
#include <memory>
#include <vector>

using namespace std;

struct Habitante {
    QString animacion;
    double centroX;
    double x;
    double piesY;
    double altura;
    double recorrido;
    double velocidad;
    int direccion = 1;
};

class Nivel
{
public:
    explicit Nivel(int numero);
    virtual ~Nivel() = default;
    virtual void cargar() = 0;
    virtual void actualizar(double dt, Jerry &jugador) = 0;
    virtual bool verificarVictoria(const Jerry &jugador) const = 0;
    virtual bool disponible() const;
    virtual bool verificarDerrota(const Jerry &jugador) const;
    bool perderPorColisiones(Jerry &jugador);
    void comprobarColisiones(Jerry &jugador);
    void reiniciar();
    void reiniciarMemoria();
    void mostrarMensaje(const QString &texto);
    const vector<unique_ptr<Objeto>> &obtenerObjetos() const;
    const vector<unique_ptr<Obstaculo>> &obtenerObstaculos() const;
    const QVector<QRectF> &obtenerPlataformas() const;
    const QVector<Habitante> &obtenerHabitantes() const;
    QVector<QRectF> obtenerSolidos() const;
    Tom &obtenerTom();
    const Tom &obtenerTom() const;
    const FisicaJuego &obtenerFisica() const;
    double obtenerAncho() const;
    double obtenerPuertaX() const;
    double obtenerTiempo() const;
    double obtenerPuntoSeguro() const;
    const QString &obtenerMensaje() const;
    bool hayMensaje() const;
    int obtenerNumero() const;
    bool estaCompletado() const;
    void completar();
    void restaurarEscenario(double tiempo, double puntoSeguro, const vector<bool> &activos);

protected:
    void prepararCarga();
    void agregarObjeto(unique_ptr<Objeto> objeto);
    void agregarObstaculo(unique_ptr<Obstaculo> obstaculo);
    void resolverLaterales(Jerry &jugador);
    void actualizarHabitantes(double dt);
    void registrardaño(int vidasAnteriores, Jerry &jugador);
    QVector<QRectF> plataformas;
    QVector<Habitante> habitantes;
    QVector<double> puntosSeguros;
    double anchoMundo = 5035;
    double puertaX = 4800.01;
    double tiempo = 0;
    double puntoSeguroX = 180;
    double piesAnteriores = 568;
    double tiempoMensaje = 0;
    QString mensaje;

private:
    int numero;
    bool completado = false;
    vector<unique_ptr<Objeto>> objetos;
    vector<unique_ptr<Obstaculo>> obstaculos;
    unique_ptr<Tom> tom;
    FisicaJuego fisica;
};

class NivelCalles : public Nivel
{
public:
    NivelCalles();
    void cargar() override;
    void actualizar(double dt, Jerry &jugador) override;
    bool verificarVictoria(const Jerry &jugador) const override;
    void configurarDificultad(int dificultad = 1);

private:
    void recogerObjetos(Jerry &jugador);
    void actualizarPuntoSeguro(const Jerry &jugador);
    bool puntoLibre(double x) const;
    int metaQuesos = 6;
    double posicionSalida = 4800.01;
    double amplitud = 60;
    double frecuencia = 0.5;
};

class NivelTienda : public Nivel
{
public:
    NivelTienda();
    void cargar() override;
    void actualizar(double dt, Jerry &jugador) override;
    bool verificarVictoria(const Jerry &jugador) const override;
    bool disponible() const override;
    bool verificarDerrota(const Jerry &jugador) const override;
    void intentarAtrapar(Jerry &jugador);
    bool entregarOrero(Jerry &jugador);
    bool tieneOrero() const;
    bool estaEnLanzamiento() const;
    bool estaCargando() const;
    double tiempoRestante() const;
    double posicionOreroX() const;
    double posicionOreroY() const;
    double posicionAvisoX() const;
    double posicionLetreroX() const;
    const QVector<QPointF> &obtenerObjetosCaida() const;
    double posicionSiguienteObjetoX() const;
    double tiempoParaObjeto() const;

private:
    void perderOrero(Jerry &jugador, const QString &motivo, bool perderVida = true);
    unique_ptr<Llorente> llorente;
    double tiempoRestanteNivel = 60;
    double esperaLanzamiento = 1.2;
    double esperaReinicio = 0;
    double oreroX = 0;
    double oreroY = 0;
    double velocidadOreroX = 0;
    double velocidadOreroY = 0;
    double objetivoOreroX = 0;
    double avisoX = 0;
    QVector<QPointF> objetosCaida;
    double siguienteObjetoX = 360;
    double esperaObjeto = 1.0;
    double letreroX = 560;
    int direccionLetrero = 1;
    int lanzamientos = 0;
    bool oreroVisible = false;
    bool cargando = false;
    bool turnoEntregado = false;
    int objetosGenerados = 0;
};

class NivelCabildo : public Nivel
{
public:
    NivelCabildo();
    void cargar() override;
    void actualizar(double dt, Jerry &jugador) override;
    bool verificarVictoria(const Jerry &jugador) const override;
    void activarProclamacion(const Jerry &jugador);
    bool disponible() const override;

private:
    Vector2 posicionCabildo;
    bool proclamacionActivada = false;
};

#endif
