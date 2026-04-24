#pragma once
#include "personaje.h"

class volador : public personaje{

public:
    // Constructor
    volador(int v, int f, int va, int tr, int rm, int b);

    // Aquí sí decimos cómo se mueve en el tablero sobrescribiendo (override) el método del padre
    void moverEnTablero() override;

    // OJO: Todavía no definimos atacarEnArena(). Esta clase sigue siendo abstracta, 
    // lo cual es correcto porque "Terrestre" es un concepto, no un personaje final.

    // --- CUMPLE EL CONTRATO DEL ÁRBITRO (Temporal) ---
    bool esmovimientovalido(int fori, int cori, int fdest, int cdest) override; 
    bool puedeSaltar() const override { return true; }
    
    
  

};

