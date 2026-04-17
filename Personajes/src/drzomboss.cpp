
#include "drzomboss.h"
#include <iostream>

drzomboss::drzomboss(int bando)
    : terrestre(200, 40, 4, 4, 3, bando) {
}

void drzomboss::atacarEnArena() {
    std::cout << "¡el dr. zomboss da un puñetazo devastador a corta distancia!" << std::endl;
}

void drzomboss::lanzarHechizoCurar() {
    std::cout << "el dr. zomboss usa ciencia loca para reparar a un zombi." << std::endl;
}