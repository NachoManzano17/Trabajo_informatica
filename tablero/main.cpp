#include <SFML/Graphics.hpp>
#include <iostream>
#include "src/tablero.h"
#include "src/personaje.h"

int main() {
    // 1. CREAMOS LA VENTANA
    // Una ventana de 810x810 píxeles para que quepan 9 casillas de 90 píxeles
    sf::RenderWindow window(sf::VideoMode(810, 810), "Archon Warfare - SFML Version");
    window.setFramerateLimit(60); // 60 FPS

    // 2. INICIALIZAMOS TU MOTOR LÓGICO
    tablero mitablero;
    mitablero.inicializarpartida();

    // 3. VARIABLES PARA EL CONTROL DEL RATÓN
    int tamanoCasilla = 90;
    bool hayPersonajeSeleccionado = false;
    int fSel = -1; // Fila seleccionada
    int cSel = -1; // Columna seleccionada

    // 4. EL BUCLE DE JUEGO PRINCIPAL
    while (window.isOpen()) {
        sf::Event event;

        // --- A. GESTIÓN DE EVENTOS (Teclado, Ratón, Cerrar ventana) ---
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Si hacemos clic con el botón izquierdo
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                // Averiguamos en qué fila y columna hemos hecho clic
                int cClic = event.mouseButton.x / tamanoCasilla;
                int fClic = event.mouseButton.y / tamanoCasilla;

                std::cout << "Clic en casilla [" << fClic << "][" << cClic << "]\n";

                // Aquí implementaremos la lógica:
                // Si no hay nada seleccionado -> seleccionar la ficha de esa casilla
                // Si ya hay algo seleccionado -> intentar moverlo a la nueva casilla
            }
        }

        // --- B. DIBUJAR LA PANTALLA ---
        window.clear(sf::Color(30, 30, 30)); // Fondo gris oscuro

        // Dibujar el tablero (Cuadrícula 9x9)
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                sf::RectangleShape casilla(sf::Vector2f(tamanoCasilla, tamanoCasilla));
                casilla.setPosition(j * tamanoCasilla, i * tamanoCasilla);

                // Alternar colores para que parezca un tablero de ajedrez
                if ((i + j) % 2 == 0) {
                    casilla.setFillColor(sf::Color(100, 100, 100)); // Gris claro
                }
                else {
                    casilla.setFillColor(sf::Color(60, 60, 60));     // Gris medio
                }

                // Dibujar contorno de la casilla
                casilla.setOutlineThickness(-2.f); // -2 para que crezca hacia dentro
                casilla.setOutlineColor(sf::Color::Black);

                window.draw(casilla);

                // TODO: Aquí dibujaremos los personajes que estén en 'mitablero'
                // comprobando si casillas[i][j] != nullptr
            }
        }

        window.display(); // Mostrar lo que hemos dibujado
    }

    return 0;
}