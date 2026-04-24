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
    bool lanzarHechizoCurar(personaje* objetivoaliado);
    bool lanzarHechizoResucitar(personaje* objetivomuerto);
    bool lanzarHechizoTeletransportar(personaje* objetivo, int nuevafila, int nuevacolumna);
    std::string getsimbolo() const override { return "Dave"; }
};