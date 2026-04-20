// GestorArena.cpp
#include "GestorArena.h"
#include <cstdlib> 

GestorArena::GestorArena(PersonajeBaseArena* pPlanta, PersonajeBaseArena* pZombie, float ancho, float alto)
    : planta(pPlanta), zombie(pZombie), limiteAncho(ancho), limiteAlto(alto),
    combateTerminado(false), ganador(0), cooldownPlanta(0), cooldownZombie(0) {
}

GestorArena::~GestorArena() {
    obstaculos.clear();
    proyectiles.clear();
}

void GestorArena::inicializarArena(int numObstaculos) {
    planta->setPosicion(50.0f, limiteAlto / 2.0f);
    zombie->setPosicion(limiteAncho - 90.0f, limiteAlto / 2.0f);

    for (int i = 0; i < numObstaculos; ++i) {
        float ox = 150.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (limiteAncho - 300.0f)));
        float oy = 50.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (limiteAlto - 100.0f)));
        obstaculos.push_back(ObstaculoArena(ox, oy, 60.0f, 60.0f));
    }
}

bool GestorArena::checkColision(CajaColisionArena a, CajaColisionArena b) {
    return (a.x < b.x + b.ancho && a.x + a.ancho > b.x &&
        a.y < b.y + b.alto && a.y + a.alto > b.y);
}

void GestorArena::procesarEntrada() {
    float oldX_planta = planta->getX();
    float oldY_planta = planta->getY();
    float oldX_zombie = zombie->getX();
    float oldY_zombie = zombie->getY();

    // --- CONTROLES JUGADOR 1 (PLANTA) - MULTIJUGADOR LOCAL ---
    float dxP = 0, dyP = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dyP -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dyP += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dxP -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dxP += 1;

    // Normalizar diagonal para no ir más rápido
    if (dxP != 0 && dyP != 0) { dxP *= 0.7071f; dyP *= 0.7071f; }
    if (dxP != 0 || dyP != 0) planta->mover(dxP, dyP);

    if (cooldownPlanta > 0) cooldownPlanta--;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && cooldownPlanta == 0) {
        if (planta->getEsCC()) {
            // ATAQUE CUERPO A CUERPO: Comprobamos si el enemigo está muy cerca
            float distX = zombie->getX() - planta->getX();
            float distY = zombie->getY() - planta->getY();
            float distancia = std::sqrt(distX * distX + distY * distY);
            if (distancia < 70.0f) { // Rango del CC
                zombie->recibirDanio(planta->getDanio());
            }
            cooldownPlanta = 40; // El CC suele ser más lento
        }
        else {
            // ATAQUE A DISTANCIA: Crear proyectil en la dirección a la que mira
            float anguloRad = planta->getAngulo() * 3.14159265f / 180.0f;
            float velProjX = std::cos(anguloRad) * 10.0f;
            float velProjY = std::sin(anguloRad) * 10.0f;
            proyectiles.push_back(ProyectilArena(planta->getX(), planta->getY(), velProjX, velProjY, planta->getDanio(), true));
            cooldownPlanta = 20;
        }
    }

    // --- CONTROLES JUGADOR 2 (ZOMBI) - MULTIJUGADOR LOCAL ---
    float dxZ = 0, dyZ = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) dyZ -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) dyZ += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) dxZ -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dxZ += 1;

    if (dxZ != 0 && dyZ != 0) { dxZ *= 0.7071f; dyZ *= 0.7071f; }
    if (dxZ != 0 || dyZ != 0) zombie->mover(dxZ, dyZ);

    if (cooldownZombie > 0) cooldownZombie--;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && cooldownZombie == 0) {
        if (zombie->getEsCC()) {
            // ATAQUE CUERPO A CUERPO ZOMBI (Ej: SuperCerebroz)
            float distX = planta->getX() - zombie->getX();
            float distY = planta->getY() - zombie->getY();
            float distancia = std::sqrt(distX * distX + distY * distY);
            if (distancia < 70.0f) {
                planta->recibirDanio(zombie->getDanio());
            }
            cooldownZombie = 40;
        }
        else {
            // ATAQUE A DISTANCIA
            float anguloRad = zombie->getAngulo() * 3.14159265f / 180.0f;
            float velProjX = std::cos(anguloRad) * 10.0f;
            float velProjY = std::sin(anguloRad) * 10.0f;
            proyectiles.push_back(ProyectilArena(zombie->getX(), zombie->getY(), velProjX, velProjY, zombie->getDanio(), false));
            cooldownZombie = 20;
        }
    }

    resolverColisionesMovimiento(planta, oldX_planta, oldY_planta);
    resolverColisionesMovimiento(zombie, oldX_zombie, oldY_zombie);
}

void GestorArena::resolverColisionesMovimiento(PersonajeBaseArena* p, float oldX, float oldY) {
    CajaColisionArena hitbox = p->getHitbox();

    if (hitbox.x < 0 || hitbox.x + hitbox.ancho > limiteAncho ||
        hitbox.y < 0 || hitbox.y + hitbox.alto > limiteAlto) {
        p->setPosicion(oldX, oldY);
        return;
    }

    if (!p->getEsVolador()) {
        for (const auto& obs : obstaculos) {
            if (checkColision(hitbox, obs.getHitbox())) {
                p->setPosicion(oldX, oldY);
                return;
            }
        }
    }
}

void GestorArena::actualizarLider() {
    if (combateTerminado) return;

    for (auto& proj : proyectiles) {
        proj.actualizar();
        CajaColisionArena hitProj = proj.getHitbox();

        if (hitProj.x < 0 || hitProj.x > limiteAncho || hitProj.y < 0 || hitProj.y > limiteAlto) {
            proj.destruir();
            continue;
        }

        bool chocoObstaculo = false;
        for (const auto& obs : obstaculos) {
            if (checkColision(hitProj, obs.getHitbox())) {
                proj.destruir();
                chocoObstaculo = true;
                break;
            }
        }
        if (chocoObstaculo) continue;

        if (proj.esAliadoDePlanta()) {
            if (checkColision(hitProj, zombie->getHitbox())) {
                zombie->recibirDanio(proj.getDanio());
                proj.destruir();
            }
        }
        else {
            if (checkColision(hitProj, planta->getHitbox())) {
                planta->recibirDanio(proj.getDanio());
                proj.destruir();
            }
        }
    }

    auto it = proyectiles.begin();
    while (it != proyectiles.end()) {
        if (!it->estaActivo()) {
            it = proyectiles.erase(it);
        }
        else {
            ++it;
        }
    }

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
    // Fondo barra planta (rojo)
    sf::RectangleShape fondoP(sf::Vector2f(200, 20));
    fondoP.setPosition(10, 10);
    fondoP.setFillColor(sf::Color::Red);
    window.draw(fondoP);

    // Vida actual planta (verde)
    float porcP = planta->getVida() / planta->getVidaMaxima();
    sf::RectangleShape vidaP(sf::Vector2f(200 * porcP, 20));
    vidaP.setPosition(10, 10);
    vidaP.setFillColor(sf::Color::Green);
    window.draw(vidaP);

    // Fondo barra zombie (rojo)
    sf::RectangleShape fondoZ(sf::Vector2f(200, 20));
    fondoZ.setPosition(limiteAncho - 210, 10);
    fondoZ.setFillColor(sf::Color::Red);
    window.draw(fondoZ);

    // Vida actual zombie (morado)
    float porcZ = zombie->getVida() / zombie->getVidaMaxima();
    sf::RectangleShape vidaZ(sf::Vector2f(200 * porcZ, 20));
    vidaZ.setPosition(limiteAncho - 210, 10);
    vidaZ.setFillColor(sf::Color(128, 0, 128));
    window.draw(vidaZ);
}

void GestorArena::dibujarEscena(sf::RenderWindow& window) {
    for (auto& obs : obstaculos) {
        obs.dibujar(window);
    }
    for (auto& proj : proyectiles) {
        proj.dibujar(window);
    }

    planta->dibujar(window);
    zombie->dibujar(window);

    dibujarBarrasDeVida(window);
}