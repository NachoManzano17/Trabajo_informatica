#include <iostream>
#include <locale>
#include <string>
#include <vector>
#include <cmath> 
#include <map>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <chrono>
#define NOMINMAX
#include <windows.h>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "src/Menu.h"
#include "src/tablero.h"
#include "src/personaje.h"
#include "src/GestorArena.h"
#include "src/dave.h"
#include "src/drzomboss.h"
#include "src/lanzaguisantes.h" 
#include "src/zombidito.h"
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

// Variable global externa
extern bool g_VolverAlMenuPrincipal;

int main() {
    // --- 1. CONFIGURACIÓN INICIAL ---
    setlocale(LC_ALL, "spanish");
    SetConsoleOutputCP(1252);

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Archon Warfare", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    // Gestores y Motor
    VistaJuego motorVisual;
    ControladorJuego controlador;
    GestorRecursos recursos;
    GestorRanking ranking;

    GestorAudio::inicializar();
    GestorAudio::reproducirMenu();

    sf::Font fuente;
    if (!fuente.loadFromFile("COOPBL.ttf")) {
        std::cout << "Error cargando fuente" << std::endl;
    }

    // --- 2. VARIABLES DE ESTADO DEL JUEGO ---
    tablero mitablero;
    mitablero.inicializarpartida();

    int tamanoCasilla = 90;
    int offsetX = (desktop.width - (9 * tamanoCasilla)) / 2;
    int offsetY = (desktop.height - (9 * tamanoCasilla)) / 2;

    bool hayPersonajeSeleccionado = false;
    int fSel = -1, cSel = -1;
    int fOrigenTeleport = -1, cOrigenTeleport = -1;
    int menuX = 0, menuY = 0;

    bando turnoActual = bando::planta;
    EstadoJuego estadoActual = SELECCIONANDO_PIEZA;
    personaje* liderSeleccionado = nullptr;
    int ganadorJuego = 0; // 0 = Jugando, 1 = Ganan Plantas, 2 = Ganan Zombis

    sf::Clock relojOscilacion;

    // Consola In-Game
    std::vector<std::string> historialConsola;
    int maxMensajes = 5;

    // Navegación y Menús
    Menu menuPrincipal(desktop.width, desktop.height);
    bool enMenuPrincipal = true;
    bool enPantallaRanking = false;
    bool juegoPausado = false;

    // IA y Un Jugador
    bool modoUnJugador = false;
    bool eligiendoDificultadIA = false;
    bando bandoIA = bando::zombi;
    JugadorIA jugadorIA(Dificultad::Normal, 3);

    bool animandoMovimientoIA = false;
    bool esperandoResolverMovimientoIA = false;
    Movimiento movimientoIA;
    sf::Clock relojMovimientoIA;
    const float duracionMovimientoIA = 0.80f;
    const float pausaAntesArenaIA = 0.45f;

    // Cementerio
    std::vector<std::string> cementerioPlantas;
    std::vector<std::string> cementerioZombis;
    std::string muertoSeleccionado = "";


    // --- 3. FUNCIONES LAMBDA AUXILIARES ---

    auto logConsola = [&](const std::string& mensaje) {
        historialConsola.push_back(mensaje);
        if (historialConsola.size() > maxMensajes) historialConsola.erase(historialConsola.begin());
        std::cout << mensaje << std::endl;
        };

    auto resetearPartida = [&]() {
        ganadorJuego = 0;
        enMenuPrincipal = true;
        mitablero.inicializarpartida();
        turnoActual = bando::planta;
        estadoActual = SELECCIONANDO_PIEZA;
        historialConsola.clear();
        cementerioPlantas.clear();
        cementerioZombis.clear();
        animandoMovimientoIA = false;
        esperandoResolverMovimientoIA = false;
        GestorAudio::reproducirMenu();
        };

    auto comprobarVictoria = [&]() {
        controlador.evaluarVictoria(mitablero, ganadorJuego, logConsola, ranking, modoUnJugador);
        };

    logConsola("¡Bienvenido a Archon: Plantas vs Zombies!");

    // --- 4. BUCLE PRINCIPAL DEL JUEGO ---
    while (window.isOpen()) {
        sf::Event event;

        // Gestión de retorno forzado al menú (desde Arena, etc.)
        if (g_VolverAlMenuPrincipal) {
            g_VolverAlMenuPrincipal = false;
            juegoPausado = false;
            resetearPartida();
            logConsola("Combate abortado. Volviendo al menu principal...");
        }

        // Procesamiento de Eventos
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Gestión de Tecla ESC
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                if (enPantallaRanking) {
                    enPantallaRanking = false;
                    enMenuPrincipal = true;
                }
                else if (ganadorJuego != 0) {
                    resetearPartida();
                }
                else if (!enMenuPrincipal && !eligiendoDificultadIA && ganadorJuego == 0) {
                    juegoPausado = !juegoPausado;
                }
                else if (enMenuPrincipal) {
                    window.close();
                }
            }

            // --- ESTADO: MENÚ PRINCIPAL ---
            if (enMenuPrincipal) {
                EstadoMenu opcionElegida = menuPrincipal.manejarEventos(window, event);

                if (opcionElegida == EstadoMenu::UN_JUGADOR) {
                    modoUnJugador = true;
                    eligiendoDificultadIA = true;
                    enMenuPrincipal = false;
                    GestorAudio::reproducirTablero();
                    logConsola("Modo 1 jugador: elige dificultad (1 Facil | 2 Normal | 3 Dificil)");
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
                    enPantallaRanking = true;
                }
                else if (opcionElegida == EstadoMenu::SALIR) {
                    window.close();
                }
                continue;
            }

            // --- ESTADO: SELECCIÓN DIFICULTAD IA ---
            if (eligiendoDificultadIA) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    float ratonX = (float)event.mouseButton.x;
                    float ratonY = (float)event.mouseButton.y;
                    float centroX = window.getSize().x / 2.f;
                    float centroY = window.getSize().y / 2.f;

                    if (ratonY >= centroY + 20 && ratonY <= centroY + 80) { // Alto de 60
                        if (ratonX >= centroX - 340 && ratonX <= centroX - 140) {
                            jugadorIA.cambiarDificultad(Dificultad::Facil, 2);
                            eligiendoDificultadIA = false;
                            logConsola("Dificultad IA: Facil");
                        }
                        else if (ratonX >= centroX - 100 && ratonX <= centroX + 100) {
                            jugadorIA.cambiarDificultad(Dificultad::Normal, 3);
                            eligiendoDificultadIA = false;
                            logConsola("Dificultad IA: Normal");
                        }
                        else if (ratonX >= centroX + 140 && ratonX <= centroX + 340) {
                            jugadorIA.cambiarDificultad(Dificultad::Dificil, 3);
                            eligiendoDificultadIA = false;
                            logConsola("Dificultad IA: Dificil");
                        }
                    }
                }
                continue;
            }

            // Ignorar clics del jugador si es el turno de la IA
            if (modoUnJugador && turnoActual == bandoIA) {
                continue;
            }

            // --- ESTADO: MENÚ DE PAUSA ---
            if (juegoPausado) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    int botonPulsado = motorVisual.comprobarClicBotonPausa(event.mouseButton.x, event.mouseButton.y, window.getSize().x, window.getSize().y);

                    if (botonPulsado == 1) { // Reanudar
                        juegoPausado = false;
                    }
                    else if (botonPulsado == 2) { // Menú Principal
                        juegoPausado = false;
                        resetearPartida();
                        logConsola("Partida abortada. Volviendo al menu...");
                    }
                    else if (botonPulsado == 3) { // Salir
                        window.close();
                    }
                }
            }
            // --- ESTADO: JUEGO ACTIVO ---
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

        // --- GESTIÓN DE AUDIO EN PAUSA (Barras de volumen) ---
        if (juegoPausado && ganadorJuego == 0 && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            motorVisual.actualizarVolumenPausa(mousePos.x, mousePos.y, window.getSize().x, window.getSize().y);
        }

        // Hover sonoro en menú de pausa
        static int ultimoHoverPausa = 0;
        if (juegoPausado && ganadorJuego == 0) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            int hoverActualPausa = motorVisual.comprobarClicBotonPausa(mousePos.x, mousePos.y, window.getSize().x, window.getSize().y);

            if (hoverActualPausa != 0 && hoverActualPausa != ultimoHoverPausa) {
                GestorAudio::reproducirHover();
            }
            ultimoHoverPausa = hoverActualPausa;
        }
        else {
            ultimoHoverPausa = 0;
        }

        // --- LÓGICA DE LA IA ---
        if (!juegoPausado && !enMenuPrincipal && !eligiendoDificultadIA && modoUnJugador &&
            turnoActual == bandoIA && estadoActual == SELECCIONANDO_PIEZA && ganadorJuego == 0 &&
            !animandoMovimientoIA && !esperandoResolverMovimientoIA) {

            hayPersonajeSeleccionado = false;
            fSel = -1; cSel = -1;

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
            if (relojMovimientoIA.getElapsedTime().asSeconds() >= duracionMovimientoIA) {
                animandoMovimientoIA = false;
                esperandoResolverMovimientoIA = true;
                relojMovimientoIA.restart();
            }
        }

        if (!juegoPausado && esperandoResolverMovimientoIA) {
            if (relojMovimientoIA.getElapsedTime().asSeconds() >= pausaAntesArenaIA) {
                esperandoResolverMovimientoIA = false;
                jugadorIA.resolverMovimientoPreparado(mitablero, turnoActual, window, movimientoIA, logConsola, relojOscilacion, comprobarVictoria);
            }
        }

        // --- 5. RENDERIZADO (DIBUJAR PANTALLA) ---
        window.clear();

        if (enMenuPrincipal) {
            menuPrincipal.dibujar(window);
        }
        else if (enPantallaRanking) {
            menuPrincipal.dibujarPantallaRanking(window, ranking.obtenerTop(), ranking.obtenerGanadorGuerraTotal());
        }
        else {
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
