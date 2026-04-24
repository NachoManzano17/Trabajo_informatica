#pragma once
#include "terrestre.h"

class dave : public terrestre {

private:
private:
    // variables para recordar si el hechizo ya se gastó
    bool hechizocurarusado;
    bool hechizoresucitarusado;
    bool hechizoteletransportarusado;


public:
    dave(int bando);
    void atacarEnArena() override;

    
    bool puedeCurar();
    void consumirCurar();

    bool puedeResucitar();
    void consumirResucitar();

    bool puedeTeletransportar();
    void consumirTeletransportar();

    std::string getsimbolo() const override { return "Dave"; }
};
