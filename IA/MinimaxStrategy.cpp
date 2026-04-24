#include "MinimaxStrategy.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace {
    constexpr double POS_INF =  std::numeric_limits<double>::infinity();
    constexpr double NEG_INF = -std::numeric_limits<double>::infinity();

    // Valor usado para victorias y derrotas terminales. Queremos que
    // la IA prefiera ganar pronto y perder tarde, así que premiamos
    // la profundidad a la que ocurre el resultado.
    constexpr double WIN_SCORE  =  1.0e9;
    constexpr double LOSE_SCORE = -1.0e9;
}

MinimaxStrategy::MinimaxStrategy(int maxDepth_) : maxDepth(maxDepth_) {
    if (maxDepth < 1) maxDepth = 1;
}

Move MinimaxStrategy::chooseMove(const IGameState& state) {
    auto moves = state.getLegalMoves();
    if (moves.empty()) {
        throw std::runtime_error("MinimaxStrategy: no hay movimientos legales");
    }

    const int me = state.currentPlayer();
    double alpha = NEG_INF;
    const double beta = POS_INF;
    double bestValue = NEG_INF;
    Move bestMove = moves.front();

    for (const auto& m : moves) {
        auto child = state.clone();
        child->applyMove(m);
        const double value = minimax(*child, maxDepth - 1, alpha, beta, me);
        if (value > bestValue) {
            bestValue = value;
            bestMove = m;
        }
        alpha = std::max(alpha, bestValue);
    }
    return bestMove;
}

double MinimaxStrategy::minimax(const IGameState& state,
                                int depth,
                                double alpha,
                                double beta,
                                int me) {
    // Casos base.
    if (state.isTerminal()) {
        const int w = state.winner();
        if (w == me)          return WIN_SCORE  - (maxDepth - depth);
        if (w == -1)          return 0.0; // empate
        return LOSE_SCORE + (maxDepth - depth);
    }
    if (depth == 0) {
        return state.evaluate(me);
    }

    auto moves = state.getLegalMoves();
    if (moves.empty()) {
        return state.evaluate(me);
    }

    const bool maximizing = (state.currentPlayer() == me);

    if (maximizing) {
        double value = NEG_INF;
        for (const auto& m : moves) {
            auto child = state.clone();
            child->applyMove(m);
            value = std::max(value, minimax(*child, depth - 1, alpha, beta, me));
            alpha = std::max(alpha, value);
            if (alpha >= beta) break; // poda beta
        }
        return value;
    } else {
        double value = POS_INF;
        for (const auto& m : moves) {
            auto child = state.clone();
            child->applyMove(m);
            value = std::min(value, minimax(*child, depth - 1, alpha, beta, me));
            beta = std::min(beta, value);
            if (alpha >= beta) break; // poda alfa
        }
        return value;
    }
}
