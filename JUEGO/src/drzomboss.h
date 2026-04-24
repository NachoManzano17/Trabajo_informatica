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

    bool lanzarHechizoCurar(personaje* objetivoaliado);
    bool lanzarHechizoResucitar(personaje* objetivomuerto);
    bool lanzarHechizoTeletransportar(personaje* objetivo, int nuevafila, int nuevacolumna);
    std::string getsimbolo() const override { return "DrZomboss"; }
};