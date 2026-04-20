#include "GestorArena.h"
#include <SFML/Graphics.hpp>
#include <iostream>

#include "GestorArena.h"
#include <SFML/Graphics.hpp>
#include <iostream>

// PERSONAJES DE PRUEBA 

class PlantaGuisante : public PersonajeBaseArena {
private:
    sf::RectangleShape shape;
public:
    // (Vida, Vel, Daño, EsVolador, EsCC) -> Guisante: NO vuela, NO es CC
    PlantaGuisante() : PersonajeBaseArena(100.0f, 4.0f, 20.0f, false, false) {
        shape.setSize(sf::Vector2f(40, 40));
        shape.setOrigin(20, 20); // CENTRO EXACTO PARA QUE ROTE BIEN
        shape.setFillColor(sf::Color(50, 200, 50)); // Verde
    }
    void dibujar(sf::RenderWindow& window) override {
        shape.setPosition(x, y);
        shape.setRotation(anguloMirada); // APLICAMOS LA ROTACIÓN
        window.draw(shape);
    }
};

class ZombieSuperCerebroz : public PersonajeBaseArena {
private:
    sf::RectangleShape shape;
public:
    // (Vida, Vel, Daño, EsVolador, EsCC) -> SuperCerebroz: SÍ vuela, SÍ es CC
    ZombieSuperCerebroz() : PersonajeBaseArena(120.0f, 6.0f, 30.0f, true, true) {
        shape.setSize(sf::Vector2f(40, 40));
        shape.setOrigin(20, 20); // CENTRO EXACTO
        shape.setFillColor(sf::Color(150, 50, 200)); // Morado
    }
    void dibujar(sf::RenderWindow& window) override {
        shape.setPosition(x, y);
        shape.setRotation(anguloMirada); // APLICAMOS LA ROTACIÓN
        window.draw(shape);
    }
};


// MOTOR DEL JUEGO 

int main() {
    // 1. Crear ventana SFML (Resolución 800x600)
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arena de Combate PvZ");
    window.setFramerateLimit(60); // 60 FPS fijos

    // 2. Instanciamos a los personajes de prueba
    PlantaGuisante* pActual = new PlantaGuisante();
    ZombieSuperCerebroz* zActual = new ZombieSuperCerebroz();

    // 3. Arrancamos tu Gestor de Arena
    GestorArena arena(pActual, zActual, 800.0f, 600.0f);
    arena.inicializarArena(5); // 5 obstáculos aleatorios

    // 4. Bucle principal de juego (Game Loop)
    while (window.isOpen()) {

        // Controlar si el usuario le da a la 'X' para cerrar la ventana
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Si el combate no ha terminado, actualizamos posiciones, disparos y daño
        if (!arena.isTerminado()) {
            arena.procesarEntrada();
            arena.actualizarLider();
        }

        // 5. Renderizado gráfico
        window.clear(sf::Color(30, 30, 30)); // Limpiar pantalla con fondo gris oscuro

        arena.dibujarEscena(window); // Dibujamos todo (obstáculos, personajes, proyectiles, vida)

        window.display(); // Mostramos el fotograma nuevo

        // 6. Condición de fin de combate
        if (arena.isTerminado()) {
            int ganador = arena.getGanador();
            if (ganador == 1) std::cout << "¡Gano la Planta!" << std::endl;
            else if (ganador == 2) std::cout << "¡Gano el Zombie!" << std::endl;
            else std::cout << "¡Empate! (Ambos mueren)" << std::endl;

            sf::sleep(sf::seconds(2)); // Esperar 2 segundos para ver el resultado
            window.close(); // Cerramos la arena para volver al tablero
        }
    }

    // Limpiamos la memoria
    delete pActual;
    delete zActual;

    return 0;
}
