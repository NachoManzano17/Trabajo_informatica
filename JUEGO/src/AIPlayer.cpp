#include "AIPlayer.h"

#include "HeuristicStrategy.h"
#include "MinimaxStrategy.h"
#include "RandomStrategy.h"

std::unique_ptr<AIStrategy> AIPlayer::build(Difficulty d, int hardDepth) {
    switch (d) {
        case Difficulty::Easy:
            return std::make_unique<RandomStrategy>();
        case Difficulty::Medium:
            return std::make_unique<HeuristicStrategy>();
        case Difficulty::Hard:
            return std::make_unique<MinimaxStrategy>(hardDepth);
    }
    return std::make_unique<HeuristicStrategy>();
}

AIPlayer::AIPlayer(Difficulty d, int hardDepth)
    : strategy(build(d, hardDepth)), currentDifficulty(d) {}

Move AIPlayer::chooseMove(const IGameState& state) {
    return strategy->chooseMove(state);
}

void AIPlayer::setDifficulty(Difficulty d, int hardDepth) {
    currentDifficulty = d;
    strategy = build(d, hardDepth);
}

Move chooseMove(const IGameState& state, Difficulty d, int hardDepth) {
    AIPlayer p(d, hardDepth);
    return p.chooseMove(state);
}
