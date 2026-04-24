#pragma once

#include <random>
#include "AIStrategy.h"

// Nivel fácil.
//
// Elige un movimiento al azar entre los legales, pero descarta los
// peores para que no parezca un jugador que se suicida. En concreto:
//
//  1. Si algún movimiento lleva directamente a ganar, lo toma.
//  2. Descarta los movimientos que caen muy por debajo del mejor
//     (umbral relativo). Así evita jugadas manifiestamente absurdas
//     sin llegar a jugar bien.
//  3. Entre los que sobreviven, escoge uno al azar.
//
// El objetivo es que un niño pequeño pueda ganar sin esfuerzo, pero
// la IA no se autodestruya constantemente.
class RandomStrategy : public AIStrategy {
public:
    explicit RandomStrategy(unsigned seed = std::random_device{}());
    Move chooseMove(const IGameState& state) override;

private:
    std::mt19937 rng;
};
