#pragma once

#include <memory>
#include <vector>
#include "Move.h"

// Interfaz que debe satisfacer el estado del juego para que la IA
// pueda razonar sobre él. El objetivo es que la IA no conozca las
// reglas del ARCHON: solo pregunta, clona y avanza.
//
// Los compañeros NO tienen que modificar su clase de tablero.
// Basta con crear una clase adaptador (GameStateAdapter) que
// derive de IGameState y reenvíe las llamadas al tablero real.
class IGameState {
public:
    virtual ~IGameState() = default;

    // Todos los movimientos legales en la posición actual, para el
    // jugador al que le toca mover.
    virtual std::vector<Move> getLegalMoves() const = 0;

    // Copia profunda del estado. La IA la usa para explorar sin
    // modificar el estado original.
    virtual std::unique_ptr<IGameState> clone() const = 0;

    // Aplica un movimiento y avanza el turno. El movimiento debe
    // pertenecer a getLegalMoves(); no se hace doble validación.
    virtual void applyMove(const Move& m) = 0;

    // ¿Se ha acabado la partida?
    virtual bool isTerminal() const = 0;

    // Identificador del jugador al que le toca mover.
    // Convenio: 0 = Luz, 1 = Oscuridad (o el equivalente en vuestro juego).
    virtual int currentPlayer() const = 0;

    // Ganador si la partida es terminal. -1 si no ha terminado o empate.
    virtual int winner() const = 0;

    // Evaluación heurística rápida de la posición desde el punto
    // de vista del jugador 'perspectivePlayer'.
    // Valores altos -> buena para ese jugador.
    // Valores bajos -> mala para ese jugador.
    //
    // Esta función es el cerebro del juego: la IA solo es tan buena
    // como la heurística que os implementéis. Ver recomendaciones.
    virtual double evaluate(int perspectivePlayer) const = 0;
};
