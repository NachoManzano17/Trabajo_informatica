#include "dave.h"
#include <iostream>

dave::dave(int bando)
    : terrestre(200, 40, 4, 4, 3, bando) {

    hechizocurarusado = false;
    hechizoresucitarusado = false;
    hechizoteletransportarusado = false;
}

void dave::atacarEnArena() {
    std::cout << "¡dave ataca golpeando fuertemente con su sartén!" << std::endl;
}

// Lógica de Curar: Le suma 100 puntos de vida al aliado
bool dave::lanzarHechizoCurar(personaje* objetivoaliado) {
    if (hechizocurarusado) return false;

    // ¡LÓGICA REAL! Entramos en el objetivo y le sumamos vida
    objetivoaliado->curar(100);

    hechizocurarusado = true;
    return true;
}

// Lógica de Resucitar: Comprueba si está muerto y le devuelve la vida
bool dave::lanzarHechizoResucitar(personaje* objetivomuerto) {
    if (hechizoresucitarusado) return false;

    // Si la pieza ya estaba viva, el hechizo falla y no se gasta
    if (objetivomuerto->estaVivo()) {
        return false;
    }

    // ¡LÓGICA REAL! Lo revivimos dándole vida de nuevo
    objetivomuerto->curar(100);

    hechizoresucitarusado = true;
    return true;
}