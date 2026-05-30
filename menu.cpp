// Archivo Menu.cpp adaptado con comentarios y mensajes en español.
// Se mantienen los nombres de clases, funciones y variables porque deben coincidir con Menu.h.

#include "Menu.h"
#include <iostream>
#include "GestorAudio.h" // Gestor de sonido

Menu::Menu(float windowWidth, float windowHeight) {
    // 1. Cargar las 4 imágenes
    if (!texNormal.loadFromFile("menu.png") ||
        !tex1J.loadFromFile("menu.1j.png") ||
        !tex2J.loadFromFile("menu.2j.png") ||
        !texRanking.loadFromFile("menu.ranking.png")) {
        std::cout << "Error al cargar alguna imagen del menú." << std::endl;
    }

    // Le ponemos la imagen normal por defecto al arrancar
    spriteFondo.setTexture(texNormal);

    if (!fuente.loadFromFile("COOPBL.ttf")) { /* Manejar error */ }

    if (!texVictoriaPlantas.loadFromFile("fondo_victoria_plantas.png")) {}
    if (!texVictoriaZombis.loadFromFile("fondo_victoria_zombis.png")) {}
    if (!texEmpate.loadFromFile("fondo_empate.png")) {}

    // 2. Definir las zonas clicables de cada botón (posición X, posición Y, ancho y alto)
    hitbox1Jugador = sf::FloatRect(689, 468, 503, 125);    // Zona del botón "1 JUGADOR"
    hitbox2Jugadores = sf::FloatRect(695, 622, 491, 127);  // Zona del botón "2 JUGADORES"
    hitboxRanking = sf::FloatRect(695, 772, 491, 142);     // Zona del botón "RANKING"
}

void Menu::dibujar(sf::RenderWindow& window) {
    sf::Vector2i posRatonInt = sf::Mouse::getPosition(window);
    sf::Vector2f posRaton((float)posRatonInt.x, (float)posRatonInt.y);

    // Memoria para no repetir el sonido continuamente mientras el ratón está sobre el mismo botón
    static int ultimoHover = 0;
    int hoverActual = 0;

    // 2. Cambiar la imagen del menú y registrar en qué botón está el ratón
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

    // Si entramos en un botón nuevo, reproducimos el efecto de sonido
    if (hoverActual != 0 && hoverActual != ultimoHover) {
        GestorAudio::reproducirHover();
    }

    // Guardamos el botón actual para comparar en el siguiente frame (fotograma)
    ultimoHover = hoverActual;

    // 3. Dibujar el fondo con la imagen correcta
    window.draw(spriteFondo);
}

EstadoMenu Menu::manejarEventos(sf::RenderWindow& window, sf::Event& event) {
    // Detectar si se hizo clic con el botón izquierdo del ratón
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {

            // Obtener las coordenadas del ratón en la ventana
            sf::Vector2f posRaton((float)event.mouseButton.x, (float)event.mouseButton.y);

            // Comprobar si el clic cayó dentro de alguna zona clicable
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

    // 1. Elegimos el fondo correcto
    if (idGanadorGuerra == 1) {
        spriteFondoRanking.setTexture(texVictoriaPlantas);
    }
    else if (idGanadorGuerra == 2) {
        spriteFondoRanking.setTexture(texVictoriaZombis);
    }
    else {
        spriteFondoRanking.setTexture(texEmpate);
    }

    // 2. Ajustamos la escala
    float escalaX = (float)window.getSize().x / spriteFondoRanking.getTexture()->getSize().x;
    float escalaY = (float)window.getSize().y / spriteFondoRanking.getTexture()->getSize().y;
    spriteFondoRanking.setScale(escalaX, escalaY);

    // 3. Dibujamos el fondo dinámico
    window.draw(spriteFondoRanking);

    // Texto principal de la pantalla de ranking
    sf::Text textoTitulo("RANKING", fuente, 90); // Tamaño 90 para que destaque

    // Color verde estilo PvZ
    textoTitulo.setFillColor(sf::Color(50, 220, 50));

    // Borde negro para que el texto resalte sobre cualquier fondo
    textoTitulo.setOutlineColor(sf::Color::Black);
    textoTitulo.setOutlineThickness(6);

    // Centrar el texto horizontalmente y ponerlo arriba
    sf::FloatRect limitesTitulo = textoTitulo.getLocalBounds();
    textoTitulo.setPosition(
        (window.getSize().x - limitesTitulo.width) / 2.0f, // Centrado horizontalmente
        40.0f // Separación desde el borde superior
    );

    // Dibujamos el título
    window.draw(textoTitulo);
    // 5. Obtener las victorias guardadas
    int victoriasPlantas = 0;
    int victoriasZombis = 0;

    // Buscar en la lista cuántas victorias tiene cada bando
    for (const auto& par : topRanking) {
        if (par.first == "Plantas") victoriasPlantas = par.second;
        else if (par.first == "Zombis") victoriasZombis = par.second;
    }

    // 6. Preparar los textos
    // Texto de Plantas
    sf::Text textoPlantas("Plantas: " + std::to_string(victoriasPlantas), fuente, 50);
    textoPlantas.setFillColor(sf::Color(100, 255, 100));
    textoPlantas.setOutlineColor(sf::Color::Black);
    textoPlantas.setOutlineThickness(4);

    // Texto de Zombis
    sf::Text textoZombis("Zombis: " + std::to_string(victoriasZombis), fuente, 50);
    textoZombis.setFillColor(sf::Color(200, 100, 255));
    textoZombis.setOutlineColor(sf::Color::Black);
    textoZombis.setOutlineThickness(4);

    // 7. Colocar los textos según el fondo elegido
    // Ajusta estos valores X e Y si quieres mover los textos en pantalla

    if (idGanadorGuerra == 1) {
        // Fondo de victoria de Plantas
        // Colocamos ambos textos en la zona visible del fondo
        textoPlantas.setPosition(1100.0f, 300.0f);
        // Texto de Zombis debajo del de Plantas
        textoZombis.setPosition(1100.0f, 380.0f);

    }
    else if (idGanadorGuerra == 2) {
        // Fondo de victoria de Zombis
        // Texto de Plantas debajo del de Zombis
        textoPlantas.setPosition(1100.0f, 380.0f);
        // Texto de Zombis arriba
        textoZombis.setPosition(1100.0f, 300.0f);

    }
    else {
        // Fondo de empate
        // Colocamos los textos centrados y separados
        textoPlantas.setPosition(850.0f, 240.0f);
        textoZombis.setPosition(850.0f, 360.0f);
    }

    // 8. Dibujar los textos
    window.draw(textoPlantas);
    window.draw(textoZombis);
}