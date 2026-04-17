#pragma once
#include "volador.h"

class dronajo : public volador {
public:
    dronajo(int bando);
    void atacarEnArena() override;
};