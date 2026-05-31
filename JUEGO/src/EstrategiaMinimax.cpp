#include "EstrategiaMinimax.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <vector>
#include <limits>
#include <stdexcept>

namespace {
    constexpr double POS_INF =  std::numeric_limits<double>::infinity();
    constexpr double NEG_INF = -std::numeric_limits<double>::infinity();

    // Valor usado para victorias y derrotas terminales
    // la IA prefiere ganar pronto y perder tarde
    constexpr double PUNT_VICTORIA =  1.0e9;
    constexpr double PUNT_DERROTA  = -1.0e9;
}

EstrategiaMinimax::EstrategiaMinimax(int profundidad) : profundidadMaxima(profundidad) {
    if (profundidadMaxima < 1) profundidadMaxima = 1;
}

Movimiento EstrategiaMinimax::elegirMovimiento(const IEstadoJuego& estado) {
    auto movimientos = estado.getMovimientosLegales();
    if (movimientos.empty()) {
        throw std::runtime_error("EstrategiaMinimax: no hay movimientos legales");
    }

    const int yo = estado.jugadorActual();
    double alfa = NEG_INF;
    const double beta = POS_INF;
    double mejorValor = NEG_INF;

    struct Candidato {
        Movimiento movimiento;
        double valor;
    };
    std::vector<Candidato> candidatos;
    candidatos.reserve(movimientos.size());

    for (const auto& m : movimientos) {
        auto hijo = estado.clonar();
        hijo->aplicarMovimiento(m);
        const double valor = minimax(*hijo, profundidadMaxima - 1, alfa, beta, yo);
        candidatos.push_back({ m, valor });
        if (valor > mejorValor) {
            mejorValor = valor;
        }
        alfa = std::max(alfa, mejorValor);
    }

    std::sort(candidatos.begin(), candidatos.end(), [](const Candidato& a, const Candidato& b) {
        return a.valor > b.valor;
    });

    //  hay varias jugadas casi igual de buenas,
    // alterna entre ellas para usar mas variedad de piezas.
    std::vector<Movimiento> mejoresMovimientos;
    const double margen = 12.0;
    for (const Candidato& c : candidatos) {
        if (c.valor >= mejorValor - margen) mejoresMovimientos.push_back(c.movimiento);
        if (mejoresMovimientos.size() >= 3) break;
    }

    if (mejoresMovimientos.empty()) return candidatos.front().movimiento;

    static std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<size_t> dist(0, mejoresMovimientos.size() - 1);
    return mejoresMovimientos[dist(rng)];
}

double EstrategiaMinimax::minimax(const IEstadoJuego& estado,
                                  int profundidad,
                                  double alfa,
                                  double beta,
                                  int yo) {
    // Casos base
    if (estado.esTerminal()) {
        const int g = estado.ganador();
        if (g == yo) return PUNT_VICTORIA - (profundidadMaxima - profundidad);
        if (g == -1) return 0.0; //cuando es empate
        return PUNT_DERROTA + (profundidadMaxima - profundidad);
    }
    if (profundidad == 0) {
        return estado.evaluar(yo);
    }

    auto movimientos = estado.getMovimientosLegales();
    if (movimientos.empty()) {
        return estado.evaluar(yo);
    }

    const bool maximizando = (estado.jugadorActual() == yo);

    if (maximizando) {
        double valor = NEG_INF;
        for (const auto& m : movimientos) {
            auto hijo = estado.clonar();
            hijo->aplicarMovimiento(m);
            valor = std::max(valor, minimax(*hijo, profundidad - 1, alfa, beta, yo));
            alfa = std::max(alfa, valor);
            if (alfa >= beta) break; // poda beta
        }
        return valor;
    } else {
        double valor = POS_INF;
        for (const auto& m : movimientos) {
            auto hijo = estado.clonar();
            hijo->aplicarMovimiento(m);
            valor = std::min(valor, minimax(*hijo, profundidad - 1, alfa, beta, yo));
            beta = std::min(beta, valor);
            if (alfa >= beta) break; // poda alfa
        }
        return valor;
    }
}
