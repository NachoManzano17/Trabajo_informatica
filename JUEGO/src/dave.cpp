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

// --- LÓGICA DE CURAR ---
// 1. El main le pregunta a Dave si puede curar (el "OK")
bool dave::puedeCurar() {
    return !hechizocurarusado;
}
// 2. El main le avisa a Dave de que el hechizo ya se ejecutó en el tablero
void dave::consumirCurar() {
    hechizocurarusado = true;
}


// --- LÓGICA DE RESUCITAR ---
bool dave::puedeResucitar() {
    return !hechizoresucitarusado;
}
void dave::consumirResucitar() {
    hechizoresucitarusado = true;
}


// --- LÓGICA DE TELETRANSPORTAR ---
bool dave::puedeTeletransportar() {
    return !hechizoteletransportarusado;
}
void dave::consumirTeletransportar() {
    hechizoteletransportarusado = true;
}
