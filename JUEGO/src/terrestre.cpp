#include "Terrestre.h"
#include <cmath>


// El constructor de Terrestre simplemente le pasa los datos al constructor de Pieza
terrestre::terrestre(int v, int f, int va, int tr, int rm, int b)
    : personaje(v, f, va, tr, rm, b) {
}

// Implementación del movimiento
void terrestre::moverEnTablero() {
    // Aquí irá el código lógico de movimiento más adelante.
    // Por ejemplo: Comprobar que la casilla destino está en línea recta
    // y que no hay obstáculos por el medio.
}


bool terrestre::esmovimientovalido(int forigen, int corigen, int fdestino, int cdestino) {
    // calculamos cuántas casillas hay de diferencia en horizontal y vertical
    int distfila = std::abs(forigen - fdestino);
    int distcol = std::abs(corigen - cdestino);

    // regla 1: no puede moverse en diagonal (una de las dos distancias debe ser 0)
    if (distfila != 0 && distcol != 0) {
        return false;
    }

    // regla 2: la distancia no puede ser mayor a su radiomovimiento
    int distanciatotal = distfila + distcol;
    if (distanciatotal > radioMovimiento) {
        return false;
    }

    // regla 3: no puede "moverse" a la misma casilla en la que ya está
    if (distanciatotal == 0) {
        return false;
    }

    return true; // si pasa todas las pruebas, el movimiento matemático es válido
}