#pragma once

#include "EstrategiaIA.h"

// Nivel dificil.

class EstrategiaMinimax : public EstrategiaIA {
public:
    explicit EstrategiaMinimax(int profundidad = 4);
    Movimiento elegirMovimiento(const IEstadoJuego& estado) override;

private:
    int profundidadMaxima;
    double minimax(const IEstadoJuego& estado,
                   int profundidad,
                   double alfa,
                   double beta,
                   int yo);
};
