// PersonajeBaseArena.h
#pragma once
#include <SFML/Graphics.hpp>
#include "Vector2D.h"

class PersonajeBaseArena {
protected:
    Vector2D posicion; 
    float vidaActual;
    float vidaMaxima;
    float velocidad;
    float danio;
    bool esVolador;
    bool esCC;
    float anguloMirada;

public:
    PersonajeBaseArena(float vMax, float vel, float dmg, bool volador, bool cuerpoACuerpo) {
        vidaMaxima = vMax;
        vidaActual = vMax;
        velocidad = vel;
        danio = dmg;
        esVolador = volador;
        esCC = cuerpoACuerpo;
        anguloMirada = 0.0f;
    }

    virtual ~PersonajeBaseArena() {}

   
    float getVida() { return vidaActual; }
    float getVidaMaxima() { return vidaMaxima; }
    Vector2D getPosicion() { return posicion; }
    float getVelocidad() { return velocidad; }
    float getDanio() { return danio; }
    bool getEsVolador() { return esVolador; }
    bool getEsCC() { return esCC; }
    float getAngulo() { return anguloMirada; }

    void setPosicion(float nx, float ny) {
        posicion.x = nx;
        posicion.y = ny;
    }

    void recibirDanio(float cantidad) {
        vidaActual -= cantidad;
        if (vidaActual < 0) vidaActual = 0;
    }

    virtual void dibujar(sf::RenderWindow& window) = 0;

    virtual void mover(float dx, float dy) {
        posicion.x += dx * velocidad;
        posicion.y += dy * velocidad;

        if (dx != 0.0f || dy != 0.0f) {
            Vector2D dir(dx, dy);
            anguloMirada = dir.argumento() * 180.0f / 3.14159f;
        }
    }
};