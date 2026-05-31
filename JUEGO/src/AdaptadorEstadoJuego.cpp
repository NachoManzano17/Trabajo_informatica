#include "AdaptadorEstadoJuego.h"

#include <cmath>
#include <memory>
#include <vector>

AdaptadorEstadoJuego::AdaptadorEstadoJuego(const tablero& tableroReal, bando turnoActual)
    : estado(tableroReal), turno(turnoActual) {
}

int AdaptadorEstadoJuego::jugadorDesdeBando(bando equipo) {
    if (equipo == bando::planta) return 0;
    if (equipo == bando::zombi) return 1;
    return -1;
}

bando AdaptadorEstadoJuego::bandoDesdeJugador(int jugador) {
    return jugador == 0 ? bando::planta : bando::zombi;
}

std::vector<Movimiento> AdaptadorEstadoJuego::getMovimientosLegales() const {
    std::vector<Movimiento> movimientos;

    for (int f = 0; f < 9; ++f) {
        for (int c = 0; c < 9; ++c) {
            personaje* ficha = estado.getFicha(f, c);
            if (ficha == nullptr) continue;
            if (ficha->getequipo() != turno) continue;

            for (int fd = 0; fd < 9; ++fd) {
                for (int cd = 0; cd < 9; ++cd) {
                    if (fd == f && cd == c) continue;

                    if (!ficha->esmovimientovalido(f, c, fd, cd)) continue;
                    if (!ficha->puedeSaltar() && estado.hayObstaculoEnCamino(f, c, fd, cd)) continue;

                    personaje* destino = estado.getFicha(fd, cd);
                    if (destino != nullptr && destino->getequipo() == ficha->getequipo()) continue;

                    Movimiento m;
                    m.tipo = Movimiento::Tipo::Mover;
                    m.filaOrigen = f;
                    m.colOrigen = c;
                    m.filaDestino = fd;
                    m.colDestino = cd;
                    movimientos.push_back(m);
                }
            }
        }
    }

    return movimientos;
}

std::unique_ptr<IEstadoJuego> AdaptadorEstadoJuego::clonar() const {
    return std::make_unique<AdaptadorEstadoJuego>(*this);
}

void AdaptadorEstadoJuego::aplicarMovimiento(const Movimiento& m) {
    if (m.tipo == Movimiento::Tipo::Mover) {
        estado.procesarmovimiento(m.filaOrigen, m.colOrigen, m.filaDestino, m.colDestino);
    }

    estado.avanzarturno();
    turno = (turno == bando::planta) ? bando::zombi : bando::planta;
}

bool AdaptadorEstadoJuego::esTerminal() const {
    return estado.comprobarvictoria() != bando::ninguno;
}

int AdaptadorEstadoJuego::jugadorActual() const {
    return jugadorDesdeBando(turno);
}

int AdaptadorEstadoJuego::ganador() const {
    bando ganadorBando = estado.comprobarvictoria();
    if (ganadorBando == bando::ninguno) return -1;
    return jugadorDesdeBando(ganadorBando);
}

double AdaptadorEstadoJuego::valorPieza(const personaje* p) {
    if (p == nullptr) return 0.0;

    const std::string tipo = p->getsimbolo();
    double base = 100.0;

    if (tipo == "Dave" || tipo == "DrZomboss") base = 500.0;
    else if (tipo == "PlantaCarnivora" || tipo == "SuperCerebroz") base = 260.0;
    else if (tipo == "Pomelo" || tipo == "allstar") base = 230.0;
    else if (tipo == "Cactus" || tipo == "Soldado") base = 210.0;
    else if (tipo == "Dronajo" || tipo == "LoroPirata") base = 180.0;
    else if (tipo == "Lanzaguisantes" || tipo == "Zombidito") base = 120.0;

    return base + static_cast<double>(p->obtenerVida());
}

double AdaptadorEstadoJuego::puntuacionBando(bando equipo) const {
    double score = 0.0;

    for (int f = 0; f < 9; ++f) {
        for (int c = 0; c < 9; ++c) {
            personaje* p = estado.getFicha(f, c);
            if (p == nullptr || p->getequipo() != equipo) continue;

            score += valorPieza(p);

            // Pequeno premio por acercarse al centro del tablero.
            score += 8.0 - (std::abs(f - 4) + std::abs(c - 4));

            // Premio por controlar puntos de poder.
            if (estado.escasilladepoder(f, c)) score += 80.0;
        }
    }

    AdaptadorEstadoJuego copia(estado, equipo);
    score += static_cast<double>(copia.getMovimientosLegales().size()) * 2.0;

    return score;
}

double AdaptadorEstadoJuego::evaluar(int jugadorPerspectiva) const {
    bando mio = bandoDesdeJugador(jugadorPerspectiva);
    bando rival = (mio == bando::planta) ? bando::zombi : bando::planta;

    bando ganadorBando = estado.comprobarvictoria();
    if (ganadorBando == mio) return 1000000.0;
    if (ganadorBando == rival) return -1000000.0;

    return puntuacionBando(mio) - puntuacionBando(rival);
}
