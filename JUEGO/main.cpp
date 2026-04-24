#include <SFML/Graphics.hpp>
#include <iostream>
#include "src/tablero.h"
#include "src/personaje.h"
#include "src/GestorArena.h"

// Esta clase envuelve a tus personajes del tablero para que puedan pelear en la arena
class LuchadorAvatar : public PersonajeBaseArena {
private:
    sf::CircleShape shape;
    std::string letraOriginal;

public:
    // Al crearlo, le pasamos el personaje real del tablero
    LuchadorAvatar(personaje* fichaReal) : PersonajeBaseArena(100.0f, 5.0f, 20.0f, false, false) {
        letraOriginal = fichaReal->getsimbolo(); // Guardamos su letra para el futuro

        // Lo preparamos para SFML (Radio 20 pixeles)
        shape.setRadius(20.0f);
        shape.setOrigin(20.0f, 20.0f);

        // Si es planta lo pintamos verde, si es zombi morado
        if (fichaReal->getequipo() == bando::planta) {
            shape.setFillColor(sf::Color(50, 200, 50));
        }
        else {
            shape.setFillColor(sf::Color(150, 50, 200));
        }
    }

    // La función obligatoria de SFML para que aparezca en pantalla
    void dibujar(sf::RenderWindow& window) override {
        shape.setPosition(posicion.x, posicion.y);
        window.draw(shape);
    }
};

int resolverCombateEnArena(personaje* atacante, personaje* defensor, sf::RenderWindow& window) {

    // 1. Averiguamos quién es la planta y quién es el zombi en el tablero
    personaje* plantaTablero = (atacante->getequipo() == bando::planta) ? atacante : defensor;
    personaje* zombiTablero = (atacante->getequipo() == bando::zombi) ? atacante : defensor;

    // 2. Les ponemos el "Avatar" de la arena para que SFML los pueda dibujar y mover
    PersonajeBaseArena* pArena = new LuchadorAvatar(plantaTablero);
    PersonajeBaseArena* zArena = new LuchadorAvatar(zombiTablero);

    // 3. Arrancamos el gestor de arena
    GestorArena arena(pArena, zArena, 810.0f, 810.0f);
    arena.inicializarArena(5);

    int ganadorArena = 0; // 1 = Planta, 2 = Zombi

    // 4. EL BUCLE DE COMBATE EN TIEMPO REAL
    while (window.isOpen() && !arena.isTerminado()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
        }

        arena.procesarEntrada();
        arena.actualizarLider();

        window.clear(sf::Color(50, 20, 20)); // Fondo rojizo de tensión
        arena.dibujarEscena(window);
        window.display();
    }

    // 5. Acaba el combate
    if (arena.isTerminado()) {
        ganadorArena = arena.getGanador();
        sf::sleep(sf::seconds(1)); // Pausa dramática para ver quién cayó
    }

    delete pArena;
    delete zArena;

    // 6. Decidimos quién ganó (1 Atacante, 2 Defensor)
    if (ganadorArena == 0) return 0; // Empate

    if (atacante->getequipo() == bando::planta) {
        return (ganadorArena == 1) ? 1 : 2;
    }
    else {
        return (ganadorArena == 2) ? 1 : 2;
    }
}


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

        // --- A. GESTIÓN DE EVENTOS ---
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Si hacemos clic con el botón izquierdo
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int cClic = event.mouseButton.x / tamanoCasilla;
                int fClic = event.mouseButton.y / tamanoCasilla;

                // 1. SI NO TENEMOS NADA SELECCIONADO
                if (!hayPersonajeSeleccionado) {
                    personaje* fichaClicada = mitablero.getFicha(fClic, cClic);
                    if (fichaClicada != nullptr) {
                        hayPersonajeSeleccionado = true;
                        fSel = fClic;
                        cSel = cClic;
                        std::cout << "[!] Seleccionado: " << fichaClicada->getsimbolo() << "\n";
                    }
                }
                // 2. SI YA TENÍAMOS UNA FICHA AGARRADA
                else {
                    if (fClic == fSel && cClic == cSel) {
                        // Deseleccionar si pincha en sí misma (quitamos el brillo)
                        hayPersonajeSeleccionado = false;
                        fSel = -1; cSel = -1;
                        std::cout << "-> Deseleccionado\n";
                    }
                    else {
                        // Miramos qué hay en la casilla destino
                        personaje* atacante = mitablero.getFicha(fSel, cSel);
                        personaje* defensor = mitablero.getFicha(fClic, cClic);
                        if (!atacante->puedeSaltar() && mitablero.hayObstaculoEnCamino(fSel, cSel, fClic, cClic)) {
                            std::cout << "-> Movimiento invalido: Hay piezas bloqueando el camino.\n";
                            hayPersonajeSeleccionado = false;
                            fSel = -1; cSel = -1;
                        }

                        // Si hay un defensor y es enemigo... ¡A LA ARENA!
                        else if (defensor != nullptr && atacante->getequipo() != defensor->getequipo()) {
                            std::cout << "[!] COMBATE INICIADO [!]\n";

                            int resultado = resolverCombateEnArena(atacante, defensor, window);

                            if (resultado == 1) {
                                std::cout << "-> Gana Atacante\n";
                                mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);
                            }
                            else if (resultado == 2) {
                                std::cout << "-> Gana Defensor\n";
                                mitablero.eliminarFicha(fSel, cSel);
                            }
                            else {
                                std::cout << "-> Empate (Mueren los dos)\n";
                                mitablero.eliminarFicha(fSel, cSel);
                                mitablero.eliminarFicha(fClic, cClic);
                            }
                        }
                        else if (defensor == nullptr) {
                            bool exito = mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);
                            if (exito) std::cout << "-> Movimiento completado\n";
                            else std::cout << "-> Movimiento invalido\n";
                        }
                        // CASO 3: Hay un defensor y es de TU BANDO (Aliado) -> Ilegal
                        else {
                            std::cout << "-> Movimiento cancelado: No puedes pisar a un aliado.\n";
                        }

                        // Soltamos la ficha pase lo que pase
                        hayPersonajeSeleccionado = false;
                        fSel = -1; cSel = -1;
                        // Movimiento normal (casilla vacía)
                        /*else {
                            bool exito = mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);
                            if (exito) std::cout << "-> Movimiento completado\n";
                            else std::cout << "-> Movimiento invalido\n";
                        }

                        // Soltamos la ficha pase lo que pase
                        hayPersonajeSeleccionado = false;
                        fSel = -1; cSel = -1;*/
                    }
                }
            }
        

        // --- B. DIBUJAR LA PANTALLA ---
        window.clear(sf::Color(30, 30, 30));

        // 1. Dibujar el fondo cuadriculado
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                sf::RectangleShape casilla(sf::Vector2f(tamanoCasilla, tamanoCasilla));
                casilla.setPosition(j * tamanoCasilla, i * tamanoCasilla);

                if ((i + j) % 2 == 0) casilla.setFillColor(sf::Color(100, 100, 100));
                else casilla.setFillColor(sf::Color(60, 60, 60));

                casilla.setOutlineThickness(-2.f);
                casilla.setOutlineColor(sf::Color::Black);
                window.draw(casilla);
            }
        }

        // 2. Dibujar resaltos si hay un personaje seleccionado
        if (hayPersonajeSeleccionado) {
            personaje* seleccionado = mitablero.getFicha(fSel, cSel);

            // A. Brillo amarillo en la casilla donde está la ficha (Origen)
            sf::RectangleShape highlight(sf::Vector2f(tamanoCasilla, tamanoCasilla));
            highlight.setPosition(cSel * tamanoCasilla, fSel * tamanoCasilla);
            highlight.setFillColor(sf::Color(255, 255, 0, 100)); // Amarillo transparente
            window.draw(highlight);

            // B. Calcular y dibujar los "Puntos de movimiento" a lo Chess.com
            if (seleccionado != nullptr) {
                // Escaneamos todo el tablero (81 casillas)
                for (int i = 0; i < 9; ++i) {
                    for (int j = 0; j < 9; ++j) {
                        // 1. Ignoramos la casilla donde ya estamos parados
                        if (i == fSel && j == cSel) continue;

                        // 2. Le preguntamos a la ficha si sabe llegar matemáticamente a [i][j]
                        if (seleccionado->esmovimientovalido(fSel, cSel, i, j)) {

                            // ---> ¡NUEVO FILTRO VISUAL AQUÍ! <---
                            // Si la pieza NO sabe saltar Y hay alguien en medio del camino...
                            if (!seleccionado->puedeSaltar() && mitablero.hayObstaculoEnCamino(fSel, cSel, i, j)) {
                                continue; // ... abortamos y NO dibujamos el punto en esta casilla
                            }

                            // 3. Comprobamos que no haya un aliado ahí (para no pisarnos entre nosotros)
                            personaje* fichaDestino = mitablero.getFicha(i, j);
                            if (fichaDestino == nullptr || fichaDestino->getequipo() != seleccionado->getequipo()) {

                                // ¡BINGO! Es un movimiento válido. Dibujamos un círculo azulito en el centro.
                                float radioPunto = 12.0f; // Tamaño del punto
                                sf::CircleShape puntoMovimiento(radioPunto);

                                // Fórmulas matemáticas para centrar el punto exactamente en medio de la casilla
                                float centroX = (j * tamanoCasilla) + (tamanoCasilla / 2.0f) - radioPunto;
                                float centroY = (i * tamanoCasilla) + (tamanoCasilla / 2.0f) - radioPunto;
                                puntoMovimiento.setPosition(centroX, centroY);

                                // Si hay un enemigo, lo pintamos rojizo (Ataque), si está vacío, azul (Movimiento)
                                if (fichaDestino != nullptr) {
                                    puntoMovimiento.setFillColor(sf::Color(255, 50, 50, 180)); // Rojo semi-transparente
                                }
                                else {
                                    puntoMovimiento.setFillColor(sf::Color(100, 200, 255, 150)); // Azulito chess.com
                                }

                                window.draw(puntoMovimiento);
                            }
                        }
                    }
                }
            }
        }

        // 3. Dibujar las tropas (Círculos verdes y morados)
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                personaje* fichaActual = mitablero.getFicha(i, j);

                if (fichaActual != nullptr) {
                    // Creamos un círculo un poco más pequeño que la casilla
                    sf::CircleShape circulo(tamanoCasilla / 2.5f);

                    // Lo centramos matemáticamente (offset de unos 9 píxeles)
                    circulo.setPosition(j * tamanoCasilla + 9, i * tamanoCasilla + 9);

                    // Pinta de Verde si es Planta (0), Morado si es Zombi (1)
                    if (fichaActual->getequipo() == bando::planta) {
                        circulo.setFillColor(sf::Color(50, 205, 50)); // Verde lima
                    }
                    else {
                        circulo.setFillColor(sf::Color(138, 43, 226)); // Morado Zombi
                    }

                    window.draw(circulo);
                }
            }
        }

        
    }
        window.display();
 }
        return 0;
}
  
