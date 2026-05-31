#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <algorithm>
#include <stdexcept>

#include "Movimiento.h"
#include "Dificultad.h"
#include "IEstadoJuego.h"
#include "EstrategiaIA.h"
#include "tablero.h"
#include "personaje.h"




// Fachada publica del modulo de IA.
//
// Uso tipico desde el codigo del juego:
//
//     JugadorIA ia(Dificultad::Dificil);
//     Movimiento m = ia.elegirMovimiento(estadoActual);
//     estadoActual.aplicarMovimiento(m);
//
// Internamente delega en una estrategia concreta (Aleatoria,
// Heuristica o Minimax) segun el nivel de dificultad. Cambiar la
// dificultad en mitad de partida es valido.
class AdaptadorEstadoJuego;
class JugadorIA {
public:
    explicit JugadorIA(Dificultad d = Dificultad::Normal, int profundidadDificil = 4);

    Movimiento elegirMovimiento(const IEstadoJuego& estado);

    void cambiarDificultad(Dificultad d, int profundidadDificil = 4);
    Dificultad dificultad() const { return dificultadActual; }

    bool jugarTurno(tablero& mitablero, bando& turnoActual, sf::RenderWindow& window,
        const std::function<void(std::string)>& logConsola,
        sf::Clock& relojOscilacion,
        const std::function<void()>& comprobarVictoria);

    bool prepararMovimientoVisual(tablero& mitablero, bando turnoActual, Movimiento& movimientoPreparado,
        const std::function<void(std::string)>& logConsola);

    bool resolverMovimientoPreparado(tablero& mitablero, bando& turnoActual, sf::RenderWindow& window,
        const Movimiento& movimientoPreparado,
        const std::function<void(std::string)>& logConsola,
        sf::Clock& relojOscilacion,
        const std::function<void()>& comprobarVictoria);
    

private:
    std::unique_ptr<EstrategiaIA> estrategia;
    Dificultad dificultadActual;

    static std::unique_ptr<EstrategiaIA> construir(Dificultad d, int profundidadDificil);
    double valorPiezaParaIA(personaje* p);
    bool esMovimientoDeAtaque(tablero& t, const Movimiento& m, bando equipoIA);
    Movimiento elegirMovimientoIAConPresion(tablero& t, const std::vector<Movimiento>& legales, const AdaptadorEstadoJuego& estadoIA);
};

// Forma libre equivalente, util si no quereis llevar estado.
Movimiento elegirMovimiento(const IEstadoJuego& estado, Dificultad d, int profundidadDificil = 4);
