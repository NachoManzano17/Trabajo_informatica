#include "teletransporte.h"
#include <algorithm>

teletransporte::teletransporte(int v, int f, int va, int tr, int rm, int b)
    : pieza(v, f, va, tr, rm, b) {
}

void teletransporte::moverEnTablero() {
    // aquí irá el código para que desaparezca y aparezca en otra casilla
}

bool teletransporte::esMovimientoValido(int forigen, int corigen, int fdestino, int cdestino) {
    int distfila = std::abs(forigen - fdestino);
    int distcol = std::abs(corigen - cdestino);

    // calculamos la distancia real en la cuadrícula
    int distanciamax = std::max(distfila, distcol);

    // restricción 1: ¡el teletransporte también tiene un límite de distancia!
    if (distanciamax > radiomovimiento) {
        return false;
    }

    // restricción 2: no puedes gastar el turno en reaparecer donde ya estás
    if (distanciamax == 0) {
        return false;
    }

    return true;
}
