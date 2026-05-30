#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <locale>
#include <windows.h>
#include "src/Menu.h"
#include "src/tablero.h"
#include "src/personaje.h"
#include "src/GestorArena.h"
#include "src/dave.h"
#include "src/drzomboss.h"
#include "src/lanzaguisantes.h" 
#include "src/zombidito.h"
#include <cmath> 
#include <map>
#include <functional>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include "src/JugadorIA.h"
#include "src/Dificultad.h"
#include "src/AdaptadorEstadoJuego.h"
#include "src/Movimiento.h"
#include "src/LuchadorAvatar.h"
#include "src/GestorRecursos.h"
#include "src/VistaJuego.h"
#include "src/ControladorJuego.h"
#include "src/GestorRanking.h"
#include "src/GestorAudio.h"





int main() {
    VistaJuego motorVisual;
    ControladorJuego controlador;
    setlocale(LC_ALL, "spanish");
    SetConsoleOutputCP(1252);

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Archon Warfare", sf::Style::Fullscreen);
    window.setFramerateLimit(60);


    GestorRecursos recursos;
    GestorRanking ranking;
    GestorAudio::inicializar();
    GestorAudio::reproducirMenu();

    // Configuramos el fondo usando el gestor
    sf::Sprite spriteFondo(recursos.getTextura("fondo"));
    spriteFondo.setScale((float)desktop.width / recursos.getTextura("fondo").getSize().x, (float)desktop.height / recursos.getTextura("fondo").getSize().y);

    tablero mitablero;
    mitablero.inicializarpartida();

    int tamanoCasilla = 90;
    bool hayPersonajeSeleccionado = false;
    int fSel = -1;
    int cSel = -1;
    bando turnoActual = bando::planta;
    int ganadorJuego = 0; // 0 = Jugando, 1 = Ganan Plantas, 2 = Ganan Zombis

    sf::Clock relojOscilacion;

    int offsetX = (desktop.width - (9 * tamanoCasilla)) / 2;
    int offsetY = (desktop.height - (9 * tamanoCasilla)) / 2;


    EstadoJuego estadoActual = SELECCIONANDO_PIEZA;
    int fOrigenTeleport = -1, cOrigenTeleport = -1;
    int menuX = 0, menuY = 0;
    personaje* liderSeleccionado = nullptr;

    sf::Font fuente;
    if (!fuente.loadFromFile("COOPBL.ttf")) std::cout << "Error cargando fuente" << std::endl;

    std::vector<std::string> historialConsola;
    int maxMensajes = 5;

    bool modoUnJugador = false;

    auto logConsola = [&](std::string mensaje) {
        historialConsola.push_back(mensaje);
        if (historialConsola.size() > maxMensajes) historialConsola.erase(historialConsola.begin());
        std::cout << mensaje << std::endl;
        };


    auto comprobarVictoria = [&]() {
        // 1. REVISAR ANIQUILACIÓN TOTAL
        int totalPlantas = 0; int totalZombis = 0;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                personaje* p = mitablero.getFicha(i, j);
                if (p != nullptr) {
                    if (p->getequipo() == bando::planta) totalPlantas++;
                    else totalZombis++;
                }
            }
        }

        if (totalPlantas == 0) {
            logConsola("¡VICTORIA ZOMBI!"); ganadorJuego = 2; ranking.registrarVictoria("Zombis");
            GestorAudio::reproducirVictoria(modoUnJugador, 2); // <--- NUEVO
        }
        else if (totalZombis == 0) {
            logConsola("¡VICTORIA PLANTA!"); ganadorJuego = 1; ranking.registrarVictoria("Plantas");
            GestorAudio::reproducirVictoria(modoUnJugador, 1); // <--- NUEVO
        }

        // 2. REVISAR NODOS DE PODER
        int controlPlanta = 0; int controlZombi = 0;
        int casillasPoder[5][2] = { {0,4}, {4,0}, {4,4}, {4,8}, {8,4} };

        for (int i = 0; i < 5; i++) {
            personaje* ocupante = mitablero.getFicha(casillasPoder[i][0], casillasPoder[i][1]);
            if (ocupante != nullptr) {
                if (ocupante->getequipo() == bando::planta) controlPlanta++;
                else controlZombi++;
            }
        }

        if (controlPlanta == 5) {
            logConsola("¡DOMINIO PLANTA!"); ganadorJuego = 1; ranking.registrarVictoria("Plantas");
            GestorAudio::reproducirVictoria(modoUnJugador, 1); // <--- NUEVO
        }
        else if (controlZombi == 5) {
            logConsola("¡DOMINIO ZOMBI!"); ganadorJuego = 2; ranking.registrarVictoria("Zombis");
            GestorAudio::reproducirVictoria(modoUnJugador, 2); // <--- NUEVO
        }
        };
    logConsola("¡Bienvenido a Archon: Plantas vs Zombies!");

    sf::Text txtHechizo1("1. Curar", fuente, 20);
    sf::Text txtHechizo2("2. Resucitar", fuente, 20);
    sf::Text txtHechizo3("3. Teletransportar", fuente, 20);

    Menu menuPrincipal(desktop.width, desktop.height);
    bool enMenuPrincipal = true;
    bool enPantallaRanking = false;
    bool juegoPausado = false;

    bool eligiendoDificultadIA = false;

    std::vector<std::string> cementerioPlantas;
    std::vector<std::string> cementerioZombis;
    std::string muertoSeleccionado = "";

    bando bandoIA = bando::zombi;
    JugadorIA jugadorIA(Dificultad::Normal, 3);
    bool animandoMovimientoIA = false;
    bool esperandoResolverMovimientoIA = false;
    Movimiento movimientoIA;
    sf::Clock relojMovimientoIA;
    const float duracionMovimientoIA = 0.80f;
    const float pausaAntesArenaIA = 0.45f;


    extern bool g_VolverAlMenuPrincipal;

    while (window.isOpen()) {
        sf::Event event;

        if (g_VolverAlMenuPrincipal) {
            g_VolverAlMenuPrincipal = false;
            enMenuPrincipal = true;
            juegoPausado = false;
            ganadorJuego = 0;
            mitablero.inicializarpartida(); // Reseteamos posiciones originales del tablero
            turnoActual = bando::planta;    // Turno inicial de plantas
            estadoActual = SELECCIONANDO_PIEZA;
            historialConsola.clear();
            cementerioPlantas.clear();
            cementerioZombis.clear();
            animandoMovimientoIA = false;
            esperandoResolverMovimientoIA = false;
            logConsola("Combate abortado. Volviendo al menu principal...");
            GestorAudio::reproducirMenu();
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                // ---> ¡NUEVO! Si estamos en el ranking, volver al menú <---
                if (enPantallaRanking) {
                    enPantallaRanking = false;
                    enMenuPrincipal = true;
                }
                else if (ganadorJuego != 0) {
                    ganadorJuego = 0;
                    enMenuPrincipal = true;
                    mitablero.inicializarpartida();
                    turnoActual = bando::planta;
                    estadoActual = SELECCIONANDO_PIEZA;
                    historialConsola.clear();
                    animandoMovimientoIA = false;
                    esperandoResolverMovimientoIA = false;
                    GestorAudio::reproducirMenu();

                }
                // Si estamos jugando y nadie ha ganado, el ESC pausa o reanuda
                else if (!enMenuPrincipal && !eligiendoDificultadIA && ganadorJuego == 0) {
                    juegoPausado = !juegoPausado;
                }
                // Si ya estábamos en el menú de inicio, salimos del juego
                else if (enMenuPrincipal) {
                    window.close();
                }
            }
            if (enMenuPrincipal) {
                EstadoMenu opcionElegida = menuPrincipal.manejarEventos(window, event);

                if (opcionElegida == EstadoMenu::UN_JUGADOR) {
                    modoUnJugador = true;
                    eligiendoDificultadIA = true;
                    enMenuPrincipal = false;
                    GestorAudio::reproducirTablero();
                    logConsola("Modo 1 jugador: elige dificultad con 1, 2 o 3.");
                    logConsola("1 Facil | 2 Normal | 3 Dificil");
                }
                else if (opcionElegida == EstadoMenu::DOS_JUGADORES) {
                    modoUnJugador = false;
                    eligiendoDificultadIA = false;
                    enMenuPrincipal = false;
                    GestorAudio::reproducirTablero();
                    logConsola("Modo 2 jugadores iniciado.");
                }
                else if (opcionElegida == EstadoMenu::RANKING) {
                    ranking.cargarDatos();
                    enMenuPrincipal = false;
                    enPantallaRanking = true; // Apagamos el menú y encendemos el ranking
                }
                else if (opcionElegida == EstadoMenu::SALIR) {
                    window.close();
                }
                continue;
            }

            if (eligiendoDificultadIA) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    float ratonX = (float)event.mouseButton.x;
                    float ratonY = (float)event.mouseButton.y;
                    float centroX = window.getSize().x / 2.f;
                    float centroY = window.getSize().y / 2.f;

                    // Las Y de todos los botones son centroY + 50, y tienen un alto de 60 (±30 desde el centro)
                    if (ratonY >= centroY + 50 - 30 && ratonY <= centroY + 50 + 30) {

                        // Botón Fácil (X: centroX - 240, ancho 200 -> ±100)
                        if (ratonX >= centroX - 240 - 100 && ratonX <= centroX - 240 + 100) {
                            jugadorIA.cambiarDificultad(Dificultad::Facil, 2);
                            eligiendoDificultadIA = false;
                            logConsola("Dificultad IA: Facil");
                        }
                        // Botón Normal (X: centroX, ancho 200 -> ±100)
                        else if (ratonX >= centroX - 100 && ratonX <= centroX + 100) {
                            jugadorIA.cambiarDificultad(Dificultad::Normal, 3);
                            eligiendoDificultadIA = false;
                            logConsola("Dificultad IA: Normal");
                        }
                        // Botón Difícil (X: centroX + 240, ancho 200 -> ±100)
                        else if (ratonX >= centroX + 240 - 100 && ratonX <= centroX + 240 + 100) {
                            jugadorIA.cambiarDificultad(Dificultad::Dificil, 3);
                            eligiendoDificultadIA = false;
                            logConsola("Dificultad IA: Dificil");
                        }
                    }
                }
                // El 'continue' es crucial para que el clic no pase de largo y mueva el tablero
                continue;
            }

            if (modoUnJugador && turnoActual == bandoIA) {
                continue;
            }

            if (juegoPausado) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    float ratonX = (float)event.mouseButton.x;
                    float ratonY = (float)event.mouseButton.y;
                    float centroX = window.getSize().x / 2.f;
                    float centroY = window.getSize().y / 2.f;

                    // Comprobamos si el clic cae dentro de los botones (Ancho 300, Alto 50)
                    // 1. Botón Reanudar (Y = -20)
                    if (ratonX >= centroX - 150 && ratonX <= centroX + 150 && ratonY >= centroY - 20 - 25 && ratonY <= centroY - 20 + 25) {
                        juegoPausado = false;
                    }
                    // 2. Botón Menú Principal (Y = 50)
                    else if (ratonX >= centroX - 150 && ratonX <= centroX + 150 && ratonY >= centroY + 50 - 25 && ratonY <= centroY + 50 + 25) {
                        juegoPausado = false;
                        enMenuPrincipal = true;
                        ganadorJuego = 0;
                        mitablero.inicializarpartida(); // Reseteamos el tablero
                        turnoActual = bando::planta;    // Reseteamos el turno
                        estadoActual = SELECCIONANDO_PIEZA;
                        historialConsola.clear();
                        animandoMovimientoIA = false;
                        esperandoResolverMovimientoIA = false;
                        logConsola("Partida abortada. Volviendo al menu...");
                        GestorAudio::reproducirMenu();
                    }
                    // 3. Botón Salir (Y = 120)
                    else if (ratonX >= centroX - 150 && ratonX <= centroX + 150 && ratonY >= centroY + 120 - 25 && ratonY <= centroY + 120 + 25) {
                        window.close();
                    }
                }
            }
            // SI ESTAMOS JUGANDO NORMALMENTE
            else if (ganadorJuego == 0) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    controlador.procesarClicRaton(
                        event.mouseButton.x, event.mouseButton.y, offsetX, offsetY, tamanoCasilla,
                        estadoActual, mitablero, hayPersonajeSeleccionado, fSel, cSel,
                        turnoActual, liderSeleccionado, menuX, menuY, fOrigenTeleport, cOrigenTeleport,
                        logConsola, relojOscilacion, window, modoUnJugador, jugadorIA.dificultad(), comprobarVictoria,
                        cementerioPlantas, cementerioZombis, muertoSeleccionado);
                }
            }
        }

        // --- CONTROL DE BARRAS DE VOLUMEN EN TIEMPO REAL (Arrastrar ratón) ---
        if (juegoPausado && ganadorJuego == 0 && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            float ratonX = (float)mousePos.x;
            float ratonY = (float)mousePos.y;
            float centroX = window.getSize().x / 2.f;
            float centroY = window.getSize().y / 2.f;

            float xBase = centroX - 150.f; // Inicio de las barras

            // Si hacemos clic/arrastramos en la zona de la barra de Música
            // Le damos un poco de margen en Y (±15 px) para que sea más fácil acertar al hacer clic
            if (ratonX >= xBase && ratonX <= xBase + 300.f &&
                ratonY >= centroY + 190.f - 15.f && ratonY <= centroY + 190.f + 35.f) {

                float nuevoVolumen = ((ratonX - xBase) / 300.f) * 100.f;
                GestorAudio::setVolumenMusica(nuevoVolumen);
            }

            // Si hacemos clic/arrastramos en la zona de la barra de SFX
            else if (ratonX >= xBase && ratonX <= xBase + 300.f &&
                ratonY >= centroY + 260.f - 15.f && ratonY <= centroY + 260.f + 35.f) {

                float nuevoVolumen = ((ratonX - xBase) / 300.f) * 100.f;
                GestorAudio::setVolumenSFX(nuevoVolumen);
            }
        }

        static int ultimoHoverPausa = 0;
        if (juegoPausado && ganadorJuego == 0) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            float ratonX = (float)mousePos.x;
            float ratonY = (float)mousePos.y;
            float centroX = window.getSize().x / 2.f;
            float centroY = window.getSize().y / 2.f;

            int hoverActualPausa = 0;

            // Mismas coordenadas que usas para el clic, pero para detectar posición
            if (ratonX >= centroX - 150 && ratonX <= centroX + 150) {
                if (ratonY >= centroY - 20 - 25 && ratonY <= centroY - 20 + 25) hoverActualPausa = 1;      // Reanudar
                else if (ratonY >= centroY + 50 - 25 && ratonY <= centroY + 50 + 25) hoverActualPausa = 2; // Menú Principal
                else if (ratonY >= centroY + 120 - 25 && ratonY <= centroY + 120 + 25) hoverActualPausa = 3; // Salir
            }

            if (hoverActualPausa != 0 && hoverActualPausa != ultimoHoverPausa) {
                GestorAudio::reproducirHover();
            }
            ultimoHoverPausa = hoverActualPausa;
        }
        else {
            ultimoHoverPausa = 0; // Reseteamos si no estamos en pausa
        }

        if (!juegoPausado && !enMenuPrincipal && !eligiendoDificultadIA && modoUnJugador &&
            turnoActual == bandoIA && estadoActual == SELECCIONANDO_PIEZA && ganadorJuego == 0 &&
            !animandoMovimientoIA && !esperandoResolverMovimientoIA) {
            hayPersonajeSeleccionado = false;
            fSel = -1;
            cSel = -1;

            if (jugadorIA.prepararMovimientoVisual(mitablero, turnoActual, movimientoIA, logConsola)) {
                animandoMovimientoIA = true;
                esperandoResolverMovimientoIA = false;
                relojMovimientoIA.restart();
            }
            else {
                turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
            }
        }

        if (!juegoPausado && animandoMovimientoIA) {
            float tiempo = relojMovimientoIA.getElapsedTime().asSeconds();
            if (tiempo >= duracionMovimientoIA) {
                animandoMovimientoIA = false;
                esperandoResolverMovimientoIA = true;
                relojMovimientoIA.restart();
            }
        }

        if (!juegoPausado && esperandoResolverMovimientoIA) {
            float tiempo = relojMovimientoIA.getElapsedTime().asSeconds();
            if (tiempo >= pausaAntesArenaIA) {
                esperandoResolverMovimientoIA = false;
                jugadorIA.resolverMovimientoPreparado(mitablero, turnoActual, window, movimientoIA, logConsola, relojOscilacion, comprobarVictoria);
            }
        }

       // --- B. DIBUJAR LA PANTALLA ---
        window.clear();
        if (enMenuPrincipal) {
            menuPrincipal.dibujar(window);
        }
        else if (enPantallaRanking) {
            menuPrincipal.dibujarPantallaRanking(window, ranking.obtenerTop(), ranking.obtenerGanadorGuerraTotal());
        }
        else {
            // ¡Toda la magia gráfica en una sola línea!
            motorVisual.dibujarTableroEInterfaz(window, recursos, mitablero, estadoActual, turnoActual,
                menuX, menuY, hayPersonajeSeleccionado, fSel, cSel,
                offsetX, offsetY, tamanoCasilla, relojOscilacion,
                historialConsola, fuente, eligiendoDificultadIA, ganadorJuego, juegoPausado, cementerioPlantas, cementerioZombis,
                animandoMovimientoIA || esperandoResolverMovimientoIA, &movimientoIA,
                esperandoResolverMovimientoIA ? 1.0f : relojMovimientoIA.getElapsedTime().asSeconds() / duracionMovimientoIA
            );
        }
        window.display();

    }
    GestorAudio::limpiar();

    return 0;
}
