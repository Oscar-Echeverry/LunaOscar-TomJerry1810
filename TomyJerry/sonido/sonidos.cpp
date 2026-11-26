#include "sonidos.h"
#include <QFile>
#include <QUrl>
#include <stdexcept>

Sonidos::Sonidos(QObject *parent) : QObject(parent)
{
    cargar(musica, ":/sonidos/fondo.wav", 0.32f);
    musica.setLoopCount(QSoundEffect::Infinite);
    const char *archivos[] = {"recoger", "queso", "moneda", "dano", "empujar", "victoria", "salto", "avanzar"};
    for (size_t i = 0; i < efectos.size(); ++i) {
        efectos[i] = std::make_unique<QSoundEffect>();
        cargar(*efectos[i], QString(":/sonidos/%1.wav").arg(archivos[i]), 0.65f);
    }
}

void Sonidos::validarWav(const QString &ruta)
{
    QFile archivo(ruta);
    if (!archivo.open(QIODevice::ReadOnly))
        throw std::runtime_error(QString("No se puede abrir el sonido: %1").arg(ruta).toStdString());
    const QByteArray cabecera = archivo.read(44);
    if (cabecera.size() != 44 || !cabecera.startsWith("RIFF")
        || cabecera.mid(8, 4) != "WAVE" || cabecera.mid(12, 4) != "fmt "
        || cabecera.mid(36, 4) != "data" || archivo.size() <= 44)
        throw std::runtime_error(QString("El archivo de sonido WAV está dañado: %1").arg(ruta).toStdString());
}

void Sonidos::cargar(QSoundEffect &efecto, const QString &ruta, float volumen)
{
    validarWav(ruta);
    connect(&efecto, &QSoundEffect::statusChanged, this, [this, &efecto, ruta] {
        if (efecto.status() == QSoundEffect::Error)
            emit errorReproduccion("No se pudo reproducir " + ruta + ". Revisa la salida de audio.");
        if (&efecto == &musica && efecto.status() == QSoundEffect::Ready)
            actualizarMusica(enPartida);
    });
    efecto.setSource(QUrl("qrc" + ruta));
    efecto.setVolume(volumen);
}

void Sonidos::reproducir(EventoSonido evento)
{
    const size_t indice = size_t(evento);
    if (indice >= efectos.size())
        throw std::invalid_argument("El evento de sonido no existe.");
    if (activo && efectos[indice]->status() == QSoundEffect::Ready)
        efectos[indice]->play();
}

void Sonidos::actualizarMusica(bool jugando)
{
    const bool estabaJugando = enPartida;
    enPartida = jugando;
    if (activo && jugando) {
        if (musica.status() == QSoundEffect::Ready && !musica.isPlaying())
            musica.play();
    } else {
        musica.stop();
        if (!activo || estabaJugando)
            for (const auto &efecto : efectos)
                efecto->stop();
    }
}

void Sonidos::establecerActivo(bool valor)
{
    activo = valor;
    actualizarMusica(enPartida);
}

bool Sonidos::estaActivo() const { return activo; }