#include "dronajo.h"
#include <iostream>

dronajo::dronajo(int bando)
    : volador(60, 10, 10, 1, 5, bando) {
}

void dronajo::atacarEnArena() {
    std::cout << "¡el dornajo dispara ráfagas de ametralladora de agujas!" << std::endl;
}