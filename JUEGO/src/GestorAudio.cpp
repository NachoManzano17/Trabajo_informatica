#include "GestorAudio.h"

sf::Music* GestorAudio::musicaMenu = nullptr;
sf::Music* GestorAudio::musicaTablero = nullptr;
sf::Music* GestorAudio::musicaArena = nullptr;
sf::SoundBuffer GestorAudio::bufferHover;
sf::Sound GestorAudio::sonidoHover;

std::map<std::string, sf::SoundBuffer> GestorAudio::buffersAtaque;
std::map<std::string, sf::Sound> GestorAudio::sonidosAtaque;

sf::SoundBuffer GestorAudio::bufCurar, GestorAudio::bufRevivir, GestorAudio::bufTele, GestorAudio::bufPoder, GestorAudio::bufVictoria, GestorAudio::bufDerrota;
sf::Sound GestorAudio::sndCurar, GestorAudio::sndRevivir, GestorAudio::sndTele, GestorAudio::sndPoder, GestorAudio::sndVictoria, GestorAudio::sndDerrota;

void GestorAudio::inicializar() {
    musicaMenu = new sf::Music();
    if (musicaMenu->openFromFile("menu_pvz.wav")) { musicaMenu->setLoop(true); musicaMenu->setVolume(40.f); }
    musicaTablero = new sf::Music();
    if (musicaTablero->openFromFile("galaxy_tablero.wav")) { musicaTablero->setLoop(true); musicaTablero->setVolume(30.f); }
    musicaArena = new sf::Music();
    if (musicaArena->openFromFile("mortalkombat_arena.wav")) { musicaArena->setLoop(true); musicaArena->setVolume(50.f); }

    if (bufferHover.loadFromFile("selector_menu.wav")) { sonidoHover.setBuffer(bufferHover); sonidoHover.setVolume(70.f); }

    auto cargarSnd = [](sf::SoundBuffer& b, sf::Sound& s, std::string a) { if (b.loadFromFile(a)) s.setBuffer(b); };

    cargarSnd(bufCurar, sndCurar, "curar.wav");
    cargarSnd(bufRevivir, sndRevivir, "revivir.wav");
    cargarSnd(bufTele, sndTele, "teletransporte.wav");
    cargarSnd(bufPoder, sndPoder, "punto_poder.wav");
    cargarSnd(bufVictoria, sndVictoria, "victoria.wav");
    cargarSnd(bufDerrota, sndDerrota, "derrota.wav");

    // Le ponemos volumen 100 y avisos de error 
    auto cargarAtaque = [&](std::string nombre, std::string archivo) {
        if (buffersAtaque[nombre].loadFromFile(archivo)) {
            sonidosAtaque[nombre].setBuffer(buffersAtaque[nombre]);
            sonidosAtaque[nombre].setVolume(100.f); 
        }
        else {
            std::cout << "¡ERROR AUDIO!: No encuentro el archivo " << archivo << "\n";
        }
        };

    cargarAtaque("Lanzaguisantes", "lanzaguisantes.wav");
    cargarAtaque("Zombidito", "zombidito.wav");
    cargarAtaque("Cactus", "cactus.wav");
    cargarAtaque("Soldado", "soldado.wav");
    cargarAtaque("PlantaCarnivora", "planta_carnivora.wav");
    cargarAtaque("SuperCerebroz", "super.wav");
    cargarAtaque("Dronajo", "dronajo.wav");
    cargarAtaque("LoroPirata", "loro.wav");
    cargarAtaque("Pomelo", "pomelo.wav");
    cargarAtaque("allstar", "all_star.wav");
    cargarAtaque("Dave", "dave.wav");
    cargarAtaque("DrZomboss", "zomboss.wav");
}

void GestorAudio::reproducirAtaque(std::string nombrePersonaje) {
    // Comprobamos si el personaje existe en nuestra lista
    if (sonidosAtaque.count(nombrePersonaje) > 0) {
        sonidosAtaque[nombrePersonaje].play();
    }
    else {
        // Si la arena manda un nombre raro, nos avisa en la consola
        std::cout << "Aviso: No hay sonido para el personaje -> " << nombrePersonaje << "\n";
    }
}

void GestorAudio::reproducirCurar() { sndCurar.play(); }
void GestorAudio::reproducirRevivir() { sndRevivir.play(); }
void GestorAudio::reproducirTeletransporte() { sndTele.play(); }
void GestorAudio::reproducirPoder() { sndPoder.play(); }

void GestorAudio::reproducirVictoria(bool modoUnJugador, int bandoGanador) {
    if (musicaTablero->getStatus() == sf::SoundSource::Playing) musicaTablero->stop();

    // Si es 2 Jugadores, o si es 1 Jugador y ganan las plantas, suena Victoria
    if (!modoUnJugador || bandoGanador == 1) sndVictoria.play();
    // Si es 1 Jugador y ganan los zombis (bando 2), suena Derrota
    else sndDerrota.play();
}

void GestorAudio::reproducirHover() { sonidoHover.play(); }
void GestorAudio::reproducirMenu() {
    // Detenemos la música de los otros modos de juego
    if (musicaTablero->getStatus() == sf::SoundSource::Playing) musicaTablero->stop();
    if (musicaArena->getStatus() == sf::SoundSource::Playing) musicaArena->stop();

    // Detener los audios de fin de partida 
    sndVictoria.stop();
    sndDerrota.stop();

    // Reproducimos la música del menú si no está sonando ya
    if (musicaMenu->getStatus() != sf::SoundSource::Playing) musicaMenu->play();
}

void GestorAudio::reproducirTablero() { if (musicaMenu->getStatus() == sf::SoundSource::Playing) musicaMenu->stop(); if (musicaArena->getStatus() == sf::SoundSource::Playing) musicaArena->stop(); if (musicaTablero->getStatus() != sf::SoundSource::Playing) musicaTablero->play(); }
void GestorAudio::reproducirArena() { if (musicaTablero->getStatus() == sf::SoundSource::Playing) musicaTablero->pause(); if (musicaMenu->getStatus() == sf::SoundSource::Playing) musicaMenu->stop(); musicaArena->play(); }
void GestorAudio::finDeArena() { musicaArena->stop(); if (musicaTablero->getStatus() == sf::SoundSource::Paused) musicaTablero->play(); }
void GestorAudio::limpiar() { delete musicaMenu; delete musicaTablero; delete musicaArena; }

// Inicializamos a tope
float GestorAudio::volumenMusicaGlobal = 100.f;
float GestorAudio::volumenSFXGlobal = 100.f;

void GestorAudio::setVolumenMusica(float v) {
    volumenMusicaGlobal = v;
    if (volumenMusicaGlobal < 0.f) volumenMusicaGlobal = 0.f;
    if (volumenMusicaGlobal > 100.f) volumenMusicaGlobal = 100.f;

    // Aplicamos como porcentaje a los volúmenes base originales
    float mult = volumenMusicaGlobal / 100.f;
    if (musicaMenu) musicaMenu->setVolume(40.f * mult);
    if (musicaTablero) musicaTablero->setVolume(30.f * mult);
    if (musicaArena) musicaArena->setVolume(50.f * mult);
}

void GestorAudio::setVolumenSFX(float v) {
    volumenSFXGlobal = v;
    if (volumenSFXGlobal < 0.f) volumenSFXGlobal = 0.f;
    if (volumenSFXGlobal > 100.f) volumenSFXGlobal = 100.f;

    sonidoHover.setVolume(volumenSFXGlobal);
    sndCurar.setVolume(volumenSFXGlobal);
    sndRevivir.setVolume(volumenSFXGlobal);
    sndTele.setVolume(volumenSFXGlobal);
    sndPoder.setVolume(volumenSFXGlobal);
    sndVictoria.setVolume(volumenSFXGlobal);
    sndDerrota.setVolume(volumenSFXGlobal);

    // Actualizamos todos los ataques del mapa
    for (auto& par : sonidosAtaque) {
        par.second.setVolume(volumenSFXGlobal);
    }
}

float GestorAudio::getVolumenMusica() { return volumenMusicaGlobal; }
float GestorAudio::getVolumenSFX() { return volumenSFXGlobal; }