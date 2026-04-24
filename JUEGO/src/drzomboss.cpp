#include "drzomboss.h"
#include <iostream>


drzomboss::drzomboss(int bando)
    : terrestre(200, 40, 4, 4, 3, bando) {

    hechizocurarusado = false;
    hechizoresucitarusado = false;
    hechizoteletransportarusado = false;
}

void drzomboss::atacarEnArena() {
    std::cout << "¡dave ataca golpeando fuertemente con su sartén!" << std::endl;
}

// --- LÓGICA DE CURAR ---
// 1. El main le pregunta a Dave si puede curar (el "OK")
bool drzomboss::puedeCurar() {
    return !hechizocurarusado;
}
// 2. El main le avisa a Dave de que el hechizo ya se ejecutó en el tablero
void drzomboss::consumirCurar() {
    hechizocurarusado = true;
}


// --- LÓGICA DE RESUCITAR ---
bool drzomboss::puedeResucitar() {
    return !hechizoresucitarusado;
}
void drzomboss::consumirResucitar() {
    hechizoresucitarusado = true;
}


// --- LÓGICA DE TELETRANSPORTAR ---
bool drzomboss::puedeTeletransportar() {
    return !hechizoteletransportarusado;
}
void drzomboss::consumirTeletransportar() {
    hechizoteletransportarusado = true;
}
