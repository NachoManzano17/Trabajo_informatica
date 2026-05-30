#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>


// 1. EL ENUM DEBE IR ARRIBA DEL TODO, ANTES DE LA CLASE
enum class EstadoMenu {
    MENU,
    UN_JUGADOR,
    DOS_JUGADORES,
    RANKING,
    SALIR
};

// 2. AHORA SÍ, LA CLASE MENU
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

    // Cajas invisibles (Hitboxes) para detectar el clic
    sf::FloatRect hitbox1Jugador;
    sf::FloatRect hitbox2Jugadores;
    sf::FloatRect hitboxRanking;

public:
    Menu(float windowWidth, float windowHeight);

    // Dibuja el menú en la ventana
    void dibujar(sf::RenderWindow& window);
    void dibujarPantallaRanking(sf::RenderWindow& window, const std::vector<std::pair<std::string, int>>& topRanking);

    // Gestiona los clics del ratón y devuelve el nuevo estado
    EstadoMenu manejarEventos(sf::RenderWindow& window, sf::Event& event);
    void dibujarPantallaRanking(sf::RenderWindow& window, const std::vector<std::pair<std::string, int>>& topRanking, int idGanadorGuerra);
};