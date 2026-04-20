// ObstaculoArena.cpp
#include "ObstaculoArena.h"

ObstaculoArena::ObstaculoArena(float startX, float startY, float w, float h)
    : x(startX), y(startY), ancho(w), alto(h) {
    // Configuramos la apariencia básica del obstáculo (luego pondremos texturas aquí)
    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(w, h));
    shape.setFillColor(sf::Color(100, 100, 100)); // Color gris oscuro
}

CajaColisionArena ObstaculoArena::getHitbox() const {
    return { x, y, ancho, alto };
}

void ObstaculoArena::dibujar(sf::RenderWindow& window) {
    window.draw(shape);
}