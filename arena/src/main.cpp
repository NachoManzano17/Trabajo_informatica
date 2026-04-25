#include <SFML/Graphics.hpp>
#include "GestorArena.h"
#include <iostream>



class PlantaGuisante : public PersonajeBaseArena {
private:
    sf::RectangleShape shape;
public:
    PlantaGuisante() : PersonajeBaseArena(100.0f, 4.0f, 20.0f, false, false) {
        shape.setSize(sf::Vector2f(40, 40));
        shape.setOrigin(20, 20);
        shape.setFillColor(sf::Color(50, 200, 50));
    }
    void dibujar(sf::RenderWindow& window) override {
        shape.setPosition(posicion.x, posicion.y);
        shape.setRotation(anguloMirada);
        window.draw(shape);
    }
};

class ZombieSuperCerebroz : public PersonajeBaseArena {
private:
    sf::RectangleShape shape;
public:
    ZombieSuperCerebroz() : PersonajeBaseArena(120.0f, 6.0f, 30.0f, true, true) {
        shape.setSize(sf::Vector2f(40, 40));
        shape.setOrigin(20, 20);
        shape.setFillColor(sf::Color(150, 50, 200));
    }
    void dibujar(sf::RenderWindow& window) override {
        shape.setPosition(posicion.x, posicion.y);
        shape.setRotation(anguloMirada);
        window.draw(shape);
    }
};


// --- EL MOTOR PRINCIPAL ---
int main() {
    // 1. Obtenemos la resolución real de tu monitor
    sf::VideoMode resolucion = sf::VideoMode::getDesktopMode();

    // 2. Abrimos en Pantalla Completa
    sf::RenderWindow window(resolucion, "PvZ Arena", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    // 3. Creamos a los personajes de prueba
    PlantaGuisante* pActual = new PlantaGuisante();
    ZombieSuperCerebroz* zActual = new ZombieSuperCerebroz();

    // 4. Extraemos el ancho y el alto
    float anchoReal = (float)resolucion.width;
    float altoReal = (float)resolucion.height;

    // 5. Se lo pasamos a tu Gestor
    GestorArena arena(pActual, zActual, anchoReal, altoReal);

    // Le pedimos exactamente 6 obstáculos siempre
    arena.inicializarArena(6);

    // --- GAME LOOP ---
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Sistema de seguridad: ESCAPE para salir de pantalla completa
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        if (!arena.isTerminado()) {
            arena.procesarEntrada();
            arena.actualizarLider();
        }

        window.clear(sf::Color(30, 30, 30));
        arena.dibujarEscena(window);
        window.display();

        if (arena.isTerminado()) {
            sf::sleep(sf::seconds(2));
            window.close();
        }
    }

    delete pActual;
    delete zActual;

    return 0;
}