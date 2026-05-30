#pragma once
#include <SFML/Audio.hpp>
#include <iostream>
#include <map>
#include <string>

class GestorAudio {
private:
    static sf::Music* musicaMenu;
    static sf::Music* musicaTablero;
    static sf::Music* musicaArena;
    static sf::SoundBuffer bufferHover;
    static sf::Sound sonidoHover;

//Mapas para los 12 personajes 
    static std::map<std::string, sf::SoundBuffer> buffersAtaque;
    static std::map<std::string, sf::Sound> sonidosAtaque;

   
    static sf::SoundBuffer bufCurar, bufRevivir, bufTele, bufPoder, bufVictoria, bufDerrota;
    static sf::Sound sndCurar, sndRevivir, sndTele, sndPoder, sndVictoria, sndDerrota;

    // Variables para el control de volumen global (0 a 100)
    static float volumenMusicaGlobal;
    static float volumenSFXGlobal;

public:
    static void inicializar();
    static void reproducirMenu();
    static void reproducirTablero();
    static void reproducirArena();
    static void finDeArena();
    static void reproducirHover();
    static void reproducirAtaque(std::string nombrePersonaje);
    static void reproducirCurar();
    static void reproducirRevivir();
    static void reproducirTeletransporte();
    static void reproducirPoder();
    static void reproducirVictoria(bool modoUnJugador, int bandoGanador);

    // Métodos para cambiar y obtener el volumen
    static void setVolumenMusica(float v);
    static void setVolumenSFX(float v);
    static float getVolumenMusica();
    static float getVolumenSFX();

    static void limpiar();
};