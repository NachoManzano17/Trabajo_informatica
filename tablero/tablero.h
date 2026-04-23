#pragma once
#include "personaje.h"

// Estados del ciclo de Archon
enum class estadociclo { luz_maxima, luz_media, neutral, oscuridad_media, oscuridad_maxima };

class tablero {
private:
    personaje* casillas[9][9];
    int turnoglobal; // El reloj del juego
    // preparacion para la regla de archon: el ciclo de dia/noche
    // 0 = neutral, 1 = ventaja plantas, 2 = ventaja zombis
    int estadocasillasoscilantes;

public:
    tablero();
    ~tablero();

    void inicializarpartida();

    // tus herramientas de arbitro
    personaje* getpersonajeen(int f, int c) const;
    void colocarpersonaje(int f, int c, personaje* p); // util para hacer pruebas

    // la funcion que detonara los combates
    bool procesarmovimiento(int fori, int cori, int fdest, int cdest);

    // para ver que estas haciendo
    void dibujarconsola() const;

    bool escasilladepoder(int f, int c) const;

    bando comprobarvictoria() const;

    // Gestión del tiempo
    void avanzarturno();
    estadociclo getestadociclo() const;

    int calcularbonusvida(bando equipo) const;
    int calcularbonusataque(bando equipo) const;

    bool procesarhechizo(int fori, int cori, int hechizo_id, int fdest, int cdest);

    // Para que la consola nos diga qué hora es
    void dibujarciclo() const;
};
