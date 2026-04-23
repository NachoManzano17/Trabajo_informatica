#pragma once
#include "pieza.h" // Necesitamos incluir al padre

class terrestre : public pieza {
public:
    // Constructor
    terrestre(int v, int f, int va, int tr, int rm, int b);

    // Aquí sí decimos cómo se mueve en el tablero sobrescribiendo (override) el método del padre
    void moverEnTablero() override;

    bool esMovimientoValido(int forigen, int corigen, int fdestino, int cdestino) override; 
    // OJO: Todavía no definimos atacarEnArena(). Esta clase sigue siendo abstracta, 
    // lo cual es correcto porque "Terrestre" es un concepto, no un personaje final.
};