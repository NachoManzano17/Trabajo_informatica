#include "VistaJuego.h"
#include <cmath>
#include <iostream>
#include "GestorAudio.h"

void VistaJuego::dibujarTableroEInterfaz(sf::RenderWindow& window, GestorRecursos& recursos, tablero& mitablero,
    int estadoActual, bando turnoActual,
    int menuX, int menuY, bool hayPersonajeSeleccionado, int fSel, int cSel,
    int offsetX, int offsetY, int tamanoCasilla,
    sf::Clock& relojOscilacion, const std::vector<std::string>& historialConsola,
    sf::Font& fuente, bool eligiendoDificultadIA, int ganadorJuego, bool juegoPausado, const std::vector<std::string>& cementerioPlantas, const std::vector<std::string>& cementerioZombis,
    bool animandoMovimientoIA, const Movimiento* movimientoIA, float progresoMovimientoIA) {

    // 0. Dibujar el fondo a pantalla completa
    sf::Sprite spriteFondo(recursos.getTextura("fondo"));
    spriteFondo.setScale((float)window.getSize().x / recursos.getTextura("fondo").getSize().x,
        (float)window.getSize().y / recursos.getTextura("fondo").getSize().y);
    window.draw(spriteFondo);

    // 1. Dibujar el fondo cuadriculado con las texturas
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            sf::RectangleShape casilla(sf::Vector2f(tamanoCasilla, tamanoCasilla));
            casilla.setPosition(offsetX + (j * tamanoCasilla), offsetY + (i * tamanoCasilla));

            if ((i + j) % 2 == 0) casilla.setTexture(&recursos.getTextura("cesped"));
            else casilla.setTexture(&recursos.getTextura("tierra"));

            casilla.setFillColor(sf::Color::White);
            casilla.setOutlineThickness(-2.f);
            casilla.setOutlineColor(sf::Color::Black);
            window.draw(casilla);

            bool esPoder = ((i == 4 && j == 0) || (i == 4 && j == 8) || (i == 0 && j == 4) || (i == 8 && j == 4) || (i == 4 && j == 4));
            bool esOscilante = ((i == 2 && j == 2) || (i == 2 && j == 6) || (i == 6 && j == 2) || (i == 6 && j == 6));
            casilla.setOutlineThickness(0.f);

            if (esPoder) {
                sf::Sprite spritePoder(recursos.getTextura("casilla_poder"));
                spritePoder.setOrigin(recursos.getTextura("casilla_poder").getSize().x / 2.0f, recursos.getTextura("casilla_poder").getSize().y / 2.0f);
                spritePoder.setPosition(offsetX + (j * tamanoCasilla) + (tamanoCasilla / 2.0f), offsetY + (i * tamanoCasilla) + (tamanoCasilla / 2.0f));
                float escalaPoder = 88.0f / recursos.getTextura("casilla_poder").getSize().x;
                spritePoder.setScale(escalaPoder, escalaPoder);
                window.draw(spritePoder);
            }
            else if (esOscilante) {
                float tiempo = relojOscilacion.getElapsedTime().asSeconds() * 0.5f;
                float factor = (std::sin(tiempo) + 1.0f) / 2.0f;
                sf::Uint8 alfaPlanta = (sf::Uint8)(factor * 220.0f);
                sf::Uint8 alfaZombi = (sf::Uint8)((1.0f - factor) * 220.0f);

                auto dibujarSpriteCentrado = [&](sf::Texture& tex, sf::Uint8 alfa) {
                    if (alfa > 0) {
                        sf::Sprite sprite(tex);
                        sprite.setOrigin(tex.getSize().x / 2.0f, tex.getSize().y / 2.0f);
                        sprite.setPosition(offsetX + (j * tamanoCasilla) + (tamanoCasilla / 2.0f), offsetY + (i * tamanoCasilla) + (tamanoCasilla / 2.0f));
                        float escala = 75.0f / tex.getSize().x;
                        sprite.setScale(escala, escala);
                        sprite.setColor(sf::Color(255, 255, 255, alfa));
                        window.draw(sprite);
                    }
                    };
                dibujarSpriteCentrado(recursos.getTextura("casilla_planta"), alfaPlanta);
                dibujarSpriteCentrado(recursos.getTextura("casilla_zombi"), alfaZombi);
            }
        }
    }

    // 2. Dibujar resaltos si hay un personaje seleccionado
    if (hayPersonajeSeleccionado) {
        personaje* seleccionado = mitablero.getFicha(fSel, cSel);
        sf::RectangleShape highlight(sf::Vector2f(tamanoCasilla, tamanoCasilla));
        highlight.setPosition(offsetX + (cSel * tamanoCasilla), offsetY + (fSel * tamanoCasilla));
        highlight.setFillColor(sf::Color(255, 255, 0, 100));
        window.draw(highlight);

        if (seleccionado != nullptr) {
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j < 9; ++j) {
                    if (i == fSel && j == cSel) continue;
                    if (seleccionado->esmovimientovalido(fSel, cSel, i, j)) {
                        if (!seleccionado->puedeSaltar() && mitablero.hayObstaculoEnCamino(fSel, cSel, i, j)) continue;
                        personaje* fichaDestino = mitablero.getFicha(i, j);
                        if (fichaDestino == nullptr || fichaDestino->getequipo() != seleccionado->getequipo()) {
                            float radioPunto = 12.0f;
                            sf::CircleShape puntoMovimiento(radioPunto);
                            puntoMovimiento.setPosition(offsetX + (j * tamanoCasilla) + (tamanoCasilla / 2.0f) - radioPunto,
                                offsetY + (i * tamanoCasilla) + (tamanoCasilla / 2.0f) - radioPunto);
                            if (fichaDestino != nullptr) puntoMovimiento.setFillColor(sf::Color(255, 50, 50, 180));
                            else puntoMovimiento.setFillColor(sf::Color(100, 200, 255, 150));
                            window.draw(puntoMovimiento);
                        }
                    }
                }
            }
        }
    }

    // 3. Dibujar las tropas
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            personaje* fichaActual = mitablero.getFicha(i, j);
            if (animandoMovimientoIA && movimientoIA != nullptr && i == movimientoIA->filaOrigen && j == movimientoIA->colOrigen) {
                continue;
            }
            if (fichaActual != nullptr) {
                std::string simbolo = fichaActual->getsimbolo();
                if (recursos.tieneSprite(simbolo)) {
                    sf::Sprite& spritePersonaje = recursos.getSprite(simbolo);
                    spritePersonaje.setPosition(offsetX + (j * tamanoCasilla) + 5.f, offsetY + (i * tamanoCasilla) + 5.f);
                    window.draw(spritePersonaje);
                }
                else {
                    sf::CircleShape circulo(tamanoCasilla / 2.5f);
                    circulo.setPosition(offsetX + (j * tamanoCasilla) + 9, offsetY + (i * tamanoCasilla) + 9);
                    if (fichaActual->getequipo() == bando::planta) circulo.setFillColor(sf::Color(50, 205, 50));
                    else circulo.setFillColor(sf::Color(138, 43, 226));
                    window.draw(circulo);
                }
            }
        }
    }

    if (animandoMovimientoIA && movimientoIA != nullptr) {
        personaje* fichaAnimada = mitablero.getFicha(movimientoIA->filaOrigen, movimientoIA->colOrigen);
        if (fichaAnimada != nullptr) {
            float t = progresoMovimientoIA;
            if (t < 0.0f) t = 0.0f;
            if (t > 1.0f) t = 1.0f;

            float xOrigen = offsetX + (movimientoIA->colOrigen * tamanoCasilla) + 5.0f;
            float yOrigen = offsetY + (movimientoIA->filaOrigen * tamanoCasilla) + 5.0f;
            float xDestino = offsetX + (movimientoIA->colDestino * tamanoCasilla) + 5.0f;
            float yDestino = offsetY + (movimientoIA->filaDestino * tamanoCasilla) + 5.0f;
            float xActual = xOrigen + (xDestino - xOrigen) * t;
            float yActual = yOrigen + (yDestino - yOrigen) * t;

            std::string simbolo = fichaAnimada->getsimbolo();
            if (recursos.tieneSprite(simbolo)) {
                sf::Sprite& spritePersonaje = recursos.getSprite(simbolo);
                spritePersonaje.setPosition(xActual, yActual);
                window.draw(spritePersonaje);
            }
            else {
                sf::CircleShape circulo(tamanoCasilla / 2.5f);
                circulo.setPosition(xActual + 4.0f, yActual + 4.0f);
                if (fichaAnimada->getequipo() == bando::planta) circulo.setFillColor(sf::Color(50, 205, 50));
                else circulo.setFillColor(sf::Color(138, 43, 226));
                window.draw(circulo);
            }
        }
    }

    // 4. Menú de acción del líder (usamos los números equivalentes al enum si están en otro archivo, o el enum directo)
    // Nota: El 1 equivale a MENU_ACCION_LIDER y el 2 a MENU_HECHIZOS según el orden de tu enum.
    if (estadoActual == 1 /* MENU_ACCION_LIDER */) {
        sf::RectangleShape fondoMenuLider(sf::Vector2f(200, 85));
        fondoMenuLider.setPosition(menuX, menuY);
        fondoMenuLider.setFillColor(sf::Color(20, 60, 80, 240));
        fondoMenuLider.setOutlineThickness(2);
        fondoMenuLider.setOutlineColor(sf::Color::Cyan);
        window.draw(fondoMenuLider);

        sf::Text txtOpcionLider1("1. Moverse", fuente, 20);
        sf::Text txtOpcionLider2("2. Hechizos", fuente, 20);
        txtOpcionLider1.setPosition(menuX + 10, menuY + 10);
        txtOpcionLider2.setPosition(menuX + 10, menuY + 45);
        window.draw(txtOpcionLider1);
        window.draw(txtOpcionLider2);
    }

    // 5. Menú de hechizos
    if (estadoActual == 2 /* MENU_HECHIZOS */) {
        sf::RectangleShape fondoMenu(sf::Vector2f(200, 120));
        fondoMenu.setPosition(menuX, menuY);
        fondoMenu.setFillColor(sf::Color(40, 40, 40, 235));
        fondoMenu.setOutlineThickness(2);
        fondoMenu.setOutlineColor(sf::Color::Yellow);
        window.draw(fondoMenu);

        sf::Text txtHechizo1("1. Curar", fuente, 20);
        sf::Text txtHechizo2("2. Resucitar", fuente, 20);
        sf::Text txtHechizo3("3. Teletrans", fuente, 20);
        txtHechizo1.setPosition(menuX + 10, menuY + 10);
        txtHechizo2.setPosition(menuX + 10, menuY + 45);
        txtHechizo3.setPosition(menuX + 10, menuY + 80);
        window.draw(txtHechizo1);
        window.draw(txtHechizo2);
        window.draw(txtHechizo3);
    }
    // Menú de Cementerio
    if (estadoActual == 7 /* ELEGIR_MUERTO_RESUCITAR */) {
        const std::vector<std::string>& miCementerio = (turnoActual == bando::planta) ? cementerioPlantas : cementerioZombis;

        sf::RectangleShape fondoGraveyard(sf::Vector2f(200.f, miCementerio.size() * 30.f));
        fondoGraveyard.setPosition(menuX, menuY);
        fondoGraveyard.setFillColor(sf::Color(60, 20, 60, 240)); // Morado oscuro nigromante
        fondoGraveyard.setOutlineThickness(2);
        fondoGraveyard.setOutlineColor(sf::Color::Magenta);
        window.draw(fondoGraveyard);

        for (size_t i = 0; i < miCementerio.size(); i++) {
            sf::Text txtMuerto(sf::String::fromUtf8(miCementerio[i].begin(), miCementerio[i].end()), fuente, 18);
            txtMuerto.setPosition(menuX + 10, menuY + (i * 30.f) + 5.f);
            window.draw(txtMuerto);
        }
    }

    // 6. Consola in-game
    sf::RectangleShape fondoConsola(sf::Vector2f(810.f, 150.f));
    fondoConsola.setPosition(0.f, 945.f);
    fondoConsola.setFillColor(sf::Color(15, 15, 15, 230));
    fondoConsola.setOutlineThickness(2.f);
    fondoConsola.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(fondoConsola);

    sf::Text textoConsola("", fuente, 18);
    textoConsola.setFillColor(sf::Color(50, 255, 50));
    for (size_t i = 0; i < historialConsola.size(); ++i) {
        textoConsola.setString(sf::String::fromUtf8(historialConsola[i].begin(), historialConsola[i].end()));
        textoConsola.setPosition(15.f, 950.f + (i * 25.f));
        window.draw(textoConsola);
    }

    // 7. Chivato de turnos
    sf::CircleShape chivatoTurno(20.0f);
    chivatoTurno.setPosition(20.0f, 20.0f);
    if (turnoActual == bando::planta) chivatoTurno.setFillColor(sf::Color(50, 205, 50));
    else chivatoTurno.setFillColor(sf::Color(138, 43, 226));
    chivatoTurno.setOutlineThickness(3.0f);
    chivatoTurno.setOutlineColor(sf::Color::White);
    window.draw(chivatoTurno);

    // 8. Pantalla de Dificultad IA
    // 8. Pantalla de Dificultad IA
    if (eligiendoDificultadIA) {
        sf::RectangleShape fondoDif(sf::Vector2f(800.f, 300.f));
        fondoDif.setOrigin(400.f, 150.f); // Centramos el origen
        fondoDif.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
        fondoDif.setFillColor(sf::Color(15, 15, 15, 240));
        fondoDif.setOutlineThickness(3.f);
        fondoDif.setOutlineColor(sf::Color::White);
        window.draw(fondoDif);

        sf::Text txtDif("Elige dificultad de la IA", fuente, 38);
        txtDif.setFillColor(sf::Color::White);
        sf::FloatRect boundsDif = txtDif.getLocalBounds();
        txtDif.setOrigin(boundsDif.left + boundsDif.width / 2.f, boundsDif.top + boundsDif.height / 2.f);
        txtDif.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 80.f);
        window.draw(txtDif);

        // Función rápida para pintar botones de dificultad
        auto dibujarBotonDif = [&](std::string texto, float offsetX, sf::Color colorBorde) {
            sf::RectangleShape btn(sf::Vector2f(200.f, 60.f));
            btn.setOrigin(100.f, 30.f);
            btn.setPosition(window.getSize().x / 2.f + offsetX, window.getSize().y / 2.f + 50.f);
            btn.setFillColor(sf::Color(40, 40, 40));
            btn.setOutlineThickness(2.f);
            btn.setOutlineColor(colorBorde);
            window.draw(btn);

            sf::Text txt(sf::String::fromUtf8(texto.begin(), texto.end()), fuente, 24);
            sf::FloatRect txtBounds = txt.getLocalBounds();
            txt.setOrigin(txtBounds.left + txtBounds.width / 2.f, txtBounds.top + txtBounds.height / 2.f);
            txt.setPosition(window.getSize().x / 2.f + offsetX, window.getSize().y / 2.f + 50.f);
            window.draw(txt);
            };

        // Dibujamos los 3 botones (separados horizontalmente)
        dibujarBotonDif("Fácil", -240.f, sf::Color(50, 255, 50));   // Verde
        dibujarBotonDif("Normal", 0.f, sf::Color(255, 255, 50));    // Amarillo
        dibujarBotonDif("Difícil", 240.f, sf::Color(255, 50, 50));  // Rojo
    }
    // 9. PANTALLA DE VICTORIA (Superpuesta al final de todo)
    if (ganadorJuego != 0) {
        // 1. Fondo semitransparente oscuro
        sf::RectangleShape veloOscuro(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
        veloOscuro.setFillColor(sf::Color(0, 0, 0, 210));
        window.draw(veloOscuro);

        // --- 2. Imagen gigante del ganador ---
        static sf::Texture texVicPlantas;
        static sf::Texture texVicZombis;
        static bool texturasVicCargadas = false;

        if (!texturasVicCargadas) {
            // Se asume que los archivos están en la carpeta principal
            if (!texVicPlantas.loadFromFile("victoria_plantas.png") ||
                !texVicZombis.loadFromFile("victoria_zombis.png")) {
                std::cout << "Error al cargar las texturas de victoria. Asegúrate de que los archivos .png estén junto al .exe.\n";
            }
            texturasVicCargadas = true;
        }

        sf::Sprite spriteVictoria;
        if (ganadorJuego == 1) {
            spriteVictoria.setTexture(texVicPlantas);
        }
        else {
            spriteVictoria.setTexture(texVicZombis);
        }

        // ============================================================
        // --- CORRECCIÓN: Lógica de centrado y ajuste automático ---
        // ============================================================
        sf::FloatRect imgRect = spriteVictoria.getLocalBounds();
        float screenWidth = (float)window.getSize().x;
        float screenHeight = (float)window.getSize().y;

        // Definimos un margen para que la imagen no toque los bordes (que ocupe el 85% de la pantalla)
        float targetWidth = screenWidth * 1.00f;
        float targetHeight = screenHeight * 1.00f;

        // Calculamos los factores de escala necesarios para ancho y alto
        float scaleX = targetWidth / imgRect.width;
        float scaleY = targetHeight / imgRect.height;

        // Elegimos la escala más pequeña de las dos para que la imagen quepa entera sin recortarse
        float finalScale = std::min(scaleX, scaleY);
        spriteVictoria.setScale(finalScale, finalScale);

        // Establecemos el origen en el centro de la imagen local
        spriteVictoria.setOrigin(imgRect.width / 2.0f, imgRect.height / 2.0f);

        // Posicionamos en el centro absoluto de la ventana
        spriteVictoria.setPosition(screenWidth / 2.0f, screenHeight / 2.0f);

        window.draw(spriteVictoria);

        
    }
    // 10. MENÚ DE PAUSA
    if (juegoPausado && ganadorJuego == 0) {
        // Velo oscuro
        sf::RectangleShape velo(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
        velo.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(velo);

        // Título
        sf::Text titulo("JUEGO PAUSADO", fuente, 60);
        titulo.setFillColor(sf::Color::White);
        sf::FloatRect bounds = titulo.getLocalBounds();
        titulo.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        titulo.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 120);
        window.draw(titulo);

        // Función para dibujar botones
        auto dibujarBoton = [&](std::string texto, float offsetY) {
            sf::RectangleShape btn(sf::Vector2f(300, 50));
            btn.setOrigin(150, 25);
            btn.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + offsetY);
            btn.setFillColor(sf::Color(40, 40, 40));
            btn.setOutlineThickness(2);
            btn.setOutlineColor(sf::Color::White);
            window.draw(btn);

            sf::Text txt(sf::String::fromUtf8(texto.begin(), texto.end()), fuente, 24);
            sf::FloatRect txtBounds = txt.getLocalBounds();
            txt.setOrigin(txtBounds.left + txtBounds.width / 2.f, txtBounds.top + txtBounds.height / 2.f);
            txt.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + offsetY);
            window.draw(txt);
            };

        // --- NUEVO: BARRAS DE VOLUMEN ---
        auto dibujarBarraVolumen = [&](std::string texto, float offsetY, float porcentaje) {
            float xBase = window.getSize().x / 2.f - 150.f; // Alineado con la izquierda del botón
            float yBase = window.getSize().y / 2.f + offsetY;

            // Texto descriptivo ("Música" o "Efectos")
            sf::Text txt(sf::String::fromUtf8(texto.begin(), texto.end()), fuente, 20);
            txt.setPosition(xBase, yBase - 25.f);
            window.draw(txt);

            // Barra de fondo (Gris oscuro)
            sf::RectangleShape barraFondo(sf::Vector2f(300.f, 20.f));
            barraFondo.setPosition(xBase, yBase);
            barraFondo.setFillColor(sf::Color(60, 60, 60));
            barraFondo.setOutlineThickness(2);
            barraFondo.setOutlineColor(sf::Color::White);
            window.draw(barraFondo);

            // Barra de relleno (Verde o del color que prefieras)
            sf::RectangleShape barraProgreso(sf::Vector2f(300.f * (porcentaje / 100.f), 20.f));
            barraProgreso.setPosition(xBase, yBase);
            barraProgreso.setFillColor(sf::Color(50, 205, 50)); // Verde Plantas
            window.draw(barraProgreso);
            };

        // Pintamos las barras un poco más abajo que los botones (offset 190 y 260)
        dibujarBarraVolumen("Música", 190.f, GestorAudio::getVolumenMusica());
        dibujarBarraVolumen("Efectos (SFX)", 260.f, GestorAudio::getVolumenSFX());

        // Dibujamos los 3 botones (separados por altura)
        dibujarBoton("Reanudar", -20);
        dibujarBoton("Menú Principal", 50);
        dibujarBoton("Salir del Juego", 120);
    }
}