#pragma once
#include "volador.h"

class dronajo : public volador {
public:
    dronajo(int bando);
    void atacarEnArena() override;
    std::string getsimbolo() const override { return "Dronajo"; }
};