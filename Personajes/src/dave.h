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

    // dejaremos preparadas las funciones de los conjuros (se harán más adelante)
    bool lanzarHechizoCurar(pieza* objetivoaliado);
    bool lanzarHechizoResucitar(pieza* objetivomuerto);
    bool lanzarHechizoTeletransportar(pieza* objetivo, int nuevafila, int nuevacolumna);
};