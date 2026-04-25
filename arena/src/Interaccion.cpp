// Interaccion.cpp
#include "Interaccion.h"

// Interaccion.cpp
#include "Interaccion.h"

bool Interaccion::colisionCajas(Vector2D pos1, float ancho1, float alto1, Vector2D pos2, float ancho2, float alto2) {
    if (pos1.x < pos2.x + ancho2 && pos1.x + ancho1 > pos2.x &&
        pos1.y < pos2.y + alto2 && pos1.y + alto1 > pos2.y) {
        return true;
    }
    return false;
}

bool Interaccion::colision(PersonajeBaseArena* p, ObstaculoArena* o) {
    // Calculamos el origen 
    Vector2D posP(p->getPosicion().x - 20.0f, p->getPosicion().y - 20.0f);
    return colisionCajas(posP, 40.0f, 40.0f, o->getPosicion(), 60.0f, 60.0f);
}

bool Interaccion::colision(ProyectilArena* proj, ObstaculoArena* o) {
    return colisionCajas(proj->getPosicion(), 10.0f, 10.0f, o->getPosicion(), 60.0f, 60.0f);
}