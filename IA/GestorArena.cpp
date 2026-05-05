#include "GestorArena.h"
#include <stdlib.h> 
#include <math.h>  
#include <algorithm> 
#include <utility>   
#include <cmath>


GestorArena::GestorArena(PersonajeBaseArena* pPlanta, PersonajeBaseArena* pZombie, float anchoP, float altoP) {
    planta = pPlanta;
    zombie = pZombie;
    pantallaAncho = anchoP;
    pantallaAlto = altoP;

    // 1. Cargamos el fondo y lo escalamos a pantalla completa
    if (texturaFondo.loadFromFile("fondo_arena.jpg")) {
        spriteFondo.setTexture(texturaFondo);

        // ¡Esto es lo que se había borrado por error!
        float anchoImagen = (float)texturaFondo.getSize().x;
        float altoImagen = (float)texturaFondo.getSize().y;
        spriteFondo.setScale(pantallaAncho / anchoImagen, pantallaAlto / altoImagen);
    } // <--- ¡AQUÍ ESTÁ LA LLAVE QUE FALTABA!

    // --- CARGAR LOS NEUMÁTICOS ---
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

    // --- CARGAR IMÁGENES DE LAS BARRAS DE VIDA ---
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

    // 2. FIJAMOS EL ÁREA DE JUEGO (800x600)
    limiteAncho = 800.0f;
    limiteAlto = 600.0f;

    // 3. CALCULAMOS EL CENTRADO
    offsetX = (anchoP - limiteAncho) / 2.0f;
    offsetY = (altoP - limiteAlto) / 2.0f;

    combateTerminado = false;
    ganador = 0;
    cooldownPlanta = 0;
    cooldownZombie = 0;
    zombieControladoPorIA = false;
    dificultadIA = Difficulty::Medium;
    direccionIA = Vector2D(-1.0f, 0.0f);
}

void GestorArena::setZombieControladoPorIA(bool activo) {
    zombieControladoPorIA = activo;
}

void GestorArena::setDificultadIA(Difficulty dificultad) {
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

    // La IA siempre apunta al jugador humano. Esto es clave para que dispare mucho más.
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

    if (dificultadIA == Difficulty::Medium) {
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
    else if (dificultadIA == Difficulty::Hard) {
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
            // Cuerpo a cuerpo: va mucho más directo a matar.
            if (distancia > distanciaMelee * 0.75f) {
                dx = ux * agresividad;
                dy = uy * agresividad;
            }
            else {
                // Si ya está encima, no se queda quieto: presiona y rodea un poco.
                float lateral = (dificultadIA == Difficulty::Hard) ? 0.35f : 0.22f;
                dx = ux * 0.75f + (-uy) * lateral;
                dy = uy * 0.75f + ux * lateral;
            }
        }
        else {
            if (!tiroLibre) {
                // Si no puede disparar, no espera: se mueve hacia el jugador rodeando obstáculos.
                float lateral = (dificultadIA == Difficulty::Hard) ? 1.00f : 0.75f;
                dx = ux * 0.90f + (-uy) * lateral;
                dy = uy * 0.90f + ux * lateral;
            }
            else if (distancia > distanciaDisparoMax) {
                dx = ux * agresividad;
                dy = uy * agresividad;
            }
            else if (distancia < distanciaDisparoMin) {
                // Antes retrocedía demasiado. Ahora solo se separa un poco para seguir disparando.
                if (dificultadIA == Difficulty::Hard) {
                    dx = -ux * 0.25f + (-uy) * 0.75f;
                    dy = -uy * 0.25f + ux * 0.75f;
                }
                else if (dificultadIA == Difficulty::Medium) {
                    dx = -ux * 0.40f + (-uy) * 0.50f;
                    dy = -uy * 0.40f + ux * 0.50f;
                }
                else {
                    dx = -ux * 0.60f + (-uy) * 0.30f;
                    dy = -uy * 0.60f + ux * 0.30f;
                }
            }
            else {
                // Rango bueno: avanza y orbita, manteniendo presión y disparo constante.
                float lateral = 0.45f;
                float avance = 0.40f;
                if (dificultadIA == Difficulty::Medium) { lateral = 0.65f; avance = 0.55f; }
                if (dificultadIA == Difficulty::Hard) { lateral = 0.85f; avance = 0.75f; }

                dx = ux * avance + (-uy) * lateral;
                dy = uy * avance + ux * lateral;
            }

            // Esquiva, pero sin dejar de ir hacia la planta.
            if (peligro && dificultadIA != Difficulty::Easy) {
                dx += -uy * 0.85f + ux * 0.25f;
                dy += ux * 0.85f + uy * 0.25f;
            }
        }

        // Fácil aún comete algún error, pero menos que antes.
        if (dificultadIA == Difficulty::Easy) {
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

    // Cooldowns base según arma. Luego se reducen mucho más por dificultad.
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
                planta->recibirDanio(zombie->getDanio() * 0.9f);
                cooldownZombie = std::max(7, (int)(24 * factorCooldownMelee));
            }
        }
        else if (distancia < 850.0f && tiroLibre) {
            float rad = zombie->getAngulo() * 3.14159f / 180.0f;

            // Disparo principal.
            proyectiles.push_back(ProyectilArena(zombie->getPosicion().x, zombie->getPosicion().y,
                (float)cos(rad) * velBalaZ, (float)sin(rad) * velBalaZ, zombie->getDanio(), false, tipoZ));

            // En difícil dispara una segunda bala ligera a veces para que sea mucho más peligroso.
            // No se hace siempre para no romper visualmente la arena.
            if (dificultadIA == Difficulty::Hard && (rand() % 100) < 35) {
                float desvio = ((rand() % 11) - 5) * 0.015f;
                proyectiles.push_back(ProyectilArena(zombie->getPosicion().x, zombie->getPosicion().y,
                    (float)cos(rad + desvio) * velBalaZ, (float)sin(rad + desvio) * velBalaZ,
                    zombie->getDanio(), false, tipoZ));
            }
            else if (dificultadIA == Difficulty::Medium && zombie->getDanio() == 10 && (rand() % 100) < 25) {
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

    // --- Definiciones de armas a distancia ---
    int cdP = 20; float velBalaP = 10.0f; TipoArma tipoP = TipoArma::Basico;
    if (planta->getDanio() == 50) { cdP = 60; velBalaP = 25.0f; tipoP = TipoArma::Franco; }
    else if (planta->getDanio() == 45) { cdP = 55; velBalaP = 6.0f; tipoP = TipoArma::Pesado; }
    else if (planta->getDanio() == 10) { cdP = 8; velBalaP = 15.0f; tipoP = TipoArma::Rafaga; }

    int cdZ = 20; float velBalaZ = 10.0f; TipoArma tipoZ = TipoArma::Basico;
    if (zombie->getDanio() == 50) { cdZ = 60; velBalaZ = 25.0f; tipoZ = TipoArma::Franco; }
    else if (zombie->getDanio() == 45) { cdZ = 55; velBalaZ = 6.0f; tipoZ = TipoArma::Pesado; }
    else if (zombie->getDanio() == 10) { cdZ = 8; velBalaZ = 15.0f; tipoZ = TipoArma::Rafaga; }

    // ==========================================
    // --- CONTROLES PLANTA ---
    // ==========================================
    float dxP = 0.0f, dyP = 0.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dyP -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dyP += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dxP -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dxP += 1.0f;
    if (dxP != 0.0f && dyP != 0.0f) { dxP *= 0.7071f; dyP *= 0.7071f; }
    if (dxP != 0.0f || dyP != 0.0f) planta->mover(dxP, dyP);

    if (cooldownPlanta > 0) cooldownPlanta--;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && cooldownPlanta == 0) {
        if (planta->getEsCC()) {
            float rad = planta->getAngulo() * 3.14159f / 180.0f;
            float dist = (float)sqrt(pow(zombie->getPosicion().x - planta->getPosicion().x, 2) + pow(zombie->getPosicion().y - planta->getPosicion().y, 2));

            // 1. MELÉ DE EMBESTIDA (Carnívora: Daño 70)
            if (planta->getDanio() == 70) {
                Vector2D posAtacante = planta->getPosicion();
                planta->setPosicion(posAtacante.x + cos(rad) * 90.0f, posAtacante.y + sin(rad) * 90.0f);

                for (int i = 0; i < 4; i++) {
                    ParticulaTierra p;
                    p.posicion = { posAtacante.x + (rand() % 20 - 10), posAtacante.y + (rand() % 20 - 10) };
                    p.vidaMaxima = 0.6f + (rand() % 4 / 10.0f);
                    p.vidaActual = p.vidaMaxima;
                    p.radio = 8.0f + (rand() % 5);
                    p.color = sf::Color(140 + rand() % 40, 100 + rand() % 30, 60 + rand() % 20, 200);
                    particulas.push_back(p);
                }

                if (dist < 100.0f) zombie->recibirDanio(planta->getDanio() * 0.8f);
                cooldownPlanta = 50; // Tarda mucho en volver a saltar
            }
            // 2. CUERPO A CUERPO NORMAL (Dave: Daño 40)
            else {
                // Sin saltos ni humo, solo sartenazo a quien esté cerca (80px)
                if (dist < 80.0f) zombie->recibirDanio(planta->getDanio() * 0.8f);
                cooldownPlanta = 30; // Ataca bastante más rápido
            }
        }
        else { // DISPAROS
            float rad = planta->getAngulo() * 3.14159f / 180.0f;
            proyectiles.push_back(ProyectilArena(planta->getPosicion().x, planta->getPosicion().y, (float)cos(rad) * velBalaP, (float)sin(rad) * velBalaP, planta->getDanio(), true, tipoP));
            cooldownPlanta = cdP;
        }
    }

    // ==========================================
    // --- CONTROLES ZOMBI ---
    // ==========================================
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
            if (zombie->getEsCC()) {
                float rad = zombie->getAngulo() * 3.14159f / 180.0f;
                float dist = (float)sqrt(pow(planta->getPosicion().x - zombie->getPosicion().x, 2) + pow(planta->getPosicion().y - zombie->getPosicion().y, 2));

                // 1. MELÉ DE EMBESTIDA (SuperCerebroz: Daño 70)
                if (zombie->getDanio() == 70) {
                    Vector2D posAtacante = zombie->getPosicion();
                    zombie->setPosicion(posAtacante.x + cos(rad) * 90.0f, posAtacante.y + sin(rad) * 90.0f);

                    for (int i = 0; i < 4; i++) {
                        ParticulaTierra p;
                        p.posicion = { posAtacante.x + (rand() % 20 - 10), posAtacante.y + (rand() % 20 - 10) };
                        p.vidaMaxima = 0.6f + (rand() % 4 / 10.0f);
                        p.vidaActual = p.vidaMaxima;
                        p.radio = 8.0f + (rand() % 5);
                        p.color = sf::Color(140 + rand() % 40, 100 + rand() % 30, 60 + rand() % 20, 200);
                        particulas.push_back(p);
                    }

                    if (dist < 100.0f) planta->recibirDanio(zombie->getDanio() * 0.8f);
                    cooldownZombie = 50;
                }
                // 2. CUERPO A CUERPO NORMAL (DrZomboss: Daño 40)
                else {
                    if (dist < 80.0f) planta->recibirDanio(zombie->getDanio() * 0.8f);
                    cooldownZombie = 30;
                }
            }
            else { // DISPAROS
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

        // Si después del empujoncito ya está a salvo y no pisa un neumático, ¡el deslizamiento funciona!
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
                // --- NUEVO MULTIPLICADOR: 0.8f ---
                zombie->recibirDanio(proyectiles[i].getDanio() * 0.8f);
                proyectiles[i].destruir();
            }
        }
        else {
            Vector2D posP(planta->getPosicion().x - 30.0f, planta->getPosicion().y - 30.0f);
            if (Interaccion::colisionCajas(posProj, 15.0f, 15.0f, posP, 60.0f, 60.0f)) {
                // --- NUEVO MULTIPLICADOR: 0.8f ---
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

    // 1. Centrado vertical estricto en la pantalla
    float posY_Barras = (pantallaAlto / 2.0f) - (altoBarra / 2.0f);

    // 2. Distancia IDÉNTICA a los bordes laterales 
    float margenLateral = 70.0f;

    // ==========================================
    //            BARRA PLANTA (IZQUIERDA)
    // ==========================================
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

    // --- CÁLCULO DE LA ROSA ---
    float anchoRosaVisual = texRosa.getSize().x * spriteRosa.getScale().x;
    float altoRosaVisual = texRosa.getSize().y * spriteRosa.getScale().y;

    // Centramos la rosa horizontalmente respecto a la barra
    float xRosa = posX_Planta - ((anchoRosaVisual - anchoBarra) / 2.0f);
    // Añadimos 45 píxeles para que la rosa baje y pise la barra
    float yRosa = yActualP - altoRosaVisual + 45.0f;

    spriteRosa.setPosition(xRosa, yRosa);
    window.draw(spriteRosa);


    // ==========================================
    //            BARRA ZOMBI (DERECHA)
    // ==========================================
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

    // --- CÁLCULO DE LA MANO ---
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

    // --- CORRECCIÓN: Usamos el spriteNeumatico para dibujar los obstáculos ---
    for (unsigned int i = 0; i < obstaculos.size(); i++) {
        spriteNeumatico.setPosition(obstaculos[i].getPosicion().x, obstaculos[i].getPosicion().y);
        window.draw(spriteNeumatico);
    }

    // --- Dibujar el humo/tierra antes de los personajes ---
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
    // 1. Porcentajes exactos sacados de tus fotos (respecto al tamaño total de la imagen)
    float pctTopY = 0.183f;      // Y de arriba (aprox 18.3%)
    float pctBottomY = 0.813f;   // Y de abajo (aprox 81.3%)

    float pctTopLeftX = 0.294f;    // X superior izquierda (29.4%)
    float pctTopRightX = 0.703f;   // X superior derecha (70.3%)
    float pctBottomLeftX = 0.190f; // X inferior izquierda (19.0%)
    float pctBottomRightX = 0.803f;// X inferior derecha (80.3%)

    // 2. Convertimos esos porcentajes a píxeles LÓGICOS de tu juego.
    // (Multiplicamos el porcentaje por el tamaño del monitor y le quitamos el centrado de la cámara)
    float yTop = (pantallaAlto * pctTopY) - offsetY;
    float yBottom = (pantallaAlto * pctBottomY) - offsetY;

    float xTopLeft = (pantallaAncho * pctTopLeftX) - offsetX;
    float xTopRight = (pantallaAncho * pctTopRightX) - offsetX;
    float xBottomLeft = (pantallaAncho * pctBottomLeftX) - offsetX;
    float xBottomRight = (pantallaAncho * pctBottomRightX) - offsetX;

    // 3. Comprobamos si se sale por el fondo o por la parte delantera de la pista
    if (pos.y < yTop || pos.y > yBottom) {
        return true;
    }

    // 4. MAGIA MATEMÁTICA: Interpolación lineal para las paredes diagonales
    float porcentajeY = (pos.y - yTop) / (yBottom - yTop);
    float limiteIzqActual = xTopLeft + porcentajeY * (xBottomLeft - xTopLeft);
    float limiteDerActual = xTopRight + porcentajeY * (xBottomRight - xTopRight);

    // 5. Comprobamos si choca con las gradas laterales
    if (pos.x < limiteIzqActual || pos.x > limiteDerActual) {
        return true;
    }

    return false; // Si llega aquí, está a salvo dentro de la línea azul
}

