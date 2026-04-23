#include "teletransporte.h"

teletransporte::teletransporte(int v, int f, int va, int tr, int rm, int b)
    : pieza(v, f, va, tr, rm, b) {
}

void teletransporte::moverEnTablero() {
    // aquí irá el código para que desaparezca y aparezca en otra casilla
}
bool teletransporte::esMovimientoValido(int forigen, int corigen, int fdestino, int cdestino) {
    // regla 1: el único movimiento ilegal para un teletransporte es quedarse en el mismo sitio
    if (forigen == fdestino && corigen == cdestino) {
        return false;
    }

    // a cualquier otra casilla de la matriz, la respuesta siempre es sí
    return true;
}