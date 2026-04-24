#pragma once
#include "terrestre.h"

class cactus : public terrestre {
public:
    cactus(int bando);
    void atacarEnArena() override;
    std::string getsimbolo() const override { return "Cactus"; }
};