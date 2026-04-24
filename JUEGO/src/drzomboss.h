#pragma once
#include "terrestre.h"

class drzomboss : public terrestre {

private:
    bool hechizocurarusado;
    bool hechizoresucitarusado;
    bool hechizoteletransportarusado;


public:
    drzomboss(int bando);
    void atacarEnArena() override;

    bool puedeCurar();
    void consumirCurar();

    bool puedeResucitar();
    void consumirResucitar();

    bool puedeTeletransportar();
    void consumirTeletransportar();
    std::string getsimbolo() const override { return "DrZomboss"; }
};
