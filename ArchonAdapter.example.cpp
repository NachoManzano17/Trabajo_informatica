// Esqueleto de adaptador sobre el juego real de tus compañeros.
//
// Este archivo NO se compila tal cual: es una plantilla con huecos.
// La idea es que tus compañeros no modifiquen su clase de tablero.
// Tú escribes una clase adaptadora que derive de IGameState y
// reenvía las llamadas a la clase real del equipo.
//
// Cuando tengas la clase real delante, te llevará entre 30 y 60
// líneas escribir este adaptador de verdad.

#if 0  // bloqueado a propósito para que no compile hasta que lo adaptes

#include <memory>
#include "IGameState.h"
#include "Tablero.h"   // la clase de tus compañeros
#include "Pieza.h"     // su jerarquía de piezas

class ArchonAdapter : public IGameState {
public:
    explicit ArchonAdapter(const Tablero& tab) : tablero(tab) {}

    std::vector<Move> getLegalMoves() const override {
        std::vector<Move> out;

        // 1) Movimientos normales. Recorre las piezas del jugador
        //    que tiene el turno y pide sus casillas alcanzables.
        for (const auto& p : tablero.piezasDe(tablero.turno())) {
            for (const auto& destino : p.movimientosLegales(tablero)) {
                Move m;
                m.kind    = Move::Kind::Move;
                m.fromRow = p.fila();
                m.fromCol = p.columna();
                m.toRow   = destino.fila;
                m.toCol   = destino.columna;
                out.push_back(m);
            }
        }

        // 2) Conjuros del hechicero (si aún tiene disponibles).
        if (auto* h = tablero.hechiceroDe(tablero.turno())) {
            for (int spellId : h->conjurosDisponibles()) {
                for (const auto& obj : h->objetivosValidos(spellId, tablero)) {
                    Move m;
                    m.kind      = Move::Kind::Spell;
                    m.spellId   = spellId;
                    m.targetRow = obj.fila;
                    m.targetCol = obj.columna;
                    out.push_back(m);
                }
            }
        }
        return out;
    }

    std::unique_ptr<IGameState> clone() const override {
        // Importante: clone profunda. Si Tablero ya tiene copy
        // constructor profundo, basta con new ArchonAdapter(*this).
        return std::make_unique<ArchonAdapter>(*this);
    }

    void applyMove(const Move& m) override {
        if (m.kind == Move::Kind::Move) {
            tablero.mover(m.fromRow, m.fromCol, m.toRow, m.toCol);
            // El combate en la arena se resuelve aquí mismo con un
            // cálculo probabilístico. Ver recomendaciones sobre cómo
            // modelarlo para que la IA no se tome en serio su
            // determinismo.
        } else if (m.kind == Move::Kind::Spell) {
            tablero.lanzarConjuro(tablero.turno(), m.spellId,
                                  m.targetRow, m.targetCol);
        }
        tablero.pasarTurno();
    }

    bool isTerminal() const override { return tablero.haFinalizado(); }

    int currentPlayer() const override {
        // 0 = Luz, 1 = Oscuridad.
        return tablero.turno() == Bando::Luz ? 0 : 1;
    }

    int winner() const override {
        if (!tablero.haFinalizado()) return -1;
        auto g = tablero.ganador();
        if (g == Bando::Luz)       return 0;
        if (g == Bando::Oscuridad) return 1;
        return -1;
    }

    double evaluate(int me) const override {
        // Una heurística razonable para ARCHON combina:
        //   - material (suma de valores de piezas vivas)
        //   - control de los 5 puntos de poder
        //   - proximidad de piezas al hechicero rival
        //   - hechizos aún disponibles
        //   - bono por casillas del propio color bajo tus piezas
        //
        // Devolver el valor DESDE EL PUNTO DE VISTA DE 'me'.
        double mio = puntuacionBando(me);
        double suyo = puntuacionBando(1 - me);
        return mio - suyo;
    }

private:
    Tablero tablero;  // copia por valor: permite clonar barato

    double puntuacionBando(int jugador) const {
        // Implementar según las métricas que os parezcan relevantes.
        return 0.0;
    }
};

#endif
