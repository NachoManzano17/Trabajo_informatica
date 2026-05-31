#include "GestorRecursos.h"
#include <iostream>

GestorRecursos::GestorRecursos() {
    if (!texturas["cesped"].loadFromFile("cesped.PNG")) std::cout << "[ERROR] Falta cesped.PNG\n";
    if (!texturas["tierra"].loadFromFile("tierra.PNG")) std::cout << "[ERROR] Falta tierra.PNG\n";
    if (!texturas["fondo"].loadFromFile("fondo.PNG")) std::cout << "[ERROR] Falta fondo.PNG\n";
    if (!texturas["casilla_planta"].loadFromFile("casilla_planta.png")) std::cout << "[ERROR] Falta casilla_planta.png\n";
    if (!texturas["casilla_zombi"].loadFromFile("casilla_zombi.png")) std::cout << "[ERROR] Falta casilla_zombi.png\n";
    if (!texturas["casilla_poder"].loadFromFile("casilla_poder.png")) std::cout << "[ERROR] Falta casilla_poder.png\n";

    auto cargarFicha = [&](std::string id, std::string ruta, float baseScale = 90.0f) {
        if (!texturas[id].loadFromFile(ruta)) {
            std::cout << "[ERROR] Falta " << ruta << "\n";
        }
        sprites[id].setTexture(texturas[id]);
        sprites[id].setScale(baseScale / texturas[id].getSize().x, baseScale / texturas[id].getSize().y);
        };

    cargarFicha("Dave", "dave.png");
    cargarFicha("allstar", "allstar.png");
    cargarFicha("Cactus", "cactus.png");
    cargarFicha("Dronajo", "dronajo.png");
    cargarFicha("DrZomboss", "drzomboss.png");
    cargarFicha("Lanzaguisantes", "lanzaguisantes.png");
    cargarFicha("LoroPirata", "loropirata.png");
    cargarFicha("PlantaCarnivora", "plantacarnivora.png");
    cargarFicha("Pomelo", "pomelo.png", 80.0f);
    cargarFicha("Soldado", "soldado.png");
    cargarFicha("SuperCerebroz", "supercerebroz.png");
    cargarFicha("Zombidito", "zombidito.png");
}

sf::Sprite& GestorRecursos::getSprite(const std::string& nombre) {
    return sprites[nombre];
}

sf::Texture& GestorRecursos::getTextura(const std::string& nombre) {
    return texturas[nombre];
}

bool GestorRecursos::tieneSprite(const std::string& nombre) {
    return sprites.count(nombre) > 0;
}