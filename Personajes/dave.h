#pragma once
#include "terrestre.h"

class dave : public terrestre {
public:
    dave(int bando);
    void atacarEnArena() override;

    // dejaremos preparadas las funciones de los conjuros (se harán más adelante)
    void lanzarHechizoCurar();
};