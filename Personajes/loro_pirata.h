#pragma once
#include "volador.h"

class loropirata : public volador {
public:
    loropirata(int bando);
    void atacarEnArena() override;
};