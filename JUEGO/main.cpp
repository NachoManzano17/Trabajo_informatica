#include <SFML/Graphics.hpp>
#include <iostream>
#include <locale>
#include <windows.h>
#include "src/tablero.h"
#include "src/personaje.h"
#include "src/GestorArena.h"
#include "src/dave.h"
#include "src/drzomboss.h"
#include "src/lanzaguisantes.h" 
#include "src/zombidito.h"
#include <cmath> 


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
    // 1. CONFIGURACIÓN DEL IDIOMA
    setlocale(LC_ALL, "spanish"); // Configura el idioma de C++
    SetConsoleOutputCP(1252);     // Configura la consola de Windows para leer acentos y ñ

    // ---> ¡NUEVO! 1. CREAMOS LA VENTANA A PANTALLA COMPLETA <---
    // Preguntamos a Windows de qué tamaño es tu pantalla (ej: 1920x1080)
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Archon Warfare", sf::Style::Fullscreen);
    window.setFramerateLimit(60);// 60 FPS

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

    //Estados gestion de hechizos
    enum EstadoJuego {
        SELECCIONANDO_PIEZA,
        MENU_ACCION_LIDER,
        MENU_HECHIZOS,
        ESPERANDO_OBJETIVO_CURAR,
        ESPERANDO_ORIGEN_TELEPORT,
        ESPERANDO_DESTINO_TELEPORT,
        ESPERANDO_DESTINO_RESUCITAR
    };
    EstadoJuego estadoActual = SELECCIONANDO_PIEZA;
    int fOrigenTeleport = -1;
    int cOrigenTeleport = -1;
    int menuX = 0;
    int menuY = 0;
    personaje* liderSeleccionado = nullptr;

    sf::Font fuente;
    if (!fuente.loadFromFile("COOPBL.ttf")) { // Asegúrate de tener el archivo arial.ttf en la carpeta del proyecto
        std::cout << "Error cargando fuente" << std::endl;
    }
    //VARIABLES PARA LA CONSOLA IN-GAME
    std::vector<std::string> historialConsola;
    int maxMensajes = 5; // Cuántas líneas de texto quieres que se vean a la vez

    // Truco de C++11: Una función Lambda rápida para imprimir en ambos lados
    auto logConsola = [&](std::string mensaje) {
        historialConsola.push_back(mensaje);
        // Si nos pasamos de líneas, borramos la primera (la más vieja)
        if (historialConsola.size() > maxMensajes) {
            historialConsola.erase(historialConsola.begin());
        }
        // Lo seguimos imprimiendo en la consola negra por si acaso para depurar
        std::cout << mensaje << std::endl;
        };

    // Mensaje de bienvenida
    logConsola("¡Bienvenido a Archon: Plantas vs Zombies!");

    sf::Text txtHechizo1("1. Curar", fuente, 20);
    sf::Text txtHechizo2("2. Resucitar", fuente, 20);
    sf::Text txtHechizo3("3. Teletransportar", fuente, 20);


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
                // 1. Comprobamos si el clic cae físicamente dentro de la cuadrícula
                bool clicEnTablero = (ratonX >= 0 && ratonX < (9 * tamanoCasilla) && ratonY >= 0 && ratonY < (9 * tamanoCasilla));

                // 2. Aceptamos el clic SI está en el tablero, O SI estamos interactuando con algún menú
                if (clicEnTablero || estadoActual == MENU_ACCION_LIDER || estadoActual == MENU_HECHIZOS) {

                    int cClic = ratonX / tamanoCasilla;
                    int fClic = ratonY / tamanoCasilla;

                    // --- 1. SI ESTAMOS JUGANDO NORMAL ---
                    if (estadoActual == SELECCIONANDO_PIEZA) {
                        personaje* fichaClicada = mitablero.getFicha(fClic, cClic);

                        // 1A. NO TENEMOS NADA SELECCIONADO (Fase de coger pieza)
                        if (!hayPersonajeSeleccionado) {
                            if (fichaClicada != nullptr) {
                                // [NUEVO] ¡Control de turnos!
                                if (fichaClicada->getequipo() == turnoActual) {

                                    // Si es líder de tu turno, abre menú
                                    if (fichaClicada->getsimbolo() == "Dave" || fichaClicada->getsimbolo() == "DrZomboss") {
                                        estadoActual = MENU_ACCION_LIDER;
                                        menuX = event.mouseButton.x;
                                        menuY = event.mouseButton.y;
                                        liderSeleccionado = fichaClicada;
                                        fSel = fClic;
                                        cSel = cClic;
                                        logConsola("Seleccionado: " + fichaClicada->getsimbolo());
                                        logConsola("Abriendo menu del lider");
                                    }
                                    // Si es tropa normal de tu turno, la seleccionamos
                                    else {
                                        hayPersonajeSeleccionado = true;
                                        fSel = fClic;
                                        cSel = cClic;
                                        logConsola("Seleccionado: " + fichaClicada->getsimbolo());
                                    }
                                }
                                else {
                                    logConsola("¡Quieto ahi! No es tu turno.");
                                }
                            }
                        }
                        // 1B. YA TENEMOS UNA PIEZA AGARRADA (Fase de mover/atacar)
                        else {
                            if (fClic == fSel && cClic == cSel) {
                                hayPersonajeSeleccionado = false;
                                fSel = -1; cSel = -1;
                                logConsola("-> Deseleccionado");
                            }
                            else {
                                personaje* atacante = mitablero.getFicha(fSel, cSel);
                                personaje* defensor = mitablero.getFicha(fClic, cClic);

                                if (!atacante->puedeSaltar() && mitablero.hayObstaculoEnCamino(fSel, cSel, fClic, cClic)) {
                                    logConsola("-> Movimiento invalido: Hay piezas bloqueando el camino.");
                                    hayPersonajeSeleccionado = false;
                                    fSel = -1; cSel = -1;
                                }
                                else if (defensor != nullptr && atacante->getequipo() != defensor->getequipo()) {
                                    logConsola("¡COMBATE INICIADO!");
                                    int resultado = resolverCombateEnArena(atacante, defensor, window);

                                    if (resultado == 1) {
                                        logConsola("-> Gana Atacante");
                                        mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);
                                    }
                                    else if (resultado == 2) {
                                        logConsola("-> Gana Defensor");
                                        mitablero.eliminarFicha(fSel, cSel);
                                    }
                                    else {
                                        logConsola("-> Empate (Mueren los dos)");
                                        mitablero.eliminarFicha(fSel, cSel);
                                        mitablero.eliminarFicha(fClic, cClic);
                                    }
                                    // [NUEVO] Cambio de turno tras el combate
                                    turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                                }
                                else if (defensor == nullptr) {
                                    bool exito = mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);
                                    if (exito) {
                                        logConsola("-> Movimiento completado");
                                        // [NUEVO] Cambio de turno tras mover con éxito
                                        turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                                    }
                                    else {
                                        logConsola("-> Movimiento invalido");
                                    }
                                }
                                else {
                                    logConsola("-> Movimiento cancelado: No puedes pisar a un aliado.");
                                }

                                hayPersonajeSeleccionado = false;
                                fSel = -1; cSel = -1;
                            }
                        }
                    }
                    // --- 2 SI ESTAMOS ELIGIENDO QUÉ HACE EL LÍDER ---
                    else if (estadoActual == MENU_ACCION_LIDER) {
                        int relX = event.mouseButton.x - menuX;
                        int relY = event.mouseButton.y - menuY;

                        // Menú más pequeñito (200x85)
                        if (relX < 0 || relX > 200 || relY < 0 || relY > 85) {
                            estadoActual = SELECCIONANDO_PIEZA;
                            liderSeleccionado = nullptr;
                            fSel = -1; cSel = -1;
                            logConsola("-> Menu cancelado");
                        }
                        else {
                            // Opción 1: Moverse (Y entre 10 y 40 aprox)
                            if (relY >= 10 && relY <= 40) {
                                estadoActual = SELECCIONANDO_PIEZA;
                                hayPersonajeSeleccionado = true; // ¡Lo seleccionamos para mover!
                                logConsola("Seleccionado: " + liderSeleccionado->getsimbolo());
                            }
                            // Opción 2: Usar Hechizos (Y entre 45 y 75 aprox)
                            else if (relY >= 45 && relY <= 75) {
                                estadoActual = MENU_HECHIZOS;
                                // Deseleccionamos el modo "mover" para pasar al modo "hechizo"
                                hayPersonajeSeleccionado = false;
                                fSel = -1; cSel = -1;
                                logConsola("Abriendo menu de hechizos");
                            }
                        }
                    }
                    // --- 3. SI ESTAMOS EN EL MENÚ DE HECHIZOS ---
                    else if (estadoActual == MENU_HECHIZOS) {
                        int relX = event.mouseButton.x - menuX;
                        int relY = event.mouseButton.y - menuY;

                        if (relX < 0 || relX > 200 || relY < 0 || relY > 150) {
                            estadoActual = SELECCIONANDO_PIEZA;
                            liderSeleccionado = nullptr;
                            logConsola("-> Menu cerrado\n");
                        }
                        else {
                            // Truco de veterano: Sacamos las variables de permiso fuera para no duplicar código
                            bool puedeC = false;
                            bool puedeT = false;

                            if (dave* liderDave = dynamic_cast<dave*>(liderSeleccionado)) {
                                puedeC = liderDave->puedeCurar();
                                puedeT = liderDave->puedeTeletransportar();
                            }
                            else if (drzomboss* liderZomboss = dynamic_cast<drzomboss*>(liderSeleccionado)) {
                                puedeC = liderZomboss->puedeCurar();
                                puedeT = liderZomboss->puedeTeletransportar();
                            }

                            // Clic en Opción 1: CURAR
                            if (relY >= 10 && relY <= 40) {
                                if (puedeC) {
                                    estadoActual = ESPERANDO_OBJETIVO_CURAR;
                                    logConsola("MODO CURAR: Selecciona un aliado herido.\n");
                                }
                                else {
                                    logConsola("-> Hechizo Curar ya gastado.\n");
                                    estadoActual = SELECCIONANDO_PIEZA;
                                }
                            }
                            // Clic en Opción 2: RESUCITAR 
                            else if (relY >= 45 && relY <= 75) {
                                bool puedeR = false;

                                // Comprobamos quién es el líder y si tiene el hechizo
                                if (dave* liderDave = dynamic_cast<dave*>(liderSeleccionado)) puedeR = liderDave->puedeResucitar();
                                else if (drzomboss* liderZomboss = dynamic_cast<drzomboss*>(liderSeleccionado)) puedeR = liderZomboss->puedeResucitar();

                                if (puedeR) {
                                    estadoActual = ESPERANDO_DESTINO_RESUCITAR;
                                    logConsola("MODO RESUCITAR: Haz clic en una casilla vacia junto al lider.");
                                }
                                else {
                                    logConsola("-> Hechizo Resucitar ya gastado.");
                                    estadoActual = SELECCIONANDO_PIEZA;
                                }
                            }
                            // Clic en Opción 3: TELETRANSPORTAR
                            else if (relY >= 80 && relY <= 110) {
                                if (puedeT) {
                                    estadoActual = ESPERANDO_ORIGEN_TELEPORT;
                                    logConsola("MODO TELEPORT: Selecciona a quien quieres mover.\n");
                                }
                                else {
                                    logConsola("-> Hechizo Teletransporte ya gastado.\n");
                                    estadoActual = SELECCIONANDO_PIEZA;
                                }
                            }
                        }
                    }
                    // --- 3. EJECUTANDO HECHIZOS ---
                     //HECHIZO CURAR
                    else if (estadoActual == ESPERANDO_OBJETIVO_CURAR) {
                        personaje* objetivo = mitablero.getFicha(fClic, cClic);
                        if (objetivo != nullptr && objetivo->getequipo() == liderSeleccionado->getequipo()) {
                            objetivo->curar(100);

                            if (dave* d = dynamic_cast<dave*>(liderSeleccionado)) d->consumirCurar();
                            else if (drzomboss* z = dynamic_cast<drzomboss*>(liderSeleccionado)) z->consumirCurar();

                            logConsola("-> ¡Aliado curado exitosamente!");
                            // [NUEVO] Cambio de turno al lanzar hechizo
                            turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                        }
                        else {
                            logConsola("-> Objetivo invalido. Cura cancelada.");
                        }
                        estadoActual = SELECCIONANDO_PIEZA;
                        liderSeleccionado = nullptr;
                    }
                    //HECHIZO RESUCITAR
                    else if (estadoActual == ESPERANDO_DESTINO_RESUCITAR) {
                        personaje* destino = mitablero.getFicha(fClic, cClic);

                        int fLider = -1, cLider = -1;
                        for (int i = 0; i < 9; i++) {
                            for (int j = 0; j < 9; j++) {
                                if (mitablero.getFicha(i, j) == liderSeleccionado) {
                                    fLider = i; cLider = j;
                                }
                            }
                        }

                        if (destino == nullptr && std::abs(fClic - fLider) <= 1 && std::abs(cClic - cLider) <= 1) {
                            personaje* piezaResucitada = nullptr;
                            if (liderSeleccionado->getequipo() == bando::planta) {
                                piezaResucitada = new lanzaguisantes(0);
                            }
                            else {
                                piezaResucitada = new zombidito(1);
                            }

                            mitablero.colocarpersonaje(fClic, cClic, piezaResucitada);

                            if (dave* d = dynamic_cast<dave*>(liderSeleccionado)) d->consumirResucitar();
                            else if (drzomboss* z = dynamic_cast<drzomboss*>(liderSeleccionado)) z->consumirResucitar();

                            logConsola("-> ¡Pieza resucitada con exito!");
                            // [NUEVO] Cambio de turno al resucitar
                            turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                        }
                        else {
                            logConsola("-> Invalido: La casilla debe estar vacia y pegada al lider.");
                        }

                        estadoActual = SELECCIONANDO_PIEZA;
                        liderSeleccionado = nullptr;
                    }

                    //HECHIZO TELETRANSPORTAR
                    else if (estadoActual == ESPERANDO_ORIGEN_TELEPORT) {
                        personaje* objetivo = mitablero.getFicha(fClic, cClic);
                        if (objetivo != nullptr && objetivo->getequipo() == liderSeleccionado->getequipo()) {
                            fOrigenTeleport = fClic;
                            cOrigenTeleport = cClic;
                            estadoActual = ESPERANDO_DESTINO_TELEPORT;
                            logConsola("-> Origen fijado. Haz clic en una casilla vacia de destino.");
                        }
                        else {
                            logConsola("-> Aliado invalido. Teletransporte cancelado.");
                            estadoActual = SELECCIONANDO_PIEZA;
                        }
                    }
                    else if (estadoActual == ESPERANDO_DESTINO_TELEPORT) {
                        personaje* destino = mitablero.getFicha(fClic, cClic);

                        // Solo exigimos que la casilla destino esté vacía, ¡nada más!
                        if (destino == nullptr) {

                            // [NUEVO] Usamos la función forzada del tablero
                            mitablero.forzarMovimiento(fOrigenTeleport, cOrigenTeleport, fClic, cClic);

                            if (dave* d = dynamic_cast<dave*>(liderSeleccionado)) d->consumirTeletransportar();
                            else if (drzomboss* z = dynamic_cast<drzomboss*>(liderSeleccionado)) z->consumirTeletransportar();

                            logConsola("-> ¡Teletransporte completado!");
                            turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                        }
                        else {
                            logConsola("-> Casilla ocupada. Teletransporte cancelado.");
                        }

                        estadoActual = SELECCIONANDO_PIEZA;
                        liderSeleccionado = nullptr;
                    }
                }
            }

        }
        // --- B. DIBUJAR LA PANTALLA ---
        window.clear();

        //  Dibujar el fondo cuadriculado
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

        // 4 Dibujar el menú de acción del líder
        if (estadoActual == MENU_ACCION_LIDER) {
            sf::RectangleShape fondoMenuLider(sf::Vector2f(200, 85));
            fondoMenuLider.setPosition(menuX, menuY);
            fondoMenuLider.setFillColor(sf::Color(20, 60, 80, 240)); // Tono azulado para diferenciar
            fondoMenuLider.setOutlineThickness(2);
            fondoMenuLider.setOutlineColor(sf::Color::Cyan);
            window.draw(fondoMenuLider);

            sf::Text txtOpcionLider1("1. Moverse", fuente, 20);
            sf::Text txtOpcionLider2("2. Hechizos", fuente, 20);

            txtOpcionLider1.setPosition(menuX + 10, menuY + 10);
            txtOpcionLider2.setPosition(menuX + 10, menuY + 45);

            window.draw(txtOpcionLider1);
            window.draw(txtOpcionLider2);
        }
        // 5. Dibujar el menú de hechizos
        if (estadoActual == MENU_HECHIZOS) {
            sf::RectangleShape fondoMenu(sf::Vector2f(200, 120));
            fondoMenu.setPosition(menuX, menuY);
            fondoMenu.setFillColor(sf::Color(40, 40, 40, 235));
            fondoMenu.setOutlineThickness(2);
            fondoMenu.setOutlineColor(sf::Color::Yellow);
            window.draw(fondoMenu);

            txtHechizo1.setPosition(menuX + 10, menuY + 10);
            txtHechizo2.setPosition(menuX + 10, menuY + 45);
            txtHechizo3.setPosition(menuX + 10, menuY + 80);

            window.draw(txtHechizo1);
            window.draw(txtHechizo2);
            window.draw(txtHechizo3);
        }
        // 6. DIBUJAR LA CONSOLA IN-GAME 
        // A. El fondo negro tipo terminal
        sf::RectangleShape fondoConsola(sf::Vector2f(810.f, 150.f)); // Ancho como el tablero, alto 150px
        fondoConsola.setPosition(0.f, 945.f); // Justo debajo del tablero
        fondoConsola.setFillColor(sf::Color(15, 15, 15, 230)); // Casi negro
        fondoConsola.setOutlineThickness(2.f);
        fondoConsola.setOutlineColor(sf::Color(100, 100, 100)); // Borde gris
        window.draw(fondoConsola);

        // B. Escribir los mensajes guardados
        sf::Text textoConsola("", fuente, 18); // Tamaño de letra 18
        textoConsola.setFillColor(sf::Color(50, 255, 50)); // Color verde

        for (size_t i = 0; i < historialConsola.size(); ++i) {
            textoConsola.setString(sf::String::fromUtf8(historialConsola[i].begin(), historialConsola[i].end()));
            // Cada línea baja 25 píxeles respecto a la anterior
            textoConsola.setPosition(15.f, 950.f + (i * 25.f));
            window.draw(textoConsola);
        }

        // 6. EL CHIVATO VISUAL DE TURNOS
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

