#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class GestorRecursos {
    std::map<std::string, sf::Texture> texturas;
    std::map<std::string, sf::Sprite> sprites;

public:
    GestorRecursos(); 

    sf::Sprite& getSprite(const std::string& nombre);
    sf::Texture& getTextura(const std::string& nombre);
    bool tieneSprite(const std::string& nombre); 
};