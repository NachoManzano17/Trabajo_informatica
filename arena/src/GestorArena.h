// GestorArena.h
#pragma once
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "PersonajeBaseArena.h"
#include "ObstaculoArena.h"
#include "ProyectilArena.h"
#include "Interaccion.h"

class GestorArena {
private:
    PersonajeBaseArena* planta;
    PersonajeBaseArena* zombie;

    std::vector<ObstaculoArena> obstaculos;
    std::vector<ProyectilArena> proyectiles;

    float limiteAncho, limiteAlto;
    bool combateTerminado;
    int ganador;

    int cooldownPlanta;
    int cooldownZombie;

    void resolverColisionesMovimiento(PersonajeBaseArena* p, float oldX, float oldY);
    void dibujarBarrasDeVida(sf::RenderWindow& window);

public:
    GestorArena(PersonajeBaseArena* pPlanta, PersonajeBaseArena* pZombie, float ancho, float alto);
    ~GestorArena();

    void inicializarArena(int numObstaculos);
    void procesarEntrada();
    void actualizarLider();
    void dibujarEscena(sf::RenderWindow& window);

    bool isTerminado() { return combateTerminado; }
    int getGanador() { return ganador; }
};