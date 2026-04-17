#pragma once
#include "pieza.h"

class teletransporte : public pieza {
public:
    teletransporte(int v, int f, int va, int tr, int rm, int b);

    void moverEnTablero() override;
};