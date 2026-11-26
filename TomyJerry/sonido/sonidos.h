#ifndef SONIDOS_H
#define SONIDOS_H

#include "logica/eventosonido.h"
#include <QObject>
#include <QSoundEffect>
#include <array>
#include <memory>

class Sonidos : public QObject
{
    Q_OBJECT
public:
    explicit Sonidos(QObject *parent = nullptr);
    static void validarWav(const QString &ruta);
    void reproducir(EventoSonido evento);
    void actualizarMusica(bool jugando);
    void establecerActivo(bool valor);
    bool estaActivo() const;

signals:
    void errorReproduccion(const QString &mensaje);

private:
    void cargar(QSoundEffect &efecto, const QString &ruta, float volumen);
    QSoundEffect musica;
    std::array<std::unique_ptr<QSoundEffect>, size_t(EventoSonido::Cantidad)> efectos;
    bool activo = true;
    bool enPartida = false;
};
#endif