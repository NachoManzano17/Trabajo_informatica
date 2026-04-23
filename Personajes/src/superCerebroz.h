#pragma once
#include "teletransporte.h"

class superCerebroz : public teletransporte {
public:
    superCerebroz(int bando);
    void atacarEnArena() override;
};