#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>
#include "tablero.h"
#include "personaje.h"
#include "Dificultad.h"

enum EstadoJuego {
    SELECCIONANDO_PIEZA,
    MENU_ACCION_LIDER,
    MENU_HECHIZOS,
    ESPERANDO_OBJETIVO_CURAR, 
    ESPERANDO_ORIGEN_TELEPORT,
    ESPERANDO_DESTINO_TELEPORT,
    ESPERANDO_DESTINO_RESUCITAR,
    ELEGIR_MUERTO_RESUCITAR
};

class ControladorJuego {
public:
    // Esta mega-función recibe las riendas del juego cada vez que el jugador hace clic
    void procesarClicRaton(int mouseX, int mouseY, int offsetX, int offsetY, int tamanoCasilla,
        EstadoJuego& estadoActual, tablero& mitablero,
        bool& hayPersonajeSeleccionado, int& fSel, int& cSel,
        bando& turnoActual, personaje*& liderSeleccionado,
        int& menuX, int& menuY, int& fOrigenTeleport, int& cOrigenTeleport,
        const std::function<void(std::string)>& logConsola,
        sf::Clock& relojOscilacion, sf::RenderWindow& window,
        bool modoUnJugador, Dificultad dificultadIA,
        const std::function<void()>& comprobarVictoria, std::vector<std::string>& cementerioPlantas,
        std::vector<std::string>& cementerioZombis, std::string& muertoSeleccionado);
 void evaluarVictoria(tablero& mitablero, int& ganadorJuego, std::function<void(const std::string&)> logConsola, class GestorRanking& ranking, bool modoUnJugador);
};
