#pragma once

#include "EstrategiaIA.h"

// Solo mira un turno por delante, asi que no preve jugadas combinadas del rival
class EstrategiaHeuristica : public EstrategiaIA {
public:
    Movimiento elegirMovimiento(const IEstadoJuego& estado) override;
};
