#pragma once

#include "AIStrategy.h"

// Nivel difícil.
//
// Minimax con poda alfa-beta. La profundidad se fija en el
// constructor. Una profundidad de 3 o 4 suele ser un buen punto
// de equilibrio entre calidad y tiempo de respuesta para juegos
// de tablero pequeños con factor de ramificación moderado.
//
// La IA supone que el oponente también juega óptimamente. Para
// ARCHON esto no es estrictamente cierto (hay combates con cierta
// aleatoriedad), pero la aproximación funciona si la evaluación
// incorpora el valor esperado del combate.
class MinimaxStrategy : public AIStrategy {
public:
    explicit MinimaxStrategy(int maxDepth = 4);
    Move chooseMove(const IGameState& state) override;

private:
    int maxDepth;

    // Devuelve el valor del estado desde el punto de vista de 'me'.
    double minimax(const IGameState& state,
                   int depth,
                   double alpha,
                   double beta,
                   int me);
};
