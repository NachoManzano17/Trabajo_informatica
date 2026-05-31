#include "ControladorJuego.h"
#include "GestorArena.h"
#include "dave.h"
#include "drzomboss.h"
#include "lanzaguisantes.h"
#include "zombidito.h"
#include "cactus.h"
#include "dronajo.h"
#include "planta_carnivora.h"
#include "allstar.h"
#include "soldado.h"
#include "loro_pirata.h"
#include "superCerebroz.h"
#include "pomelo.h"
#include <cmath>
#include "GestorAudio.h"
#include "GestorRanking.h"

void ControladorJuego::procesarClicRaton(int mouseX, int mouseY, int offsetX, int offsetY, int tamanoCasilla,
    EstadoJuego& estadoActual, tablero& mitablero,
    bool& hayPersonajeSeleccionado, int& fSel, int& cSel,
    bando& turnoActual, personaje*& liderSeleccionado,
    int& menuX, int& menuY, int& fOrigenTeleport, int& cOrigenTeleport,
    const std::function<void(std::string)>& logConsola,
    sf::Clock& relojOscilacion, sf::RenderWindow& window,
    bool modoUnJugador, Dificultad dificultadIA,
    const std::function<void()>& comprobarVictoria, std::vector<std::string>& cementerioPlantas,
    std::vector<std::string>& cementerioZombis, std::string& muertoSeleccionado) {

    // 1. Calculamos las coordenadas relativas al tablero
    int ratonX = mouseX - offsetX;
    int ratonY = mouseY - offsetY;

    bool clicEnTablero = (ratonX >= 0 && ratonX < (9 * tamanoCasilla) && ratonY >= 0 && ratonY < (9 * tamanoCasilla));

    if (clicEnTablero || estadoActual == MENU_ACCION_LIDER || estadoActual == MENU_HECHIZOS || estadoActual == ELEGIR_MUERTO_RESUCITAR) {

        int cClic = ratonX / tamanoCasilla;
        int fClic = ratonY / tamanoCasilla;

        auto mandarAlCementerio = [&](personaje* victima) {
            if (!victima) return;
            std::string sim = victima->getsimbolo();
            if (sim == "Dave" || sim == "DrZomboss") return;
            if (victima->getequipo() == bando::planta) cementerioPlantas.push_back(sim);
            else cementerioZombis.push_back(sim);
            };

        // --- MODO SELECCIONANDO PIEZA ---
        if (estadoActual == SELECCIONANDO_PIEZA) {
            personaje* fichaClicada = mitablero.getFicha(fClic, cClic);

            if (!hayPersonajeSeleccionado) {
                if (fichaClicada != nullptr) {
                    if (fichaClicada->getequipo() == turnoActual) {
                        if (fichaClicada->getsimbolo() == "Dave" || fichaClicada->getsimbolo() == "DrZomboss") {
                            estadoActual = MENU_ACCION_LIDER;
                            menuX = mouseX;
                            menuY = mouseY;
                            liderSeleccionado = fichaClicada;
                            fSel = fClic; cSel = cClic;
                            logConsola("Seleccionado: " + fichaClicada->getsimbolo());
                            logConsola("Abriendo menu del lider");
                        }
                        else {
                            hayPersonajeSeleccionado = true;
                            fSel = fClic; cSel = cClic;
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
                        if (!atacante->esmovimientovalido(fSel, cSel, fClic, cClic)) {
                            logConsola("-> Ataque invalido: La pieza no se mueve asi.");
                            hayPersonajeSeleccionado = false;
                            fSel = -1; cSel = -1;
                        }
                        else {
                            int bandoFavorecido = 0;
                            bool esOscilante = ((fClic == 2 && cClic == 2) || (fClic == 2 && cClic == 6) ||
                                (fClic == 6 && cClic == 2) || (fClic == 6 && cClic == 6));

                            if (esOscilante) {
                                float tiempoAtaque = relojOscilacion.getElapsedTime().asSeconds() * 0.5f;
                                float factorAtaque = (std::sin(tiempoAtaque) + 1.0f) / 2.0f;
                                if (factorAtaque < 0.5f) bandoFavorecido = 1;
                                else bandoFavorecido = 2;
                            }

                            logConsola("¡COMBATE INICIADO!");
                            if (bandoFavorecido == 1) logConsola("-> ¡Plantas con ventaja de terreno (+30%)!");
                            if (bandoFavorecido == 2) logConsola("-> ¡Zombis con ventaja de terreno (+30%)!");

                            bool combateConZombieIA = modoUnJugador && (atacante->getequipo() == bando::zombi || defensor->getequipo() == bando::zombi);

                            int resultado = resolverCombateEnArena(atacante, defensor, window, bandoFavorecido, combateConZombieIA, dificultadIA);

                            if (resultado == 1) {
                                logConsola("-> Gana Atacante");
                                mandarAlCementerio(mitablero.getFicha(fClic, cClic));
                                mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);

                             
                                if (mitablero.escasilladepoder(fClic, cClic)) GestorAudio::reproducirPoder();
                            }
                            else if (resultado == 2) {
                                logConsola("-> Gana Defensor");
                                mandarAlCementerio(mitablero.getFicha(fSel, cSel));
                                mitablero.eliminarFicha(fSel, cSel);
                            }
                            else {
                                logConsola("-> Empate (Mueren los dos)");
                                mandarAlCementerio(mitablero.getFicha(fSel, cSel));
                                mandarAlCementerio(mitablero.getFicha(fClic, cClic));
                                mitablero.eliminarFicha(fSel, cSel);
                                mitablero.eliminarFicha(fClic, cClic);
                            }
                            turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                            comprobarVictoria();
                        }
                    }
                    else if (defensor == nullptr) {
                        bool exito = mitablero.procesarmovimiento(fSel, cSel, fClic, cClic);
                        if (exito) {
                            logConsola("-> Movimiento completado");

                            
                            if (mitablero.escasilladepoder(fClic, cClic)) GestorAudio::reproducirPoder();

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
        // --- MODO ACCIÓN LIDER ---
        else if (estadoActual == MENU_ACCION_LIDER) {
            int relX = mouseX - menuX;
            int relY = mouseY - menuY;

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
        // --- MODO HECHIZOS ---
        else if (estadoActual == MENU_HECHIZOS) {
            int relX = mouseX - menuX;
            int relY = mouseY - menuY;

            if (relX < 0 || relX > 200 || relY < 0 || relY > 150) {
                estadoActual = SELECCIONANDO_PIEZA;
                liderSeleccionado = nullptr;
                logConsola("-> Menu cerrado\n");
            }
            else {
                bool puedeC = false; bool puedeT = false;

                if (dave* liderDave = dynamic_cast<dave*>(liderSeleccionado)) {
                    puedeC = liderDave->puedeCurar(); puedeT = liderDave->puedeTeletransportar();
                }
                else if (drzomboss* liderZomboss = dynamic_cast<drzomboss*>(liderSeleccionado)) {
                    puedeC = liderZomboss->puedeCurar(); puedeT = liderZomboss->puedeTeletransportar();
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
                    if (dave* d = dynamic_cast<dave*>(liderSeleccionado)) puedeR = d->puedeResucitar();
                    else if (drzomboss* z = dynamic_cast<drzomboss*>(liderSeleccionado)) puedeR = z->puedeResucitar();

                    if (puedeR) {
                        std::vector<std::string>& miCementerio = (turnoActual == bando::planta) ? cementerioPlantas : cementerioZombis;
                        if (miCementerio.empty()) {
                            logConsola("-> Tu cementerio esta vacio. Nadie a quien revivir.");
                            estadoActual = SELECCIONANDO_PIEZA;
                        }
                        else {
                            estadoActual = ELEGIR_MUERTO_RESUCITAR;
                            logConsola("MODO RESUCITAR: Elige a quien revivir del menu.");
                        }
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
        // --- EJECUCIÓN DE HECHIZOS ---
       
	   // Hechizo Curar: El jugador debe seleccionar a un aliado herido para curarlo.
       // Si la curación es efectiva, se consume el hechizo y se pasa el turno. 
       // Si el objetivo ya está a tope, se cancela la acción sin gastar el hechizo ni el turno.
        else if (estadoActual == ESPERANDO_OBJETIVO_CURAR) {
            personaje* objetivo = mitablero.getFicha(fClic, cClic);

            if (objetivo != nullptr && objetivo->getequipo() == liderSeleccionado->getequipo()) {

				// Verificamos si la curación va a ser efectiva antes de consumir el hechizo
                if (objetivo->obtenerVida() < objetivo->getvidaMaxima()) {
                    GestorAudio::reproducirCurar();

                    objetivo->curar(100);

                    if (dave* d = dynamic_cast<dave*>(liderSeleccionado)) d->consumirCurar();
                    else if (drzomboss* z = dynamic_cast<drzomboss*>(liderSeleccionado)) z->consumirCurar();

                    logConsola("-> ¡Aliado curado exitosamente!");

                    // Solo pasamos el turno si la curación ha sido efectiva
                    turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                    comprobarVictoria();
                }
                else {
                    // Si ya está a tope, avisamos y no gastamos el hechizo ni el turno
                    logConsola("-> El aliado ya tiene la vida al maximo.");
                }
            }
            else {
                logConsola("-> Objetivo invalido. Cura cancelada.");
            }

            // Siempre volvemos al estado normal al terminar, sea éxito o no
            estadoActual = SELECCIONANDO_PIEZA;
            liderSeleccionado = nullptr;
            }
			// Seleccionar Muerto a Resucitar: Mostramos el cementerio como un menú y dejamos que el jugador elija
        else if (estadoActual == ELEGIR_MUERTO_RESUCITAR) {
            std::vector<std::string>& miCementerio = (turnoActual == bando::planta) ? cementerioPlantas : cementerioZombis;
            int relX = mouseX - menuX;
            int relY = mouseY - menuY;

            // Cada fila de muerto mide 30px
            if (relX >= 0 && relX <= 200 && relY >= 0 && relY < miCementerio.size() * 30) {
                int indiceElegido = relY / 30;
                muertoSeleccionado = miCementerio[indiceElegido];

                // Lo borramos del cementerio para que no pueda revivir 2 veces
                miCementerio.erase(miCementerio.begin() + indiceElegido);

                estadoActual = ESPERANDO_DESTINO_RESUCITAR;
                logConsola("-> Has elegido revivir a: " + muertoSeleccionado);
                logConsola("-> Haz clic en una casilla vacia junto al lider.");
            }
            else {
                logConsola("-> Resurreccion cancelada.");
                estadoActual = SELECCIONANDO_PIEZA;
                liderSeleccionado = nullptr;
            }
        }
        else if (estadoActual == ESPERANDO_DESTINO_RESUCITAR) {
            personaje* destino = mitablero.getFicha(fClic, cClic);
            int fLider = -1, cLider = -1;
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if (mitablero.getFicha(i, j) == liderSeleccionado) { fLider = i; cLider = j; }
                }
            }
            if (destino == nullptr && std::abs(fClic - fLider) <= 1 && std::abs(cClic - cLider) <= 1) {
                personaje* piezaResucitada = nullptr;
                int equipo = (turnoActual == bando::planta) ? 0 : 1;

                // Creamos el personaje resucitado
                if (muertoSeleccionado == "Lanzaguisantes") piezaResucitada = new lanzaguisantes(equipo);
                else if (muertoSeleccionado == "Cactus") piezaResucitada = new cactus(equipo);
                else if (muertoSeleccionado == "Pomelo") piezaResucitada = new pomelo(equipo);
                else if (muertoSeleccionado == "Dronajo") piezaResucitada = new dronajo(equipo);
                else if (muertoSeleccionado == "PlantaCarnivora") piezaResucitada = new plantaCarnivora(equipo);
                else if (muertoSeleccionado == "Zombidito") piezaResucitada = new zombidito(equipo);
                else if (muertoSeleccionado == "AllStar") piezaResucitada = new allstar(equipo);
                else if (muertoSeleccionado == "Soldado") piezaResucitada = new soldado(equipo);
                else if (muertoSeleccionado == "LoroPirata") piezaResucitada = new loropirata(equipo);
                else if (muertoSeleccionado == "SuperCerebroz") piezaResucitada = new superCerebroz(equipo);

                if (piezaResucitada != nullptr) {
                    GestorAudio::reproducirRevivir();
                    mitablero.colocarpersonaje(fClic, cClic, piezaResucitada);
                    if (dave* d = dynamic_cast<dave*>(liderSeleccionado)) d->consumirResucitar();
                    else if (drzomboss* z = dynamic_cast<drzomboss*>(liderSeleccionado)) z->consumirResucitar();

                    logConsola("-> ¡" + muertoSeleccionado + " ha resucitado!");
                    turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                    comprobarVictoria();
                }
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
                fOrigenTeleport = fClic; cOrigenTeleport = cClic;
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
                GestorAudio::reproducirTeletransporte();

                mitablero.forzarMovimiento(fOrigenTeleport, cOrigenTeleport, fClic, cClic);

                if (dave* d = dynamic_cast<dave*>(liderSeleccionado)) d->consumirTeletransportar();
                else if (drzomboss* z = dynamic_cast<drzomboss*>(liderSeleccionado)) z->consumirTeletransportar();

                logConsola("-> ¡Teletransporte completado!");
                turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
                comprobarVictoria();
            }
            else { logConsola("-> Casilla ocupada. Teletransporte cancelado."); }

            estadoActual = SELECCIONANDO_PIEZA;
            liderSeleccionado = nullptr;
            }
    } 
} 
void ControladorJuego::evaluarVictoria(tablero& mitablero, int& ganadorJuego, std::function<void(const std::string&)> logConsola, GestorRanking& ranking, bool modoUnJugador) {
    // 1. REVISAR ANIQUILACIÓN TOTAL
    int totalPlantas = 0, totalZombis = 0;
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
        GestorAudio::reproducirVictoria(modoUnJugador, 2);
    }
    else if (totalZombis == 0) {
        logConsola("¡VICTORIA PLANTA!"); ganadorJuego = 1; ranking.registrarVictoria("Plantas");
        GestorAudio::reproducirVictoria(modoUnJugador, 1);
    }

    // 2. REVISAR NODOS DE PODER
    int controlPlanta = 0, controlZombi = 0;
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
        GestorAudio::reproducirVictoria(modoUnJugador, 1);
    }
    else if (controlZombi == 5) {
        logConsola("¡DOMINIO ZOMBI!"); ganadorJuego = 2; ranking.registrarVictoria("Zombis");
        GestorAudio::reproducirVictoria(modoUnJugador, 2);
    }
}
