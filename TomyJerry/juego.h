#ifndef JUEGO_H
#define JUEGO_H
#include "logica/personajes.h"
#include "logica/niveles.h"
#include "logica/eventosonido.h"
#include <memory>
#include <vector>

using namespace std;

enum class EstadoJuego { Portada, Presentacion, SeleccionDificultad, Jugando, Invitacion, Pausa, Derrota, Victoria };

class Juego
{
public:
    Juego();
    void iniciar();
    void mostrarSeleccionNivel();
    void volverAPortada();
    void iniciarNivel(int numero);
    void actualizar(double dt);
    void cambiarNivel();
    void pasarNivel();
    void continuarAlCabildo();
    void guardar();
    void cargar();
    void reiniciar();
    void nuevaPartida();
    void pausar();
    void intentarEntrar();
    void presionarEspacio();
    void activarBurbuja();
    void establecerDireccion(int direccion, int vertical = 0);
    void saltar();
    void correr();
    EstadoJuego obtenerEstado() const;
    Jerry &obtenerJugador();
    const Jerry &obtenerJugador() const;
    Nivel &obtenerNivel();
    const Nivel &obtenerNivel() const;
    double obtenerCamaraX() const;
    void establecerDificultad(Dificultad valor);
    Dificultad obtenerDificultad() const;
    vector<EventoSonido> consumirSonidos();

private:
    void prepararIntento(bool nuevaPartida);
    void actualizarCamara();

    vector<unique_ptr<Nivel>> niveles;
    unique_ptr<Jerry> jugador;
    unique_ptr<Jerry> inicioNivelTienda;
    unique_ptr<Jerry> inicioNivelCabildo;

    int nivelActual = 0;
    bool pausado = false;
    EstadoJuego estado = EstadoJuego::Portada;
    double camaraX = 0;
    vector<EventoSonido> sonidosPendientes;

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