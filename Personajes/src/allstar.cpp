#include "allstar.h"
#include <iostream>

allstar::allstar(int bando)
    : terrestre(150, 30, 6, 3, 2, bando) {
}

void allstar::atacarEnArena() {
    std::cout << "¡el all star lanza un proyectil pesado de fútbol americano!" << std::endl;
}