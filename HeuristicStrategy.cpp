#include "HeuristicStrategy.h"

#include <limits>
#include <stdexcept>

Move HeuristicStrategy::chooseMove(const IGameState& state) {
    auto moves = state.getLegalMoves();
    if (moves.empty()) {
        throw std::runtime_error("HeuristicStrategy: no hay movimientos legales");
    }

    const int me = state.currentPlayer();
    double best = -std::numeric_limits<double>::infinity();
    Move bestMove = moves.front();

    for (const auto& m : moves) {
        auto child = state.clone();
        child->applyMove(m);

        // Victoria inmediata: atajo y valor máximo.
        if (child->isTerminal() && child->winner() == me) {
            return m;
        }

        const double score = child->evaluate(me);
        if (score > best) {
            best = score;
            bestMove = m;
        }
    }
    return bestMove;
}
