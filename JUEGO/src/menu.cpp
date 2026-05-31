#include "Menu.h"
#include <iostream>
#include "GestorAudio.h" 

Menu::Menu(float windowWidth, float windowHeight) {
    if (!texNormal.loadFromFile("menu.png") ||
        !tex1J.loadFromFile("menu.1j.png") ||
        !tex2J.loadFromFile("menu.2j.png") ||
        !texRanking.loadFromFile("menu.ranking.png")) {
        std::cout << "Error al cargar alguna imagen del menú." << std::endl;
    }

    spriteFondo.setTexture(texNormal);

    if (!fuente.loadFromFile("COOPBL.ttf")) {  }

    if (!texVictoriaPlantas.loadFromFile("fondo_victoria_plantas.png")) {}
    if (!texVictoriaZombis.loadFromFile("fondo_victoria_zombis.png")) {}
    if (!texEmpate.loadFromFile("fondo_empate.png")) {}

    hitbox1Jugador = sf::FloatRect(689, 468, 503, 125);    
    hitbox2Jugadores = sf::FloatRect(695, 622, 491, 127); 
    hitboxRanking = sf::FloatRect(695, 772, 491, 142);    
}

void Menu::dibujar(sf::RenderWindow& window) {
    sf::Vector2i posRatonInt = sf::Mouse::getPosition(window);
    sf::Vector2f posRaton((float)posRatonInt.x, (float)posRatonInt.y);

    static int ultimoHover = 0;
    int hoverActual = 0;

    if (hitbox1Jugador.contains(posRaton)) {
        spriteFondo.setTexture(tex1J);
        hoverActual = 1;
    }
    else if (hitbox2Jugadores.contains(posRaton)) {
        spriteFondo.setTexture(tex2J);
        hoverActual = 2;
    }
    else if (hitboxRanking.contains(posRaton)) {
        spriteFondo.setTexture(texRanking);
        hoverActual = 3;
    }
    else {
        spriteFondo.setTexture(texNormal);
        hoverActual = 0;
    }

    if (hoverActual != 0 && hoverActual != ultimoHover) {
        GestorAudio::reproducirHover();
    }

    ultimoHover = hoverActual;

    window.draw(spriteFondo);
}

EstadoMenu Menu::manejarEventos(sf::RenderWindow& window, sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {

            sf::Vector2f posRaton((float)event.mouseButton.x, (float)event.mouseButton.y);

            if (hitbox1Jugador.contains(posRaton)) {
                std::cout << "¡Clic en 1 JUGADOR!" << std::endl;
                return EstadoMenu::UN_JUGADOR;
            }
            else if (hitbox2Jugadores.contains(posRaton)) {
                std::cout << "¡Clic en 2 JUGADORES!" << std::endl;
                return EstadoMenu::DOS_JUGADORES;
            }
            else if (hitboxRanking.contains(posRaton)) {
                std::cout << "¡Clic en RANKING!" << std::endl;
                return EstadoMenu::RANKING;
            }
        }
    }
    return EstadoMenu::MENU;
}

void Menu::dibujarPantallaRanking(sf::RenderWindow& window, const std::vector<std::pair<std::string, int>>& topRanking, int idGanadorGuerra) {

    if (idGanadorGuerra == 1) {
        spriteFondoRanking.setTexture(texVictoriaPlantas);
    }
    else if (idGanadorGuerra == 2) {
        spriteFondoRanking.setTexture(texVictoriaZombis);
    }
    else {
        spriteFondoRanking.setTexture(texEmpate);
    }

    float escalaX = (float)window.getSize().x / spriteFondoRanking.getTexture()->getSize().x;
    float escalaY = (float)window.getSize().y / spriteFondoRanking.getTexture()->getSize().y;
    spriteFondoRanking.setScale(escalaX, escalaY);

    window.draw(spriteFondoRanking);

    sf::Text textoTitulo("RANKING", fuente, 90); 

    textoTitulo.setFillColor(sf::Color(50, 220, 50));

    textoTitulo.setOutlineColor(sf::Color::Black);
    textoTitulo.setOutlineThickness(6);

    sf::FloatRect limitesTitulo = textoTitulo.getLocalBounds();
    textoTitulo.setPosition(
        (window.getSize().x - limitesTitulo.width) / 2.0f, 
        40.0f 
    );

    window.draw(textoTitulo);
    int victoriasPlantas = 0;
    int victoriasZombis = 0;

    for (const auto& par : topRanking) {
        if (par.first == "Plantas") victoriasPlantas = par.second;
        else if (par.first == "Zombis") victoriasZombis = par.second;
    }

    sf::Text textoPlantas("Plantas: " + std::to_string(victoriasPlantas), fuente, 50);
    textoPlantas.setFillColor(sf::Color(100, 255, 100));
    textoPlantas.setOutlineColor(sf::Color::Black);
    textoPlantas.setOutlineThickness(4);

    sf::Text textoZombis("Zombis: " + std::to_string(victoriasZombis), fuente, 50);
    textoZombis.setFillColor(sf::Color(200, 100, 255));
    textoZombis.setOutlineColor(sf::Color::Black);
    textoZombis.setOutlineThickness(4);


    if (idGanadorGuerra == 1) {
        textoPlantas.setPosition(1100.0f, 300.0f);
        textoZombis.setPosition(1100.0f, 380.0f);

    }
    else if (idGanadorGuerra == 2) {
        textoPlantas.setPosition(1100.0f, 380.0f);
        textoZombis.setPosition(1100.0f, 300.0f);

    }
    else {
        textoPlantas.setPosition(850.0f, 240.0f);
        textoZombis.setPosition(850.0f, 360.0f);
    }

    window.draw(textoPlantas);
    window.draw(textoZombis);
}