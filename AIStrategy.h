#pragma once

#include "IGameState.h"
#include "Move.h"

// Interfaz común a los tres niveles de dificultad.
// Cada nivel es una implementación concreta de AIStrategy.
// AIPlayer escoge cuál usar y delega en ella.
class AIStrategy {
public:
    virtual ~AIStrategy() = default;

    // Devuelve el movimiento elegido para el jugador que tiene
    // el turno en 'state'. No modifica 'state'.
    virtual Move chooseMove(const IGameState& state) = 0;
};
