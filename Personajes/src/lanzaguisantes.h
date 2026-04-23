#pragma once
#include "terrestre.h"

class lanzaguisantes : public terrestre {
public:
    // fíjate que al constructor solo le pasaremos el 'bando' (ej: 0 para plantas).
    // los stats (vida, fuerza...) los definiremos fijos dentro del .cpp
    lanzaguisantes(int bando);

    // obligatoriamente tenemos que sobrescribir el ataque
    void atacarEnArena() override;
};