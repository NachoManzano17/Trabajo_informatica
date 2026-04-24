#pragma once
#include "terrestre.h"

class pomelo : public terrestre {
public:
    pomelo(int bando);
    void atacarEnArena() override;
    std::string getsimbolo() const override { return "Pomelo"; }
};