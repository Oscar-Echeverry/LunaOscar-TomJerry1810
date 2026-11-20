#include "niveles.h"
#include "configuracion.h"
#include <algorithm>
#include <cmath>
#include <utility>

Nivel::Nivel(int numero) : numero(numero), tom(std::make_unique<Tom>())
{
    tom->establecerNivel(this);
}

bool Nivel::disponible() const { return true; }
void Nivel::reiniciar() { cargar(); }
void Nivel::reiniciarMemoria() { tom->reiniciar(true); }

bool Nivel::verificarDerrota(const Jerry &jugador) const
{
    return !jugador.estaVivo() && !jugador.estaHerido();
}

void Nivel::prepararCarga()
{
    completado = false;
    objetos.clear();
    obstaculos.clear();
}

void Nivel::agregarObjeto(std::unique_ptr<Objeto> objeto) { objetos.push_back(std::move(objeto)); }
void Nivel::agregarObstaculo(std::unique_ptr<Obstaculo> obstaculo) { obstaculos.push_back(std::move(obstaculo)); }

void Nivel::resolverLaterales(Jerry &jugador)
{
    Vector2 posicion = jugador.obtenerPosicion();
    posicion.x = std::clamp(posicion.x, 28.0, anchoMundo - 28);
    jugador.establecerPosicion(posicion);
    for (const QRectF &caja : plataformas) {
        if (jugador.rectanguloColision().intersects(caja)) {
            if (jugador.obtenerVelocidad().x > 0)
                posicion.x = caja.left() - 15;
            else if (jugador.obtenerVelocidad().x < 0)
                posicion.x = caja.right() + 15;
            jugador.establecerPosicion(posicion);
        }
    }
}

void Nivel::comprobarColisiones(Jerry &jugador)
{
    Vector2 posicion = jugador.obtenerPosicion();
    Vector2 velocidad = jugador.obtenerVelocidad();
    jugador.establecerEnSuelo(false);
    for (const QRectF &caja : plataformas) {
        const QRectF cuerpo = jugador.rectanguloColision();
        const bool encima = cuerpo.right() > caja.left() && cuerpo.left() < caja.right();
        if (encima && velocidad.y >= 0 && piesAnteriores <= caja.top() + 0.01 && posicion.y >= caja.top()) {
            posicion.y = caja.top();
            velocidad.y = 0;
            jugador.establecerEnSuelo(true);
        } else if (encima && velocidad.y < 0 && piesAnteriores - 59 >= caja.bottom()
                   && posicion.y - 59 < caja.bottom()) {
            posicion.y = caja.bottom() + 59;
            velocidad.y = 0;
        }
        jugador.establecerPosicion(posicion);
    }
    if (posicion.y >= Configuracion::sueloY && piesAnteriores <= Configuracion::sueloY + 0.01) {
        posicion.y = Configuracion::sueloY;
        velocidad.y = 0;
        jugador.establecerEnSuelo(true);
    }
    jugador.establecerPosicion(posicion);
    jugador.establecerVelocidad(velocidad);
}

bool Nivel::perderPorColisiones(Jerry &jugador)
{
    const int anteriores = jugador.obtenerVidas();
    for (const auto &obstaculo : obstaculos)
        obstaculo->aplicardaño(jugador);
    if (jugador.obtenerPosicion().y > Configuracion::altoFondo + 140)
        jugador.perderVida();
    return jugador.obtenerVidas() < anteriores;
}

void Nivel::registrardaño(int vidasAnteriores, Jerry &jugador)
{
    if (jugador.obtenerVidas() == vidasAnteriores)
        return;
    if (jugador.estaVivo()) {
        double xTom = std::max(30.0, puntoSeguroX - 300.0);
        const QVector<QRectF> solidos = obtenerSolidos();
        for (; xTom > 30.0; xTom -= 40.0) {
            tom->establecerPosicion({xTom, Configuracion::sueloY});
            const bool bloqueado = std::any_of(solidos.cbegin(), solidos.cend(),
                                               [this](const QRectF &solido) {
                                                   return tom->rectanguloColision().intersects(solido);
                                               });
            if (!bloqueado)
                break;
        }
        tom->establecerPosicion({std::max(30.0, xTom), Configuracion::sueloY});
        tom->establecerVelocidad({});
    }
    tom->retirarse(2.5);
    mostrarMensaje(jugador.estaVivo() ? "¡Cuidado! Volverás al último punto seguro."
                                      : "Tom te ha alcanzado. Puedes intentarlo de nuevo.");
}

void Nivel::actualizarHabitantes(double dt)
{
    for (Habitante &habitante : habitantes) {
        habitante.x += habitante.velocidad * habitante.direccion * dt;
        if (habitante.x > habitante.centroX + habitante.recorrido) {
            habitante.x = habitante.centroX + habitante.recorrido;
            habitante.direccion = -1;
        } else if (habitante.x < habitante.centroX - habitante.recorrido) {
            habitante.x = habitante.centroX - habitante.recorrido;
            habitante.direccion = 1;
        }
    }
}

void Nivel::mostrarMensaje(const QString &texto) { mensaje = texto; tiempoMensaje = 4; }
const std::vector<std::unique_ptr<Objeto>> &Nivel::obtenerObjetos() const { return objetos; }
const std::vector<std::unique_ptr<Obstaculo>> &Nivel::obtenerObstaculos() const { return obstaculos; }
// Mantener la firma exactamente igual a la declaración en niveles.h.
const QVector<QRectF> &Nivel::obtenerPlataformas() const { return plataformas; }
const QVector<Habitante> &Nivel::obtenerHabitantes() const { return habitantes; }

// Los sólidos incluyen plataformas y obstáculos para la física de Tom.
QVector<QRectF> Nivel::obtenerSolidos() const
{
    QVector<QRectF> solidos = plataformas;
    for (const auto &obstaculo : obstaculos)
        solidos.append(obstaculo->rectanguloColision());
    return solidos;
}

Tom &Nivel::obtenerTom() { return *tom; }
const Tom &Nivel::obtenerTom() const { return *tom; }
const FisicaJuego &Nivel::obtenerFisica() const { return fisica; }
// Ancho del mundo utilizado por Tom y la IA.
double Nivel::obtenerAncho() const { return anchoMundo; }
double Nivel::obtenerPuertaX() const { return puertaX; }
double Nivel::obtenerTiempo() const { return tiempo; }
double Nivel::obtenerPuntoSeguro() const { return puntoSeguroX; }
const QString &Nivel::obtenerMensaje() const { return mensaje; }
bool Nivel::hayMensaje() const { return tiempoMensaje > 0; }
int Nivel::obtenerNumero() const { return numero; }
bool Nivel::estaCompletado() const { return completado; }
void Nivel::completar() { completado = true; }

void Nivel::restaurarEscenario(double tiempoGuardado, double puntoSeguro, const std::vector<bool> &activos)
{
    tiempo = tiempoGuardado;
    puntoSeguroX = puntoSeguro;
    for (size_t i = 0; i < objetos.size() && i < activos.size(); ++i)
        objetos[i]->establecerActivo(activos[i]);
    for (auto &obstaculo : obstaculos) {
        obstaculo->establecerTiempo(tiempo);
        obstaculo->actualizarFisica(fisica, 0);
    }
}

NivelCalles::NivelCalles() : Nivel(1) {}

void NivelCalles::cargar()
{
    const double sueloY = Configuracion::sueloY;
    prepararCarga();
    tiempo = 0;
    puntoSeguroX = 180;
    configurarDificultad();
    plataformas = {QRectF(975, sueloY - 64, 187, 64), QRectF(2670, sueloY - 64, 187, 64),
                   QRectF(4110, sueloY - 64, 187, 64)};
    const QVector<double> centros = {690, 1590, 2330, 3220, 3910, 4490};
    for (int i = 0; i < centros.size(); ++i) {
        auto barril = std::make_unique<Obstaculo>(Vector2{centros[i], sueloY}, i * 0.9,
                                                  TipoMovimiento::OSCILATORIO, amplitud, frecuencia);
        barril->actualizarFisica(obtenerFisica(), 0);
        agregarObstaculo(std::move(barril));
    }
    for (Vector2 posicion : {Vector2{360, sueloY - 12}, {1068, sueloY - 76}, {1880, sueloY - 12},
                             {2763, sueloY - 76}, {3500, sueloY - 12}, {4203, sueloY - 76}})
        agregarObjeto(std::make_unique<Queso>(posicion));
    agregarObjeto(std::make_unique<Llave>(Vector2{4620, sueloY - 14}));
    const QVector<double> monedas = {470, 540, 850, 1230, 1320, 1730, 2040, 2140,
                                     2510, 2920, 3030, 3380, 3650, 3760, 4350, 4730};
    for (double x : monedas)
        agregarObjeto(std::make_unique<Moneda>(Vector2{x, sueloY - 18}));
    puntosSeguros = {180, 850, 1400, 2100, 2970, 3700, 4320, 4740};
    habitantes = {{"campesino", 500, 500, 535, 98, 52, 14},
                  {"nina", 1320, 1320, 544, 70, 58, 20, -1},
                  {"dama", 1980, 1980, 534, 96, 34, 10},
                  {"perro", 2500, 2500, 550, 43, 65, 23},
                  {"nino", 3080, 3080, 542, 72, 58, 19},
                  {"campesino", 3750, 3750, 536, 98, 42, 12, -1},
                  {"dama", 4550, 4550, 532, 93, 38, 10, -1}};
    obtenerTom().reiniciar(false);
    mostrarMensaje("¡Toma ventaja! Mantén Mayús para correr. Reúne los 6 quesos y la llave.");
}

void NivelCalles::configurarDificultad(int dificultad)
{
    if (dificultad != 1)
        return;
    metaQuesos = 6;
    amplitud = 60;
    frecuencia = 0.5;
    anchoMundo = Configuracion::anchoMundo;
    puertaX = posicionSalida = Configuracion::puertaX;
}

void NivelCalles::actualizar(double dt, Jerry &jugador)
{
    tiempo += dt;
    tiempoMensaje = std::max(0.0, tiempoMensaje - dt);
    for (const auto &obstaculo : obtenerObstaculos())
        obstaculo->actualizarFisica(obtenerFisica(), dt);
    actualizarHabitantes(dt);
    const int vidasAnteriores = jugador.obtenerVidas();
    const bool estabaHerido = jugador.estaHerido();
    obtenerTom().actualizarIA(dt, jugador);
    jugador.actualizar(dt);
    if (estabaHerido && !jugador.estaHerido() && jugador.estaVivo()) {
        jugador.reaparecer({puntoSeguroX, Configuracion::sueloY});
        obtenerTom().retirarse(1.8);
    } else if (!jugador.estaHerido() && jugador.estaVivo()) {
        resolverLaterales(jugador);
        piesAnteriores = jugador.obtenerPosicion().y;
        obtenerFisica().aplicarParabola(jugador, dt);
        comprobarColisiones(jugador);
        recogerObjetos(jugador);
        perderPorColisiones(jugador);
        actualizarPuntoSeguro(jugador);
    }
    registrardaño(vidasAnteriores, jugador);
}

void NivelCalles::recogerObjetos(Jerry &jugador)
{
    for (const auto &objeto : obtenerObjetos()) {
        if (!objeto->estaActivo())
            continue;
        const int quesosAntes = jugador.obtenerQuesos();
        const bool llaveAntes = jugador.tieneLaLlave();
        objeto->recoger(jugador);
        if (jugador.obtenerQuesos() > quesosAntes)
            mostrarMensaje(QString("¡Queso %1 de 6! Salto +20 % durante 5 segundos.").arg(jugador.obtenerQuesos()));
        else if (!llaveAntes && jugador.tieneLaLlave())
            mostrarMensaje("¡Tienes la llave! Busca la puerta de la tienda de Llorente.");
    }
}

bool NivelCalles::puntoLibre(double x) const
{
    if (std::abs(obtenerTom().obtenerPosicion().x - x) < 230)
        return false;
    for (const auto &obstaculo : obtenerObstaculos()) {
        if (std::abs(obstaculo->obtenerCentro().x - x) < obstaculo->obtenerAmplitud() + 90)
            return false;
    }
    for (const QRectF &caja : plataformas) {
        if (x > caja.left() - 35 && x < caja.right() + 35)
            return false;
    }
    return true;
}

void NivelCalles::actualizarPuntoSeguro(const Jerry &jugador)
{
    if (!jugador.estaEnSuelo() || jugador.obtenerPosicion().y != Configuracion::sueloY || jugador.estaHerido())
        return;
    for (double punto : puntosSeguros) {
        if (punto > puntoSeguroX && std::abs(jugador.obtenerPosicion().x - punto) < 45 && puntoLibre(punto))
            puntoSeguroX = punto;
    }
}

bool NivelCalles::verificarVictoria(const Jerry &jugador) const
{
    return jugador.puedeSalir() && jugador.obtenerQuesos() == metaQuesos
           && std::abs(jugador.obtenerPosicion().x - posicionSalida) <= 100
           && jugador.estaEnSuelo() && jugador.obtenerPosicion().y == Configuracion::sueloY;
}

NivelTienda::NivelTienda() : Nivel(2), llorente(std::make_unique<Llorente>()) {}
void NivelTienda::cargar() { prepararCarga(); }
void NivelTienda::actualizar(double, Jerry &) {}
bool NivelTienda::verificarVictoria(const Jerry &) const { return false; }
bool NivelTienda::disponible() const { return false; }

NivelCabildo::NivelCabildo() : Nivel(3) {}
void NivelCabildo::cargar() { prepararCarga(); proclamacionActivada = false; }
void NivelCabildo::actualizar(double, Jerry &) {}
bool NivelCabildo::verificarVictoria(const Jerry &) const { return false; }
bool NivelCabildo::disponible() const { return false; }

void NivelCabildo::activarProclamacion(const Jerry &jugador)
{
    if (jugador.estaVivo() && !jugador.estaHerido() && jugador.tieneElFlorero()
        && jugador.obtenerPosicion().distancia(posicionCabildo) <= 100)
        proclamacionActivada = true;
}
