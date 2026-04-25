// ProyectilArena.h
#pragma once
#include <SFML/Graphics.hpp>
#include "Vector2D.h"

class ProyectilArena {
private:
    Vector2D posicion;
    Vector2D velocidad;
    float danio;
    bool esDePlanta;
    bool activo;
    sf::CircleShape shape;

public:
    ProyectilArena(float startX, float startY, float dirX, float dirY, float dmg, bool dePlanta);

    void actualizar();
    void dibujar(sf::RenderWindow& window);

    Vector2D getPosicion() { return posicion; }
    bool estaActivo() { return activo; }
    void destruir() { activo = false; }
    float getDanio() { return danio; }
    bool esAliadoDePlanta() { return esDePlanta; }
};