#include "GestorArena.h"
#include <SFML/Graphics.hpp>
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

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arena de Combate PvZ");
    window.setFramerateLimit(60);

    PlantaGuisante* pActual = new PlantaGuisante();
    ZombieSuperCerebroz* zActual = new ZombieSuperCerebroz();

    GestorArena arena(pActual, zActual, 800.0f, 600.0f);
    arena.inicializarArena(5);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
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
            int ganador = arena.getGanador();
            if (ganador == 1) std::cout << "¡Gano la Planta!" << std::endl;
            else if (ganador == 2) std::cout << "¡Gano el Zombie!" << std::endl;
            else std::cout << "¡Empate! (Ambos mueren)" << std::endl;

            sf::sleep(sf::seconds(2));
            window.close();
        }
    }

    delete pActual;
    delete zActual;

    return 0;
}