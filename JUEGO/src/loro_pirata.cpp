#include "loro_pirata.h"
#include <iostream>

loropirata::loropirata(int bando)
    : volador(60, 10, 10, 1, 5, bando) {
}

void loropirata::atacarEnArena() {
    std::cout << "¡el loro pirata dispara ráfagas de láser desde el cielo!" << std::endl;
}