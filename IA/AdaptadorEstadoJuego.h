#pragma once

#include "IEstadoJuego.h"
#include "tablero.h"

class AdaptadorEstadoJuego : public IEstadoJuego {
public:
    AdaptadorEstadoJuego(const tablero& tableroReal, bando turnoActual);

    std::vector<Movimiento> getMovimientosLegales() const override;
    std::unique_ptr<IEstadoJuego> clonar() const override;
    void aplicarMovimiento(const Movimiento& m) override;
    bool esTerminal() const override;
    int jugadorActual() const override;
    int ganador() const override;
    double evaluar(int jugadorPerspectiva) const override;

    const tablero& getTableroSimulado() const { return estado; }

private:
    tablero estado;
    bando turno;

    static int jugadorDesdeBando(bando equipo);
    static bando bandoDesdeJugador(int jugador);
    static double valorPieza(const personaje* p);
    double puntuacionBando(bando equipo) const;
};
