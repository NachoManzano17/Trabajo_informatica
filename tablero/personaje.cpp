#include "personaje.h"


// Implementación del constructor
personaje::personaje(int v, int f, int va, int tr, int rm, int b) {
    vida = v;
    fuerza = f;
    velocidadAtaque = va;
    tiempoRecarga = tr;
    radioMovimiento = rm;
    mi_bando = (bando)b;
}

// Implementación de los métodos comunes
void personaje::recibirDano(int cantidad) {
    vida -= cantidad;
    if (vida < 0) {
        vida = 0; // Para que la vida no sea negativa
    }
}

bool personaje::estaVivo() {
    return vida > 0;
}

// NOTA: moverEnTablero() y atacarEnArena() NO se programan aquí porque son "= 0" en el .h