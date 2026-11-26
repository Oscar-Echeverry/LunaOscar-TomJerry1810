#include "niveles.h"
#include "configuracion.h"
#include <algorithm>
#include <cmath>
#include <queue>
#include <limits>
#include <functional>
#include <stdexcept>

namespace {
constexpr double escalaX = 1120.0 / 1672;
constexpr double escalaY = 630.0 / 941;
constexpr int pasoRed = 5;
constexpr int columnas = 224;
constexpr int filas = 126;
Vector2 punto(double x, double y) { return {x * escalaX, y * escalaY}; }
QRectF cuerpo(Vector2 p, double semiancho = 10, double alto = 10)
{
    return {p.x - semiancho, p.y - alto, semiancho * 2, alto};
}
}

NivelCabildo::NivelCabildo() : Nivel(3) {}

QString NivelCabildo::nombreFondo(int pantalla)
{
    static const QStringList nombres = {
        "05_Plaza_fondo_recomendado.png", "02_Plaza_del_mercado.png",
        "04_Plaza_rutas_secretas.png", "03_Plaza_abierta.png", "01_Plaza_del_cabildo.png"
    };
    return nombres[std::clamp(pantalla, 0, 4)];
}

QString NivelCabildo::nombrePlaza(int pantalla)
{
    static const QStringList nombres = {
        QStringLiteral("PLAZA DE ENTRADA"), QStringLiteral("PLAZA DEL MERCADO"),
        QStringLiteral("RUTAS SECRETAS"), QStringLiteral("PLAZA ABIERTA"),
        QStringLiteral("PLAZA DEL CABILDO")
    };
    return nombres[std::clamp(pantalla, 0, 4)];
}

void NivelCabildo::cargar()
{
    prepararCarga();
    anchoMundo = Configuracion::anchoVista;
    pantallaActual = 0;
    tiempo = tiempoMensaje = 0;
    direccionX = direccionY = 0;
    mirandoX = 0;
    mirandoY = -1;
    mensajeEntregado = false;
    empujes = 0;
    cargarPlaza();
    mostrarMensaje("¡Abre el paso! Acércate a la caja del camino y pulsa E para empujarla.");
}

void NivelCabildo::cargarPlaza()
{
    puestos.clear();
    vallas.clear();
    escenografia.clear();
    cajas.clear();
    habitantes.clear();
    plataformas.clear();
    // Coordenadas sobre las imágenes originales de 1672 x 941.
    // Los puestos, muros, edificios, jardines y cajas del fondo son sólidos.
    auto solido = [this](double x, double y, double ancho, double alto) {
        puestos.append(QRectF(x * escalaX, y * escalaY, ancho * escalaX, alto * escalaY));
    };
    // La parte alta se pinta delante de quien camina detrás; sólo la base ocupa suelo.
    auto decorado = [this, &solido](double x, double y, double ancho, double alto, double proporcionBase = 0.5) {
        const QRectF imagen(x * escalaX, y * escalaY, ancho * escalaX, alto * escalaY);
        escenografia.append({imagen, imagen.bottom()});
        solido(x, y + alto * (1 - proporcionBase), ancho, alto * proporcionBase);
    };
    auto caja = [this](double x, double y) {
        const Vector2 p = punto(x, y);
        cajas.append({QRectF(p.x, p.y, 32, 32), {}});
    };
    auto habitante = [this](const QString &nombre, double x, double y, double recorrido) {
        const Vector2 p = punto(x, y);
        habitantes.append({nombre, p.x, p.x, p.y, 37, recorrido * escalaX, 25, 1});
    };
    limitesPlaza = QRectF(145 * escalaX, 330 * escalaY, 1390 * escalaX, 540 * escalaY);
    posicionInicio = punto(835, 850);
    posicionCabildo = punto(835, 395);

    switch (pantallaActual) {
    case 0:
        solido(0, 0, 1672, 310);
        decorado(700, 490, 280, 175, 0.65);
        decorado(300, 325, 355, 130, 0.4);
        decorado(1010, 325, 280, 160, 0.4);
        decorado(300, 620, 340, 160, 0.4);
        decorado(1035, 625, 275, 165, 0.4);
        decorado(330, 410, 110, 125);
        decorado(1240, 410, 135, 145);
        decorado(480, 710, 185, 130);
        decorado(1005, 710, 180, 140);
        solido(170, 330, 45, 60);
        solido(200, 470, 75, 100);
        solido(300, 575, 120, 85);
        solido(235, 725, 90, 65);
        solido(1430, 530, 80, 105);
        solido(1370, 400, 50, 65);
        solido(1275, 690, 45, 80);
        caja(245, 600);
        caja(1375, 660);
        habitante("campesino", 610, 575, 40);
        habitante("dama", 1080, 575, 40);
        break;
    case 1:
        limitesPlaza = QRectF(80 * escalaX, 140 * escalaY, 1490 * escalaX, 740 * escalaY);
        posicionInicio = punto(1120, 865);
        posicionCabildo = punto(1435, 395);
        solido(0, 0, 620, 120);
        solido(590, 0, 1082, 285);
        solido(0, 120, 75, 821);
        solido(0, 880, 1672, 61);
        // Muros del rodeo de la izquierda, siguiendo los pasillos del dibujo.
        decorado(75, 150, 100, 55);
        solido(165, 150, 45, 90);
        decorado(210, 145, 290, 55);
        solido(490, 180, 45, 145);
        decorado(325, 250, 270, 100, 0.35);
        solido(325, 250, 30, 75);
        solido(100, 240, 30, 160);
        decorado(130, 235, 80, 40);
        solido(100, 395, 30, 155);
        decorado(130, 525, 100, 40);
        solido(195, 485, 40, 165);
        decorado(230, 630, 170, 35);
        solido(420, 380, 40, 255);
        decorado(235, 475, 95, 35);
        decorado(75, 650, 110, 55);
        solido(95, 700, 35, 100);
        decorado(130, 785, 205, 40);
        solido(235, 825, 165, 116);
        decorado(320, 710, 160, 35);
        decorado(300, 315, 40, 120, 0.8);
        decorado(230, 475, 85, 100, 0.7);
        decorado(165, 690, 120, 75, 0.7);
        decorado(290, 620, 60, 70, 0.7);
        decorado(350, 365, 45, 60, 0.7);
        // Puestos: techo y base se dibujan a distinta profundidad.
        decorado(490, 380, 115, 130);
        decorado(460, 565, 80, 185, 0.8);
        decorado(645, 295, 135, 110);
        decorado(985, 295, 120, 125);
        decorado(650, 475, 100, 105);
        decorado(815, 410, 120, 140);
        decorado(625, 615, 125, 130);
        decorado(755, 600, 125, 135);
        decorado(700, 405, 110, 145, 0.25);
        decorado(600, 610, 80, 60, 0.4);
        decorado(1020, 485, 170, 155, 0.65);
        decorado(975, 465, 95, 70, 0.5);
        decorado(1110, 465, 105, 70, 0.5);
        decorado(975, 610, 95, 70, 0.5);
        decorado(1110, 610, 105, 70, 0.5);
        decorado(1415, 485, 160, 150, 0.65);
        decorado(1310, 660, 115, 100, 0.55);
        decorado(890, 755, 165, 120);
        decorado(1175, 770, 105, 110);
        decorado(545, 775, 100, 100);
        caja(1290, 550);
        caja(550, 545);
        habitante("nina", 1300, 450, 65);
        habitante("campesino", 900, 355, 90);
        break;
    case 2:
        posicionInicio = punto(920, 815);
        posicionCabildo = punto(355, 395);
        solido(0, 0, 1672, 315);
        solido(0, 400, 380, 541);
        solido(380, 735, 200, 206);
        solido(960, 740, 210, 201);
        solido(930, 225, 510, 120);
        solido(1145, 320, 125, 210);
        solido(1100, 580, 130, 170);
        solido(1390, 680, 282, 260);
        decorado(500, 345, 130, 110);
        decorado(900, 350, 150, 105);
        decorado(635, 425, 235, 220, 0.85);
        decorado(420, 590, 120, 105);
        decorado(935, 590, 120, 110);
        solido(595, 765, 90, 75);
        solido(765, 755, 105, 80);
        decorado(1295, 365, 150, 110);
        solido(1280, 450, 55, 95);
        solido(1350, 555, 65, 110);
        solido(1250, 700, 50, 85);
        caja(545, 485);
        caja(1030, 500);
        habitante("dama", 480, 535, 40);
        habitante("nino", 920, 395, 65);
        break;
    case 3:
        posicionCabildo = punto(835, 330);
        limitesPlaza.setTop(280 * escalaY);
        solido(0, 0, 1672, 270);
        solido(0, 0, 130, 941);
        solido(1545, 0, 127, 941);
        solido(255, 235, 175, 180);
        solido(1230, 245, 185, 145);
        solido(200, 550, 105, 165);
        solido(1320, 555, 135, 160);
        decorado(390, 385, 160, 140);
        decorado(1160, 390, 145, 140);
        decorado(450, 600, 165, 145);
        decorado(1100, 595, 165, 145);
        decorado(735, 450, 205, 175, 0.85);
        decorado(520, 325, 120, 90);
        decorado(1040, 325, 120, 90);
        decorado(585, 740, 120, 85);
        decorado(985, 740, 125, 85);
        solido(130, 365, 95, 110);
        solido(1435, 510, 95, 120);
        caja(640, 620);
        caja(990, 610);
        habitante("campesino", 660, 490, 45);
        habitante("nina", 1010, 490, 40);
        break;
    default:
        posicionCabildo = punto(815, 395);
        // Los callejones laterales continúan por encima de la fachada.
        // Sus límites siguen las casas, en lugar de cerrar todo el ancho de la plaza.
        limitesPlaza = QRectF(165 * escalaX, 180 * escalaY, 1390 * escalaX, 690 * escalaY);
        solido(395, 0, 890, 335);
        solido(0, 0, 165, 941);
        solido(1570, 0, 102, 941);
        solido(1470, 690, 202, 251);
        solido(1435, 255, 35, 240);
        solido(335, 340, 60, 185);
        solido(420, 675, 55, 140);
        solido(1185, 635, 60, 170);
        solido(1260, 395, 50, 125);
        decorado(510, 365, 160, 160);
        decorado(940, 365, 165, 160);
        decorado(510, 575, 160, 155);
        decorado(935, 575, 165, 155);
        decorado(745, 495, 140, 120, 0.8);
        decorado(695, 475, 90, 65);
        decorado(840, 475, 90, 65);
        decorado(695, 580, 90, 80);
        decorado(840, 580, 90, 80);
        decorado(585, 760, 110, 65);
        decorado(935, 760, 110, 65);
        // Las pilas ocupan suelo en su base; Jerry puede pasar por detrás
        // y el dibujo de las cajas lo tapa a la profundidad correspondiente.
        decorado(365, 535, 95, 140, 0.35);
        decorado(1265, 215, 40, 65, 0.5);
        decorado(1370, 205, 100, 105, 0.5);
        solido(1370, 505, 100, 70);
        solido(1405, 615, 65, 115);
        solido(1285, 755, 100, 70);
        caja(250, 560);
        caja(1300, 580);
        habitante("dama", 710, 425, 20);
        habitante("nino", 1160, 570, 35);
        break;
    }
    colocarCajaEnCamino();
    puertaX = posicionCabildo.x;
    puntoSeguroX = posicionInicio.x;
    rastroJerry = {posicionInicio};
    indiceRastroTom = 0;
    caminoTom.clear();
    esperaCaminoTom = esperaRastro = 0;
    tiempoTomAturdido = tiempoDañoHabitante = 0;
    tiempoSinVerJerry = 0;
    // Las señales siguen suelo transitable. Tom comienza sobre el camino corto;
    // el rodeo penaliza acercarse a ese puesto de vigilancia.
    actualizarRutas(posicionInicio);
    obtenerTom().establecerPosicion(posicionVigilancia);
    obtenerTom().establecerVelocidad({});
}

void NivelCabildo::colocarCajaEnCamino()
{
    // Una abertura de 36 px para una caja de 32 px: Jerry (20 px) no cabe a su lado.
    // Las vallas cruzan todo el camino; la caja puede deslizarse hacia suelo libre.
    const double y = posicionInicio.y - 45;
    for (int distancia = 0; distancia <= 300; distancia += 5) {
        for (int sentido : {1, -1}) {
            const double x = posicionInicio.x + distancia * sentido;
            if (!posicionLibre(QRectF(x - 16, y - 75, 32, 107)))
                continue;
            const QRectF izquierda(limitesPlaza.left(), y, x - 18 - limitesPlaza.left(), 12);
            const QRectF derecha(x + 18, y, limitesPlaza.right() - x - 18, 12);
            if (izquierda.width() <= 0 || derecha.width() <= 0)
                continue;
            puestos.append(izquierda);
            puestos.append(derecha);
            cajas.append({QRectF(x - 16, y - 10, 32, 32), {}});
            cajaDelPaso = cajas.size() - 1;
            const Vector2 acceso{x, y + 43};
            const bool accesible = !buscarCamino(posicionInicio, acceso, 10, 10).isEmpty();
            const bool salida = !buscarCamino(posicionInicio, posicionCabildo, 10, 10,
                                              false, cajaDelPaso).isEmpty();
            if (accesible && salida) {
                vallas = {izquierda, derecha};
                pasoAbierto = false;
                return;
            }
            cajas.removeLast();
            puestos.removeLast();
            puestos.removeLast();
        }
    }
    throw std::runtime_error("No se pudo crear un paso de cajas transitable en la plaza "
                             + std::to_string(pantallaActual + 1) + ".");
}

void NivelCabildo::actualizarRutas(Vector2 inicio)
{
    const int ignorar = pasoAbierto ? -1 : cajaDelPaso;
    rutaCorta = buscarCamino(inicio, posicionCabildo, 12, 12, false, ignorar);
    posicionVigilancia = rutaCorta.isEmpty() ? posicionCabildo
                                             : rutaCorta[rutaCorta.size() * 3 / 5];
    rutaAlternativa = buscarCamino(inicio, posicionCabildo, 12, 12, true, ignorar);
    // El rodeo lateral pasa junto a una caja empujable, fuera del camino corto.
    const int cajaDelRodeo[] = {1, 1, 1, 1, 0};
    const QRectF &cajaRodeo = cajas[cajaDelRodeo[pantallaActual]].rectangulo;
    const Vector2 pasoLateral{cajaRodeo.right() + 16, cajaRodeo.center().y() + 5};
    if (posicionLibre(cuerpo(pasoLateral, 12, 12))) {
        QVector<Vector2> ida = buscarCamino(inicio, pasoLateral, 12, 12, true, ignorar);
        if (!ida.isEmpty()) {
            const auto vuelta = buscarCamino(ida.last(), posicionCabildo, 12, 12, true, ignorar);
            if (!vuelta.isEmpty()) {
                ida += vuelta;
                rutaAlternativa = ida;
            }
        }
    }
    rutaCorta.prepend(inicio);
    rutaAlternativa.prepend(inicio);
}

void NivelCabildo::establecerDireccion(int horizontal, int vertical)
{
    direccionX = std::clamp(horizontal, -1, 1);
    direccionY = std::clamp(vertical, -1, 1);
    if (direccionX != 0 || direccionY != 0) {
        mirandoX = direccionX != 0 ? direccionX : 0;
        mirandoY = direccionX == 0 ? direccionY : 0;
    }
}

bool NivelCabildo::disponible() const { return true; }
bool NivelCabildo::verificarVictoria(const Jerry &) const { return mensajeEntregado; }
const QVector<NivelCabildo::CajaDeslizable> &NivelCabildo::obtenerCajas() const { return cajas; }
const QVector<QRectF> &NivelCabildo::obtenerPuestos() const { return puestos; }
const QVector<NivelCabildo::ElementoPlaza> &NivelCabildo::obtenerEscenografia() const { return escenografia; }
const QVector<Vector2> &NivelCabildo::obtenerRastroJerry() const { return rastroJerry; }
const QVector<Vector2> &NivelCabildo::obtenerRutaCorta() const { return rutaCorta; }
const QVector<Vector2> &NivelCabildo::obtenerRutaAlternativa() const { return rutaAlternativa; }
Vector2 NivelCabildo::obtenerCabildo() const { return posicionCabildo; }
Vector2 NivelCabildo::obtenerInicio() const { return posicionInicio; }
QRectF NivelCabildo::obtenerLimitesPlaza() const { return limitesPlaza; }
int NivelCabildo::obtenerPantalla() const { return pantallaActual; }
bool NivelCabildo::tomEstaAturdido() const { return tiempoTomAturdido > 0; }
void NivelCabildo::establecerDificultad(Dificultad valor)
{
    Nivel::establecerDificultad(valor);
    esperaCaminoTom = 0;
}
const QVector<QRectF> &NivelCabildo::obtenerVallas() const { return vallas; }
int NivelCabildo::obtenerEmpujes() const { return empujes; }

bool NivelCabildo::posicionLibre(const QRectF &rectangulo, int ignorarCaja) const
{
    if (!limitesPlaza.contains(rectangulo))
        return false;
    for (const QRectF &obstaculo : puestos)
        if (rectangulo.intersects(obstaculo))
            return false;
    for (int i = 0; i < cajas.size(); ++i)
        if (i != ignorarCaja && rectangulo.intersects(cajas[i].rectangulo))
            return false;
    return true;
}

QVector<Vector2> NivelCabildo::buscarCamino(Vector2 inicio, Vector2 destino,
                                            double semiancho, double alto, bool evitarVigilancia,
                                            int ignorarCaja) const
{
    // Red con el tamaño del actor: Tom rodea los sólidos y las cajas no sellan la salida.
    const auto indice = [](Vector2 p) {
        return std::clamp(int(std::round(p.y / pasoRed)), 0, filas - 1) * columnas
               + std::clamp(int(std::round(p.x / pasoRed)), 0, columnas - 1);
    };
    const auto posicion = [](int i) { return Vector2{double(i % columnas * pasoRed),
                                                     double(i / columnas * pasoRed)}; };
    QVector<int> anterior(columnas * filas, -1);
    using Paso = std::pair<double, int>;
    std::priority_queue<Paso, std::vector<Paso>, std::greater<Paso>> pendientes;
    QVector<double> costos(columnas * filas, std::numeric_limits<double>::infinity());
    const int primero = indice(inicio);
    anterior[primero] = primero;
    costos[primero] = 0;
    pendientes.push({0, primero});
    int ultimo = -1;
    while (!pendientes.empty()) {
        const auto [costoActual, actual] = pendientes.top();
        pendientes.pop();
        if (costoActual > costos[actual])
            continue;
        if (posicion(actual).distancia(destino) <= 18) {
            ultimo = actual;
            break;
        }
        const int x = actual % columnas;
        const int y = actual / columnas;
        const int vecinos[] = {actual - 1, actual + 1, actual - columnas, actual + columnas};
        for (int j = 0; j < 4; ++j) {
            if ((j == 0 && x == 0) || (j == 1 && x == columnas - 1)
                || (j == 2 && y == 0) || (j == 3 && y == filas - 1))
                continue;
            const int siguiente = vecinos[j];
            const double riesgo = evitarVigilancia
                                      ? 20 * std::max(0.0, 1 - posicion(siguiente).distancia(posicionVigilancia) / 150) : 0;
            const double costo = costoActual + 1 + riesgo;
            if (costo >= costos[siguiente]
                || !posicionLibre(cuerpo(posicion(siguiente), semiancho, alto), ignorarCaja))
                continue;
            anterior[siguiente] = actual;
            costos[siguiente] = costo;
            pendientes.push({costo, siguiente});
        }
    }
    QVector<Vector2> camino;
    if (ultimo < 0)
        return camino;
    for (int i = ultimo; i != primero; i = anterior[i])
        camino.prepend(posicion(i));
    if (camino.isEmpty())
        camino.append(destino);
    return camino;
}

void NivelCabildo::actualizar(double dt, Jerry &jugador)
{
    if (dt <= 0 || mensajeEntregado)
        return;
    // Subpasos también si se usa el nivel fuera del bucle fijo del juego.
    if (dt > Configuracion::pasoTiempo + 0.000001) {
        while (dt > 0.000001) {
            const double paso = std::min(dt, Configuracion::pasoTiempo);
            actualizar(paso, jugador);
            dt -= paso;
        }
        return;
    }
    tiempo += dt;
    tiempoMensaje = std::max(0.0, tiempoMensaje - dt);
    tiempoDañoHabitante = std::max(0.0, tiempoDañoHabitante - dt);
    tiempoTomAturdido = std::max(0.0, tiempoTomAturdido - dt);
    jugador.actualizarEfectos(dt);
    jugador.establecerEnSuelo(true);
    Vector2 p = jugador.obtenerPosicion();
    const Vector2 antes = p;
    const double magnitud = std::hypot(double(direccionX), double(direccionY));
    if (jugador.estaVivo() && magnitud > 0) {
        const double rapidez = 185;
        const Vector2 paso{direccionX / magnitud * rapidez * dt, direccionY / magnitud * rapidez * dt};
        if (posicionLibre(cuerpo({p.x + paso.x, p.y})))
            p.x += paso.x;
        if (posicionLibre(cuerpo({p.x, p.y + paso.y})))
            p.y += paso.y;
        // mover() también altera la velocidad; se asigna después la velocidad real.
        if (direccionX != 0)
            jugador.mover({double(direccionX), 0}, 0);
    }
    jugador.establecerPosicion(p);
    jugador.establecerVelocidad({(p.x - antes.x) / dt, (p.y - antes.y) / dt});
    esperaRastro += dt;
    if (p.distancia(rastroJerry.last()) >= 24 && esperaRastro >= 0.12) {
        rastroJerry.append(p);
        esperaRastro = 0;
        if (rastroJerry.size() > 80) {
            rastroJerry.removeFirst();
            indiceRastroTom = std::max(0, indiceRastroTom - 1);
        }
    }
    actualizarCajas(dt, jugador);
    for (Habitante &h : habitantes) {
        const double nuevoX = h.x + h.direccion * h.velocidad * dt;
        if (std::abs(nuevoX - h.centroX) > h.recorrido
            || !posicionLibre(cuerpo({nuevoX, h.piesY})))
            h.direccion *= -1;
        else
            h.x = nuevoX;
        if (tiempoDañoHabitante <= 0 && cuerpo(p).intersects(cuerpo({h.x, h.piesY}))) {
            jugador.recibirDaño(1);
            tiempoDañoHabitante = 1;
        }
    }
    actualizarTom(dt, jugador);
    // Los arcos marcados conectan las plazas. Sólo el último recibe el mensaje con E.
    if (jugador.estaVivo() && pantallaActual < 4 && p.distancia(posicionCabildo) < 34) {
        jugador.sumarPuntos(100);
        ++pantallaActual;
        cargarPlaza();
        jugador.establecerPosicion(posicionInicio);
        jugador.establecerVelocidad({});
        jugador.activarProteccion(1.5);
        mostrarMensaje("¡Plaza superada! +100 puntos. " + nombrePlaza(pantallaActual)
                       + ": rojo vigilado / verde menos vigilado.");
    }
}

void NivelCabildo::actualizarCajas(double dt, Jerry &jugador)
{
    bool huboMovimiento = false;
    bool cajaDetenida = false;
    for (int i = 0; i < cajas.size(); ++i) {
        CajaDeslizable &caja = cajas[i];
        if (std::hypot(caja.velocidad.x, caja.velocidad.y) < 1)
            continue;
        const QRectF anterior = caja.rectangulo;
        const QRectF destino = anterior.translated(caja.velocidad.x * dt, caja.velocidad.y * dt);
        if (!posicionLibre(destino, i) || destino.intersects(cuerpo(jugador.obtenerPosicion()))) {
            caja.velocidad = {};
            cajaDetenida = true;
            continue;
        }
        caja.rectangulo = destino;
        huboMovimiento = true;
        if (buscarCamino(jugador.obtenerPosicion(), posicionCabildo, 10, 10,
                         false, pasoAbierto ? -1 : cajaDelPaso).isEmpty()) {
            caja.rectangulo = anterior;
            caja.velocidad = {};
            cajaDetenida = true;
            mostrarMensaje("La caja se detuvo para mantener una salida libre.");
            continue;
        }
        const bool golpe = caja.rectangulo.intersects(cuerpo(obtenerTom().obtenerPosicion(), 12, 12));
        if (golpe) {
            tiempoTomAturdido = 1.0;
            caja.rectangulo = anterior; // Tom no queda dentro de un sólido al recuperarse.
            caja.velocidad = {};
            cajaDetenida = true;
            obtenerTom().establecerVelocidad({});
            mostrarMensaje("¡Tom está aturdido durante un segundo!");
        } else {
            caja.velocidad.x *= std::exp(-3.2 * dt);
            caja.velocidad.y *= std::exp(-3.2 * dt);
            if (std::hypot(caja.velocidad.x, caja.velocidad.y) < 7) {
                caja.velocidad = {};
                cajaDetenida = true;
            }
        }
        esperaCaminoTom = 0;
    }
    if (huboMovimiento && !pasoAbierto
        && !buscarCamino(jugador.obtenerPosicion(), posicionCabildo, 10, 10).isEmpty()) {
        pasoAbierto = true;
        actualizarRutas(posicionInicio);
        mostrarMensaje("¡Paso abierto! Sigue las rutas y usa las cajas para detener a Tom.");
    }
    if (pasoAbierto && cajaDetenida)
        actualizarRutas(posicionInicio);
}

void NivelCabildo::actualizarTom(double dt, Jerry &jugador)
{
    Tom &tom = obtenerTom();
    if (tiempoTomAturdido > 0 || !jugador.estaVivo()) {
        tom.establecerVelocidad({});
        return;
    }
    Vector2 p = tom.obtenerPosicion();
    const Vector2 antes = p;
    const Vector2 jerry = jugador.obtenerPosicion();
    const ConfiguracionTom ajustes = configuracionTom(dificultad);
    tiempoSinVerJerry += dt;
    esperaCaminoTom -= dt;
    if (esperaCaminoTom <= 0) {
        // Ve a Jerry a corta distancia; al perderlo busca las zonas que recuerda.
        bool visible = p.distancia(jerry) < ajustes.radioVision;
        for (int i = 1; visible && i < 12; ++i) {
            const double t = i / 12.0;
            visible = posicionLibre(cuerpo({p.x + (jerry.x - p.x) * t,
                                            p.y + (jerry.y - p.y) * t}, 8, 16));
        }
        if (visible)
            tiempoSinVerJerry = 0;
        Vector2 objetivo = jerry;
        // En difícil intenta cortar el paso según la dirección y velocidad observadas.
        if (visible && ajustes.anticipacion > 0) {
            const Vector2 velocidad = jugador.obtenerVelocidad();
            const Vector2 predicha{jerry.x + velocidad.x * ajustes.anticipacion,
                                   jerry.y + velocidad.y * ajustes.anticipacion};
            if (posicionLibre(cuerpo(predicha, 12, 12)))
                objetivo = predicha;
        }
        if (!visible && tiempoSinVerJerry <= ajustes.tiempoBusqueda) {
            if (p.distancia(rastroJerry[indiceRastroTom]) < 30
                && indiceRastroTom < rastroJerry.size() - 1)
                ++indiceRastroTom;
            objetivo = rastroJerry[indiceRastroTom];
        } else if (!visible) {
            objetivo = posicionVigilancia;
        }
        caminoTom = buscarCamino(p, objetivo);
        if (caminoTom.isEmpty() && indiceRastroTom < rastroJerry.size() - 1)
            ++indiceRastroTom;
        esperaCaminoTom = ajustes.intervaloDecision;
    }
    double avanceDisponible = ajustes.rapidez * dt;
    while (!caminoTom.isEmpty() && avanceDisponible > 0.000001) {
        const Vector2 objetivo = caminoTom.first();
        const double distancia = p.distancia(objetivo);
        const double paso = std::min(avanceDisponible, distancia);
        if (distancia > 0.001) {
            const Vector2 siguiente{p.x + (objetivo.x - p.x) / distancia * paso,
                                    p.y + (objetivo.y - p.y) / distancia * paso};
            if (posicionLibre(cuerpo(siguiente, 12, 12)))
                p = siguiente;
            else {
                esperaCaminoTom = 0;
                break;
            }
        }
        avanceDisponible -= paso;
        if (distancia <= paso + 0.01)
            caminoTom.removeFirst();
    }
    if (std::abs(p.x - antes.x) > 0.001)
        tom.mover({p.x > antes.x ? 1.0 : -1.0, 0}, 0);
    tom.establecerPosicion(p);
    tom.establecerVelocidad({(p.x - antes.x) / dt, (p.y - antes.y) / dt});
    if (cuerpo(p, 12, 12).intersects(cuerpo(jerry)))
        jugador.recibirDaño(1);
}

bool NivelCabildo::empujarCaja(Jerry &jugador)
{
    const Vector2 p = jugador.obtenerPosicion();
    const QRectF pies = cuerpo(p);
    int cercana = -1;
    double menorDistancia = 19;
    for (int i = 0; i < cajas.size(); ++i) {
        const QRectF &r = cajas[i].rectangulo;
        const double dx = std::max({r.left() - pies.right(), pies.left() - r.right(), 0.0});
        const double dy = std::max({r.top() - pies.bottom(), pies.top() - r.bottom(), 0.0});
        const double distancia = std::hypot(dx, dy);
        if (distancia < menorDistancia) {
            cercana = i;
            menorDistancia = distancia;
        }
    }
    if (cercana < 0)
        return false;
    CajaDeslizable &caja = cajas[cercana];
    const QPointF haciaCaja = caja.rectangulo.center() - pies.center();
    // E junto a una cara orienta a Jerry hacia esa caja, incluso si acababa de girar.
    mirandoX = std::abs(haciaCaja.x()) >= std::abs(haciaCaja.y()) ? (haciaCaja.x() > 0 ? 1 : -1) : 0;
    mirandoY = mirandoX == 0 ? (haciaCaja.y() > 0 ? 1 : -1) : 0;
    caja.velocidad = {mirandoX * 330.0, mirandoY * 330.0};
    ++empujes;
    mostrarMensaje("¡Caja empujada! Se desliza y puede aturdir a Tom.");
    return true;
}

bool NivelCabildo::interactuar(Jerry &jugador)
{
    if (!jugador.estaVivo() || mensajeEntregado)
        return false;
    if (pantallaActual == 4 && jugador.obtenerPosicion().distancia(posicionCabildo) <= 48) {
        mensajeEntregado = true;
        jugador.sumarPuntos(500);
        completar();
        mostrarMensaje("¡Mensaje entregado al cabildo! +500 puntos.");
        return true;
    }
    if (!empujarCaja(jugador))
        mostrarMensaje(pantallaActual == 4 ? "Acércate a la puerta iluminada del cabildo y pulsa E."
                                           : "Sigue el arco dorado hasta la siguiente plaza. E: empujar cajas.");
    return false;
}