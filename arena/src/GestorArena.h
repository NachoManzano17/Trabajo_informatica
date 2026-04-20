// GestorArena.h
#pragma once
#include "PersonajeBaseArena.h"
#include "ObstaculoArena.h"
#include "ProyectilArena.h"
#include <vector>

class GestorArena {
private:
    PersonajeBaseArena* planta;
    PersonajeBaseArena* zombie;

    std::vector<ObstaculoArena> obstaculos;
    std::vector<ProyectilArena> proyectiles;

    float limiteAncho, limiteAlto;
    bool combateTerminado;
    int ganador;

    // Control de disparo para no saturar la pantalla
    int cooldownPlanta;
    int cooldownZombie;

    bool checkColision(CajaColisionArena a, CajaColisionArena b);
    void resolverColisionesMovimiento(PersonajeBaseArena* p, float oldX, float oldY);
    void dibujarBarrasDeVida(sf::RenderWindow& window);

public:
    GestorArena(PersonajeBaseArena* pPlanta, PersonajeBaseArena* pZombie, float ancho, float alto);
    ~GestorArena();

    void inicializarArena(int numObstaculos);
    void procesarEntrada();
    void actualizarLider();
    void dibujarEscena(sf::RenderWindow& window);

    bool isTerminado() const { return combateTerminado; }
    int getGanador() const { return ganador; }
};