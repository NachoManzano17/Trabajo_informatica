#pragma once
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "personaje.h"
#include "PersonajeBaseArena.h"
#include "ObstaculoArena.h"
#include "ProyectilArena.h"
#include "Interaccion.h"
#include "Dificultad.h"



// Estructura para el humo/tierra del ataque
struct ParticulaTierra {
    Vector2D posicion;
    float vidaActual;
    float vidaMaxima;
    float radio;
    sf::Color color;
};


class GestorArena {
private:
    PersonajeBaseArena* planta;
    PersonajeBaseArena* zombie;

    std::vector<ObstaculoArena> obstaculos;
    std::vector<ProyectilArena> proyectiles;
    std::vector<ParticulaTierra> particulas;
    std::string nombrePlanta;
    std::string nombreZombie;


    float limiteAncho, limiteAlto;
    bool combateTerminado;
    int ganador;

    int cooldownPlanta;
    int cooldownZombie;
    bool zombieControladoPorIA;
    Dificultad dificultadIA;
    sf::Clock relojDecisionIA;
    Vector2D direccionIA;
    bool lineaDeTiroLibre(Vector2D origen, Vector2D destino);
    bool hayPeligroDeProyectilPlanta(Vector2D posicionZombie);
    void controlarZombieIA();

    void resolverColisionesMovimiento(PersonajeBaseArena* p, float oldX, float oldY);
    void dibujarBarrasDeVida(sf::RenderWindow& window);

    // fondo y centrado
    sf::Texture texturaFondo;
    sf::Sprite spriteFondo;
    sf::Texture texturaNeumatico;
    sf::Sprite spriteNeumatico;
    sf::Texture texVidaPlanta;
    sf::Texture texVidaZombi;
    sf::Texture texRosa;
    sf::Texture texMano;
    sf::Sprite spriteRosa;
    sf::Sprite spriteMano;
    float offsetX, offsetY; // Espacio vacío para centrar el juego
    float pantallaAncho, pantallaAlto; // Tamaño total del monitor
    bool estaFueraDeLimites(Vector2D pos);
    bool chocaConAlgo(PersonajeBaseArena* p);
    //Dificultad dificultadIA;

public:
    GestorArena(PersonajeBaseArena* pPlanta, PersonajeBaseArena* pZombie, float ancho, float alto);
    ~GestorArena();
   
    void inicializarArena(int numObstaculos);
    void setZombieControladoPorIA(bool activo);
    //void setDificultadIA(Dificultad dificultad);
    void procesarEntrada();
    void actualizarLider();
    void dibujarEscena(sf::RenderWindow& window);
    void setNombres(std::string p, std::string z) { nombrePlanta = p; nombreZombie = z; }
    void setDificultad(Dificultad dificultad);

    void setDificultadIA(Dificultad dificultad);

    bool isTerminado() { return combateTerminado; }
    int getGanador() { return ganador; }
};

int resolverCombateEnArena(personaje* atacante, personaje* defensor, sf::RenderWindow& window,
    int bandoFavorecido = 0, bool zombieControladoPorIA = false, Dificultad dificultadArenaIA = Dificultad::Normal);
