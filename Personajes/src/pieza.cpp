#include "pieza.h"


// Implementación del constructor
pieza::pieza(int v, int f, int va, int tr, int rm, int b) {
    vida = v;
    fuerza = f;
    velocidadAtaque = va;
    tiempoRecarga = tr;
    radioMovimiento = rm;
    bando = b;
}

// Implementación del destructor (vacío de momento)
pieza::~pieza() {
}

// Implementación de los métodos comunes
void pieza::recibirDano(int cantidad) {
    vida -= cantidad;
    if (vida < 0) {
        vida = 0; // Para que la vida no sea negativa
    }
}

bool pieza::estaVivo() {
    return vida > 0;
}
void pieza::curar(int cantidad) {
    vida += cantidad;
    // (Opcional) Aquí podrías poner un límite, ej: if (vida > 200) vida = 200;
}

int pieza::obtenerVida() {
    return vida;
}
// NOTA: moverEnTablero() y atacarEnArena() NO se programan aquí porque son "= 0" en el .h