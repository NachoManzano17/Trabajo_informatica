#include "JugadorIA.h"
#include "GestorArena.h"
#include "EstrategiaHeuristica.h"
#include "EstrategiaMinimax.h"
#include "EstrategiaAleatoria.h"
#include "AdaptadorEstadoJuego.h"
#include "Movimiento.h"
#include "tablero.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>
#include <string>
#include <vector>

std::unique_ptr<EstrategiaIA> JugadorIA::construir(Dificultad d, int profundidadDificil) {
    switch (d) {
    case Dificultad::Facil:
        return std::make_unique<EstrategiaAleatoria>();
    case Dificultad::Normal:
        return std::make_unique<EstrategiaHeuristica>();
    case Dificultad::Dificil:
        return std::make_unique<EstrategiaMinimax>(profundidadDificil);
    }

    return std::make_unique<EstrategiaHeuristica>();
}

JugadorIA::JugadorIA(Dificultad d, int profundidadDificil)
    : estrategia(construir(d, profundidadDificil)), dificultadActual(d) {
}

Movimiento JugadorIA::elegirMovimiento(const IEstadoJuego& estado) {
    return estrategia->elegirMovimiento(estado);
}

void JugadorIA::cambiarDificultad(Dificultad d, int profundidadDificil) {
    dificultadActual = d;
    estrategia = construir(d, profundidadDificil);
}

Movimiento elegirMovimiento(const IEstadoJuego& estado, Dificultad d, int profundidadDificil) {
    JugadorIA jugador(d, profundidadDificil);
    return jugador.elegirMovimiento(estado);
}

double JugadorIA::valorPiezaParaIA(personaje* p) {
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

bool JugadorIA::esMovimientoDeAtaque(tablero& t, const Movimiento& m, bando equipoIA) {
    personaje* destino = t.getFicha(m.filaDestino, m.colDestino);
    return destino != nullptr && destino->getequipo() != equipoIA;
}

Movimiento JugadorIA::elegirMovimientoIAConPresion(
    tablero& t,
    const std::vector<Movimiento>& legales,
    const AdaptadorEstadoJuego& estadoIA
) {
    static std::mt19937 generadorAleatorio(
        static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    );

    const Dificultad dificultadIA = dificultad();
    const bando equipoIA = (estadoIA.jugadorActual() == 0) ? bando::planta : bando::zombi;

    std::vector<Movimiento> ataques;
    std::vector<Movimiento> noAtaques;

    ataques.reserve(legales.size());
    noAtaques.reserve(legales.size());

    for (const Movimiento& m : legales) {
        if (esMovimientoDeAtaque(t, m, equipoIA)) {
            ataques.push_back(m);
        }
        else {
            noAtaques.push_back(m);
        }
    }

    int probabilidadBuscarAtaque = 35;
    if (dificultadIA == Dificultad::Normal) probabilidadBuscarAtaque = 72;
    if (dificultadIA == Dificultad::Dificil) probabilidadBuscarAtaque = 88;

    std::uniform_int_distribution<int> probabilidad(0, 99);

    if (!ataques.empty() && probabilidad(generadorAleatorio) < probabilidadBuscarAtaque) {
        std::sort(ataques.begin(), ataques.end(), [&](const Movimiento& a, const Movimiento& b) {
            personaje* destinoA = t.getFicha(a.filaDestino, a.colDestino);
            personaje* destinoB = t.getFicha(b.filaDestino, b.colDestino);

            double valorA = valorPiezaParaIA(destinoA);
            double valorB = valorPiezaParaIA(destinoB);

            valorA += ((a.filaOrigen * 9 + a.colOrigen) % 5) * 2.0;
            valorB += ((b.filaOrigen * 9 + b.colOrigen) % 5) * 2.0;

            return valorA > valorB;
        });

        size_t limite = 1;

        if (dificultadIA == Dificultad::Facil) {
            limite = std::min<size_t>(ataques.size(), 4);
        }
        else if (dificultadIA == Dificultad::Normal) {
            limite = std::min<size_t>(ataques.size(), 3);
        }
        else {
            limite = std::min<size_t>(ataques.size(), 2);
        }

        std::uniform_int_distribution<size_t> distribucion(0, limite - 1);
        return ataques[distribucion(generadorAleatorio)];
    }

    Movimiento elegido = elegirMovimiento(estadoIA);

    bool elegidoLegal = false;
    for (const Movimiento& m : legales) {
        if (m == elegido) {
            elegidoLegal = true;
            break;
        }
    }

    if (!elegidoLegal) {
        elegido = legales.front();
    }

    int probabilidadVariar = 35;
    if (dificultadIA == Dificultad::Normal) probabilidadVariar = 30;
    if (dificultadIA == Dificultad::Dificil) probabilidadVariar = 12;

    if (!noAtaques.empty() && probabilidad(generadorAleatorio) < probabilidadVariar) {
        std::shuffle(noAtaques.begin(), noAtaques.end(), generadorAleatorio);
        return noAtaques.front();
    }

    return elegido;
}

bool JugadorIA::jugarTurno(
    tablero& mitablero,
    bando& turnoActual,
    sf::RenderWindow& window,
    const std::function<void(std::string)>& logConsola,
    sf::Clock& relojOscilacion,
    const std::function<void()>& comprobarVictoria
) {
    AdaptadorEstadoJuego estadoIA(mitablero, turnoActual);
    std::vector<Movimiento> legales = estadoIA.getMovimientosLegales();

    if (legales.empty()) {
        logConsola("IA: no tiene movimientos legales. Pierde el turno.");
        turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
        return false;
    }

    Movimiento elegido = elegirMovimientoIAConPresion(mitablero, legales, estadoIA);

    bool esLegal = false;
    for (const Movimiento& m : legales) {
        if (m == elegido) {
            esLegal = true;
            break;
        }
    }

    if (!esLegal) {
        logConsola("IA: el movimiento elegido no era legal. Uso el primero.");
        elegido = legales.front();
    }

    personaje* atacante = mitablero.getFicha(elegido.filaOrigen, elegido.colOrigen);
    personaje* defensor = mitablero.getFicha(elegido.filaDestino, elegido.colDestino);

    if (atacante == nullptr) {
        logConsola("IA: no se encontro la ficha origen.");
        return false;
    }

    logConsola("IA mueve " + atacante->getsimbolo() + " de (" +
        std::to_string(elegido.filaOrigen) + "," + std::to_string(elegido.colOrigen) +
        ") a (" + std::to_string(elegido.filaDestino) + "," + std::to_string(elegido.colDestino) + ")");

    if (defensor != nullptr && defensor->getequipo() != atacante->getequipo()) {
        int bandoFavorecido = 0;

        bool esOscilante =
            (elegido.filaDestino == 2 && elegido.colDestino == 2) ||
            (elegido.filaDestino == 2 && elegido.colDestino == 6) ||
            (elegido.filaDestino == 6 && elegido.colDestino == 2) ||
            (elegido.filaDestino == 6 && elegido.colDestino == 6);

        if (esOscilante) {
            float tiempoAtaque = relojOscilacion.getElapsedTime().asSeconds() * 0.5f;
            float factorAtaque = (std::sin(tiempoAtaque) + 1.0f) / 2.0f;

            if (factorAtaque < 0.5f) {
                bandoFavorecido = 1;
            }
            else {
                bandoFavorecido = 2;
            }
        }

        logConsola("IA inicia combate");

        if (bandoFavorecido == 1) {
            logConsola("-> Plantas con ventaja de terreno (+30%).");
        }

        if (bandoFavorecido == 2) {
            logConsola("-> Zombis con ventaja de terreno (+30%).");
        }

        int resultado = resolverCombateEnArena(
            atacante,
            defensor,
            window,
            bandoFavorecido,
            true,
            dificultad()
        );

        if (resultado == 1) {
            logConsola("-> Gana la IA atacante.");
            mitablero.procesarmovimiento(
                elegido.filaOrigen,
                elegido.colOrigen,
                elegido.filaDestino,
                elegido.colDestino
            );
        }
        else if (resultado == 2) {
            logConsola("-> Gana el defensor humano.");
            mitablero.eliminarFicha(elegido.filaOrigen, elegido.colOrigen);
        }
        else {
            logConsola("-> Empate: mueren los dos.");
            mitablero.eliminarFicha(elegido.filaOrigen, elegido.colOrigen);
            mitablero.eliminarFicha(elegido.filaDestino, elegido.colDestino);
        }
    }
    else {
        bool exito = mitablero.procesarmovimiento(
            elegido.filaOrigen,
            elegido.colOrigen,
            elegido.filaDestino,
            elegido.colDestino
        );

        if (!exito) {
            logConsola("IA: el tablero rechazo el movimiento.");
            return false;
        }

        logConsola("-> Movimiento de IA completado.");
    }

    turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
    comprobarVictoria();

    return true;
}

bool JugadorIA::prepararMovimientoVisual(
    tablero& mitablero,
    bando turnoActual,
    Movimiento& movimientoPreparado,
    const std::function<void(std::string)>& logConsola
) {
    AdaptadorEstadoJuego estadoIA(mitablero, turnoActual);
    std::vector<Movimiento> legales = estadoIA.getMovimientosLegales();

    if (legales.empty()) {
        logConsola("IA: no tiene movimientos legales. Pierde el turno.");
        return false;
    }

    Movimiento elegido = elegirMovimientoIAConPresion(mitablero, legales, estadoIA);

    bool esLegal = false;
    for (const Movimiento& m : legales) {
        if (m == elegido) {
            esLegal = true;
            break;
        }
    }

    if (!esLegal) {
        logConsola("IA: el movimiento elegido no era legal. Uso el primero.");
        elegido = legales.front();
    }

    personaje* atacante = mitablero.getFicha(elegido.filaOrigen, elegido.colOrigen);

    if (atacante == nullptr) {
        logConsola("IA: no se encontro la ficha origen.");
        return false;
    }

    movimientoPreparado = elegido;

    logConsola("IA mueve " + atacante->getsimbolo() + " de (" +
        std::to_string(elegido.filaOrigen) + "," + std::to_string(elegido.colOrigen) +
        ") a (" + std::to_string(elegido.filaDestino) + "," + std::to_string(elegido.colDestino) + ")");

    return true;
}

bool JugadorIA::resolverMovimientoPreparado(
    tablero& mitablero,
    bando& turnoActual,
    sf::RenderWindow& window,
    const Movimiento& elegido,
    const std::function<void(std::string)>& logConsola,
    sf::Clock& relojOscilacion,
    const std::function<void()>& comprobarVictoria
) {
    personaje* atacante = mitablero.getFicha(elegido.filaOrigen, elegido.colOrigen);
    personaje* defensor = mitablero.getFicha(elegido.filaDestino, elegido.colDestino);

    if (atacante == nullptr) {
        logConsola("IA: no se encontro la ficha origen.");
        return false;
    }

    if (defensor != nullptr && defensor->getequipo() != atacante->getequipo()) {
        int bandoFavorecido = 0;

        bool esOscilante =
            (elegido.filaDestino == 2 && elegido.colDestino == 2) ||
            (elegido.filaDestino == 2 && elegido.colDestino == 6) ||
            (elegido.filaDestino == 6 && elegido.colDestino == 2) ||
            (elegido.filaDestino == 6 && elegido.colDestino == 6);

        if (esOscilante) {
            float tiempoAtaque = relojOscilacion.getElapsedTime().asSeconds() * 0.5f;
            float factorAtaque = (std::sin(tiempoAtaque) + 1.0f) / 2.0f;

            if (factorAtaque < 0.5f) {
                bandoFavorecido = 1;
            }
            else {
                bandoFavorecido = 2;
            }
        }

        logConsola("IA inicia combate");

        if (bandoFavorecido == 1) {
            logConsola("-> Plantas con ventaja de terreno (+30%).");
        }

        if (bandoFavorecido == 2) {
            logConsola("-> Zombis con ventaja de terreno (+30%).");
        }

        int resultado = resolverCombateEnArena(
            atacante,
            defensor,
            window,
            bandoFavorecido,
            true,
            dificultad()
        );

        if (resultado == 1) {
            logConsola("-> Gana la IA atacante.");
            mitablero.procesarmovimiento(
                elegido.filaOrigen,
                elegido.colOrigen,
                elegido.filaDestino,
                elegido.colDestino
            );
        }
        else if (resultado == 2) {
            logConsola("-> Gana el defensor humano.");
            mitablero.eliminarFicha(elegido.filaOrigen, elegido.colOrigen);
        }
        else {
            logConsola("-> Empate: mueren los dos.");
            mitablero.eliminarFicha(elegido.filaOrigen, elegido.colOrigen);
            mitablero.eliminarFicha(elegido.filaDestino, elegido.colDestino);
        }
    }
    else {
        bool exito = mitablero.procesarmovimiento(
            elegido.filaOrigen,
            elegido.colOrigen,
            elegido.filaDestino,
            elegido.colDestino
        );

        if (!exito) {
            logConsola("IA: el tablero rechazo el movimiento.");
            return false;
        }

        logConsola("-> Movimiento de IA completado.");
    }

    turnoActual = (turnoActual == bando::planta) ? bando::zombi : bando::planta;
    comprobarVictoria();

    return true;
}

