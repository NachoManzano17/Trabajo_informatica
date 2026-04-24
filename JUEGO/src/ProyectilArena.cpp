// ProyectilArena.cpp
#include "ProyectilArena.h"
#include <math.h>

ProyectilArena::ProyectilArena(float startX, float startY, float dirX, float dirY, float dmg, bool dePlanta) {
    posicion.x = startX;
    posicion.y = startY;
    velocidad.x = dirX;
    velocidad.y = dirY;
    danio = dmg;
    esDePlanta = dePlanta;
    activo = true;

    shape.setRadius(5.0f);
    shape.setScale(2.0f, 1.0f);
    shape.setOrigin(5.0f, 5.0f);
    shape.setPosition(posicion.x, posicion.y);

    float angulo = (float)atan2(velocidad.y, velocidad.x) * 180.0f / 3.14159265f;
    shape.setRotation(angulo);

    if (esDePlanta) {
        shape.setFillColor(sf::Color::Green);
    }
    else {
        shape.setFillColor(sf::Color::Magenta);
    }
}

void ProyectilArena::actualizar() {
    if (activo == false) return;

    posicion.x += velocidad.x;
    posicion.y += velocidad.y;
    shape.setPosition(posicion.x, posicion.y);
}

void ProyectilArena::dibujar(sf::RenderWindow& window) {
    if (activo == false) return;
    window.draw(shape);
}