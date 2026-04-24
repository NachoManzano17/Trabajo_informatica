// ObstaculoArena.cpp
#include "ObstaculoArena.h"

ObstaculoArena::ObstaculoArena(float startX, float startY, float w, float h) {
    posicion.x = startX;
    posicion.y = startY;
    ancho = w;
    alto = h;

    shape.setPosition(posicion.x, posicion.y);
    shape.setSize(sf::Vector2f(w, h));
    shape.setFillColor(sf::Color(100, 100, 100)); // Gris
}

void ObstaculoArena::dibujar(sf::RenderWindow& window) {
    window.draw(shape);
}