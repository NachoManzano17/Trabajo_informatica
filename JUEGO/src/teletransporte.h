#pragma once
#include "personaje.h"

class teletransporte : public personaje {
public:
    teletransporte(int v, int f, int va, int tr, int rm, int b);

    void moverEnTablero() override;

    bool esmovimientovalido(int fori, int cori, int fdest, int cdest) override;
    bool puedeSaltar() const override { return true; }

};