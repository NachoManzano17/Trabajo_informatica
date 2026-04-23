#include "planta_carnivora.h"
#include <iostream>

// stats: vida=120, fuerza=70 (mucho daño), velAtaque=3 (lento), recarga=6 (muy lenta), mov=4
plantaCarnivora::plantaCarnivora(int bando)
    : teletransporte(120, 70, 3, 6, 4, bando) {
}

void plantaCarnivora::atacarEnArena() {
    std::cout << "¡la planta carnivora da un mordisco letal a corta distancia!" << std::endl;
}