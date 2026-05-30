#pragma once

#include <random>
#include "EstrategiaIA.h"

class EstrategiaAleatoria : public EstrategiaIA {
public:
    explicit EstrategiaAleatoria(unsigned semilla = std::random_device{}());
    Movimiento elegirMovimiento(const IEstadoJuego& estado) override;

private:
    std::mt19937 rng;
};

