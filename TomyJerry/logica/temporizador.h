#ifndef TEMPORIZADOR_H
#define TEMPORIZADOR_H

class Temporizador
{
public:
    void iniciar(double segundos);
    void actualizar(double dt);
    bool termino() const;
    double obtenerTiempoRestante() const;

private:
    double tiempoRestante = 0;
    bool activo = false;
};
#endif
