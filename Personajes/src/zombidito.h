#pragma once
#include "terrestre.h"

class zombidito : public terrestre {
public:
    zombidito(int bando);
    void atacarEnArena() override;
};