#include "EstrategiaHeuristica.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>


Movimiento EstrategiaHeuristica::elegirMovimiento(const IEstadoJuego& estado) {
    auto movimientos = estado.getMovimientosLegales();
    if (movimientos.empty()) {
        throw std::runtime_error("EstrategiaHeuristica: no hay movimientos legales");
    }

    const int yo = estado.jugadorActual();

    struct MovimientoPuntuado {
        Movimiento movimiento;
        double puntuacion;
    };

    std::vector<MovimientoPuntuado> puntuados;
    puntuados.reserve(movimientos.size());

    for (const auto& m : movimientos) {
        auto hijo = estado.clonar();
        hijo->aplicarMovimiento(m);

        if (hijo->esTerminal() && hijo->ganador() == yo) {
            return m;
        }

        puntuados.push_back({ m, hijo->evaluar(yo) });
    }

    std::sort(puntuados.begin(), puntuados.end(), [](const MovimientoPuntuado& a, const MovimientoPuntuado& b) {
        return a.puntuacion > b.puntuacion;
    });

    
    // Escoge entre los mejores candidatos para que use mas variedad de piezas
    static std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    const size_t limite = std::min<size_t>(puntuados.size(), 4);
    std::uniform_int_distribution<size_t> dist(0, limite - 1);
    // Devuelve uno de los mejores movimientos de forma aleatoria
    return puntuados[dist(rng)].movimiento;
}
