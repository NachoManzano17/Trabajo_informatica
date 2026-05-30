#pragma once

#include "IEstadoJuego.h"
#include "Movimiento.h"

// Interfaz comun a los tres niveles de dificultad.

class EstrategiaIA {
public:
    virtual ~EstrategiaIA() = default;

    
    virtual Movimiento elegirMovimiento(const IEstadoJuego& estado) = 0;
};
