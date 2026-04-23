#include "superCerebroz.h"
#include <iostream>

// usamos los mismos stats para mantener el equilibrio perfecto
superCerebroz::superCerebroz(int bando)
    : teletransporte(120, 70, 3, 6, 4, bando) {
}

void superCerebroz::atacarEnArena() {
    std::cout << "¡el supercerebroz lanza una ráfaga de heroicos puñetazos!" << std::endl;
}