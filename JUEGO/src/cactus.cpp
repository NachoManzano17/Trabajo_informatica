#include "cactus.h"
#include <iostream>

// stats: vida=80, fuerza=50 (mucho daño), velAtaque=8 (proyectil rápido), recarga=5 (lenta), mov=2
cactus::cactus(int bando)
    : terrestre(80, 50, 8, 5, 2, bando) {
}

void cactus::atacarEnArena() {
    std::cout << "¡el cactus dispara un pincho a alta velocidad!" << std::endl;
}