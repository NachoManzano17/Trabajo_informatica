// GestorArena.cpp
#include "GestorArena.h"
#include <stdlib.h> 
#include <math.h>  
#include <algorithm> 
#include <utility>   


GestorArena::GestorArena(PersonajeBaseArena* pPlanta, PersonajeBaseArena* pZombie, float anchoP, float altoP) {
    planta = pPlanta;
    zombie = pZombie;
    pantallaAncho = anchoP;
    pantallaAlto = altoP;

    // 1. Cargamos el fondo y lo escalamos a pantalla completa
    if (texturaFondo.loadFromFile("imagenes/fondo_arena.jpg")) {
        spriteFondo.setTexture(texturaFondo);

        // ¡Esto es lo que se había borrado por error!
        float anchoImagen = (float)texturaFondo.getSize().x;
        float altoImagen = (float)texturaFondo.getSize().y;
        spriteFondo.setScale(pantallaAncho / anchoImagen, pantallaAlto / altoImagen);
    } // <--- ¡AQUÍ ESTÁ LA LLAVE QUE FALTABA!

    // --- CARGAR LOS NEUMÁTICOS ---
    if (texturaNeumatico.loadFromFile("imagenes/neumaticos.png")) {
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
    texVidaPlanta.loadFromFile("imagenes/verde_barra_vida.png");
    texVidaZombi.loadFromFile("imagenes/morado_barra_vida.png");

    if (texRosa.loadFromFile("imagenes/rosa_barra_vida.png")) {
        spriteRosa.setTexture(texRosa);
        float escalaRosa = 110.0f / texRosa.getSize().x;
        spriteRosa.setScale(escalaRosa, escalaRosa);
    }

    if (texMano.loadFromFile("imagenes/mano_barra_vida.png")) {
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

    // 1. Lista de "Cosas a dibujar"
    std::vector<ObjetoProfundidad> orden;

    // Metemos a los personajes
    ObjetoProfundidad opPlanta = { planta->getPosicion().y, 1, 0 };
    orden.push_back(opPlanta);

    ObjetoProfundidad opZombi = { zombie->getPosicion().y, 2, 0 };
    orden.push_back(opZombi);

    // Metemos TODOS los obstáculos
    for (unsigned int i = 0; i < obstaculos.size(); i++) {
        ObjetoProfundidad opObs = { obstaculos[i].getPosicion().y, 3, (int)i };
        orden.push_back(opObs);
    }

    // Metemos TODOS los proyectiles
    for (unsigned int i = 0; i < proyectiles.size(); i++) {
        ObjetoProfundidad opProj = { proyectiles[i].getPosicion().y, 4, (int)i };
        orden.push_back(opProj);
    }

    // 2. Ordenamos todo de una sola pasada
    std::sort(orden.begin(), orden.end());

    // 3. Dibujamos en el orden correcto
    for (unsigned int i = 0; i < orden.size(); i++) {
        if (orden[i].tipo == 1) {
            planta->dibujar(window);
        }
        else if (orden[i].tipo == 2) {
            zombie->dibujar(window);
        }
        else if (orden[i].tipo == 3) {
            Vector2D pos = obstaculos[orden[i].indice].getPosicion();
            spriteNeumatico.setPosition(pos.x, pos.y);
            window.draw(spriteNeumatico);
        }
        else if (orden[i].tipo == 4) {
            proyectiles[orden[i].indice].dibujar(window);
        }
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