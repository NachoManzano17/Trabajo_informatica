#include "volador.h"


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