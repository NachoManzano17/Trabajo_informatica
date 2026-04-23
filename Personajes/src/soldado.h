#pragma once
#include "terrestre.h"

class soldado : public terrestre {
public:
    soldado(int bando);
    void atacarEnArena() override;
};