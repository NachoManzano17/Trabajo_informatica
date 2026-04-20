// ProyectilArena.cpp
#include "ProyectilArena.h"

ProyectilArena::ProyectilArena(float startX, float startY, float dirX, float dirY, float dmg, bool dePlanta)
    : x(startX), y(startY), velX(dirX), velY(dirY), danio(dmg), esDePlanta(dePlanta), activo(true) {

    // Un poco ovalado (como un láser o un guisante estirado) para que se note la rotación
    shape.setRadius(5.0f);
    shape.setScale(2.0f, 1.0f); // Estirar en X
    shape.setOrigin(5.0f, 5.0f); // Centro
    shape.setPosition(x, y);

    // Calcular hacia dónde apunta el proyectil
    float angulo = std::atan2(dirY, dirX) * 180.0f / 3.14159265f;
    shape.setRotation(angulo);

    if (esDePlanta) {
        shape.setFillColor(sf::Color::Green);
    }
    else {
        shape.setFillColor(sf::Color::Magenta);
    }
}

void ProyectilArena::actualizar() {
    if (!activo) return;
    x += velX;
    y += velY;
    shape.setPosition(x, y); // Sincronizamos la figura gráfica con la lógica
}

CajaColisionArena ProyectilArena::getHitbox() const {
    return { x, y, 10.0f, 10.0f }; // Diámetro del proyectil (radio * 2)
}

void ProyectilArena::dibujar(sf::RenderWindow& window) {
    if (!activo) return;
    window.draw(shape);
}