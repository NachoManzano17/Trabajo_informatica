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
    // ---> ¡NUEVO! 1. CREAMOS LA VENTANA A PANTALLA COMPLETA <---
    // Preguntamos a Windows de qué tamaño es tu pantalla (ej: 1920x1080)
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Archon Warfare", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    // ---> ¡NUEVO! 2. CARGAMOS TODAS LAS TEXTURAS <---
    sf::Texture texturaCesped;
    if (!texturaCesped.loadFromFile("cesped.PNG")) std::cout << "[ERROR] Falta cesped.PNG\n";

    sf::Texture texturaTierra;
    if (!texturaTierra.loadFromFile("tierra.PNG")) std::cout << "[ERROR] Falta tierra.PNG\n";

    sf::Texture texturaFondo;
    if (!texturaFondo.loadFromFile("fondo.PNG")) std::cout << "[ERROR] Falta fondo.PNG\n";

    sf::Sprite spriteFondo(texturaFondo);
    // Escalamos el fondo por si tu pantalla es más grande o pequeña que la imagen original
    spriteFondo.setScale((float)desktop.width / texturaFondo.getSize().x, (float)desktop.height / texturaFondo.getSize().y);


    // 3. INICIALIZAMOS TU MOTOR LÓGICO
    tablero mitablero;
    mitablero.inicializarpartida();

    // 4. VARIABLES PARA EL RATÓN Y EL CENTRADO
    int tamanoCasilla = 90;
    bool hayPersonajeSeleccionado = false;
    int fSel = -1;
    int cSel = -1;
    bando turnoActual = bando::planta;

    // ---> ¡NUEVO! CÁLCULO DEL CENTRO EXACTO DE LA PANTALLA <---
    // (Ancho de la pantalla - Ancho del tablero(810)) / 2
    int offsetX = (desktop.width - (9 * tamanoCasilla)) / 2;
    int offsetY = (desktop.height - (9 * tamanoCasilla)) / 2;


    // 5. EL BUCLE DE JUEGO PRINCIPAL
    while (window.isOpen()) {
        sf::Event event;

        // --- A. GESTIÓN DE EVENTOS ---
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // Para poder salir del juego al pulsar la tecla ESCAPE
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }

            // Si hacemos clic con el botón izquierdo
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                // ---> ¡NUEVO! AJUSTAMOS EL CLIC DEL RATÓN AL CENTRO DE LA PANTALLA <---
                int ratonX = event.mouseButton.x - offsetX;
                int ratonY = event.mouseButton.y - offsetY;

                // Comprobamos que hemos hecho clic DENTRO del tablero de 810x810, no en los árboles del fondo
                if (ratonX >= 0 && ratonX < (9 * tamanoCasilla) && ratonY >= 0 && ratonY < (9 * tamanoCasilla)) {

                    int cClic = ratonX / tamanoCasilla;
                    int fClic = ratonY / tamanoCasilla;

                    // 1. SI NO TENEMOS NADA SELECCIONADO
                    if (!hayPersonajeSeleccionado) {
                        personaje* fichaClicada = mitablero.getFicha(fClic, cClic);

                        if (fichaClicada != nullptr) {
                            if (fichaClicada->getequipo() == turnoActual) {
                                hayPersonajeSeleccionado = true;
                                fSel = fClic;
                                cSel = cClic;
                                std::cout << "[!] Seleccionado: " << fichaClicada->getsimbolo() << "\n";
                            }
                            else {
                                std::cout << "[X] ¡Quieto ahi! No es tu turno.\n";
                            }
                        }
                    }
                    // 2. SI YA TENÍAMOS UNA FICHA AGARRADA
                    else {
                        if (fClic == fSel && cClic == cSel) {
                            hayPersonajeSeleccionado = false;
                            fSel = -1; cSel = -1;
                        }
                        else {
                            personaje* atacante = mitablero.getFicha(fSel, cSel);
                            personaje* defensor = mitablero.getFicha(fClic, cClic);

                            if (defensor != nullptr && atacante->getequipo() != defensor->getequipo()) {
                                std::cout << "[!] COMBATE INICIADO [!]\n";
                                int resultado = resolverCombateEnArena(atacante, defensor, window);

                                if (resultado == 1) {
                                    mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);
                                }
                                else if (resultado == 2) {
                                    mitablero.eliminarFicha(fSel, cSel);
                                }
                                else {
                                    mitablero.eliminarFicha(fSel, cSel);
                                    mitablero.eliminarFicha(fClic, cClic);
                                }
                                turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                            }
                            else if (defensor == nullptr) {
                                bool exito = mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);
                                if (exito) {
                                    turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                                }
                            }

                            hayPersonajeSeleccionado = false;
                            fSel = -1; cSel = -1;
                        }
                    }
                }
            }
        }


        // --- B. DIBUJAR LA PANTALLA ---
        window.clear();

        // ---> ¡NUEVO! DIBUJAMOS EL FONDO LO PRIMERO DE TODO <---
        window.draw(spriteFondo);

        // 1. Dibujar el fondo cuadriculado con las texturas y el offset
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                sf::RectangleShape casilla(sf::Vector2f(tamanoCasilla, tamanoCasilla));

                // Le sumamos offsetX y offsetY para que se dibuje en el centro
                casilla.setPosition(offsetX + (j * tamanoCasilla), offsetY + (i * tamanoCasilla));

                if ((i + j) % 2 == 0) casilla.setTexture(&texturaCesped);
                else casilla.setTexture(&texturaTierra);

                casilla.setOutlineThickness(-1.f);
                casilla.setOutlineColor(sf::Color(20, 20, 20, 100)); // Borde muy sutil
                window.draw(casilla);
            }
        }

        // 2. Dibujar resaltos si hay un personaje seleccionado
        if (hayPersonajeSeleccionado) {
            personaje* seleccionado = mitablero.getFicha(fSel, cSel);

            sf::RectangleShape highlight(sf::Vector2f(tamanoCasilla, tamanoCasilla));
            highlight.setPosition(offsetX + (cSel * tamanoCasilla), offsetY + (fSel * tamanoCasilla));
            highlight.setFillColor(sf::Color(255, 255, 0, 100));
            window.draw(highlight);

            if (seleccionado != nullptr) {
                for (int i = 0; i < 9; ++i) {
                    for (int j = 0; j < 9; ++j) {
                        if (i == fSel && j == cSel) continue;

                        if (seleccionado->esmovimientovalido(fSel, cSel, i, j)) {
                            if (!seleccionado->puedeSaltar() && mitablero.hayObstaculoEnCamino(fSel, cSel, i, j)) continue;

                            personaje* fichaDestino = mitablero.getFicha(i, j);
                            if (fichaDestino == nullptr || fichaDestino->getequipo() != seleccionado->getequipo()) {

                                float radioPunto = 12.0f;
                                sf::CircleShape puntoMovimiento(radioPunto);

                                float centroX = (j * tamanoCasilla) + (tamanoCasilla / 2.0f) - radioPunto;
                                float centroY = (i * tamanoCasilla) + (tamanoCasilla / 2.0f) - radioPunto;
                                puntoMovimiento.setPosition(offsetX + centroX, offsetY + centroY);

                                if (fichaDestino != nullptr) puntoMovimiento.setFillColor(sf::Color(255, 50, 50, 180));
                                else puntoMovimiento.setFillColor(sf::Color(100, 200, 255, 150));

                                window.draw(puntoMovimiento);
                            }
                        }
                    }
                }
            }
        }

        // 3. Dibujar las tropas
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                personaje* fichaActual = mitablero.getFicha(i, j);

                if (fichaActual != nullptr) {
                    sf::CircleShape circulo(tamanoCasilla / 2.5f);
                    circulo.setPosition(offsetX + (j * tamanoCasilla) + 9, offsetY + (i * tamanoCasilla) + 9);

                    if (fichaActual->getequipo() == bando::planta) circulo.setFillColor(sf::Color(50, 205, 50));
                    else circulo.setFillColor(sf::Color(138, 43, 226));

                    window.draw(circulo);
                }
            }
        }

        // 4. EL CHIVATO VISUAL DE TURNOS
        sf::CircleShape chivatoTurno(20.0f);
        chivatoTurno.setPosition(20.0f, 20.0f); // Lo alejamos un poco de la esquina para la pantalla completa

        if (turnoActual == bando::planta) chivatoTurno.setFillColor(sf::Color(50, 205, 50));
        else chivatoTurno.setFillColor(sf::Color(138, 43, 226));

        chivatoTurno.setOutlineThickness(3.0f);
        chivatoTurno.setOutlineColor(sf::Color::White);

        window.draw(chivatoTurno);

        window.display();
    }

    return 0;
}
  
