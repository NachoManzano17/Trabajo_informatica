#pragma once
#include "terrestre.h"

class allstar : public terrestre {
public:
    allstar(int bando);
    void atacarEnArena() override;
};