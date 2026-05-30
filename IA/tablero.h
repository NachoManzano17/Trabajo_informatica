#pragma once
#include "personaje.h"

// Estados del ciclo de Archon
enum class estadociclo { luz_maxima, luz_media, neutral, oscuridad_media, oscuridad_maxima };

class tablero {
private:
    personaje* casillas[9][9];
    int turnoglobal; 
    // 0 = neutral, 1 = ventaja plantas, 2 = ventaja zombis
    int estadocasillasoscilantes;

public:
    tablero();
    tablero(const tablero& otro);
    tablero& operator=(const tablero& otro);
    ~tablero();

    void inicializarpartida();

    // tus herramientas de arbitro
    personaje* getpersonajeen(int f, int c) const;
    void colocarpersonaje(int f, int c, personaje* p); // util para hacer pruebas

   
    bool procesarmovimiento(int fori, int cori, int fdest, int cdest);

    void eliminarFicha(int f, int c);

    void forzarMovimiento(int fOrig, int cOrig, int fDest, int cDest);
    
    void dibujarconsola() const;

    bool escasilladepoder(int f, int c) const;

    bando comprobarvictoria() const;

    // Gestión del tiempo
    void avanzarturno();
    estadociclo getestadociclo() const;

    int calcularbonusvida(bando equipo) const;
    int calcularbonusataque(bando equipo) const;

    bool procesarhechizo(int fori, int cori, int hechizo_id, int fdest, int cdest);

    
    void dibujarciclo() const;

    bool hayObstaculoEnCamino(int fOri, int cOri, int fDest, int cDest) const;
    // Función para que SFML sepa qué hay en cada casilla
    personaje* getFicha(int f, int c) const {
        return casillas[f][c];
    }
};
