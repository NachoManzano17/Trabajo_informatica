#include "volador.h"
#include <cmath>
#include <algorithm> 


// El constructor de Terrestre simplemente le pasa los datos al constructor de Pieza
volador::volador(int v, int f, int va, int tr, int rm, int b)
    : pieza(v, f, va, tr, rm, b) {
}

// Implementación del movimiento
void volador::moverEnTablero() {
    // Aquí irá el código lógico de movimiento más adelante.
    // Por ejemplo: Comprobar que la casilla destino está en línea recta
    // y que no hay obstáculos por el medio.
}
#include <cmath>
#include <algorithm> 

bool volador::esMovimientoValido(int forigen, int corigen, int fdestino, int cdestino) {
    int distfila = std::abs(forigen - fdestino);
    int distcol = std::abs(corigen - cdestino);

    // en movimiento con diagonales permitidas, la distancia es el valor más grande de los dos
    int distanciamax = std::max(distfila, distcol);

    // regla 1: no superar el radio
    if (distanciamax > radioMovimiento) {
        return false;
    }

    // regla 2: no quedarse quieto
    if (distanciamax == 0) {
        return false;
    }

    return true;
}