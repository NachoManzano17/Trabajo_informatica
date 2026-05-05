#include <SFML/Graphics.hpp>
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
#include "src/AIPlayer.h"
#include "src/Difficulty.h"
#include "src/GameStateAdapter.h"
#include "src/Move.h"

class LuchadorAvatar : public PersonajeBaseArena {
private:
    sf::CircleShape shape;
    std::string letraOriginal;
    bool esZombi;
    bool mirandoIzquierda;
    float radioFisico;
    bool haMovido = false;

public:
    LuchadorAvatar(personaje* fichaReal, bool tieneBoost = false) : PersonajeBaseArena(100.0f, 5.0f, 20.0f, false, false) {
        letraOriginal = fichaReal->getsimbolo();
        esZombi = (fichaReal->getequipo() == bando::zombi);

        mirandoIzquierda = esZombi;

        float vMax = 100.0f; float dmg = 20.0f; float vel = 5.0f;
        bool volador = false; bool melee = false;
        radioFisico = 35.0f;

        if (letraOriginal == "Cactus" || letraOriginal == "Soldado") { vMax = 80; dmg = 50; vel = 4.0f; }
        else if (letraOriginal == "Pomelo" || letraOriginal == "allstar") { vMax = 150; dmg = 45; vel = 2.5f; }
        else if (letraOriginal == "SuperCerebroz") {
            vMax = 120; dmg = 70; vel = 5.0f; melee = true; volador = false;
            radioFisico = 50.0f;
        }
        else if (letraOriginal == "PlantaCarnivora") { vMax = 120; dmg = 70; vel = 5.0f; melee = true; }
        else if (letraOriginal == "Dronajo" || letraOriginal == "LoroPirata") { vMax = 60; dmg = 10; vel = 7.0f; volador = true; }
        else if (letraOriginal == "Dave" || letraOriginal == "DrZomboss") { vMax = 200; dmg = 40; vel = 4.0f; melee = true; }
        else { vMax = 100; dmg = 20; vel = 5.0f; }

        this->vidaMaxima = vMax;
        this->vidaActual = (float)fichaReal->obtenerVida();
        this->danio = dmg; this->velocidad = vel;
        this->esVolador = volador; this->esCC = melee;

        shape.setRadius(radioFisico);
        shape.setOrigin(radioFisico, radioFisico);
        if (!esZombi) shape.setFillColor(sf::Color(50, 200, 50, 100));
        else shape.setFillColor(sf::Color(150, 50, 200, 100));

        // APLICAR EL BOOST DE LA CASILLA OSCILANTE
        if (tieneBoost) {
            this->vidaMaxima *= 1.3f; // +30% de Vida Máxima
            this->vidaActual *= 1.3f; // +30% de Vida Actual
            this->danio *= 1.2f;      // +20% de Daño
        }
    }

    void mover(float dx, float dy) override {
        PersonajeBaseArena::mover(dx, dy);
        if (dx != 0.0f || dy != 0.0f) haMovido = true;
    }

    void dibujar(sf::RenderWindow& window) override {
        shape.setPosition(posicion.x, posicion.y);

        if (haMovido) {
            float angulo = getAngulo();
            while (angulo < 0.0f) angulo += 360.0f;
            while (angulo >= 360.0f) angulo -= 360.0f;

            if (angulo > 90.0f && angulo < 270.0f) mirandoIzquierda = true;
            else if (angulo < 90.0f || angulo > 270.0f) mirandoIzquierda = false;
        }

        static std::map<std::string, sf::Texture> texturas;
        static std::map<std::string, sf::Sprite> sprites;
        static bool cargadas = false;

        if (!cargadas) {
            auto cargarFicha = [&](std::string nombreSimbolo, std::string archivoPng) {
                if (texturas[nombreSimbolo].loadFromFile(archivoPng)) {
                    sprites[nombreSimbolo].setTexture(texturas[nombreSimbolo]);
                    sprites[nombreSimbolo].setOrigin(texturas[nombreSimbolo].getSize().x / 2.0f, texturas[nombreSimbolo].getSize().y / 2.0f);
                }
                };
            cargarFicha("Dave", "dave.png"); cargarFicha("allstar", "allstar.png");
            cargarFicha("Cactus", "cactus.png"); cargarFicha("Dronajo", "dronajo.png");
            cargarFicha("DrZomboss", "drzomboss.png"); cargarFicha("Lanzaguisantes", "lanzaguisantes.png");
            cargarFicha("LoroPirata", "loropirata.png"); cargarFicha("PlantaCarnivora", "plantacarnivora.png");
            cargarFicha("Pomelo", "pomelo.png"); cargarFicha("Soldado", "soldado.png");
            cargarFicha("SuperCerebroz", "supercerebroz.png"); cargarFicha("Zombidito", "zombidito.png");
            cargadas = true;
        }

        if (sprites.count(letraOriginal) > 0) {
            sprites[letraOriginal].setPosition(posicion.x, posicion.y);
            float tamanoVisual = (letraOriginal == "SuperCerebroz") ? 115.0f : 70.0f;
            float escalaX = tamanoVisual / texturas[letraOriginal].getSize().x;
            float escalaY = tamanoVisual / texturas[letraOriginal].getSize().y;

            if (!esZombi && mirandoIzquierda) escalaX = -escalaX;
            else if (esZombi && !mirandoIzquierda) escalaX = -escalaX;

            sprites[letraOriginal].setScale(escalaX, escalaY);
            window.draw(sprites[letraOriginal]);
        }
        else { window.draw(shape); }
    }
};

// Función combinada con IA y Boosts visuales
int resolverCombateEnArena(personaje* atacante, personaje* defensor, sf::RenderWindow& window, int bandoFavorecido = 0, bool zombieControladoPorIA = false, Difficulty dificultadArenaIA = Difficulty::Medium) {

    personaje* plantaTablero = (atacante->getequipo() == bando::planta) ? atacante : defensor;
    personaje* zombiTablero = (atacante->getequipo() == bando::zombi) ? atacante : defensor;

    bool plantaBoost = (bandoFavorecido == 1);
    bool zombiBoost = (bandoFavorecido == 2);

    PersonajeBaseArena* pArena = new LuchadorAvatar(plantaTablero, plantaBoost);
    PersonajeBaseArena* zArena = new LuchadorAvatar(zombiTablero, zombiBoost);

    float anchoReal = (float)window.getSize().x;
    float altoReal = (float)window.getSize().y;
    GestorArena arena(pArena, zArena, anchoReal, altoReal);

    arena.inicializarArena(6);
    arena.setZombieControladoPorIA(zombieControladoPorIA);
    arena.setDificultadIA(dificultadArenaIA);

    int ganadorArena = 0;

    while (window.isOpen() && !arena.isTerminado()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
                return 0;
            }
        }

        arena.procesarEntrada();
        arena.actualizarLider();

        window.clear(sf::Color(30, 30, 30));
        arena.dibujarEscena(window);
        window.display();
    }

    if (arena.isTerminado()) {
        ganadorArena = arena.getGanador();

        float danioP = plantaTablero->obtenerVida() - pArena->getVida();
        if (danioP > 0) plantaTablero->recibirDano((int)danioP);

        float danioZ = zombiTablero->obtenerVida() - zArena->getVida();
        if (danioZ > 0) zombiTablero->recibirDano((int)danioZ);

        sf::sleep(sf::seconds(2));
    }

    delete pArena;
    delete zArena;

    if (ganadorArena == 0) return 0; // Empate

    if (atacante->getequipo() == bando::planta) {
        return (ganadorArena == 1) ? 1 : 2;
    }
    else {
        return (ganadorArena == 2) ? 1 : 2;
    }
}

namespace {
    double valorPiezaParaIA(personaje* p) {
        if (p == nullptr) return 0.0;
        const std::string tipo = p->getsimbolo();
        double base = 100.0;
        if (tipo == "Dave" || tipo == "DrZomboss") base = 520.0;
        else if (tipo == "PlantaCarnivora" || tipo == "SuperCerebroz") base = 285.0;
        else if (tipo == "Pomelo" || tipo == "allstar") base = 255.0;
        else if (tipo == "Cactus" || tipo == "Soldado") base = 235.0;
        else if (tipo == "Dronajo" || tipo == "LoroPirata") base = 205.0;
        else if (tipo == "Lanzaguisantes" || tipo == "Zombidito") base = 145.0;
        return base + p->obtenerVida();
    }

    bool esMovimientoDeAtaque(tablero& t, const Move& m, bando equipoIA) {
        personaje* destino = t.getFicha(m.toRow, m.toCol);
        return destino != nullptr && destino->getequipo() != equipoIA;
    }

    Move elegirMovimientoIAConPresion(tablero& t, const std::vector<Move>& legales, AIPlayer& ia, const GameStateAdapter& estadoIA) {
        static std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());

        const Difficulty dificultad = ia.difficulty();
        const bando equipoIA = (estadoIA.currentPlayer() == 0) ? bando::planta : bando::zombi;

        std::vector<Move> ataques;
        std::vector<Move> noAtaques;
        ataques.reserve(legales.size());
        noAtaques.reserve(legales.size());

        for (const Move& m : legales) {
            if (esMovimientoDeAtaque(t, m, equipoIA)) ataques.push_back(m);
            else noAtaques.push_back(m);
        }

        int probBuscarAtaque = 35;
        if (dificultad == Difficulty::Medium) probBuscarAtaque = 72;
        if (dificultad == Difficulty::Hard) probBuscarAtaque = 88;

        if (!ataques.empty() && (rand() % 100) < probBuscarAtaque) {
            std::sort(ataques.begin(), ataques.end(), [&](const Move& a, const Move& b) {
                personaje* da = t.getFicha(a.toRow, a.toCol);
                personaje* db = t.getFicha(b.toRow, b.toCol);
                double va = valorPiezaParaIA(da);
                double vb = valorPiezaParaIA(db);

                va += ((a.fromRow * 9 + a.fromCol) % 5) * 2.0;
                vb += ((b.fromRow * 9 + b.fromCol) % 5) * 2.0;
                return va > vb;
                });

            size_t limite = 1;
            if (dificultad == Difficulty::Easy) limite = std::min<size_t>(ataques.size(), 4);
            else if (dificultad == Difficulty::Medium) limite = std::min<size_t>(ataques.size(), 3);
            else limite = std::min<size_t>(ataques.size(), 2);

            std::uniform_int_distribution<size_t> dist(0, limite - 1);
            return ataques[dist(rng)];
        }

        Move elegido = ia.chooseMove(estadoIA);

        bool elegidoLegal = false;
        for (const Move& m : legales) {
            if (m == elegido) { elegidoLegal = true; break; }
        }
        if (!elegidoLegal) elegido = legales.front();

        int probVariar = 35;
        if (dificultad == Difficulty::Medium) probVariar = 30;
        if (dificultad == Difficulty::Hard) probVariar = 12;

        if (!noAtaques.empty() && (rand() % 100) < probVariar) {
            std::shuffle(noAtaques.begin(), noAtaques.end(), rng);
            return noAtaques.front();
        }

        return elegido;
    }
}

// Actualizamos ejecutarTurnoIA para que sepa mirar los buffos de la casilla y comprobar victoria
bool ejecutarTurnoIA(tablero& mitablero,
    AIPlayer& ia,
    bando& turnoActual,
    sf::RenderWindow& window,
    const std::function<void(std::string)>& logConsola,
    sf::Clock& relojOscilacion,
    const std::function<void()>& comprobarVictoria) {

    GameStateAdapter estadoIA(mitablero, turnoActual);
    std::vector<Move> legales = estadoIA.getLegalMoves();

    if (legales.empty()) {
        logConsola("IA: no tiene movimientos legales. Pierde el turno.");
        turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
        return false;
    }

    Move elegido;
    try {
        elegido = elegirMovimientoIAConPresion(mitablero, legales, ia, estadoIA);
    }
    catch (const std::exception& e) {
        logConsola(std::string("IA: error al elegir movimiento: ") + e.what());
        elegido = legales.front();
    }

    bool esLegal = false;
    for (const Move& m : legales) {
        if (m == elegido) {
            esLegal = true;
            break;
        }
    }

    if (!esLegal) {
        logConsola("IA: el movimiento elegido no era legal. Uso el primer.");
        elegido = legales.front();
    }

    personaje* atacante = mitablero.getFicha(elegido.fromRow, elegido.fromCol);
    personaje* defensor = mitablero.getFicha(elegido.toRow, elegido.toCol);

    if (atacante == nullptr) {
        logConsola("IA: no se encontro la ficha origen.");
        return false;
    }

    logConsola("IA mueve " + atacante->getsimbolo() + " de (" +
        std::to_string(elegido.fromRow) + "," + std::to_string(elegido.fromCol) +
        ") a (" + std::to_string(elegido.toRow) + "," + std::to_string(elegido.toCol) + ")");

    if (defensor != nullptr && defensor->getequipo() != atacante->getequipo()) {

        // LA IA TAMBIÉN CÁLCULA SU VENTAJA DE TERRENO
        int bandoFavorecido = 0;
        bool esOscilante = ((elegido.toRow == 2 && elegido.toCol == 2) || (elegido.toRow == 2 && elegido.toCol == 6) ||
            (elegido.toRow == 6 && elegido.toCol == 2) || (elegido.toRow == 6 && elegido.toCol == 6));

        if (esOscilante) {
            float tiempoAtaque = relojOscilacion.getElapsedTime().asSeconds() * 0.5f;
            float factorAtaque = (std::sin(tiempoAtaque) + 1.0f) / 2.0f;
            if (factorAtaque < 0.5f) bandoFavorecido = 1; // Planta
            else bandoFavorecido = 2; // Zombi
        }

        logConsola("IA inicia combate");
        if (bandoFavorecido == 1) logConsola("-> ¡Plantas con ventaja de terreno (+30%)!");
        if (bandoFavorecido == 2) logConsola("-> ¡Zombis con ventaja de terreno (+30%)!");

        int resultado = resolverCombateEnArena(atacante, defensor, window, bandoFavorecido, true, ia.difficulty());

        if (resultado == 1) {
            logConsola("-> Gana la IA atacante");
            mitablero.procesarmovimiento(elegido.fromRow, elegido.fromCol, elegido.toRow, elegido.toCol);
        }
        else if (resultado == 2) {
            logConsola("-> Gana el defensor humano");
            mitablero.eliminarFicha(elegido.fromRow, elegido.fromCol);
        }
        else {
            logConsola("-> Empate: mueren los dos");
            mitablero.eliminarFicha(elegido.fromRow, elegido.fromCol);
            mitablero.eliminarFicha(elegido.toRow, elegido.toCol);
        }
    }
    else {
        bool exito = mitablero.procesarmovimiento(elegido.fromRow, elegido.fromCol, elegido.toRow, elegido.toCol);
        if (!exito) {
            logConsola("IA: el tablero rechazo el movimiento.");
            return false;
        }
        logConsola("-> Movimiento de IA completado");
    }

    turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
    comprobarVictoria(); // <-- Fíjate, la IA también verifica si te acaba de ganar
    return true;
}

int main() {
    setlocale(LC_ALL, "spanish");
    SetConsoleOutputCP(1252);

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(desktop, "Archon Warfare", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    sf::Texture texturaCesped;
    if (!texturaCesped.loadFromFile("cesped.PNG")) std::cout << "[ERROR] Falta cesped.PNG\n";
    sf::Texture texturaTierra;
    if (!texturaTierra.loadFromFile("tierra.PNG")) std::cout << "[ERROR] Falta tierra.PNG\n";
    sf::Texture texturaFondo;
    if (!texturaFondo.loadFromFile("fondo.PNG")) std::cout << "[ERROR] Falta fondo.PNG\n";

    sf::Sprite spriteFondo(texturaFondo);
    spriteFondo.setScale((float)desktop.width / texturaFondo.getSize().x, (float)desktop.height / texturaFondo.getSize().y);

    sf::Texture texturaCasillaPlanta;
    if (!texturaCasillaPlanta.loadFromFile("casilla_planta.png")) std::cout << "[ERROR] Falta casilla_planta.png\n";
    sf::Texture texturaCasillaZombi;
    if (!texturaCasillaZombi.loadFromFile("casilla_zombi.png")) std::cout << "[ERROR] Falta casilla_zombi.png\n";
    sf::Texture texturaPoder;
    if (!texturaPoder.loadFromFile("casilla_poder.png")) std::cout << "[ERROR] Falta casilla_poder.png\n";

    auto cargarFichaTablero = [&](sf::Texture& tex, sf::Sprite& spr, std::string archivo, float baseScale = 90.0f) {
        if (!tex.loadFromFile(archivo)) std::cout << "[ERROR] Falta " << archivo << "\n";
        spr.setTexture(tex);
        spr.setScale(baseScale / tex.getSize().x, baseScale / tex.getSize().y);
        };

    sf::Texture texturaDave, texturaallstar, texturaCactus, texturaDronajo, texturaDrZomboss, texturaLanzaguisantes;
    sf::Texture texturaLoroPirata, texturaPlantaCarnivora, texturaPomelo, texturaSoldado, texturaSuperCerebroz, texturaZombidito;
    sf::Sprite spriteDave, spriteallstar, spriteCactus, spriteDronajo, spriteDrZomboss, spriteLanzaguisantes;
    sf::Sprite spriteLoroPirata, spritePlantaCarnivora, spritePomelo, spriteSoldado, spriteSuperCerebroz, spriteZombidito;

    cargarFichaTablero(texturaDave, spriteDave, "dave.png");
    cargarFichaTablero(texturaallstar, spriteallstar, "allstar.png");
    cargarFichaTablero(texturaCactus, spriteCactus, "cactus.png");
    cargarFichaTablero(texturaDronajo, spriteDronajo, "dronajo.png");
    cargarFichaTablero(texturaDrZomboss, spriteDrZomboss, "drzomboss.png");
    cargarFichaTablero(texturaLanzaguisantes, spriteLanzaguisantes, "lanzaguisantes.png");
    cargarFichaTablero(texturaLoroPirata, spriteLoroPirata, "loropirata.png");
    cargarFichaTablero(texturaPlantaCarnivora, spritePlantaCarnivora, "plantacarnivora.png");
    cargarFichaTablero(texturaPomelo, spritePomelo, "pomelo.png", 80.0f);
    cargarFichaTablero(texturaSoldado, spriteSoldado, "soldado.png");
    cargarFichaTablero(texturaSuperCerebroz, spriteSuperCerebroz, "supercerebroz.png");
    cargarFichaTablero(texturaZombidito, spriteZombidito, "zombidito.png");

    tablero mitablero;
    mitablero.inicializarpartida();

    int tamanoCasilla = 90;
    bool hayPersonajeSeleccionado = false;
    int fSel = -1;
    int cSel = -1;
    bando turnoActual = bando::planta;

    sf::Clock relojOscilacion;

    int offsetX = (desktop.width - (9 * tamanoCasilla)) / 2;
    int offsetY = (desktop.height - (9 * tamanoCasilla)) / 2;

    enum EstadoJuego {
        SELECCIONANDO_PIEZA, MENU_ACCION_LIDER, MENU_HECHIZOS,
        ESPERANDO_OBJETIVO_CURAR, ESPERANDO_ORIGEN_TELEPORT,
        ESPERANDO_DESTINO_TELEPORT, ESPERANDO_DESTINO_RESUCITAR
    };
    EstadoJuego estadoActual = SELECCIONANDO_PIEZA;
    int fOrigenTeleport = -1, cOrigenTeleport = -1;
    int menuX = 0, menuY = 0;
    personaje* liderSeleccionado = nullptr;

    sf::Font fuente;
    if (!fuente.loadFromFile("COOPBL.ttf")) std::cout << "Error cargando fuente" << std::endl;

    std::vector<std::string> historialConsola;
    int maxMensajes = 5;

    auto logConsola = [&](std::string mensaje) {
        historialConsola.push_back(mensaje);
        if (historialConsola.size() > maxMensajes) historialConsola.erase(historialConsola.begin());
        std::cout << mensaje << std::endl;
        };

    auto comprobarVictoria = [&]() {
        int controlPlanta = 0;
        int controlZombi = 0;
        int casillasPoder[5][2] = { {0,4}, {4,0}, {4,4}, {4,8}, {8,4} };

        for (int i = 0; i < 5; i++) {
            personaje* ocupante = mitablero.getFicha(casillasPoder[i][0], casillasPoder[i][1]);
            if (ocupante != nullptr) {
                if (ocupante->getequipo() == bando::planta) controlPlanta++;
                else controlZombi++;
            }
        }

        if (controlPlanta == 5) logConsola("¡VICTORIA SUPREMA! Las Plantas dominan los Nodos de Poder.");
        else if (controlZombi == 5) logConsola("¡VICTORIA SUPREMA! Los Zombis dominan los Nodos de Poder.");
        };

    logConsola("¡Bienvenido a Archon: Plantas vs Zombies!");

    sf::Text txtHechizo1("1. Curar", fuente, 20);
    sf::Text txtHechizo2("2. Resucitar", fuente, 20);
    sf::Text txtHechizo3("3. Teletransportar", fuente, 20);

    Menu menuPrincipal(desktop.width, desktop.height);
    bool enMenuPrincipal = true;

    bool modoUnJugador = false;
    bool eligiendoDificultadIA = false;
    bando bandoIA = bando::zombi;
    AIPlayer jugadorIA(Difficulty::Medium, 3);

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            if (enMenuPrincipal) {
                EstadoMenu opcionElegida = menuPrincipal.manejarEventos(window, event);

                if (opcionElegida == EstadoMenu::UN_JUGADOR) {
                    modoUnJugador = true;
                    eligiendoDificultadIA = true;
                    enMenuPrincipal = false;
                    logConsola("Modo 1 jugador: elige dificultad con 1, 2 o 3.");
                    logConsola("1 Facil | 2 Normal | 3 Dificil");
                }
                else if (opcionElegida == EstadoMenu::DOS_JUGADORES) {
                    modoUnJugador = false;
                    eligiendoDificultadIA = false;
                    enMenuPrincipal = false;
                    logConsola("Modo 2 jugadores iniciado.");
                }
                else if (opcionElegida == EstadoMenu::SALIR) {
                    window.close();
                }
                continue;
            }

            if (eligiendoDificultadIA) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
                        jugadorIA.setDifficulty(Difficulty::Easy, 2);
                        eligiendoDificultadIA = false;
                        logConsola("Dificultad IA: Facil");
                    }
                    else if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
                        jugadorIA.setDifficulty(Difficulty::Medium, 3);
                        eligiendoDificultadIA = false;
                        logConsola("Dificultad IA: Normal");
                    }
                    else if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) {
                        jugadorIA.setDifficulty(Difficulty::Hard, 3);
                        eligiendoDificultadIA = false;
                        logConsola("Dificultad IA: Dificil");
                    }
                }
                continue;
            }

            if (modoUnJugador && turnoActual == bandoIA) {
                continue;
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

                int ratonX = event.mouseButton.x - offsetX;
                int ratonY = event.mouseButton.y - offsetY;

                bool clicEnTablero = (ratonX >= 0 && ratonX < (9 * tamanoCasilla) && ratonY >= 0 && ratonY < (9 * tamanoCasilla));

                if (clicEnTablero || estadoActual == MENU_ACCION_LIDER || estadoActual == MENU_HECHIZOS) {

                    int cClic = ratonX / tamanoCasilla;
                    int fClic = ratonY / tamanoCasilla;

                    if (estadoActual == SELECCIONANDO_PIEZA) {
                        personaje* fichaClicada = mitablero.getFicha(fClic, cClic);

                        if (!hayPersonajeSeleccionado) {
                            if (fichaClicada != nullptr) {
                                if (fichaClicada->getequipo() == turnoActual) {
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

                                    int bandoFavorecido = 0;
                                    bool esOscilante = ((fClic == 2 && cClic == 2) || (fClic == 2 && cClic == 6) ||
                                        (fClic == 6 && cClic == 2) || (fClic == 6 && cClic == 6));

                                    if (esOscilante) {
                                        float tiempoAtaque = relojOscilacion.getElapsedTime().asSeconds() * 0.5f;
                                        float factorAtaque = (std::sin(tiempoAtaque) + 1.0f) / 2.0f;

                                        if (factorAtaque < 0.5f) bandoFavorecido = 1; // Planta
                                        else bandoFavorecido = 2; // Zombi
                                    }

                                    logConsola("¡COMBATE INICIADO!");
                                    if (bandoFavorecido == 1) logConsola("-> ¡Plantas con ventaja de terreno (+30%)!");
                                    if (bandoFavorecido == 2) logConsola("-> ¡Zombis con ventaja de terreno (+30%)!");

                                    bool combateConZombieIA = modoUnJugador && (atacante->getequipo() == bando::zombi || defensor->getequipo() == bando::zombi);

                                    // LA FUNCIÓN SE ADAPTA A LA IA Y A LA VENTAJA DE TERRENO
                                    int resultado = resolverCombateEnArena(atacante, defensor, window, bandoFavorecido, combateConZombieIA, jugadorIA.difficulty());

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
                                    turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                                    comprobarVictoria();

                                }
                                else if (defensor == nullptr) {
                                    bool exito = mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);
                                    if (exito) {
                                        logConsola("-> Movimiento completado");
                                        turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                                        comprobarVictoria();
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
                    else if (estadoActual == MENU_ACCION_LIDER) {
                        int relX = event.mouseButton.x - menuX;
                        int relY = event.mouseButton.y - menuY;

                        if (relX < 0 || relX > 200 || relY < 0 || relY > 85) {
                            estadoActual = SELECCIONANDO_PIEZA;
                            liderSeleccionado = nullptr;
                            fSel = -1; cSel = -1;
                            logConsola("-> Menu cancelado");
                        }
                        else {
                            if (relY >= 10 && relY <= 40) {
                                estadoActual = SELECCIONANDO_PIEZA;
                                hayPersonajeSeleccionado = true;
                                logConsola("Seleccionado: " + liderSeleccionado->getsimbolo());
                            }
                            else if (relY >= 45 && relY <= 75) {
                                estadoActual = MENU_HECHIZOS;
                                hayPersonajeSeleccionado = false;
                                fSel = -1; cSel = -1;
                                logConsola("Abriendo menu de hechizos");
                            }
                        }
                    }
                    else if (estadoActual == MENU_HECHIZOS) {
                        int relX = event.mouseButton.x - menuX;
                        int relY = event.mouseButton.y - menuY;

                        if (relX < 0 || relX > 200 || relY < 0 || relY > 150) {
                            estadoActual = SELECCIONANDO_PIEZA;
                            liderSeleccionado = nullptr;
                            logConsola("-> Menu cerrado\n");
                        }
                        else {
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
                            else if (relY >= 45 && relY <= 75) {
                                bool puedeR = false;

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
                    else if (estadoActual == ESPERANDO_OBJETIVO_CURAR) {
                        personaje* objetivo = mitablero.getFicha(fClic, cClic);
                        if (objetivo != nullptr && objetivo->getequipo() == liderSeleccionado->getequipo()) {
                            objetivo->curar(100);

                            if (dave* d = dynamic_cast<dave*>(liderSeleccionado)) d->consumirCurar();
                            else if (drzomboss* z = dynamic_cast<drzomboss*>(liderSeleccionado)) z->consumirCurar();

                            logConsola("-> ¡Aliado curado exitosamente!");
                            turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                            comprobarVictoria();
                        }
                        else {
                            logConsola("-> Objetivo invalido. Cura cancelada.");
                        }
                        estadoActual = SELECCIONANDO_PIEZA;
                        liderSeleccionado = nullptr;
                    }
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
                            turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                            comprobarVictoria();
                        }
                        else {
                            logConsola("-> Invalido: La casilla debe estar vacia y pegada al lider.");
                        }

                        estadoActual = SELECCIONANDO_PIEZA;
                        liderSeleccionado = nullptr;
                    }
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

                        if (destino == nullptr) {
                            mitablero.forzarMovimiento(fOrigenTeleport, cOrigenTeleport, fClic, cClic);

                            if (dave* d = dynamic_cast<dave*>(liderSeleccionado)) d->consumirTeletransportar();
                            else if (drzomboss* z = dynamic_cast<drzomboss*>(liderSeleccionado)) z->consumirTeletransportar();

                            logConsola("-> ¡Teletransporte completado!");
                            turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                            comprobarVictoria();
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

        // ----------------------------------------------------
        // ---> ¡EL CEREBRO DE LA MÁQUINA SE PONE EN MARCHA! <---
        if (!enMenuPrincipal && !eligiendoDificultadIA && modoUnJugador && turnoActual == bandoIA && estadoActual == SELECCIONANDO_PIEZA) {
            hayPersonajeSeleccionado = false;
            fSel = -1;
            cSel = -1;
            // ¡PUM! Le pasamos el reloj de oscilaciones para que la IA sepa si ataca dopada
            ejecutarTurnoIA(mitablero, jugadorIA, turnoActual, window, logConsola, relojOscilacion, comprobarVictoria);
        }
        // ----------------------------------------------------

        // --- B. DIBUJAR LA PANTALLA ---
        window.clear();
        if (enMenuPrincipal) {
            menuPrincipal.dibujar(window);
        }
        else {
            window.draw(spriteFondo);

            // 1. Dibujar el fondo cuadriculado con las texturas
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    sf::RectangleShape casilla(sf::Vector2f(tamanoCasilla, tamanoCasilla));
                    casilla.setPosition(offsetX + (j * tamanoCasilla), offsetY + (i * tamanoCasilla));

                    if ((i + j) % 2 == 0) casilla.setTexture(&texturaCesped);
                    else casilla.setTexture(&texturaTierra);

                    casilla.setFillColor(sf::Color::White);
                    casilla.setOutlineThickness(-2.f);
                    casilla.setOutlineColor(sf::Color::Black);
                    window.draw(casilla);

                    bool esPoder = ((i == 4 && j == 0) || (i == 4 && j == 8) ||
                        (i == 0 && j == 4) || (i == 8 && j == 4) ||
                        (i == 4 && j == 4));

                    bool esOscilante = ((i == 2 && j == 2) || (i == 2 && j == 6) ||
                        (i == 6 && j == 2) || (i == 6 && j == 6));

                    casilla.setOutlineThickness(0.f);

                    if (esPoder) {
                        sf::Sprite spritePoder(texturaPoder);
                        spritePoder.setOrigin(texturaPoder.getSize().x / 2.0f, texturaPoder.getSize().y / 2.0f);
                        spritePoder.setPosition(offsetX + (j * tamanoCasilla) + (tamanoCasilla / 2.0f),
                            offsetY + (i * tamanoCasilla) + (tamanoCasilla / 2.0f));

                        float escalaPoder = 88.0f / texturaPoder.getSize().x;
                        spritePoder.setScale(escalaPoder, escalaPoder);
                        spritePoder.setColor(sf::Color(255, 255, 255, 255));
                        window.draw(spritePoder);
                    }
                    else if (esOscilante) {
                        float tiempo = relojOscilacion.getElapsedTime().asSeconds() * 0.5f;
                        float factor = (std::sin(tiempo) + 1.0f) / 2.0f;

                        sf::Uint8 alfaPlanta = (sf::Uint8)(factor * 220.0f);
                        sf::Uint8 alfaZombi = (sf::Uint8)((1.0f - factor) * 220.0f);

                        auto dibujarSpriteCentrado = [&](sf::Texture& tex, sf::Uint8 alfa) {
                            if (alfa > 0) {
                                sf::Sprite sprite(tex);
                                sprite.setOrigin(tex.getSize().x / 2.0f, tex.getSize().y / 2.0f);
                                sprite.setPosition(offsetX + (j * tamanoCasilla) + (tamanoCasilla / 2.0f),
                                    offsetY + (i * tamanoCasilla) + (tamanoCasilla / 2.0f));

                                float escala = 75.0f / tex.getSize().x;
                                sprite.setScale(escala, escala);
                                sprite.setColor(sf::Color(255, 255, 255, alfa));
                                window.draw(sprite);
                            }
                            };

                        dibujarSpriteCentrado(texturaCasillaPlanta, alfaPlanta);
                        dibujarSpriteCentrado(texturaCasillaZombi, alfaZombi);
                    }
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
                        if (fichaActual->getsimbolo() == "Dave") {
                            spriteDave.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteDave);
                        }
                        else if (fichaActual->getsimbolo() == "allstar") {
                            spriteallstar.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteallstar);
                        }
                        else if (fichaActual->getsimbolo() == "Cactus") {
                            spriteCactus.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteCactus);
                        }
                        else if (fichaActual->getsimbolo() == "Dronajo") {
                            spriteDronajo.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteDronajo);
                        }
                        else if (fichaActual->getsimbolo() == "DrZomboss") {
                            spriteDrZomboss.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteDrZomboss);
                        }
                        else if (fichaActual->getsimbolo() == "Lanzaguisantes") {
                            spriteLanzaguisantes.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteLanzaguisantes);
                        }
                        else if (fichaActual->getsimbolo() == "LoroPirata") {
                            spriteLoroPirata.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteLoroPirata);
                        }
                        else if (fichaActual->getsimbolo() == "PlantaCarnivora") {
                            spritePlantaCarnivora.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spritePlantaCarnivora);
                        }
                        else if (fichaActual->getsimbolo() == "Pomelo") {
                            spritePomelo.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spritePomelo);
                        }
                        else if (fichaActual->getsimbolo() == "Soldado") {
                            spriteSoldado.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteSoldado);
                        }
                        else if (fichaActual->getsimbolo() == "SuperCerebroz") {
                            spriteSuperCerebroz.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteSuperCerebroz);
                        }
                        else if (fichaActual->getsimbolo() == "Zombidito") {
                            spriteZombidito.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                            window.draw(spriteZombidito);
                        }
                        else {
                            sf::CircleShape circulo(tamanoCasilla / 2.5f);
                            circulo.setPosition(offsetX + (j * tamanoCasilla) + 9, offsetY + (i * tamanoCasilla) + 9);

                            if (fichaActual->getequipo() == bando::planta) circulo.setFillColor(sf::Color(50, 205, 50));
                            else circulo.setFillColor(sf::Color(138, 43, 226));

                            window.draw(circulo);
                        }
                    }
                }
            }

            // 4 Dibujar el menú de acción del líder
            if (estadoActual == MENU_ACCION_LIDER) {
                sf::RectangleShape fondoMenuLider(sf::Vector2f(200, 85));
                fondoMenuLider.setPosition(menuX, menuY);
                fondoMenuLider.setFillColor(sf::Color(20, 60, 80, 240));
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
            sf::RectangleShape fondoConsola(sf::Vector2f(810.f, 150.f));
            fondoConsola.setPosition(0.f, 945.f);
            fondoConsola.setFillColor(sf::Color(15, 15, 15, 230));
            fondoConsola.setOutlineThickness(2.f);
            fondoConsola.setOutlineColor(sf::Color(100, 100, 100));
            window.draw(fondoConsola);

            sf::Text textoConsola("", fuente, 18);
            textoConsola.setFillColor(sf::Color(50, 255, 50));

            for (size_t i = 0; i < historialConsola.size(); ++i) {
                textoConsola.setString(sf::String::fromUtf8(historialConsola[i].begin(), historialConsola[i].end()));
                textoConsola.setPosition(15.f, 950.f + (i * 25.f));
                window.draw(textoConsola);
            }

            // 7. EL CHIVATO VISUAL DE TURNOS
            sf::CircleShape chivatoTurno(20.0f);
            chivatoTurno.setPosition(20.0f, 20.0f);

            if (turnoActual == bando::planta) chivatoTurno.setFillColor(sf::Color(50, 205, 50));
            else chivatoTurno.setFillColor(sf::Color(138, 43, 226));

            chivatoTurno.setOutlineThickness(3.0f);
            chivatoTurno.setOutlineColor(sf::Color::White);

            window.draw(chivatoTurno);

            if (eligiendoDificultadIA) {
                sf::RectangleShape fondoDif(sf::Vector2f(760.f, 220.f));
                fondoDif.setPosition((desktop.width - 760.f) / 2.f, (desktop.height - 220.f) / 2.f);
                fondoDif.setFillColor(sf::Color(10, 10, 10, 230));
                fondoDif.setOutlineThickness(3.f);
                fondoDif.setOutlineColor(sf::Color::White);
                window.draw(fondoDif);

                sf::Text txtDif("Elige dificultad de la IA", fuente, 34);
                txtDif.setFillColor(sf::Color::White);
                txtDif.setPosition(fondoDif.getPosition().x + 70.f, fondoDif.getPosition().y + 35.f);
                window.draw(txtDif);

                sf::Text txtOpciones("Pulsa 1 = Facil     2 = Normal     3 = Dificil", fuente, 26);
                txtOpciones.setFillColor(sf::Color(50, 255, 50));
                txtOpciones.setPosition(fondoDif.getPosition().x + 70.f, fondoDif.getPosition().y + 115.f);
                window.draw(txtOpciones);
            }
        }
        window.display();
    }

    return 0;
}
