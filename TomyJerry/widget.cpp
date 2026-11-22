#include "widget.h"
#include "./ui_widget.h"
#include "logica/configuracion.h"
#include <QFocusEvent>
#include <QKeyEvent>
#include <QPushButton>
#include <algorithm>
#include <cmath>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->vistaJuego->establecerJuego(&juego);
    connect(ui->paginaInicio, &PantallaInicio::jugar, this, [this] { iniciarIntento(true); });
    connect(ui->botonPausa, &QPushButton::clicked, this, &Widget::cambiarPausa);
    connect(ui->paginaPausa, &PantallaPausa::continuar, this, &Widget::cambiarPausa);
    connect(ui->paginaPausa, &PantallaPausa::reiniciar, this, [this] { iniciarIntento(false); });
    connect(ui->paginaResultado, &PantallaResultado::repetir, this, [this] { iniciarIntento(false); });
    connect(ui->paginaPausa, &PantallaPausa::nuevaPartida, this, [this] { iniciarIntento(true); });
    connect(ui->paginaResultado, &PantallaResultado::nuevaPartida, this, [this] { iniciarIntento(true); });
    temporizador.setTimerType(Qt::PreciseTimer);
    temporizador.setInterval(8);
    connect(&temporizador, &QTimer::timeout, this, &Widget::avanzarTiempo);
    reloj.start();
    temporizador.start();
    actualizarInterfaz();
}

Widget::~Widget() { delete ui; }

void Widget::avanzarTiempo()
{
    const double transcurrido = std::min(reloj.restart() / 1000.0, 0.1);
    if (juego.obtenerEstado() == EstadoJuego::Jugando) {
        acumulador += transcurrido;
        while (acumulador >= Configuracion::pasoTiempo) {
            if (teclas.contains(Qt::Key_Shift))
                juego.correr();
            juego.actualizar(Configuracion::pasoTiempo);
            acumulador -= Configuracion::pasoTiempo;
        }
    } else {
        acumulador = 0;
    }
    actualizarInterfaz();
    ui->vistaJuego->update();
}

void Widget::actualizarInterfaz()
{
    const Jerry &jerry = juego.obtenerJugador();
    const EstadoJuego estado = juego.obtenerEstado();
    QString corazones;
    for (int i = 0; i < 3; ++i)
        corazones += i < jerry.obtenerVidas() ? QStringLiteral("♥ ") : QStringLiteral("♡ ");
    ui->etiquetaVidas->setText("VIDAS: " + corazones.trimmed());
    ui->etiquetaQuesos->setText(QString("QUESOS: %1 / 6").arg(jerry.obtenerQuesos()));
    ui->etiquetaLlave->setText(jerry.tieneLaLlave() ? "LLAVE: SÍ" : "LLAVE: NO");
    ui->etiquetaPuntos->setText(QString("PUNTOS: %1").arg(jerry.obtenerPuntos()));
    QString carrera = "DISPONIBLE";
    double carga = 1;
    if (jerry.obtenerTiempoAgilidad() > 0) {
        carrera = QString("ACTIVA · %1 s").arg(jerry.obtenerTiempoAgilidad(), 0, 'f', 1);
        carga = jerry.obtenerTiempoAgilidad() / 2;
    } else if (jerry.obtenerTiempoRecarga() > 0) {
        carrera = QString("ESPERA · %1 s").arg(int(std::ceil(jerry.obtenerTiempoRecarga())));
        carga = 1 - jerry.obtenerTiempoRecarga() / 6;
    }
    ui->etiquetaCarrera->setText("CARRERA: " + carrera);
    ui->barraCarrera->setValue(int(carga * 1000));
    const Nivel &nivel = juego.obtenerNivel();
    const QStringList &errores = ui->vistaJuego->obtenerErroresRecursos();
    ui->etiquetaAviso->setVisible(!errores.isEmpty() || (estado == EstadoJuego::Jugando && nivel.hayMensaje()));
    ui->etiquetaAviso->setText(errores.isEmpty() ? nivel.obtenerMensaje()
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
                                              jerry.obtenerQuesos(), jerry.obtenerPuntos());
    }
}

void Widget::limpiarTeclas()
{
    teclas.clear();
    juego.establecerDireccion(0);
}

void Widget::iniciarIntento(bool nuevaPartida)
{
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
    if (event->isAutoRepeat()) {
        event->accept();
        return;
    }
    const int tecla = event->key();
    const EstadoJuego estado = juego.obtenerEstado();
    if (estado == EstadoJuego::Presentacion) {
        if (tecla == Qt::Key_Return || tecla == Qt::Key_Enter || tecla == Qt::Key_Space)
            iniciarIntento(true);
    } else if (tecla == Qt::Key_Escape) {
        cambiarPausa();
    } else if (estado != EstadoJuego::Jugando && (tecla == Qt::Key_R || tecla == Qt::Key_N)) {
        iniciarIntento(tecla == Qt::Key_N);
    } else if (estado == EstadoJuego::Jugando) {
        teclas.insert(tecla);
        juego.establecerDireccion(int(teclas.contains(Qt::Key_Right)) - int(teclas.contains(Qt::Key_Left)));
        if (tecla == Qt::Key_Space)
            juego.saltar();
        if (tecla == Qt::Key_Shift)
            juego.correr();
        if (tecla == Qt::Key_E)
            juego.intentarEntrar();
    }
    event->accept();
    actualizarInterfaz();
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        teclas.remove(event->key());
        juego.establecerDireccion(int(teclas.contains(Qt::Key_Right)) - int(teclas.contains(Qt::Key_Left)));
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