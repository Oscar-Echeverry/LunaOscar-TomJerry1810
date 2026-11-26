#ifndef DIFICULTAD_H
#define DIFICULTAD_H

#include <stdexcept>

enum class Dificultad { Facil, Normal, Dificil };

struct ConfiguracionTom {
    double rapidez;
    double radioVision;
    double intervaloDecision;
    double anticipacion;
    double rapidezCalle;
    double visionCalle;
    double ventajaInicial;
    double tiempoBusqueda;
};

inline ConfiguracionTom configuracionTom(Dificultad dificultad)
{
    switch (dificultad) {
    case Dificultad::Facil: return {95, 160, 0.90, 0, 160, 380, 6, 2.5};
    case Dificultad::Normal: return {150, 300, 0.35, 0.25, 245, 850, 3, 6};
    case Dificultad::Dificil: return {210, 600, 0.10, 0.85, 315, 1400, 1, 12};
    }
    throw std::invalid_argument("La dificultad seleccionada no es válida.");
}

inline double factorPeligros(Dificultad dificultad)
{
    switch (dificultad) {
    case Dificultad::Facil: return 0.65;
    case Dificultad::Normal: return 1.0;
    case Dificultad::Dificil: return 1.4;
    }
    throw std::invalid_argument("La dificultad seleccionada no es válida.");
}

inline const char *nombreDificultad(Dificultad dificultad)
{
    switch (dificultad) {
    case Dificultad::Facil: return "FÁCIL";
    case Dificultad::Normal: return "NORMAL";
    case Dificultad::Dificil: return "DIFÍCIL";
    }
    throw std::invalid_argument("La dificultad seleccionada no es válida.");
}
#endif