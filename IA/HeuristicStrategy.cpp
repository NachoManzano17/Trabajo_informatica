#include "HeuristicStrategy.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

Move HeuristicStrategy::chooseMove(const IGameState& state) {
    auto moves = state.getLegalMoves();
    if (moves.empty()) {
        throw std::runtime_error("HeuristicStrategy: no hay movimientos legales");
    }

    const int me = state.currentPlayer();

    struct ScoredMove {
        Move move;
        double score;
    };

    std::vector<ScoredMove> scored;
    scored.reserve(moves.size());

    for (const auto& m : moves) {
        auto child = state.clone();
        child->applyMove(m);

        if (child->isTerminal() && child->winner() == me) {
            return m;
        }

        scored.push_back({ m, child->evaluate(me) });
    }

    std::sort(scored.begin(), scored.end(), [](const ScoredMove& a, const ScoredMove& b) {
        return a.score > b.score;
    });

    // Nivel medio: no siempre coge exactamente el mismo movimiento.
    // Escoge entre los mejores candidatos para que use más variedad de piezas,
    // pero sin hacer jugadas claramente malas.
    static std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    const size_t limite = std::min<size_t>(scored.size(), 4);
    std::uniform_int_distribution<size_t> dist(0, limite - 1);
    return scored[dist(rng)].move;
}
