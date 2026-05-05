#pragma once

#include "IGameState.h"
#include "tablero.h"

class GameStateAdapter : public IGameState {
public:
    GameStateAdapter(const tablero& tableroReal, bando turnoActual);

    std::vector<Move> getLegalMoves() const override;
    std::unique_ptr<IGameState> clone() const override;
    void applyMove(const Move& m) override;
    bool isTerminal() const override;
    int currentPlayer() const override;
    int winner() const override;
    double evaluate(int perspectivePlayer) const override;

    const tablero& getTableroSimulado() const { return estado; }

private:
    tablero estado;
    bando turno;

    static int jugadorDesdeBando(bando equipo);
    static bando bandoDesdeJugador(int jugador);
    static double valorPieza(const personaje* p);
    double puntuacionBando(bando equipo) const;
};
