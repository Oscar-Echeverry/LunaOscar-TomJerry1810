#ifndef JUEGO_H
#define JUEGO_H
#include "logica/personajes.h"
#include "logica/niveles.h"
#include <memory>
#include <vector>

using namespace std;

enum class EstadoJuego { Presentacion, Jugando, Pausa, Derrota, Victoria };

class Juego
{
public:
    Juego();
    void iniciar();
    void actualizar(double dt);
    void cambiarNivel();
    void pasarNivel();
    void guardar();
    void cargar();
    void reiniciar();
    void nuevaPartida();
    void pausar();
    void intentarEntrar();
    void presionarEspacio();
    void establecerDireccion(int direccion);
    void saltar();
    void correr();
    EstadoJuego obtenerEstado() const;
    Jerry &obtenerJugador();
    const Jerry &obtenerJugador() const;
    Nivel &obtenerNivel();
    const Nivel &obtenerNivel() const;
    double obtenerCamaraX() const;

private:
    void prepararIntento(bool nuevaPartida);
    void actualizarCamara();

    vector<unique_ptr<Nivel>> niveles;
    unique_ptr<Jerry> jugador;
    unique_ptr<Jerry> inicioNivelTienda;

    int nivelActual = 0;
    bool pausado = false;
    EstadoJuego estado = EstadoJuego::Presentacion;
    double camaraX = 0;

    struct PartidaGuardada {
        Jerry jugador;
        Tom tom;
        double tiempo = 0;
        double puntoSeguro = 180;
        vector<bool> objetosActivos;
    };

    unique_ptr<PartidaGuardada> partidaGuardada;
};
#endif
