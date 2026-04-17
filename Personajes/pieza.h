#pragma once // Esto evita que el archivo se incluya varias veces por error

class pieza {
protected:
    // Ponemos los atributos como 'protected' para que las clases hijas (plantas/zombis) puedan usarlos
    int vida;
    int fuerza;
    int velocidadAtaque;
    int tiempoRecarga;
    int radioMovimiento;
    int bando; // Por ejemplo: 0 para Plantas, 1 para Zombis

public:
    // Constructor (para crear la pieza) y Destructor
    pieza(int v, int f, int va, int tr, int rm, int b);
    virtual ~pieza(); // El destructor siempre debe ser virtual en clases base

    // Métodos polimórficos puros (= 0). Obligan a las clases hijas a programar cómo hacen esto.
    virtual void moverEnTablero() = 0;
    virtual void atacarEnArena() = 0;

    // Métodos comunes que funcionan igual para todos
    void recibirDano(int cantidad);
    bool estaVivo();
};