#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>


enum class EstadoMenu {
    MENU,
    UN_JUGADOR,
    DOS_JUGADORES,
    RANKING,
    SALIR
};

class Menu {
    sf::Texture texNormal;
    sf::Texture tex1J;
    sf::Texture tex2J;
    sf::Texture texRanking;
    sf::Sprite spriteFondo;
    sf::Font fuente;
    sf::Texture texVictoriaPlantas;
    sf::Texture texVictoriaZombis;
    sf::Texture texEmpate;
    sf::Sprite spriteFondoRanking;

    sf::FloatRect hitbox1Jugador;
    sf::FloatRect hitbox2Jugadores;
    sf::FloatRect hitboxRanking;

public:
    Menu(float windowWidth, float windowHeight);

    void dibujar(sf::RenderWindow& window);
    void dibujarPantallaRanking(sf::RenderWindow& window, const std::vector<std::pair<std::string, int>>& topRanking);

    EstadoMenu manejarEventos(sf::RenderWindow& window, sf::Event& event);
    void dibujarPantallaRanking(sf::RenderWindow& window, const std::vector<std::pair<std::string, int>>& topRanking, int idGanadorGuerra);
};