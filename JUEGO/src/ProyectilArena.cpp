#include "ProyectilArena.h"
#include <cmath>

ProyectilArena::ProyectilArena(float x, float y, float velX, float velY, float d, bool planta, TipoArma t) {
    posicion = { x, y };
    velocidad = { velX, velY };
    danio = d;
    esDePlanta = planta;
    viva = true;
    tipo = t;
    usaSprite = false;

    // Cargamos la textura del balón de rugby una sola vez 
    static sf::Texture texRugby;
    static sf::Texture texCerebro;
    static sf::Texture texGuisante;
    static sf::Texture texSol;
    static sf::Texture texCactus;
    static sf::Texture texBala;
    static sf::Texture texDronajo;
    static sf::Texture texLoro;
    static bool texCargada = false;
    if (!texCargada) {
        texRugby.loadFromFile("balon_rugby.png");
        texCerebro.loadFromFile("ataque_cerebro.png");
        texGuisante.loadFromFile("ataque_guisante.png");
        texSol.loadFromFile("ataque_sol.png");
        texCactus.loadFromFile("ataque_cactus.png");
        texBala.loadFromFile("ataque_bala.png");
        texDronajo.loadFromFile("ataque_dronajo.png");
        texLoro.loadFromFile("ataque_loro.png");
        static sf::Texture texDronajo;

        texCargada = true;
    }

    switch (tipo) {
    case TipoArma::Basico:
        if (esDePlanta) {
            // Guisante (Planta) con imagen
            usaSprite = true;
            spriteBala.setTexture(texGuisante);
            // Tamaño de unos 25 píxeles para el guisante
            float escalaG = 65.0f / texGuisante.getSize().x;
            spriteBala.setScale(escalaG, escalaG);
            spriteBala.setOrigin(texGuisante.getSize().x / 2.0f, texGuisante.getSize().y / 2.0f);
        }
        else {
            // Cerebro (Zombidito - Zombi básico)
            usaSprite = true;
            spriteBala.setTexture(texCerebro);
            // Hacemos que mida unos 30 píxeles para que sea un tamaño normal
            float escalaC = 30.0f / texCerebro.getSize().x;
            spriteBala.setScale(escalaC, escalaC);
            spriteBala.setOrigin(texCerebro.getSize().x / 2.0f, texCerebro.getSize().y / 2.0f);
        }
        break;

    case TipoArma::Franco:
        if (esDePlanta) {
            // Ataque del Cactus con imagen
            usaSprite = true;
            spriteBala.setTexture(texCactus);
            // Tamaño de unos 45 píxeles de largo para que imponga respeto
            float escalaCactus = 45.0f / texCactus.getSize().x;
            spriteBala.setScale(escalaCactus, escalaCactus);
            spriteBala.setOrigin(texCactus.getSize().x / 2.0f, texCactus.getSize().y / 2.0f);
        }
        else {
            // Ataque del Soldado Zombi - ¡Ahora con imagen!
            usaSprite = true;
            spriteBala.setTexture(texBala);
            // Ajustamos el tamaño a 40 píxeles de largo (puedes variarlo si quieres)
            float escalaBala = 40.0f / texBala.getSize().x;
            spriteBala.setScale(escalaBala, escalaBala);
            spriteBala.setOrigin(texBala.getSize().x / 2.0f, texBala.getSize().y / 2.0f);
        }
        break;

    case TipoArma::Pesado:
        if (esDePlanta) {
            // Ataque de Pomelo con la imagen de Sol
            usaSprite = true;
            spriteBala.setTexture(texSol);
            // Al ser un ataque pesado, lo hacemos más grande (50 píxeles)
            float escalaSol = 50.0f / texSol.getSize().x;
            spriteBala.setScale(escalaSol, escalaSol);
            spriteBala.setOrigin(texSol.getSize().x / 2.0f, texSol.getSize().y / 2.0f);
        }
        else {
            // Balón de Rugby con tu imagen (All-Star)
            usaSprite = true;
            spriteBala.setTexture(texRugby);
            float escala = 60.0f / texRugby.getSize().x; // Lo hacemos de 60 píxeles para que se vea genial
            spriteBala.setScale(escala, escala);
            spriteBala.setOrigin(texRugby.getSize().x / 2.0f, texRugby.getSize().y / 2.0f);
        }
        break;

    case TipoArma::Rafaga:
        forma.setPointCount(4);
        if (esDePlanta) {
            // Ataque de Dronajo (Ajo) con imagen
            usaSprite = true;
            spriteBala.setTexture(texDronajo);
            // Tamaño de unos 25 píxeles, pequeño para que parezca una ráfaga rápida
            float escalaDronajo = 25.0f / texDronajo.getSize().x;
            spriteBala.setScale(escalaDronajo, escalaDronajo);
            spriteBala.setOrigin(texDronajo.getSize().x / 2.0f, texDronajo.getSize().y / 2.0f);
        }
        else {
            // Ataque del Loro Pirata con imagen
            usaSprite = true;
            spriteBala.setTexture(texLoro);
            // Tamaño de unos 25 píxeles, igual que el Dronajo para mantener el equilibrio
            float escalaLoro = 25.0f / texLoro.getSize().x;
            spriteBala.setScale(escalaLoro, escalaLoro);
            spriteBala.setOrigin(texLoro.getSize().x / 2.0f, texLoro.getSize().y / 2.0f);
        }
        break;
    }

    if (!usaSprite) {
        sf::FloatRect bounds = forma.getLocalBounds();
        forma.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        float angle = atan2(velY, velX) * 180.0f / 3.14159f;
        forma.setRotation(angle);
    }
    else {
        float angle = atan2(velY, velX) * 180.0f / 3.14159f;
        spriteBala.setRotation(angle);
    }
}

void ProyectilArena::actualizar() {
    posicion.x += velocidad.x;
    posicion.y += velocidad.y;
    // Si es el balón de rugby, le damos un giro chulísimo en el aire
    if (usaSprite && tipo != TipoArma::Franco && tipo != TipoArma::Rafaga) {
        spriteBala.rotate(8.0f);
    }
}

void ProyectilArena::dibujar(sf::RenderWindow& window) {

    if (usaSprite) {
        spriteBala.setPosition(posicion.x, posicion.y);
        window.draw(spriteBala);
    }
    else {
        forma.setPosition(posicion.x, posicion.y);
        window.draw(forma);
    }
}
