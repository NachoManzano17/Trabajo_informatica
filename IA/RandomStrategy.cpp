#include "RandomStrategy.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

RandomStrategy::RandomStrategy(unsigned seed) : rng(seed) {}

Move RandomStrategy::chooseMove(const IGameState& state) {
    auto moves = state.getLegalMoves();
    if (moves.empty()) {
        throw std::runtime_error("RandomStrategy: no hay movimientos legales");
    }

    const int me = state.currentPlayer();

    // Evaluamos el estado resultante de cada movimiento.
    std::vector<std::pair<Move, double>> scored;
    scored.reserve(moves.size());

    double bestScore = -std::numeric_limits<double>::infinity();

    for (const auto& m : moves) {
        auto child = state.clone();
        child->applyMove(m);

        // Victoria inmediata: atajo.
        if (child->isTerminal() && child->winner() == me) {
            return m;
        }

        const double s = child->evaluate(me);
        scored.emplace_back(m, s);
        if (s > bestScore) bestScore = s;
    }

    // Descartamos los movimientos que empeoran mucho respecto al mejor.
    // El umbral es relativo al valor absoluto del mejor. Si todo está
    // cerca, sobrevive casi todo y el azar domina.
    const double margin = 1.0 + 0.25 * std::abs(bestScore);

    std::vector<Move> acceptable;
    acceptable.reserve(scored.size());
    for (const auto& p : scored) {
        if (p.second >= bestScore - margin) {
            acceptable.push_back(p.first);
        }
    }
    if (acceptable.empty()) acceptable.push_back(scored.front().first);

    std::uniform_int_distribution<size_t> dist(0, acceptable.size() - 1);
    return acceptable[dist(rng)];
}
