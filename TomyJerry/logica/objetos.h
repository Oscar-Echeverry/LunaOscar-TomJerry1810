#ifndef OBJETOS_H
#define OBJETOS_H
#include "entidad.h"
#include <QString>

class Jerry;
class Personaje;
class FisicaJuego;

class Objeto : public Entidad
{
public:
    explicit Objeto(Vector2 posicion, double alto = 30);
    bool estaActivo() const;
    void establecerActivo(bool valor);
    void recoger(Jerry &jugador);
    void actualizar(double dt) override;
    virtual void aplicarEfecto(Jerry &jugador) = 0;
    virtual QString obtenerRecurso() const = 0;

private:
    bool activo = true;
};

class Queso : public Objeto
{
public:
    explicit Queso(Vector2 posicion);
    void aplicarEfecto(Jerry &jugador) override;
    QString obtenerRecurso() const override;
private:
    int valor = 1;
};

class Moneda : public Objeto
{
public:
    explicit Moneda(Vector2 posicion);
    void aplicarEfecto(Jerry &jugador) override;
    QString obtenerRecurso() const override;
private:
    int valor = 10;
};

class Llave : public Objeto
{
public:
    explicit Llave(Vector2 posicion);
    void aplicarEfecto(Jerry &jugador) override;
    QString obtenerRecurso() const override;
private:
    bool obtenida = false;
};

class Florero : public Objeto
{
public:
    explicit Florero(Vector2 posicion = {});
    void aplicarEfecto(Jerry &jugador) override;
    QString obtenerRecurso() const override;
private:
    bool obtenido = false;
};

enum class TipoMovimiento { ESTATICO, OSCILATORIO, FRICCION };

class Obstaculo : public Entidad
{
public:
    Obstaculo(Vector2 centro, double fase = 0, TipoMovimiento tipo = TipoMovimiento::OSCILATORIO,
              double amplitud = 60, double frecuencia = 0.5);
    void actualizar(double dt) override;
    void actualizarFisica(const FisicaJuego &fisica, double dt);
    void recibirImpulso(const Vector2 &impulso);
    void aplicardaño(Personaje &personaje);
    const Vector2 &obtenerCentro() const;
    double obtenerAmplitud() const;
    void establecerTiempo(double valor);

private:
    TipoMovimiento tipoMovimiento;
    int daño = 1;
    Vector2 centro;
    Vector2 eje{1, 0};
    double amplitud = 60;
    double frecuencia = 0.5;
    double tiempo = 0;
    double fase;
};
#endif