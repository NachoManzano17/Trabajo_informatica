#pragma once

#include "AIStrategy.h"

// Nivel medio.
//
// Aplica cada movimiento legal, pide al estado su evaluación tras
// ese movimiento y se queda con el mejor. Solo mira un turno por
// delante, así que no prevé jugadas combinadas del rival, pero
// tampoco juega a lo tonto.
//
// Da un rival aceptable para un principiante: aprovecha errores
// evidentes del jugador y se mueve hacia posiciones mejores.
class HeuristicStrategy : public AIStrategy {
public:
    Move chooseMove(const IGameState& state) override;
};
