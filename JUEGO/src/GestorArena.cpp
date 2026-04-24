// GestorArena.cpp
#include "GestorArena.h"
#include <stdlib.h> 
#include <math.h>  

GestorArena::GestorArena(PersonajeBaseArena* pPlanta, PersonajeBaseArena* pZombie, float ancho, float alto) {
    planta = pPlanta;
    zombie = pZombie;
    limiteAncho = ancho;
    limiteAlto = alto;
    combateTerminado = false;
    ganador = 0;
    cooldownPlanta = 0;
    cooldownZombie = 0;
}

GestorArena::~GestorArena() {
    obstaculos.clear();
    proyectiles.clear();
}

void GestorArena::inicializarArena(int numObstaculos) {
    planta->setPosicion(50.0f, limiteAlto / 2.0f);
    zombie->setPosicion(limiteAncho - 90.0f, limiteAlto / 2.0f);

    for (int i = 0; i < numObstaculos; i++) {
        float ox = 150.0f + (float)rand() / ((float)RAND_MAX / (limiteAncho - 300.0f));
        float oy = 50.0f + (float)rand() / ((float)RAND_MAX / (limiteAlto - 100.0f));
        obstaculos.push_back(ObstaculoArena(ox, oy, 60.0f, 60.0f));
    }
}

void GestorArena::procesarEntrada() {
    // Guardamos las posiciones previas por si chocan con algo
    float oldX_P = planta->getPosicion().x;
    float oldY_P = planta->getPosicion().y;
    float oldX_Z = zombie->getPosicion().x;
    float oldY_Z = zombie->getPosicion().y;

    // --- CONTROLES JUGADOR 1 (PLANTA) ---
    float dxP = 0.0f, dyP = 0.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dyP -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dyP += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dxP -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dxP += 1.0f;

    // Normalizar diagonal
    if (dxP != 0.0f && dyP != 0.0f) { dxP *= 0.7071f; dyP *= 0.7071f; }
    if (dxP != 0.0f || dyP != 0.0f) planta->mover(dxP, dyP);

    if (cooldownPlanta > 0) cooldownPlanta--;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && cooldownPlanta == 0) {
        if (planta->getEsCC()) {
            float distX = zombie->getPosicion().x - planta->getPosicion().x;
            float distY = zombie->getPosicion().y - planta->getPosicion().y;
            float dist = (float)sqrt(distX * distX + distY * distY);
            if (dist < 70.0f) {
                zombie->recibirDanio(planta->getDanio());
            }
            cooldownPlanta = 40;
        }
        else {
            float rad = planta->getAngulo() * 3.14159f / 180.0f;
            float projVX = (float)cos(rad) * 10.0f;
            float projVY = (float)sin(rad) * 10.0f;
            proyectiles.push_back(ProyectilArena(planta->getPosicion().x, planta->getPosicion().y, projVX, projVY, planta->getDanio(), true));
            cooldownPlanta = 20;
        }
    }

    // --- CONTROLES JUGADOR 2 (ZOMBI) ---
    float dxZ = 0.0f, dyZ = 0.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dyZ -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dyZ += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dxZ -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dxZ += 1.0f;

    if (dxZ != 0.0f && dyZ != 0.0f) { dxZ *= 0.7071f; dyZ *= 0.7071f; }
    if (dxZ != 0.0f || dyZ != 0.0f) zombie->mover(dxZ, dyZ);

    if (cooldownZombie > 0) cooldownZombie--;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && cooldownZombie == 0) {
        if (zombie->getEsCC()) {
            float distX = planta->getPosicion().x - zombie->getPosicion().x;
            float distY = planta->getPosicion().y - zombie->getPosicion().y;
            float dist = (float)sqrt(distX * distX + distY * distY);
            if (dist < 70.0f) {
                planta->recibirDanio(zombie->getDanio());
            }
            cooldownZombie = 40;
        }
        else {
            float rad = zombie->getAngulo() * 3.14159f / 180.0f;
            float projVX = (float)cos(rad) * 10.0f;
            float projVY = (float)sin(rad) * 10.0f;
            proyectiles.push_back(ProyectilArena(zombie->getPosicion().x, zombie->getPosicion().y, projVX, projVY, zombie->getDanio(), false));
            cooldownZombie = 20;
        }
    }

    resolverColisionesMovimiento(planta, oldX_P, oldY_P);
    resolverColisionesMovimiento(zombie, oldX_Z, oldY_Z);
}

void GestorArena::resolverColisionesMovimiento(PersonajeBaseArena* p, float oldX, float oldY) {
    if (p->getPosicion().x < 0 || p->getPosicion().x > limiteAncho ||
        p->getPosicion().y < 0 || p->getPosicion().y > limiteAlto) {
        p->setPosicion(oldX, oldY);
        return;
    }

    if (p->getEsVolador() == false) {
       
        for (unsigned int i = 0; i < obstaculos.size(); i++) {
            if (Interaccion::colision(p, &obstaculos[i])) {
                p->setPosicion(oldX, oldY);
                return;
            }
        }
    }
}

void GestorArena::actualizarLider() {
    if (combateTerminado == true) return;

    
    for (unsigned int i = 0; i < proyectiles.size(); i++) {
        proyectiles[i].actualizar();
        Vector2D posProj = proyectiles[i].getPosicion();

        if (posProj.x < 0 || posProj.x > limiteAncho || posProj.y < 0 || posProj.y > limiteAlto) {
            proyectiles[i].destruir();
            continue;
        }

        bool chocoObstaculo = false;
        for (unsigned int j = 0; j < obstaculos.size(); j++) {
            if (Interaccion::colision(&proyectiles[i], &obstaculos[j])) {
                proyectiles[i].destruir();
                chocoObstaculo = true;
                break;
            }
        }
        if (chocoObstaculo) continue;

        // Colisión Proyectil vs Personaje
        if (proyectiles[i].esAliadoDePlanta()) {
            Vector2D posZ(zombie->getPosicion().x - 20.0f, zombie->getPosicion().y - 20.0f);
            if (Interaccion::colisionCajas(posProj, 10.0f, 10.0f, posZ, 40.0f, 40.0f)) {
                zombie->recibirDanio(proyectiles[i].getDanio());
                proyectiles[i].destruir();
            }
        }
        else {
            Vector2D posP(planta->getPosicion().x - 20.0f, planta->getPosicion().y - 20.0f);
            if (Interaccion::colisionCajas(posProj, 10.0f, 10.0f, posP, 40.0f, 40.0f)) {
                planta->recibirDanio(proyectiles[i].getDanio());
                proyectiles[i].destruir();
            }
        }
    }

    // Eliminación de proyectiles 
    std::vector<ProyectilArena>::iterator it = proyectiles.begin();
    while (it != proyectiles.end()) {
        if (it->estaActivo() == false) {
            it = proyectiles.erase(it);
        }
        else {
            it++;
        }
    }

    // Comprobar si alguien ha muerto
    if (planta->getVida() <= 0 && zombie->getVida() <= 0) {
        combateTerminado = true;
        ganador = 0;
    }
    else if (planta->getVida() <= 0) {
        combateTerminado = true;
        ganador = 2;
    }
    else if (zombie->getVida() <= 0) {
        combateTerminado = true;
        ganador = 1;
    }
}

void GestorArena::dibujarBarrasDeVida(sf::RenderWindow& window) {
    sf::RectangleShape fondoP(sf::Vector2f(200, 20));
    fondoP.setPosition(10, 10);
    fondoP.setFillColor(sf::Color::Red);
    window.draw(fondoP);

    float porcP = planta->getVida() / planta->getVidaMaxima();
    sf::RectangleShape vidaP(sf::Vector2f(200 * porcP, 20));
    vidaP.setPosition(10, 10);
    vidaP.setFillColor(sf::Color::Green);
    window.draw(vidaP);

    sf::RectangleShape fondoZ(sf::Vector2f(200, 20));
    fondoZ.setPosition(limiteAncho - 210, 10);
    fondoZ.setFillColor(sf::Color::Red);
    window.draw(fondoZ);

    float porcZ = zombie->getVida() / zombie->getVidaMaxima();
    sf::RectangleShape vidaZ(sf::Vector2f(200 * porcZ, 20));
    vidaZ.setPosition(limiteAncho - 210, 10);
    vidaZ.setFillColor(sf::Color(128, 0, 128));
    window.draw(vidaZ);
}

void GestorArena::dibujarEscena(sf::RenderWindow& window) {
    
    for (unsigned int i = 0; i < obstaculos.size(); i++) {
        obstaculos[i].dibujar(window);
    }

    for (unsigned int i = 0; i < proyectiles.size(); i++) {
        proyectiles[i].dibujar(window);
    }

    planta->dibujar(window);
    zombie->dibujar(window);

    dibujarBarrasDeVida(window);
}