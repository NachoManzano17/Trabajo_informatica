#include "zombidito.h"
#include <iostream>

// usamos exactamente los mismos valores que en lanzaguisantes para equilibrarlo
zombidito::zombidito(int bando)
    : terrestre(100, 20, 5, 2, 3, bando) {
}

void zombidito::atacarEnArena() {
    std::cout << "¡el zombidito dispara su arma a media distancia!" << std::endl;
}