#include "Menu.h"
#include <iostream>
Menu::Menu(float windowWidth, float windowHeight) {
    // 1. Cargar la imagen de fondo que me pasaste (asegúrate de que la ruta sea correcta)
   // Cargar las 4 imágenes
    if (!texNormal.loadFromFile("menu.png") ||
        !tex1J.loadFromFile("menu.1j.png") ||
        !tex2J.loadFromFile("menu.2j.png") ||
        !texRanking.loadFromFile("menu.ranking.png")) {
        std::cout << "Error al cargar alguna imagen del menú." << std::endl;
    }

    // Le ponemos la imagen normal por defecto al arrancar
    spriteFondo.setTexture(texNormal); {
        std::cerr << "Error al cargar la imagen del menú." << std::endl;
    }
    spriteFondo.setTexture(texNormal);

    // Ajustar la imagen al tamaño de la ventana (opcional, por si la imagen es muy grande)
    // spriteFondo.setScale(windowWidth / spriteFondo.getLocalBounds().width, 
    //                      windowHeight / spriteFondo.getLocalBounds().height);
    if (!fuente.loadFromFile("COOPBL.ttf")) { /* Manejar error */ }
    // 2. Definir las Hitboxes (Rectángulos: Posición X, Posición Y, Ancho, Alto)
    // ¡IMPORTANTE! Tendrás que jugar con estos números para que encajen perfecto en tu letrero
    hitbox1Jugador = sf::FloatRect(689, 468, 503, 125);    // Zona del boton "1 JUGADOR"
    hitbox2Jugadores = sf::FloatRect(695, 622, 491, 127);  // Zona del boton "2 JUGADORES"
    hitboxRanking = sf::FloatRect(695, 772, 491, 142);     // Valores de ejemplo para "RANKING"
}

void Menu::dibujar(sf::RenderWindow& window) {
    sf::Vector2i posRatonInt = sf::Mouse::getPosition(window);
    sf::Vector2f posRaton((float)posRatonInt.x, (float)posRatonInt.y);

    // 2. Cambiar la "diapositiva" según dónde esté el ratón
    if (hitbox1Jugador.contains(posRaton)) {
        spriteFondo.setTexture(tex1J);
    }
    else if (hitbox2Jugadores.contains(posRaton)) {
        spriteFondo.setTexture(tex2J);
    }
    else if (hitboxRanking.contains(posRaton)) {
        spriteFondo.setTexture(texRanking);
    }
    else {
        // Si no está encima de ningún botón, mostramos el fondo normal
        spriteFondo.setTexture(texNormal);
    }

    // 3. Dibujar el fondo con la textura correcta
    window.draw(spriteFondo);
   
}
EstadoMenu Menu::manejarEventos(sf::RenderWindow& window, sf::Event& event) {
    // Detectar si se hizo clic con el botón izquierdo del ratón
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {

            // Obtener las coordenadas del ratón en la ventana
            sf::Vector2f posRaton(event.mouseButton.x, event.mouseButton.y);

            // Comprobar si el clic cayó dentro de alguna Hitbox
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
    // Si no se hizo clic en nada útil, seguimos en el menú
    return EstadoMenu::MENU;
}

void Menu::dibujarPantallaRanking(sf::RenderWindow& window, const std::vector<std::pair<std::string, int>>& topRanking) {
    window.draw(spriteFondo);

    sf::RectangleShape fondoOscuro(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
    fondoOscuro.setFillColor(sf::Color(0, 0, 0, 210));
    window.draw(fondoOscuro);

    sf::Text titulo("--- SALON DE LA FAMA ---", fuente, 60);
    titulo.setFillColor(sf::Color::Yellow);
    titulo.setPosition((window.getSize().x - titulo.getGlobalBounds().width) / 2.f, 100.f);
    window.draw(titulo);

    for (size_t i = 0; i < topRanking.size() && i < 10; ++i) {
        std::string textoFila = std::to_string(i + 1) + ". " + topRanking[i].first + " - " + std::to_string(topRanking[i].second) + " Victorias";
        sf::Text txtFila(textoFila, fuente, 45);
        txtFila.setPosition((window.getSize().x - txtFila.getGlobalBounds().width) / 2.f, 250.f + (i * 65.f));

        if (topRanking[i].first == "Plantas") txtFila.setFillColor(sf::Color(50, 255, 50));
        else if (topRanking[i].first == "Zombis") txtFila.setFillColor(sf::Color(150, 50, 255));
        else txtFila.setFillColor(sf::Color::White);

        window.draw(txtFila);
    }

    sf::Text txtSalir("Pulsa ESC para volver al menu", fuente, 30);
    txtSalir.setFillColor(sf::Color(200, 200, 200));
    txtSalir.setPosition((window.getSize().x - txtSalir.getGlobalBounds().width) / 2.f, window.getSize().y - 100.f);
    window.draw(txtSalir);
}