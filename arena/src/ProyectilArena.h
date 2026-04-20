// ProyectilArena.h
#pragma once
#include "PersonajeBaseArena.h"

class ProyectilArena {
private:
    float x, y;
    float velX, velY;
    float danio;
    bool esDePlanta;
    bool activo;
    sf::CircleShape shape; // Figura de SFML

public:
    ProyectilArena(float startX, float startY, float dirX, float dirY, float dmg, bool dePlanta);
    void actualizar();
    void dibujar(sf::RenderWindow& window);
    CajaColisionArena getHitbox() const;
    bool estaActivo() const { return activo; }
    void destruir() { activo = false; }
    float getDanio() const { return danio; }
    bool esAliadoDePlanta() const { return esDePlanta; }
};