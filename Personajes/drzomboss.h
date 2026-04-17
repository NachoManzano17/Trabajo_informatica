#pragma once
#include "terrestre.h"

class drzomboss : public terrestre {
public:
    drzomboss(int bando);
    void atacarEnArena() override;

    // dejaremos preparadas las funciones de los conjuros (se harán más adelante)
    void lanzarHechizoCurar();
};