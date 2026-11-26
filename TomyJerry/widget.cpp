#include "widget.h"
#include "./ui_widget.h"
#include "logica/configuracion.h"
#include <QFocusEvent>
#include <QKeyEvent>
#include <QPushButton>
#include <QMessageBox>
#include <algorithm>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(std::make_unique<Ui::Widget>())
{
    ui->setupUi(this);
    ui->vistaJuego->establecerJuego(&juego);
    connect(ui->paginaInicio, &PantallaInicio::dificultadCambiada, this, &Widget::cambiarDificultad);
    connect(ui->paginaPausa, &PantallaPausa::dificultadCambiada, this, &Widget::cambiarDificultad);
    connect(ui->paginaInicio, &PantallaInicio::sonidoCambiado, this, &Widget::cambiarSonido);
    connect(ui->paginaPausa, &PantallaPausa::sonidoCambiado, this, &Widget::cambiarSonido);
    connect(&sonidos, &Sonidos::errorReproduccion, this, [this](const QString &mensaje) {
        errorAudio = mensaje;
    });
    connect(ui->paginaInicio, &PantallaInicio::jugar, this, [this] { iniciarIntento(true); });
    connect(ui->botonPausa, &QPushButton::clicked, this, &Widget::cambiarPausa);
    connect(ui->paginaPausa, &PantallaPausa::continuar, this, &Widget::cambiarPausa);
    connect(ui->paginaPausa, &PantallaPausa::reiniciar, this, [this] { iniciarIntento(false); });
    connect(ui->paginaResultado, &PantallaResultado::repetir, this, [this] { iniciarIntento(false); });
    connect(ui->paginaPausa, &PantallaPausa::nuevaPartida, this, [this] { iniciarIntento(true); });
    connect(ui->paginaResultado, &PantallaResultado::nuevaPartida, this, [this] { iniciarIntento(true); });
    connect(ui->paginaResultado, &PantallaResultado::salir, this, &QWidget::close);
    temporizador.setTimerType(Qt::PreciseTimer);
    temporizador.setInterval(8);
    connect(&temporizador, &QTimer::timeout, this, &Widget::avanzarTiempo);
    reloj.start();
    temporizador.start();
    actualizarInterfaz();
}

Widget::~Widget() = default;

void Widget::avanzarTiempo()
{
    try {
        const double transcurrido = std::min(reloj.restart() / 1000.0, 0.1);
        if (juego.obtenerEstado() == EstadoJuego::Jugando) {
            acumulador += transcurrido;
            while (acumulador >= Configuracion::pasoTiempo) {
                if ((teclas.contains(Qt::Key_Shift) || teclas.contains(Qt::Key_CapsLock))
                    && juego.obtenerNivel().obtenerNumero() == 1)
                    juego.correr();
                juego.actualizar(Configuracion::pasoTiempo);
                acumulador -= Configuracion::pasoTiempo;
            }
        } else {
            acumulador = 0;
        }
        actualizarInterfaz();
        ui->vistaJuego->update();
    } catch (const std::exception &error) {
        mostrarError(error);
    }
}

void Widget::actualizarInterfaz()
{
    const Jerry &jerry = juego.obtenerJugador();
    const EstadoJuego estado = juego.obtenerEstado();
    QString corazones;
    for (int i = 0; i < 3; ++i)
        corazones += i < jerry.obtenerVidas() ? QStringLiteral("♥ ") : QStringLiteral("♡ ");
    ui->etiquetaVidas->setText("VIDAS: " + corazones.trimmed());
    const bool nivelTienda = juego.obtenerNivel().obtenerNumero() == 2;
    const bool nivelCabildo = juego.obtenerNivel().obtenerNumero() == 3;
    ui->etiquetaQuesos->setText(nivelCabildo ? "MENSAJE: EN MANOS"
                                : nivelTienda ? (jerry.tieneElFlorero() ? "FLORERO: EN MANOS" : "FLORERO: POR ATRAPAR")
                                              : QString("QUESOS: %1 / 6").arg(jerry.obtenerQuesos()));
    ui->etiquetaLlave->setText((nivelTienda || nivelCabildo) ? "OBJETIVO: E" : (jerry.tieneLaLlave() ? "LLAVE: SÍ" : "LLAVE: NO"));
    ui->etiquetaPuntos->setText(QString("PUNTOS: %1").arg(jerry.obtenerPuntos()));
    QString carrera = "DISPONIBLE";
    double carga = 1;
    if (jerry.obtenerTiempoAgilidad() > 0) {
        carrera = QString("ACTIVA · %1 s").arg(jerry.obtenerTiempoAgilidad(), 0, 'f', 1);
        carga = jerry.obtenerTiempoAgilidad() / 2;
    }
    if (nivelTienda) {
        const auto &tienda = static_cast<const NivelTienda &>(juego.obtenerNivel());
        ui->etiquetaCarrera->setText(QString("TIEMPO: %1 s").arg(tienda.tiempoRestante(), 0, 'f', 1));
        ui->barraCarrera->setValue(int(tienda.tiempoRestante() / 60.0 * 1000));
    } else if (nivelCabildo) {
        const auto &cabildo = static_cast<const NivelCabildo &>(juego.obtenerNivel());
        ui->etiquetaCarrera->setText(QString("PLAZA %1 / 5 · SIN LÍMITE").arg(cabildo.obtenerPantalla() + 1));
        ui->barraCarrera->setValue(estado == EstadoJuego::Victoria ? 1000
                                                                   : cabildo.obtenerPantalla() * 1000 / 5);
    } else {
        ui->etiquetaCarrera->setText("CARRERA: " + carrera);
        ui->barraCarrera->setValue(int(carga * 1000));
    }
    const Nivel &nivel = juego.obtenerNivel();
    ui->subtituloJuego->setText((nivelCabildo ? QString("NIVEL 3 · EL GRITO DE INDEPENDENCIA")
                                 : nivelTienda ? "NIVEL 2 · LA TIENDA DE LLORENTE"
                                               : "NIVEL 1 · LA PLAZA DE SANTAFÉ")
                                + " · " + QString::fromUtf8(nombreDificultad(juego.obtenerDificultad())));
    ui->etiquetaControles->setText(nivelCabildo
                                       ? "FLECHAS / WASD MOVER · E EMPUJAR / ENTREGAR · ROJO: MÁS VIGILADO · VERDE: MENOS VIGILADO · ESC PAUSA"
                                       : nivelTienda
                                             ? "← → MOVER · ↓ AGACHARSE · ESPACIO ATRAPAR · B PROTECCION 5S · MAYUS CORRER · E ENTREGAR"
                                             : "← → CAMINAR     ·     ESPACIO SALTAR     ·     MAYÚS CORRER     ·     E ENTRAR     ·     ESC PAUSA");
    ui->etiquetaControles->setText(ui->etiquetaControles->text() + " · M SONIDO");
    if (estado == EstadoJuego::Victoria)
        ui->etiquetaControles->setText("N NUEVA PARTIDA · ESC SALIR · M SONIDO");
    ui->paginaPausa->establecerNivel(nivel.obtenerNumero());
    const QStringList &errores = ui->vistaJuego->obtenerErroresRecursos();
    ui->etiquetaAviso->setVisible(!errorAudio.isEmpty() || !errores.isEmpty()
                                  || (estado == EstadoJuego::Jugando && nivel.hayMensaje()));
    ui->etiquetaAviso->setText(!errorAudio.isEmpty() ? errorAudio : errores.isEmpty() ? nivel.obtenerMensaje()
                                                                                      : "Faltan imágenes. Revisa Recursos.qrc y vuelve a compilar.");
    ui->botonPausa->setEnabled(estado == EstadoJuego::Jugando || estado == EstadoJuego::Pausa);
    ui->botonPausa->setText(estado == EstadoJuego::Pausa ? "Reanudar · Esc" : "Pausa · Esc");
    ui->panelEstado->setVisible(estado != EstadoJuego::Jugando);
    if (estado == EstadoJuego::Presentacion) {
        ui->panelEstado->setCurrentWidget(ui->paginaInicio);
    } else if (estado == EstadoJuego::Pausa) {
        ui->panelEstado->setCurrentWidget(ui->paginaPausa);
    } else if (estado == EstadoJuego::Victoria || estado == EstadoJuego::Derrota) {
        ui->panelEstado->setCurrentWidget(ui->paginaResultado);
        ui->paginaResultado->mostrarResultado(estado == EstadoJuego::Victoria,
                                              jerry.obtenerQuesos(), jerry.obtenerPuntos(),
                                              juego.obtenerNivel().obtenerNumero());
    }
    sonidos.actualizarMusica(estado == EstadoJuego::Jugando);
    for (EventoSonido evento : juego.consumirSonidos())
        sonidos.reproducir(evento);
}

void Widget::limpiarTeclas()
{
    teclas.clear();
    juego.establecerDireccion(0, 0);
    juego.obtenerJugador().establecerAgachado(false);
    juego.obtenerJugador().establecerCarreraTienda(false);
}

void Widget::iniciarIntento(bool nuevaPartida)
{
    try {
        limpiarTeclas();
        if (nuevaPartida)
            juego.nuevaPartida();
        else
            juego.reiniciar();
        acumulador = 0;
        reloj.restart();
        actualizarInterfaz();
        ui->vistaJuego->update();
        setFocus();
    } catch (const std::exception &error) {
        mostrarError(error);
    }
}

void Widget::cambiarPausa()
{
    juego.pausar();
    limpiarTeclas();
    acumulador = 0;
    reloj.restart();
    actualizarInterfaz();
    ui->vistaJuego->update();
    setFocus();
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    try {
        if (event->isAutoRepeat()) {
            event->accept();
            return;
        }
        const int tecla = event->key();
        const EstadoJuego estado = juego.obtenerEstado();
        if (tecla == Qt::Key_M) {
            cambiarSonido(!sonidos.estaActivo());
        } else if (estado == EstadoJuego::Presentacion) {
            if (tecla == Qt::Key_Return || tecla == Qt::Key_Enter || tecla == Qt::Key_Space)
                iniciarIntento(true);
        } else if (estado == EstadoJuego::Victoria && tecla == Qt::Key_Escape) {
            close();
        } else if (tecla == Qt::Key_Escape) {
            cambiarPausa();
        } else if (estado != EstadoJuego::Jugando && (tecla == Qt::Key_R || tecla == Qt::Key_N)) {
            iniciarIntento(tecla == Qt::Key_N);
        } else if (estado == EstadoJuego::Jugando) {
            teclas.insert(tecla);
            juego.establecerDireccion(
                int(teclas.contains(Qt::Key_Right) || teclas.contains(Qt::Key_D))
                    - int(teclas.contains(Qt::Key_Left) || teclas.contains(Qt::Key_A)),
                int(teclas.contains(Qt::Key_Down) || teclas.contains(Qt::Key_S))
                    - int(teclas.contains(Qt::Key_Up) || teclas.contains(Qt::Key_W)));
            if (tecla == Qt::Key_Down && juego.obtenerNivel().obtenerNumero() == 2)
                juego.obtenerJugador().establecerAgachado(true);
            if (tecla == Qt::Key_Space)
                juego.presionarEspacio();
            if (tecla == Qt::Key_B)
                juego.activarBurbuja();
            if (tecla == Qt::Key_Shift || tecla == Qt::Key_CapsLock) {
                if (juego.obtenerNivel().obtenerNumero() == 2)
                    juego.obtenerJugador().establecerCarreraTienda(true);
                else if (juego.obtenerNivel().obtenerNumero() == 1)
                    juego.correr();
            }
            if (tecla == Qt::Key_E)
                juego.intentarEntrar();
        }
        event->accept();
        actualizarInterfaz();
    } catch (const std::exception &error) {
        mostrarError(error);
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        teclas.remove(event->key());
        juego.establecerDireccion(
            int(teclas.contains(Qt::Key_Right) || teclas.contains(Qt::Key_D))
                - int(teclas.contains(Qt::Key_Left) || teclas.contains(Qt::Key_A)),
            int(teclas.contains(Qt::Key_Down) || teclas.contains(Qt::Key_S))
                - int(teclas.contains(Qt::Key_Up) || teclas.contains(Qt::Key_W)));
        if (event->key() == Qt::Key_Down)
            juego.obtenerJugador().establecerAgachado(false);
        if ((event->key() == Qt::Key_Shift || event->key() == Qt::Key_CapsLock)
            && juego.obtenerNivel().obtenerNumero() == 2)
            juego.obtenerJugador().establecerCarreraTienda(false);
    }
    event->accept();
}

void Widget::focusOutEvent(QFocusEvent *event)
{
    if (juego.obtenerEstado() == EstadoJuego::Jugando)
        juego.pausar();
    limpiarTeclas();
    actualizarInterfaz();
    QWidget::focusOutEvent(event);
}

void Widget::cambiarDificultad(Dificultad valor)
{
    try {
        juego.establecerDificultad(valor);
        ui->paginaInicio->establecerDificultad(valor);
        ui->paginaPausa->establecerDificultad(valor);
        actualizarInterfaz();
    } catch (const std::exception &error) {
        mostrarError(error);
    }
}

void Widget::cambiarSonido(bool activo)
{
    sonidos.establecerActivo(activo);
    ui->paginaInicio->establecerSonido(activo);
    ui->paginaPausa->establecerSonido(activo);
    if (!activo)
        errorAudio.clear();
}

void Widget::mostrarError(const std::exception &error)
{
    temporizador.stop();
    if (juego.obtenerEstado() == EstadoJuego::Jugando)
        juego.pausar();
    limpiarTeclas();
    sonidos.actualizarMusica(false);
    QMessageBox::critical(this, "El juego se ha detenido", QString::fromUtf8(error.what())
                                                               + "\nPuedes reiniciar el nivel o comenzar una partida nueva.");
    acumulador = 0;
    reloj.restart();
    actualizarInterfaz();
    temporizador.start();
}