#pragma once
#include <SFML/Graphics.hpp>
#include "Vector2D.h"

enum class TipoArma { Basico, Franco, Pesado, Rafaga };

class ProyectilArena {
private:
    Vector2D posicion;
    Vector2D velocidad;
    float danio;
    bool esDePlanta;
    bool viva;
    TipoArma tipo;

    // Variables gráficas
    sf::ConvexShape forma;
    sf::Sprite spriteBala;
    bool usaSprite;

public:
    ProyectilArena(float x, float y, float velX, float velY, float d, bool planta, TipoArma t);

    void actualizar();
    void dibujar(sf::RenderWindow& window);
    void destruir() { viva = false; }

    bool estaActivo() const { return viva; }
    bool esAliadoDePlanta() const { return esDePlanta; }
    float getDanio() const { return danio; }
    Vector2D getPosicion() const { return posicion; }
    TipoArma getTipo() const { return tipo; }
};
