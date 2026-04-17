#pragma once
#include "teletransporte.h"

class plantaCarnivora : public teletransporte {
public:
    plantaCarnivora(int bando);
    void atacarEnArena() override;
};