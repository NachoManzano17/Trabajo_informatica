#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "tablero.h"
#include "GestorRecursos.h"
#include "Movimiento.h"

class VistaJuego {
public:
    // La gran función que dibujará absolutamente todo el tablero y la interfaz
    void dibujarTableroEInterfaz(sf::RenderWindow& window, GestorRecursos& recursos, tablero& mitablero,
        int estadoActual, bando turnoActual,
        int menuX, int menuY, bool hayPersonajeSeleccionado, int fSel, int cSel,
        int offsetX, int offsetY, int tamanoCasilla,
        sf::Clock& relojOscilacion, const std::vector<std::string>& historialConsola,
        sf::Font& fuente, bool eligiendoDificultadIA, int ganadorJuego, bool juegoPausado,
        const std::vector<std::string>& cementerioPlantas, const std::vector<std::string>& cementerioZombis,
        bool animandoMovimientoIA = false, const Movimiento* movimientoIA = nullptr, float progresoMovimientoIA = 0.0f);
};