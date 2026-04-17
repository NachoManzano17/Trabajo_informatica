#include "soldado.h"
#include <iostream>

// usamos los mismos stats que el cactus para mantener el balance
soldado::soldado(int bando)
    : terrestre(80, 50, 8, 5, 2, bando) {
}

void soldado::atacarEnArena() {
    std::cout << "¡el soldado dispara con su rifle de francotirador!" << std::endl;
}