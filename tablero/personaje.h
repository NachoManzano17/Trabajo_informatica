#pragma once

class tablero;

// 1. ¡RECUPERAMOS TU DICCIONARIO! (Le damos valor 0 y 1 para que encaje con tu colega)
enum class bando { planta = 0, zombi = 1, ninguno = 2 };

class personaje {
protected:
    int vida;
    int fuerza;
    int velocidadAtaque;
    int tiempoRecarga;
    int radioMovimiento;

    // 2. CAMBIAMOS EL 'int bando' DE TU COLEGA POR TU TIPO DE BANDO
    bando mi_bando;

public:
    personaje(int v, int f, int va, int tr, int rm, int b);
    virtual ~personaje() {}

    virtual void moverEnTablero() = 0;
    virtual void atacarEnArena() = 0;
    void recibirDano(int cantidad);
    bool estaVivo();

    // 3. AHORA EL ÁRBITRO RECIBE LA PALABRA, NO EL NÚMERO
    virtual bando getequipo() const { return mi_bando; }
    virtual bool estamuerto() const { return vida <= 0; }
    virtual bool lanzarhechizo(int hechizo_id, int fdest, int cdest, tablero& mitablero) { return false; }

    virtual bool esmovimientovalido(int fori, int cori, int fdest, int cdest) = 0;
    virtual char getsimbolo() const = 0;
};