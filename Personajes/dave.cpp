#include "dave.h"
#include <iostream>

dave::dave(int bando)
    : terrestre(200, 40, 4, 4, 3, bando) {
}

void dave::atacarEnArena() {
    std::cout << "¡dave ataca golpeando fuertemente con su sartén!" << std::endl;
}

void dave::lanzarHechizoCurar() {
    std::cout << "dave usa un taco mágico para curar a una planta." << std::endl;
}