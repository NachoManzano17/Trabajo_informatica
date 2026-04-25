#pragma once
#include "Vector2D.h"

class ObstaculoArena {
private:
    Vector2D posicion;
    float ancho;
    float alto;

public:
    // Solo declaramos el constructor, no le damos el cuerpo aquí
    ObstaculoArena(float startX, float startY, float w, float h);

    Vector2D getPosicion() { return posicion; }
    float getAncho() { return ancho; }
    float getAlto() { return alto; }
};