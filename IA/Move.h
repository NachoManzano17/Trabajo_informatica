#pragma once

// Representación genérica de un movimiento del tablero.
//
// Esta estructura está pensada para ser lo bastante expresiva como
// para cubrir un juego tipo ARCHON sin acoplar la IA a sus reglas
// concretas. La IA no interpreta el contenido: solo pide movimientos
// al estado, los aplica y pregunta por el resultado.
//
// Kind::Move  : desplazamiento de una pieza desde (fromRow,fromCol)
//               hasta (toRow,toCol).
// Kind::Spell : conjuro del hechicero. spellId identifica el tipo.
//               (targetRow,targetCol) indica la casilla objetivo.
// Kind::Pass  : turno vacío (útil si el juego lo permite o para
//               hacer pruebas).
struct Move {
    enum class Kind { Move, Spell, Pass };

    Kind kind      = Kind::Move;
    int  fromRow   = -1;
    int  fromCol   = -1;
    int  toRow     = -1;
    int  toCol     = -1;
    int  spellId   = -1;
    int  targetRow = -1;
    int  targetCol = -1;

    bool operator==(const Move& other) const {
        return kind == other.kind
            && fromRow == other.fromRow && fromCol == other.fromCol
            && toRow == other.toRow     && toCol == other.toCol
            && spellId == other.spellId
            && targetRow == other.targetRow && targetCol == other.targetCol;
    }
};
