// ObstaculoArena.h
#pragma once
#include <SFML/Graphics.hpp>
#include "Vector2D.h"

class ObstaculoArena {
private:
    Vector2D posicion;
    float ancho;
    float alto;
    sf::RectangleShape shape;

public:
    ObstaculoArena(float startX, float startY, float w, float h);
    Vector2D getPosicion() { return posicion; }
    void dibujar(sf::RenderWindow& window);
};