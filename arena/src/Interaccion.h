// Interaccion.h
#pragma once
#include "Vector2D.h"
#include "PersonajeBaseArena.h"
#include "ObstaculoArena.h"
#include "ProyectilArena.h"

class Interaccion {
public:

    static bool colisionCajas(Vector2D pos1, float ancho1, float alto1, Vector2D pos2, float ancho2, float alto2);
    static bool colision(PersonajeBaseArena* p, ObstaculoArena* o);
    static bool colision(ProyectilArena* proj, ObstaculoArena* o);
};