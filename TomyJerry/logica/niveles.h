#ifndef NIVELES_H
#define NIVELES_H
#include "fisicajuego.h"
#include "objetos.h"
#include "personajes.h"
#include "dificultad.h"
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
    virtual void establecerDificultad(Dificultad valor);
    Dificultad obtenerDificultad() const;

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
    Dificultad dificultad = Dificultad::Normal;

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
    void configurarDificultad(Dificultad dificultad = Dificultad::Normal);
    void establecerDificultad(Dificultad valor) override;

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
    static constexpr int puntosEntregaFlorero = 500;
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
    const QVector<Proyectil> &obtenerObjetosCaida() const;
    double posicionSiguienteObjetoX() const;
    double tiempoParaObjeto() const;
    const Llorente &obtenerLlorente() const;
    bool enConversacion() const;
    QString dialogoCriollos() const;
    void avanzarDialogo();

private:
    void perderOrero(Jerry &jugador, const QString &motivo, bool perderVida = true);
    unique_ptr<Llorente> llorente;
    double tiempoRestanteNivel = 60;
    double esperaLanzamiento = 1.2;
    double esperaReinicio = 0;
    Proyectil floreroEnVuelo;
    double objetivoOreroX = 0;
    double avisoX = 0;
    QVector<Proyectil> objetosCaida;
    double siguienteObjetoX = 360;
    double esperaObjeto = 1.0;
    double tiempoConversacion = 0;
    int lanzamientos = 0;
    bool oreroVisible = false;
    bool cargando = false;
    bool turnoEntregado = false;
    int objetosGenerados = 0;
};

class NivelCabildo : public Nivel
{
public:
    struct CajaDeslizable {
        QRectF rectangulo;
        Vector2 velocidad;
    };
    struct ElementoPlaza {
        QRectF imagen;
        double profundidad;
    };

    NivelCabildo();
    void cargar() override;
    void actualizar(double dt, Jerry &jugador) override;
    bool verificarVictoria(const Jerry &jugador) const override;
    bool disponible() const override;
    void establecerDireccion(int horizontal, int vertical);
    bool interactuar(Jerry &jugador);
    const QVector<CajaDeslizable> &obtenerCajas() const;
    const QVector<QRectF> &obtenerPuestos() const;
    const QVector<ElementoPlaza> &obtenerEscenografia() const;
    const QVector<Vector2> &obtenerRastroJerry() const;
    const QVector<Vector2> &obtenerRutaCorta() const;
    const QVector<Vector2> &obtenerRutaAlternativa() const;
    Vector2 obtenerCabildo() const;
    Vector2 obtenerInicio() const;
    QRectF obtenerLimitesPlaza() const;
    int obtenerPantalla() const;
    bool tomEstaAturdido() const;
    void establecerDificultad(Dificultad valor) override;
    const QVector<QRectF> &obtenerVallas() const;
    int obtenerEmpujes() const;
    static QString nombreFondo(int pantalla);
    static QString nombrePlaza(int pantalla);
    bool hayLineaVision(Vector2 inicio, Vector2 destino) const;
    int zonaEnPlaza(Vector2 posicion) const;
    Vector2 centroZona(int zona) const;

private:
    bool posicionLibre(const QRectF &cuerpo, int ignorarCaja = -1) const;
    void actualizarCajas(double dt, Jerry &jugador);
    void actualizarTom(double dt, Jerry &jugador);
    bool empujarCaja(Jerry &jugador);
    void cargarPlaza();
    void colocarCajaEnCamino();
    void actualizarRutas(Vector2 inicio);
    QVector<Vector2> buscarCamino(Vector2 inicio, Vector2 destino,
                                  double semiancho = 12, double alto = 12,
                                  bool evitarVigilancia = false, int ignorarCaja = -1) const;

    Vector2 posicionCabildo;
    Vector2 posicionInicio;
    QRectF limitesPlaza;
    int pantallaActual = 0;
    QVector<QRectF> puestos;
    QVector<QRectF> vallas;
    QVector<ElementoPlaza> escenografia;
    QVector<CajaDeslizable> cajas;
    QVector<Vector2> rastroJerry;
    QVector<Vector2> rutaCorta;
    QVector<Vector2> rutaAlternativa;
    Vector2 posicionVigilancia;
    int direccionX = 0;
    int direccionY = 0;
    int mirandoX = 0;
    int mirandoY = -1;
    double tiempoTomAturdido = 0;
    double tiempoDañoHabitante = 0;
    bool mensajeEntregado = false;
    QVector<Vector2> caminoTom;
    double esperaRastro = 0;
    int cajaDelPaso = -1;
    int empujes = 0;
    bool pasoAbierto = false;
};

#endif
