#include "EstrategiaAleatoria.h"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

EstrategiaAleatoria::EstrategiaAleatoria(unsigned semilla) : rng(semilla) {}

Movimiento EstrategiaAleatoria::elegirMovimiento(const IEstadoJuego& estado) {
    auto movimientos = estado.getMovimientosLegales();
    if (movimientos.empty()) {
        throw std::runtime_error("EstrategiaAleatoria: no hay movimientos legales");
    }

    const int yo = estado.jugadorActual();

    //Evalua el estado resultante de cada movimiento.
    std::vector<std::pair<Movimiento, double>> puntuados;
    puntuados.reserve(movimientos.size());

    double mejorPunt = -std::numeric_limits<double>::infinity();

    for (const auto& m : movimientos) {
        auto hijo = estado.clonar();
        hijo->aplicarMovimiento(m);

        // Victoria inmediata: atajo.
        if (hijo->esTerminal() && hijo->ganador() == yo) {
            return m;
        }

        const double s = hijo->evaluar(yo);
        puntuados.emplace_back(m, s);
        if (s > mejorPunt) mejorPunt = s;
    }

    // Descartamos los movimientos que empeoran mucho respecto al mejor.

    const double margen = 1.0 + 0.25 * std::abs(mejorPunt);

    std::vector<Movimiento> aceptables;
    aceptables.reserve(puntuados.size());
    for (const auto& p : puntuados) {
        if (p.second >= mejorPunt - margen) {
            aceptables.push_back(p.first);
        }
    }
    if (aceptables.empty()) aceptables.push_back(puntuados.front().first);

    std::uniform_int_distribution<size_t> dist(0, aceptables.size() - 1);
    return aceptables[dist(rng)];
}
