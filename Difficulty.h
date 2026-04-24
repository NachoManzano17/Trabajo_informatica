#pragma once

// Niveles de dificultad de la IA.
// Easy   : comportamiento cercano al azar, con un filtro mínimo
//          para evitar movimientos manifiestamente catastróficos.
// Medium : heurística codiciosa a 1 ply (mira un movimiento por delante).
// Hard   : minimax con poda alfa-beta a profundidad configurable.
enum class Difficulty {
    Easy,
    Medium,
    Hard
};
