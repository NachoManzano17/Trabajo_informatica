// ObstaculoArena.h
#pragma once
#include "PersonajeBaseArena.h"

class ObstaculoArena {
private:
    float x, y;
    float ancho, alto;
    sf::RectangleShape shape; // Figura de SFML

public:
    ObstaculoArena(float startX, float startY, float w, float h);
    CajaColisionArena getHitbox() const;
    void dibujar(sf::RenderWindow& window);
};