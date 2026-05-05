#pragma once

#include <memory>
#include "AIStrategy.h"
#include "Difficulty.h"
#include "IGameState.h"
#include "Move.h"

// Fachada pública del módulo de IA.
//
// Uso típico desde el código del juego:
//
//     AIPlayer ia(Difficulty::Hard);
//     Move m = ia.chooseMove(estadoActual);
//     estadoActual.applyMove(m);
//
// Internamente delega en una estrategia concreta (Random, Heuristic
// o Minimax) según el nivel de dificultad. Cambiar la dificultad
// en mitad de partida es válido.
class AIPlayer {
public:
    explicit AIPlayer(Difficulty d = Difficulty::Medium, int hardDepth = 4);

    Move chooseMove(const IGameState& state);

    void setDifficulty(Difficulty d, int hardDepth = 4);
    Difficulty difficulty() const { return currentDifficulty; }

private:
    std::unique_ptr<AIStrategy> strategy;
    Difficulty currentDifficulty;

    static std::unique_ptr<AIStrategy> build(Difficulty d, int hardDepth);
};

// Forma libre equivalente, útil si no queréis llevar estado.
Move chooseMove(const IGameState& state, Difficulty d, int hardDepth = 4);
