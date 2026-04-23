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

    // dejaremos preparadas las funciones de los conjuros (se harán más adelante)
    bool lanzarHechizoCurar(pieza* objetivoaliado);
    bool lanzarHechizoResucitar(pieza* objetivomuerto);
    bool lanzarHechizoTeletransportar(pieza* objetivo, int nuevafila, int nuevacolumna);
};