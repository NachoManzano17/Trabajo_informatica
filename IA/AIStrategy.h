#pragma once

#include "IGameState.h"
#include "Move.h"


class AIStrategy {
public:
    virtual ~AIStrategy() = default;

    
    virtual Move chooseMove(const IGameState& state) = 0;
};
