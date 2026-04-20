#pragma once
#include <SFML/Graphics.hpp>
#include <cmath> // Para calcular ángulos (atan2)

struct CajaColisionArena {
    float x, y;
    float ancho, alto;
};

class PersonajeBaseArena {
protected:
    float x, y;
    float vidaActual;
    float vidaMaxima;
    float velocidad;
    float danio;
    bool esVolador;
    bool esCC; // ¿Es un atacante Cuerpo a Cuerpo?
    float anguloMirada; // En grados (0 = derecha, 90 = abajo, 180 = izquierda, -90 = arriba)

public:
    // Hemos añadido "bool cuerpoACuerpo" al constructor
    PersonajeBaseArena(float vMax, float vel, float dmg, bool volador, bool cuerpoACuerpo)
        : vidaMaxima(vMax), vidaActual(vMax), velocidad(vel), danio(dmg),
        esVolador(volador), esCC(cuerpoACuerpo), x(0), y(0), anguloMirada(0) {
    }

    virtual ~PersonajeBaseArena() {}

    float getVida() const { return vidaActual; }
    float getVidaMaxima() const { return vidaMaxima; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getVelocidad() const { return velocidad; }
    float getDanio() const { return danio; }
    bool getEsVolador() const { return esVolador; }
    bool getEsCC() const { return esCC; }
    float getAngulo() const { return anguloMirada; }

    CajaColisionArena getHitbox() const {
        // Centramos la hitbox respecto a la posición (x,y)
        return { x - 20.0f, y - 20.0f, 40.0f, 40.0f };
    }

    void setPosicion(float newX, float newY) { x = newX; y = newY; }

    void recibirDanio(float cantidad) {
        vidaActual -= cantidad;
        if (vidaActual < 0) vidaActual = 0;
    }

    virtual void dibujar(sf::RenderWindow& window) = 0;

    virtual void mover(float dx, float dy) {
        x += dx * velocidad;
        y += dy * velocidad;

        // Calcular la rotación basada en el vector de movimiento
        if (dx != 0 || dy != 0) {
            // atan2 devuelve el ángulo en radianes, lo pasamos a grados
            anguloMirada = std::atan2(dy, dx) * 180.0f / 3.14159265f;
        }
    }
};