#pragma once

// Representacion generica de un movimiento del tablero.
//
// Esta estructura esta pensada para ser lo bastante expresiva como
// para cubrir un juego tipo ARCHON sin acoplar la IA a sus reglas
// concretas. La IA no interpreta el contenido: solo pide movimientos
// al estado, los aplica y pregunta por el resultado.
//
// Tipo::Mover   : desplazamiento de una pieza desde (filaOrigen,colOrigen)
//                 hasta (filaDestino,colDestino).
// Tipo::Hechizo : conjuro del hechicero. idHechizo identifica el tipo.
//                 (filaObjetivo,colObjetivo) indica la casilla objetivo.
// Tipo::Pasar   : turno vacio (util si el juego lo permite o para
//                 hacer pruebas).
struct Movimiento {
    enum class Tipo { Mover, Hechizo, Pasar };

    Tipo tipo         = Tipo::Mover;
    int  filaOrigen   = -1;
    int  colOrigen    = -1;
    int  filaDestino  = -1;
    int  colDestino   = -1;
    int  idHechizo    = -1;
    int  filaObjetivo = -1;
    int  colObjetivo  = -1;

    bool operator==(const Movimiento& otro) const {
        return tipo == otro.tipo
            && filaOrigen == otro.filaOrigen && colOrigen == otro.colOrigen
            && filaDestino == otro.filaDestino && colDestino == otro.colDestino
            && idHechizo == otro.idHechizo
            && filaObjetivo == otro.filaObjetivo && colObjetivo == otro.colObjetivo;
    }
};
