
#include "drzomboss.h"
#include <iostream>

drzomboss::drzomboss(int bando)
    : terrestre(200, 40, 4, 4, 3, bando) {
    hechizocurarusado = false;
    hechizoresucitarusado = false;
    hechizoteletransportarusado = false;
}

void drzomboss::atacarEnArena() {
    std::cout << "¡el dr. zomboss da un puñetazo devastador a corta distancia!" << std::endl;
}

bool drzomboss::lanzarHechizoCurar(pieza* objetivoaliado) {
    if (hechizocurarusado) return false;

    // lógica real: curamos al zombi aliado
    objetivoaliado->curar(100);

    hechizocurarusado = true;
    return true;
}

bool drzomboss::lanzarHechizoResucitar(pieza* objetivomuerto) {
    if (hechizoresucitarusado) return false;

    // si el zombi ya estaba vivo, fallamos para no gastar el hechizo
    if (objetivomuerto->estaVivo()) {
        return false;
    }

    // lógica real: lo revivimos
    objetivomuerto->curar(100);

    hechizoresucitarusado = true;
    return true;
}

bool drzomboss::lanzarHechizoTeletransportar(pieza* objetivo, int nuevafila, int nuevacolumna) {
    if (hechizoteletransportarusado) return false;

    // al igual que dave, tu compañero moverá el puntero en su matriz
    // tú solo le confirmas que el hechizo se ha gastado con éxito
    hechizoteletransportarusado = true;
    return true;
}