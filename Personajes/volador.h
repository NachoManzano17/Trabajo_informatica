#pragma once
#include "pieza.h"

class volador : public pieza{

public:
    // Constructor
    volador(int v, int f, int va, int tr, int rm, int b);

    // Aquí sí decimos cómo se mueve en el tablero sobrescribiendo (override) el método del padre
    void moverEnTablero() override;

    // OJO: Todavía no definimos atacarEnArena(). Esta clase sigue siendo abstracta, 
    // lo cual es correcto porque "Terrestre" es un concepto, no un personaje final.
};

