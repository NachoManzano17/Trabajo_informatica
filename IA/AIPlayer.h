#pragma once

#include <memory>
#include "AIStrategy.h"
#include "Difficulty.h"
#include "IGameState.h"
#include "Move.h"


class AIPlayer {
public:
    explicit AIPlayer(Difficulty d = Difficulty::Medium, int hardDepth = 4);

    Move chooseMove(const IGameState& state);

    void setDifficulty(Difficulty d, int hardDepth = 4);
    Difficulty difficulty() const { return currentDifficulty; }

private:
    std::unique_ptr<AIStrategy> strategy;
    Difficulty currentDifficulty;

    static std::unique_ptr<AIStrategy> build(Difficulty d, int hardDepth);
};


Move chooseMove(const IGameState& state, Difficulty d, int hardDepth = 4);
