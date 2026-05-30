#include "terrestre.h"
#include <cmath>


// El constructor de Terrestre simplemente le pasa los datos al constructor de Pieza
terrestre::terrestre(int v, int f, int va, int tr, int rm, int b)
    : personaje(v, f, va, tr, rm, b) {
}

void terrestre::moverEnTablero() {
   
}
bool terrestre::esmovimientovalido(int forigen, int corigen, int fdestino, int cdestino) {
 
    int distfila = std::abs(forigen - fdestino);
    int distcol = std::abs(corigen - cdestino);

    // no puede moverse en diagonal 
    if (distfila != 0 && distcol != 0) {
        return false;
    }

    // la distancia no puede ser mayor a su radiomovimiento
    int distanciatotal = distfila + distcol;
    if (distanciatotal > radioMovimiento) {
        return false;
    }

    // no puede "moverse" a la misma casilla en la que ya está
    if (distanciatotal == 0) {
        return false;
    }

    return true; 
}
