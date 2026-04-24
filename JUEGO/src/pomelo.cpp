#include "pomelo.h"
#include <iostream>

pomelo::pomelo(int bando)
    : terrestre(150, 30, 6, 3, 2, bando) {
}

void pomelo::atacarEnArena() {
    std::cout << "¡el pomelo dispara un rayo de plasma continuo a media distancia!" << std::endl;
}