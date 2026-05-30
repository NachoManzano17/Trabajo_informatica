#include "GestorArena.h"
#include "GestorAudio.h"
#include <stdlib.h> 
#include <math.h>  
#include <algorithm> 
#include <utility>   
#include <cmath>
#include "LuchadorAvatar.h"
#include"Dificultad.h"


bool g_VolverAlMenuPrincipal = false;



GestorArena::GestorArena(PersonajeBaseArena* pPlanta, PersonajeBaseArena* pZombie, float anchoP, float altoP) {

    planta = pPlanta;
    zombie = pZombie;
    pantallaAncho = anchoP;
    pantallaAlto = altoP;

    // Cargamos el fondo y lo escalamos a pantalla completa
    if (texturaFondo.loadFromFile("fondo_arena.jpg")) {
        spriteFondo.setTexture(texturaFondo);

      
        float anchoImagen = (float)texturaFondo.getSize().x;
        float altoImagen = (float)texturaFondo.getSize().y;
        spriteFondo.setScale(pantallaAncho / anchoImagen, pantallaAlto / altoImagen);
    }

    // CARGAR LOS NEUMÁTICOS
    if (texturaNeumatico.loadFromFile("neumaticos.png")) {
        spriteNeumatico.setTexture(texturaNeumatico);

        float anchoLogico = 140.0f;
        float altoLogico = 30.0f;
        float anchoVisual = 110.0f;

        float escala = anchoVisual / texturaNeumatico.getSize().x;
        spriteNeumatico.setScale(escala, escala);
        float altoVisualPixeles = texturaNeumatico.getSize().y * escala;

        float ajusteX = 20.0f;
        float ajusteY = 45.0f;
        float desfaseY = altoVisualPixeles - altoLogico - ajusteY;

        spriteNeumatico.setOrigin(ajusteX / escala, desfaseY / escala);
    }

    //  CARGAR IMÁGENES DE LAS BARRAS DE VIDA 
    texVidaPlanta.loadFromFile("verde_barra_vida.png");
    texVidaZombi.loadFromFile("morado_barra_vida.png");

    if (texRosa.loadFromFile("rosa_barra_vida.png")) {
        spriteRosa.setTexture(texRosa);
        float escalaRosa = 110.0f / texRosa.getSize().x;
        spriteRosa.setScale(escalaRosa, escalaRosa);
    }

    if (texMano.loadFromFile("mano_barra_vida.png")) {
        spriteMano.setTexture(texMano);
        float escalaMano = 125.0f / texMano.getSize().x;
        spriteMano.setScale(escalaMano, escalaMano);
    }

    // FIJAMOS EL ÁREA DE JUEGO (800x600)
    limiteAncho = 800.0f;
    limiteAlto = 600.0f;

    // CALCULAMOS EL CENTRADO
    offsetX = (anchoP - limiteAncho) / 2.0f;
    offsetY = (altoP - limiteAlto) / 2.0f;

    combateTerminado = false;
    ganador = 0;
    cooldownPlanta = 0;
    cooldownZombie = 0;
    zombieControladoPorIA = false;
    dificultadIA = Dificultad::Normal;
    direccionIA = Vector2D(-1.0f, 0.0f);
}

void GestorArena::setZombieControladoPorIA(bool activo) {
    zombieControladoPorIA = activo;
}

void GestorArena::setDificultadIA(Dificultad dificultad) {
    dificultadIA = dificultad;
}
bool GestorArena::lineaDeTiroLibre(Vector2D origen, Vector2D destino) {
    float dx = destino.x - origen.x;
    float dy = destino.y - origen.y;
    float distancia = sqrt(dx * dx + dy * dy);

    if (distancia <= 1.0f) return true;

    int pasos = (int)(distancia / 20.0f);
    if (pasos < 1) pasos = 1;

    for (int i = 1; i < pasos; i++) {
        float t = (float)i / (float)pasos;
        Vector2D punto(origen.x + dx * t, origen.y + dy * t);

        for (unsigned int j = 0; j < obstaculos.size(); j++) {
            Vector2D posObs = obstaculos[j].getPosicion();
            if (Interaccion::colisionCajas(punto, 10.0f, 10.0f, posObs, 80.0f, 30.0f)) {
                return false;
            }
        }
    }

    return true;
}

bool GestorArena::hayPeligroDeProyectilPlanta(Vector2D posicionZombie) {
    for (unsigned int i = 0; i < proyectiles.size(); i++) {
        if (!proyectiles[i].esAliadoDePlanta()) continue;

        Vector2D posBala = proyectiles[i].getPosicion();
        float dx = posicionZombie.x - posBala.x;
        float dy = posicionZombie.y - posBala.y;
        float distancia = sqrt(dx * dx + dy * dy);

        if (distancia < 85.0f) {
            return true;
        }
    }
    return false;
}

void GestorArena::controlarZombieIA() {
    if (combateTerminado) return;

    Vector2D posZ = zombie->getPosicion();
    Vector2D posP = planta->getPosicion();

    float dxObjetivo = posP.x - posZ.x;
    float dyObjetivo = posP.y - posZ.y;
    float distancia = sqrt(dxObjetivo * dxObjetivo + dyObjetivo * dyObjetivo);
    if (distancia < 1.0f) distancia = 1.0f;

    float ux = dxObjetivo / distancia;
    float uy = dyObjetivo / distancia;

    // La IA siempre apunta al jugador humano
    zombie->mirarHacia(dxObjetivo, dyObjetivo);

    // Parámetros más agresivos por dificultad.
    // Fácil: sigue siendo ganable, pero ya dispara bastante más.
    // Normal: busca activamente al jugador y mantiene línea de tiro.
    // Difícil: presión casi constante, dispara en cuanto puede y remata.
    int tiempoDecision = 170;
    float agresividad = 0.90f;
    float probError = 0.12f;
    float distanciaDisparoMin = 180.0f;
    float distanciaDisparoMax = 720.0f;
    float distanciaMelee = 105.0f;
    int probAtaque = 78;
    float factorCooldownDisparo = 0.88f;
    float factorCooldownMelee = 0.82f;

    if (dificultadIA == Dificultad::Normal) {
        tiempoDecision = 95;
        agresividad = 1.18f;
        probError = 0.03f;
        distanciaDisparoMin = 115.0f;
        distanciaDisparoMax = 760.0f;
        distanciaMelee = 125.0f;
        probAtaque = 94;
        factorCooldownDisparo = 0.62f;
        factorCooldownMelee = 0.58f;
    }
    else if (dificultadIA == Dificultad::Dificil) {
        tiempoDecision = 35;
        agresividad = 1.45f;
        probError = 0.0f;
        distanciaDisparoMin = 75.0f;
        distanciaDisparoMax = 820.0f;
        distanciaMelee = 150.0f;
        probAtaque = 100;
        factorCooldownDisparo = 0.36f;
        factorCooldownMelee = 0.34f;
    }

    bool tiroLibre = lineaDeTiroLibre(posZ, posP);
    bool peligro = hayPeligroDeProyectilPlanta(posZ);

    if (relojDecisionIA.getElapsedTime().asMilliseconds() > tiempoDecision) {
        float dx = 0.0f;
        float dy = 0.0f;

        if (zombie->getEsCC()) {
            // Cuerpo a cuerpo: va mucho más directo a matar
            if (distancia > distanciaMelee * 0.75f) {
                dx = ux * agresividad;
                dy = uy * agresividad;
            }
            else {
                // Si ya está encima, no se queda quieto: presiona y rodea un poco
                float lateral = (dificultadIA == Dificultad::Dificil) ? 0.35f : 0.22f;
                dx = ux * 0.75f + (-uy) * lateral;
                dy = uy * 0.75f + ux * lateral;
            }
        }
        else {
            if (!tiroLibre) {
                // Si no puede disparar, no espera: se mueve hacia el jugador rodeando obstáculos
                float lateral = (dificultadIA == Dificultad::Dificil) ? 1.00f : 0.75f;
                dx = ux * 0.90f + (-uy) * lateral;
                dy = uy * 0.90f + ux * lateral;
            }
            else if (distancia > distanciaDisparoMax) {
                dx = ux * agresividad;
                dy = uy * agresividad;
            }
            else if (distancia < distanciaDisparoMin) {
                // separa un poco para seguir disparando
                if (dificultadIA == Dificultad::Dificil) {
                    dx = -ux * 0.25f + (-uy) * 0.75f;
                    dy = -uy * 0.25f + ux * 0.75f;
                }
                else if (dificultadIA == Dificultad::Dificil) {
                    dx = -ux * 0.40f + (-uy) * 0.50f;
                    dy = -uy * 0.40f + ux * 0.50f;
                }
                else {
                    dx = -ux * 0.60f + (-uy) * 0.30f;
                    dy = -uy * 0.60f + ux * 0.30f;
                }
            }
            else {
                // Rango bueno: avanza y orbita, manteniendo presión y disparo constante
                float lateral = 0.45f;
                float avance = 0.40f;
                if (dificultadIA == Dificultad::Normal) { lateral = 0.65f; avance = 0.55f; }
                if (dificultadIA == Dificultad::Normal) { lateral = 0.85f; avance = 0.75f; }

                dx = ux * avance + (-uy) * lateral;
                dy = uy * avance + ux * lateral;
            }

            // Esquiva, pero sin dejar de ir hacia la planta
            if (peligro && dificultadIA != Dificultad::Facil) {
                dx += -uy * 0.85f + ux * 0.25f;
                dy += ux * 0.85f + uy * 0.25f;
            }
        }

        // Fácil aún comete algún error, pero menos que antes
        if (dificultadIA == Dificultad::Facil) {
            float r = (float)rand() / (float)RAND_MAX;
            if (r < probError) {
                dx = ux * 0.45f + ((rand() % 200) - 100) / 180.0f;
                dy = uy * 0.45f + ((rand() % 200) - 100) / 180.0f;
            }
        }

        float modulo = sqrt(dx * dx + dy * dy);
        if (modulo > 1.0f) {
            dx /= modulo;
            dy /= modulo;
        }

        direccionIA = Vector2D(dx, dy);
        relojDecisionIA.restart();
    }

    float oldX = zombie->getPosicion().x;
    float oldY = zombie->getPosicion().y;

    if (direccionIA.x != 0.0f || direccionIA.y != 0.0f) {
        zombie->mover(direccionIA.x, direccionIA.y);
        resolverColisionesMovimiento(zombie, oldX, oldY);
    }

    // Cooldowns base según arma
    int cdZ = 20;
    float velBalaZ = 10.0f;
    TipoArma tipoZ = TipoArma::Basico;

    if (zombie->getDanio() == 50) { cdZ = 42; velBalaZ = 27.0f; tipoZ = TipoArma::Franco; }
    else if (zombie->getDanio() == 45) { cdZ = 38; velBalaZ = 7.5f; tipoZ = TipoArma::Pesado; }
    else if (zombie->getDanio() == 10) { cdZ = 5; velBalaZ = 17.0f; tipoZ = TipoArma::Rafaga; }
    else { cdZ = 14; velBalaZ = 12.0f; tipoZ = TipoArma::Basico; }

    bool permisoAtaque = ((rand() % 100) < probAtaque);

    if (cooldownZombie == 0 && permisoAtaque) {
        if (zombie->getEsCC()) {
            float rad = zombie->getAngulo() * 3.14159f / 180.0f;

            if (zombie->getDanio() == 70 && distancia < distanciaMelee + 35.0f) {
                GestorAudio::reproducirAtaque(nombreZombie); // <--- SONIDO EMBESTIDA IA

                Vector2D posAtacante = zombie->getPosicion();
                zombie->setPosicion(posAtacante.x + cos(rad) * 90.0f, posAtacante.y + sin(rad) * 90.0f);

                for (int i = 0; i < 6; i++) {
                    ParticulaTierra p;
                    p.posicion = { posAtacante.x + (rand() % 20 - 10), posAtacante.y + (rand() % 20 - 10) };
                    p.vidaMaxima = 0.6f + (rand() % 4 / 10.0f);
                    p.vidaActual = p.vidaMaxima;
                    p.radio = 8.0f + (rand() % 5);
                    p.color = sf::Color(140 + rand() % 40, 100 + rand() % 30, 60 + rand() % 20, 200);
                    particulas.push_back(p);
                }

                if (distancia < distanciaMelee + 15.0f) planta->recibirDanio(zombie->getDanio() * 0.9f);
                cooldownZombie = std::max(10, (int)(42 * factorCooldownMelee));
            }
            else if (zombie->getDanio() != 70 && distancia < distanciaMelee) {
                GestorAudio::reproducirAtaque(nombreZombie); //  SONIDO MELEE IA

                planta->recibirDanio(zombie->getDanio() * 0.9f);
                cooldownZombie = std::max(7, (int)(24 * factorCooldownMelee));
            }
        }
        else if (distancia < 850.0f && tiroLibre) {
            GestorAudio::reproducirAtaque(nombreZombie); // SONIDO DISPARO IA

            float rad = zombie->getAngulo() * 3.14159f / 180.0f;

            proyectiles.push_back(ProyectilArena(zombie->getPosicion().x, zombie->getPosicion().y,
                (float)cos(rad) * velBalaZ, (float)sin(rad) * velBalaZ, zombie->getDanio(), false, tipoZ));

            if (dificultadIA == Dificultad::Dificil && (rand() % 100) < 35) {
                float desvio = ((rand() % 11) - 5) * 0.015f;
                proyectiles.push_back(ProyectilArena(zombie->getPosicion().x, zombie->getPosicion().y,
                    (float)cos(rad + desvio) * velBalaZ, (float)sin(rad + desvio) * velBalaZ,
                    zombie->getDanio(), false, tipoZ));
            }
            else if (dificultadIA ==Dificultad::Normal && zombie->getDanio() == 10 && (rand() % 100) < 25) {
                proyectiles.push_back(ProyectilArena(zombie->getPosicion().x, zombie->getPosicion().y,
                    (float)cos(rad) * velBalaZ, (float)sin(rad) * velBalaZ,
                    zombie->getDanio(), false, tipoZ));
            }

            cooldownZombie = std::max(4, (int)(cdZ * factorCooldownDisparo));
        }
    }
} 

GestorArena::~GestorArena() {
    obstaculos.clear();
    proyectiles.clear();
}

void GestorArena::inicializarArena(int numObstaculos) {
    planta->setPosicion(50.0f, limiteAlto / 2.0f);
    zombie->setPosicion(limiteAncho - 90.0f, limiteAlto / 2.0f);

    obstaculos.clear();
    int intentos = 0; // Para evitar bucles infinitos si no hay hueco

    // Distancia mínima entre obstáculos (unos 150 píxeles para poder pasar entre ellos)
    float distanciaMinima = 150.0f;

    while (obstaculos.size() < numObstaculos && intentos < 1000) {
        // Generamos posición aleatoria en el centro de la arena
        float ox = 200.0f + (float)rand() / ((float)RAND_MAX / (limiteAncho - 400.0f));
        float oy = 150.0f + (float)rand() / ((float)RAND_MAX / (limiteAlto - 300.0f));

        // Comprobamos si choca con otro obstáculo ya colocado
        bool posicionValida = true;
        for (unsigned int i = 0; i < obstaculos.size(); i++) {
            float dx = ox - obstaculos[i].getPosicion().x;
            float dy = oy - obstaculos[i].getPosicion().y;
            float distancia = (float)sqrt(dx * dx + dy * dy);

            if (distancia < distanciaMinima) {
                posicionValida = false; // Está demasiado cerca, lo descartamos
                break;
            }
        }

        // Si hay espacio suficiente, lo creamos
        if (posicionValida) {
            // CAJA LÓGICA: 80 de ancho y 30 de alto 
            obstaculos.push_back(ObstaculoArena(ox, oy, 80.0f, 30.0f));
        }
        intentos++;
    }
}

void GestorArena::procesarEntrada() {
    float oldX_P = planta->getPosicion().x; float oldY_P = planta->getPosicion().y;
    float oldX_Z = zombie->getPosicion().x; float oldY_Z = zombie->getPosicion().y;

    // Definiciones de armas a distancia 
    int cdP = 20; float velBalaP = 10.0f; TipoArma tipoP = TipoArma::Basico;
    if (planta->getDanio() == 50) { cdP = 60; velBalaP = 25.0f; tipoP = TipoArma::Franco; }
    else if (planta->getDanio() == 45) { cdP = 55; velBalaP = 6.0f; tipoP = TipoArma::Pesado; }
    else if (planta->getDanio() == 10) { cdP = 8; velBalaP = 15.0f; tipoP = TipoArma::Rafaga; }

    int cdZ = 20; float velBalaZ = 10.0f; TipoArma tipoZ = TipoArma::Basico;
    if (zombie->getDanio() == 50) { cdZ = 60; velBalaZ = 25.0f; tipoZ = TipoArma::Franco; }
    else if (zombie->getDanio() == 45) { cdZ = 55; velBalaZ = 6.0f; tipoZ = TipoArma::Pesado; }
    else if (zombie->getDanio() == 10) { cdZ = 8; velBalaZ = 15.0f; tipoZ = TipoArma::Rafaga; }

    
    // CONTROLES PLANTA 
    
    float dxP = 0.0f, dyP = 0.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dyP -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dyP += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dxP -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dxP += 1.0f;
    if (dxP != 0.0f && dyP != 0.0f) { dxP *= 0.7071f; dyP *= 0.7071f; }
    if (dxP != 0.0f || dyP != 0.0f) planta->mover(dxP, dyP);

    if (cooldownPlanta > 0) cooldownPlanta--;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && cooldownPlanta == 0) {

        GestorAudio::reproducirAtaque(nombrePlanta); // <--- ¡AQUÍ SUENA LA PLANTA!

        if (planta->getEsCC()) {
            float rad = planta->getAngulo() * 3.14159f / 180.0f;
            float dist = (float)sqrt(pow(zombie->getPosicion().x - planta->getPosicion().x, 2) + pow(zombie->getPosicion().y - planta->getPosicion().y, 2));

            if (planta->getDanio() == 70) {
                Vector2D posAtacante = planta->getPosicion();
                planta->setPosicion(posAtacante.x + cos(rad) * 90.0f, posAtacante.y + sin(rad) * 90.0f);
                for (int i = 0; i < 4; i++) {
                    ParticulaTierra p;
                    p.posicion = { posAtacante.x + (rand() % 20 - 10), posAtacante.y + (rand() % 20 - 10) };
                    p.vidaMaxima = 0.6f + (rand() % 4 / 10.0f); p.vidaActual = p.vidaMaxima; p.radio = 8.0f + (rand() % 5);
                    p.color = sf::Color(140 + rand() % 40, 100 + rand() % 30, 60 + rand() % 20, 200);
                    particulas.push_back(p);
                }
                if (dist < 100.0f) zombie->recibirDanio(planta->getDanio() * 0.8f);
                cooldownPlanta = 50;
            }
            else {
                if (dist < 80.0f) zombie->recibirDanio(planta->getDanio() * 0.8f);
                cooldownPlanta = 30;
            }
        }
        else {
            float rad = planta->getAngulo() * 3.14159f / 180.0f;
            proyectiles.push_back(ProyectilArena(planta->getPosicion().x, planta->getPosicion().y, (float)cos(rad) * velBalaP, (float)sin(rad) * velBalaP, planta->getDanio(), true, tipoP));
            cooldownPlanta = cdP;
        }
    }

 
    // CONTROLES ZOMBI 
   
    if (cooldownZombie > 0) cooldownZombie--;

    if (zombieControladoPorIA) {
        controlarZombieIA();
    }
    else {
        float dxZ = 0.0f, dyZ = 0.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    dyZ -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  dyZ += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  dxZ -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) dxZ += 1.0f;
        if (dxZ != 0.0f && dyZ != 0.0f) { dxZ *= 0.7071f; dyZ *= 0.7071f; }
        if (dxZ != 0.0f || dyZ != 0.0f) zombie->mover(dxZ, dyZ);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && cooldownZombie == 0) {

            GestorAudio::reproducirAtaque(nombreZombie); 

            if (zombie->getEsCC()) {
                float rad = zombie->getAngulo() * 3.14159f / 180.0f;
                float dist = (float)sqrt(pow(planta->getPosicion().x - zombie->getPosicion().x, 2) + pow(planta->getPosicion().y - zombie->getPosicion().y, 2));

                if (zombie->getDanio() == 70) {
                    Vector2D posAtacante = zombie->getPosicion();
                    zombie->setPosicion(posAtacante.x + cos(rad) * 90.0f, posAtacante.y + sin(rad) * 90.0f);
                    for (int i = 0; i < 4; i++) {
                        ParticulaTierra p;
                        p.posicion = { posAtacante.x + (rand() % 20 - 10), posAtacante.y + (rand() % 20 - 10) };
                        p.vidaMaxima = 0.6f + (rand() % 4 / 10.0f); p.vidaActual = p.vidaMaxima; p.radio = 8.0f + (rand() % 5);
                        p.color = sf::Color(140 + rand() % 40, 100 + rand() % 30, 60 + rand() % 20, 200); particulas.push_back(p);
                    }
                    if (dist < 100.0f) planta->recibirDanio(zombie->getDanio() * 0.8f);
                    cooldownZombie = 50;
                }
                else {
                    if (dist < 80.0f) planta->recibirDanio(zombie->getDanio() * 0.8f);
                    cooldownZombie = 30;
                }
            }
            else {
                float rad = zombie->getAngulo() * 3.14159f / 180.0f;
                proyectiles.push_back(ProyectilArena(zombie->getPosicion().x, zombie->getPosicion().y, (float)cos(rad) * velBalaZ, (float)sin(rad) * velBalaZ, zombie->getDanio(), false, tipoZ));
                cooldownZombie = cdZ;
            }
        }
        resolverColisionesMovimiento(zombie, oldX_Z, oldY_Z);
    }
    resolverColisionesMovimiento(planta, oldX_P, oldY_P);
}


bool GestorArena::chocaConAlgo(PersonajeBaseArena* p) {
    if (estaFueraDeLimites(p->getPosicion())) return true;

    if (p->getEsVolador() == false) {
        for (unsigned int i = 0; i < obstaculos.size(); i++) {
            if (Interaccion::colision(p, &obstaculos[i])) return true;
        }
    }
    return false;
}


void GestorArena::resolverColisionesMovimiento(PersonajeBaseArena* p, float oldX, float oldY) {
    // Si el movimiento original es perfecto, no hacemos nada
    if (!chocaConAlgo(p)) return;

    Vector2D posActual = p->getPosicion();

    // Deslizamiento Vertical simple
    p->setPosicion(oldX, posActual.y);
    if (!chocaConAlgo(p)) return;

    // Deslizamiento Horizontal simple
    p->setPosicion(posActual.x, oldY);
    if (!chocaConAlgo(p)) return;


    // Si estamos chocando por culpa de que la pared está inclinada, recalculamos la pared a esta altura.
    float pctTopY = 0.183f;
    float pctBottomY = 0.813f;
    float yTop = (pantallaAlto * pctTopY) - offsetY;
    float yBottom = (pantallaAlto * pctBottomY) - offsetY;

    // Solo aplicamos el empuje si no se ha salido por el fondo o por abajo
    if (posActual.y >= yTop && posActual.y <= yBottom) {
        float pctTopLeftX = 0.294f;
        float pctTopRightX = 0.703f;
        float pctBottomLeftX = 0.190f;
        float pctBottomRightX = 0.803f;

        float xTopLeft = (pantallaAncho * pctTopLeftX) - offsetX;
        float xTopRight = (pantallaAncho * pctTopRightX) - offsetX;
        float xBottomLeft = (pantallaAncho * pctBottomLeftX) - offsetX;
        float xBottomRight = (pantallaAncho * pctBottomRightX) - offsetX;

        float porcentajeY = (posActual.y - yTop) / (yBottom - yTop);
        float limiteIzqActual = xTopLeft + porcentajeY * (xBottomLeft - xTopLeft);
        float limiteDerActual = xTopRight + porcentajeY * (xBottomRight - xTopRight);

        float nuevaX = oldX;

        // Si la diagonal nos ha "comido" por la izquierda, lo empujamos hacia la derecha
        if (nuevaX < limiteIzqActual) nuevaX = limiteIzqActual + 0.5f;
        // Si nos ha "comido" por la derecha, lo empujamos hacia la izquierda
        if (nuevaX > limiteDerActual) nuevaX = limiteDerActual - 0.5f;

        p->setPosicion(nuevaX, posActual.y);

        
        if (!chocaConAlgo(p)) return;
    }

    // Si todo falla (está atrapado en una esquina pura), se cancela el movimiento
    p->setPosicion(oldX, oldY);
}

void GestorArena::actualizarLider() {
    if (combateTerminado) return;

    sf::Time dt = sf::seconds(1.0f / 60.0f);
    std::vector<ParticulaTierra>::iterator itP = particulas.begin();
    while (itP != particulas.end()) {
        itP->vidaActual -= dt.asSeconds();
        if (itP->vidaActual <= 0.0f) {
            itP = particulas.erase(itP);
        }
        else {
            float pctVida = itP->vidaActual / itP->vidaMaxima;
            itP->radio += 1.0f;
            itP->color.a = (sf::Uint8)(200 * pctVida);
            itP++;
        }
    }

    for (unsigned int i = 0; i < proyectiles.size(); i++) {
        proyectiles[i].actualizar();
        Vector2D posProj = proyectiles[i].getPosicion();

        if (estaFueraDeLimites(posProj)) {
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

        if (proyectiles[i].esAliadoDePlanta()) {
            Vector2D posZ(zombie->getPosicion().x - 30.0f, zombie->getPosicion().y - 30.0f);
            if (Interaccion::colisionCajas(posProj, 15.0f, 15.0f, posZ, 60.0f, 60.0f)) {
                zombie->recibirDanio(proyectiles[i].getDanio() * 0.8f);
                proyectiles[i].destruir();
            }
        }
        else {
            Vector2D posP(planta->getPosicion().x - 30.0f, planta->getPosicion().y - 30.0f);
            if (Interaccion::colisionCajas(posProj, 15.0f, 15.0f, posP, 60.0f, 60.0f)) {
                planta->recibirDanio(proyectiles[i].getDanio() * 0.8f);
                proyectiles[i].destruir();
            }
        }
    }

    std::vector<ProyectilArena>::iterator it = proyectiles.begin();
    while (it != proyectiles.end()) {
        if (!it->estaActivo()) it = proyectiles.erase(it);
        else it++;
    }

    if (planta->getVida() <= 0 && zombie->getVida() <= 0) { combateTerminado = true; ganador = 0; }
    else if (planta->getVida() <= 0) { combateTerminado = true; ganador = 2; }
    else if (zombie->getVida() <= 0) { combateTerminado = true; ganador = 1; }
}



void GestorArena::dibujarBarrasDeVida(sf::RenderWindow& window) {
    float anchoBarra = 60.0f;
    float altoBarra = 400.0f;

    // Centrado vertical estricto en la pantalla
    float posY_Barras = (pantallaAlto / 2.0f) - (altoBarra / 2.0f);

    // Distancia a los bordes laterales 
    float margenLateral = 70.0f;


    //            BARRA PLANTA (IZQUIERDA)
    float posX_Planta = margenLateral;

    sf::RectangleShape fondoP(sf::Vector2f(anchoBarra, altoBarra));
    fondoP.setPosition(posX_Planta, posY_Barras);
    fondoP.setFillColor(sf::Color(30, 30, 30));
    fondoP.setOutlineThickness(5.0f);
    fondoP.setOutlineColor(sf::Color::Black);
    window.draw(fondoP);

    float porcP = planta->getVida() / planta->getVidaMaxima();
    float yActualP = posY_Barras + (altoBarra * (1 - porcP));
    sf::RectangleShape vidaP(sf::Vector2f(anchoBarra, altoBarra * porcP));
    vidaP.setPosition(posX_Planta, yActualP);
    vidaP.setTexture(&texVidaPlanta);
    vidaP.setTextureRect(sf::IntRect(0, 0, (int)texVidaPlanta.getSize().x, (int)(texVidaPlanta.getSize().y * porcP)));
    window.draw(vidaP);

    // CÁLCULO DE LA ROSA
    float anchoRosaVisual = texRosa.getSize().x * spriteRosa.getScale().x;
    float altoRosaVisual = texRosa.getSize().y * spriteRosa.getScale().y;

    // Centramos la rosa horizontalmente respecto a la barra
    float xRosa = posX_Planta - ((anchoRosaVisual - anchoBarra) / 2.0f);
    // Añadimos 45 píxeles para que la rosa baje y pise la barra
    float yRosa = yActualP - altoRosaVisual + 45.0f;

    spriteRosa.setPosition(xRosa, yRosa);
    window.draw(spriteRosa);


    //            BARRA ZOMBI (DERECHA)
    float posX_Zombi = pantallaAncho - margenLateral - anchoBarra;

    sf::RectangleShape fondoZ(sf::Vector2f(anchoBarra, altoBarra));
    fondoZ.setPosition(posX_Zombi, posY_Barras);
    fondoZ.setFillColor(sf::Color(30, 30, 30));
    fondoZ.setOutlineThickness(5.0f);
    fondoZ.setOutlineColor(sf::Color::Black);
    window.draw(fondoZ);

    float porcZ = zombie->getVida() / zombie->getVidaMaxima();
    float yActualZ = posY_Barras + (altoBarra * (1 - porcZ));
    sf::RectangleShape vidaZ(sf::Vector2f(anchoBarra, altoBarra * porcZ));
    vidaZ.setPosition(posX_Zombi, yActualZ);
    vidaZ.setTexture(&texVidaZombi);
    vidaZ.setTextureRect(sf::IntRect(0, 0, (int)texVidaZombi.getSize().x, (int)(texVidaZombi.getSize().y * porcZ)));
    window.draw(vidaZ);

    //  CÁLCULO DE LA MANO 
    float anchoManoVisual = texMano.getSize().x * spriteMano.getScale().x;
    float altoManoVisual = texMano.getSize().y * spriteMano.getScale().y;

    // Centramos la mano horizontalmente respecto a la barra
    float xMano = posX_Zombi - ((anchoManoVisual - anchoBarra) / 2.0f);
    // Añadimos 35 píxeles para bajar la mano sobre la textura
    float yMano = yActualZ - altoManoVisual + 45.0f;

    spriteMano.setPosition(xMano, yMano);
    window.draw(spriteMano);
}


struct ObjetoProfundidad {
    float y;
    int tipo;   // 1=Planta, 2=Zombi, 3=Obstaculo, 4=Bala
    int indice; // Para saber qué obstáculo o bala es


    bool operator<(const ObjetoProfundidad& otro) const {
        return y < otro.y;
    }
};


void GestorArena::dibujarEscena(sf::RenderWindow& window) {
    window.draw(spriteFondo);

    sf::View vistaJuego = window.getDefaultView();
    vistaJuego.move(-offsetX, -offsetY);
    window.setView(vistaJuego);

    // Usamos el spriteNeumatico para dibujar los obstáculos 
    for (unsigned int i = 0; i < obstaculos.size(); i++) {
        spriteNeumatico.setPosition(obstaculos[i].getPosicion().x, obstaculos[i].getPosicion().y);
        window.draw(spriteNeumatico);
    }

    //  Dibujar el humo/tierra antes de los personajes
    sf::CircleShape fParticula;
    for (unsigned int i = 0; i < particulas.size(); i++) {
        fParticula.setRadius(particulas[i].radio);
        fParticula.setOrigin(particulas[i].radio, particulas[i].radio);
        fParticula.setPosition(particulas[i].posicion.x, particulas[i].posicion.y);
        fParticula.setFillColor(particulas[i].color);
        window.draw(fParticula);
    }

    planta->dibujar(window);
    zombie->dibujar(window);

    for (unsigned int i = 0; i < proyectiles.size(); i++) {
        proyectiles[i].dibujar(window);
    }

    window.setView(window.getDefaultView());
    dibujarBarrasDeVida(window);
}



bool GestorArena::estaFueraDeLimites(Vector2D pos) {
    
    float pctTopY = 0.183f;      
    float pctBottomY = 0.813f; 

    float pctTopLeftX = 0.294f;   
    float pctTopRightX = 0.703f;   
    float pctBottomLeftX = 0.190f; 
    float pctBottomRightX = 0.803f;

    // Convertimos esos porcentajes a píxeles LÓGICOS de juego.
   
    float yTop = (pantallaAlto * pctTopY) - offsetY;
    float yBottom = (pantallaAlto * pctBottomY) - offsetY;

    float xTopLeft = (pantallaAncho * pctTopLeftX) - offsetX;
    float xTopRight = (pantallaAncho * pctTopRightX) - offsetX;
    float xBottomLeft = (pantallaAncho * pctBottomLeftX) - offsetX;
    float xBottomRight = (pantallaAncho * pctBottomRightX) - offsetX;

    // Comprobamos si se sale por el fondo o por la parte delantera de la pista
    if (pos.y < yTop || pos.y > yBottom) {
        return true;
    }

    // Interpolación lineal para las paredes diagonales
    float porcentajeY = (pos.y - yTop) / (yBottom - yTop);
    float limiteIzqActual = xTopLeft + porcentajeY * (xBottomLeft - xTopLeft);
    float limiteDerActual = xTopRight + porcentajeY * (xBottomRight - xTopRight);

    // Comprobamos si choca con las gradas laterales
    if (pos.x < limiteIzqActual || pos.x > limiteDerActual) {
        return true;
    }

    return false; 
}






int resolverCombateEnArena(personaje* atacante, personaje* defensor, sf::RenderWindow& window,
    int bandoFavorecido, bool zombieControladoPorIA, Dificultad dificultadArenaIA) {

    personaje* plantaTablero = (atacante->getequipo() == bando::planta) ? atacante : defensor;
    personaje* zombiTablero = (atacante->getequipo() == bando::zombi) ? atacante : defensor;

    bool plantaBoost = (bandoFavorecido == 1);
    bool zombiBoost = (bandoFavorecido == 2);

    PersonajeBaseArena* pArena = new LuchadorAvatar(plantaTablero, plantaBoost);
    PersonajeBaseArena* zArena = new LuchadorAvatar(zombiTablero, zombiBoost);

    float anchoReal = (float)window.getSize().x;
    float altoReal = (float)window.getSize().y;
    GestorArena arena(pArena, zArena, anchoReal, altoReal);

    arena.setNombres(plantaTablero->getsimbolo(), zombiTablero->getsimbolo());

    arena.inicializarArena(6);
    arena.setZombieControladoPorIA(zombieControladoPorIA);
    arena.setDificultadIA(dificultadArenaIA);

    int ganadorArena = 0;

    //  VARIABLES DE PAUSA INTEGRADAS 
    bool arenaPausada = false;
    sf::Font fuente;
    if (!fuente.loadFromFile("COOPBL.ttf")) {
        std::cout << "Error cargando fuente en Arena\n";
    }

    GestorAudio::reproducirArena();

    while (window.isOpen() && !arena.isTerminado()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                delete pArena; delete zArena;
                return 0;
            }

            // Alternar pausa con ESC
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                arenaPausada = !arenaPausada;
            }

            // Procesar clics de los botones de pausa
            if (arenaPausada && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                float ratonX = (float)event.mouseButton.x;
                float ratonY = (float)event.mouseButton.y;
                float centroX = anchoReal / 2.f;
                float centroY = altoReal / 2.f;

                // 1. Botón Reanudar (Y = centroY - 50)
                if (ratonX >= centroX - 150.f && ratonX <= centroX + 150.f &&
                    ratonY >= centroY - 50.f - 25.f && ratonY <= centroY - 50.f + 25.f) {
                    arenaPausada = false;
                }
                // 2. Botón Menú Principal (Y = centroY + 20)
                else if (ratonX >= centroX - 150.f && ratonX <= centroX + 150.f &&
                    ratonY >= centroY + 20.f - 25.f && ratonY <= centroY + 20.f + 25.f) {
                    g_VolverAlMenuPrincipal = true; // Activamos la señal para el main
                    delete pArena; delete zArena;
                    return 0; // Rompemos el combate inmediatamente
                }
                // 3. Botón Salir del Juego (Y = centroY + 90)
                else if (ratonX >= centroX - 150.f && ratonX <= centroX + 150.f &&
                    ratonY >= centroY + 90.f - 25.f && ratonY <= centroY + 90.f + 25.f) {
                    window.close();
                    delete pArena; delete zArena;
                    return 0;
                }
            }
        }

        // Control de volumen deslizable (Tiempo real)
        if (arenaPausada && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            float ratonX = (float)mousePos.x;
            float ratonY = (float)mousePos.y;
            float centroX = anchoReal / 2.f;
            float centroY = altoReal / 2.f;
            float xBase = centroX - 150.f;

            // Arrastrar barra de Música
            if (ratonX >= xBase && ratonX <= xBase + 300.f &&
                ratonY >= centroY + 170.f - 15.f && ratonY <= centroY + 170.f + 35.f) {
                float nuevoVolumen = ((ratonX - xBase) / 300.f) * 100.f;
                GestorAudio::setVolumenMusica(nuevoVolumen);
            }
            // Arrastrar barra de Efectos (SFX)
            else if (ratonX >= xBase && ratonX <= xBase + 300.f &&
                ratonY >= centroY + 240.f - 15.f && ratonY <= centroY + 240.f + 35.f) {
                float nuevoVolumen = ((ratonX - xBase) / 300.f) * 100.f;
                GestorAudio::setVolumenSFX(nuevoVolumen);
            }
        }

        // Detector de sonido Hover para los botones de la arena
        static int ultimoHoverArenaPausa = 0;
        if (arenaPausada) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            float ratonX = (float)mousePos.x;
            float ratonY = (float)mousePos.y;
            float centroX = anchoReal / 2.f;
            float centroY = altoReal / 2.f;

            int hoverActualArena = 0;
            if (ratonX >= centroX - 150.f && ratonX <= centroX + 150.f) {
                if (ratonY >= centroY - 50.f - 25.f && ratonY <= centroY - 50.f + 25.f) hoverActualArena = 1;
                else if (ratonY >= centroY + 20.f - 25.f && ratonY <= centroY + 20.f + 25.f) hoverActualArena = 2;
                else if (ratonY >= centroY + 90.f - 25.f && ratonY <= centroY + 90.f + 25.f) hoverActualArena = 3;
            }

            if (hoverActualArena != 0 && hoverActualArena != ultimoHoverArenaPausa) {
                GestorAudio::reproducirHover();
            }
            ultimoHoverArenaPausa = hoverActualArena;
        }
        else {
            ultimoHoverArenaPausa = 0;
        }

        // Solo actualizamos físicas y movimientos si NO está pausado
        if (!arenaPausada) {
            arena.procesarEntrada();
            arena.actualizarLider();
        }

        window.clear(sf::Color(30, 30, 30));
        arena.dibujarEscena(window);

        //RENDERIZADO DEL MENÚ DE PAUSA
        if (arenaPausada) {
            // Velo translúcido 
            sf::RectangleShape velo(sf::Vector2f(anchoReal, altoReal));
            velo.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(velo);

            // Título principal
            sf::Text titulo("JUEGO PAUSADO", fuente, 60);
            titulo.setFillColor(sf::Color::White);
            sf::FloatRect bounds = titulo.getLocalBounds();
            titulo.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
            titulo.setPosition(anchoReal / 2.f, altoReal / 2.f - 140.f);
            window.draw(titulo);

            // Lambda idéntica para renderizar los botones
            auto dibujarBotonArena = [&](std::string texto, float offsetY) {
                sf::RectangleShape btn(sf::Vector2f(300.f, 50.f));
                btn.setOrigin(150.f, 25.f);
                btn.setPosition(anchoReal / 2.f, altoReal / 2.f + offsetY);
                btn.setFillColor(sf::Color(40, 40, 40));
                btn.setOutlineThickness(2);
                btn.setOutlineColor(sf::Color::White);
                window.draw(btn);

                sf::Text txt(sf::String::fromUtf8(texto.begin(), texto.end()), fuente, 24);
                sf::FloatRect txtBounds = txt.getLocalBounds();
                txt.setOrigin(txtBounds.left + txtBounds.width / 2.f, txtBounds.top + txtBounds.height / 2.f);
                txt.setPosition(anchoReal / 2.f, altoReal / 2.f + offsetY);
                window.draw(txt);
                };

            // Dibujamos los 3 botones con la distribución exacta
            dibujarBotonArena("Reanudar", -50.f);
            dibujarBotonArena("Menú Principal", 20.f);
            dibujarBotonArena("Salir del Juego", 90.f);

            // Lambda idéntica para renderizar las barras deslizantes
            auto dibujarBarraVolumenArena = [&](std::string texto, float offsetY, float porcentaje) {
                float xBase = anchoReal / 2.f - 150.f;
                float yBase = altoReal / 2.f + offsetY;

                sf::Text txt(sf::String::fromUtf8(texto.begin(), texto.end()), fuente, 20);
                txt.setPosition(xBase, yBase - 25.f);
                window.draw(txt);

                sf::RectangleShape barraFondo(sf::Vector2f(300.f, 20.f));
                barraFondo.setPosition(xBase, yBase);
                barraFondo.setFillColor(sf::Color(60, 60, 60));
                barraFondo.setOutlineThickness(2);
                barraFondo.setOutlineColor(sf::Color::White);
                window.draw(barraFondo);

                sf::RectangleShape barraProgreso(sf::Vector2f(300.f * (porcentaje / 100.f), 20.f));
                barraProgreso.setPosition(xBase, yBase);
                barraProgreso.setFillColor(sf::Color(50, 205, 50));
                window.draw(barraProgreso);
                };

            // Dibujamos las barras justo debajo
            dibujarBarraVolumenArena("Música", 170.f, GestorAudio::getVolumenMusica());
            dibujarBarraVolumenArena("Efectos (SFX)", 240.f, GestorAudio::getVolumenSFX());
        }

        window.display();
    }

    GestorAudio::finDeArena();

    if (arena.isTerminado()) {
        ganadorArena = arena.getGanador();

        float danioP = plantaTablero->obtenerVida() - pArena->getVida();
        if (danioP > 0) plantaTablero->recibirDano((int)danioP);

        float danioZ = zombiTablero->obtenerVida() - zArena->getVida();
        if (danioZ > 0) zombiTablero->recibirDano((int)danioZ);

        sf::sleep(sf::seconds(2));
    }

    delete pArena;
    delete zArena;

    if (ganadorArena == 0) return 0;

    if (atacante->getequipo() == bando::planta) {
        return (ganadorArena == 1) ? 1 : 2;
    }
    else {
        return (ganadorArena == 2) ? 1 : 2;
    }
}
