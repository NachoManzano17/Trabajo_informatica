#pragma once

#include <memory>
#include <vector>
#include "Movimiento.h"

class IEstadoJuego {
public:
    virtual ~IEstadoJuego() = default;

   
    virtual std::vector<Movimiento> getMovimientosLegales() const = 0;
   
    virtual std::unique_ptr<IEstadoJuego> clonar() const = 0;

   
    virtual void aplicarMovimiento(const Movimiento& m) = 0;

   
    virtual bool esTerminal() const = 0;

   
    virtual int jugadorActual() const = 0;

    virtual int ganador() const = 0;

   
    virtual double evaluar(int jugadorPerspectiva) const = 0;
};

