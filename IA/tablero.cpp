#include "tablero.h"
#include <iostream>
#include "lanzaguisantes.h"
#include "zombidito.h"
#include "cactus.h"
#include "soldado.h"
#include "planta_carnivora.h"
#include "superCerebroz.h"
#include "dronajo.h"
#include "loro_pirata.h"
#include "pomelo.h"
#include "allstar.h"
#include "dave.h"
#include "drzomboss.h"


namespace {
    personaje* clonarPersonajeArchon(const personaje* p) {
        if (p == nullptr) return nullptr;

        int b = static_cast<int>(p->getequipo());
        personaje* copia = nullptr;
        const std::string tipo = p->getsimbolo();

        if (tipo == "Lanzaguisantes") copia = new lanzaguisantes(b);
        else if (tipo == "Zombidito") copia = new zombidito(b);
        else if (tipo == "Cactus") copia = new cactus(b);
        else if (tipo == "Soldado") copia = new soldado(b);
        else if (tipo == "PlantaCarnivora") copia = new plantaCarnivora(b);
        else if (tipo == "SuperCerebroz") copia = new superCerebroz(b);
        else if (tipo == "Dronajo") copia = new dronajo(b);
        else if (tipo == "LoroPirata") copia = new loropirata(b);
        else if (tipo == "Pomelo") copia = new pomelo(b);
        else if (tipo == "allstar") copia = new allstar(b);
        else if (tipo == "Dave") copia = new dave(b);
        else if (tipo == "DrZomboss") copia = new drzomboss(b);

        if (copia != nullptr) copia->fijarVida(p->obtenerVida());
        return copia;
    }
}


tablero::tablero() : turnoglobal(0) {
    estadocasillasoscilantes = 0; // empieza neutral
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            casillas[i][j] = nullptr;
        }
    }
}

tablero::tablero(const tablero& otro) : turnoglobal(otro.turnoglobal), estadocasillasoscilantes(otro.estadocasillasoscilantes) {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            casillas[i][j] = clonarPersonajeArchon(otro.casillas[i][j]);
        }
    }
}

tablero& tablero::operator=(const tablero& otro) {
    if (this == &otro) return *this;

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            delete casillas[i][j];
            casillas[i][j] = nullptr;
        }
    }

    turnoglobal = otro.turnoglobal;
    estadocasillasoscilantes = otro.estadocasillasoscilantes;

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            casillas[i][j] = clonarPersonajeArchon(otro.casillas[i][j]);
        }
    }

    return *this;
}
    void tablero::avanzarturno() {
        turnoglobal++;
    }

    estadociclo tablero::getestadociclo() const {
        // Dividimos el tiempo en fases (ejemplo: cada 3 turnos cambia la fase)
        // El ciclo total se repite cada 12 movimientos globales
        int fase = (turnoglobal / 3) % 4;

        switch (fase) {
        case 0: return estadociclo::luz_maxima;
        case 1: return estadociclo::neutral;
        case 2: return estadociclo::oscuridad_maxima;
        case 3: return estadociclo::neutral;
        default: return estadociclo::neutral;
        }
    }

    void tablero::dibujarciclo() const {
        estadociclo actual = getestadociclo();
        std::cout << "CICLO ACTUAL: ";
        if (actual == estadociclo::luz_maxima) std::cout << "[ LUZ TOTAL - Ventaja Plantas ]\n";
        else if (actual == estadociclo::oscuridad_maxima) std::cout << "[ OSCURIDAD TOTAL - Ventaja Zombis ]\n";
        else std::cout << "[ NEUTRAL ]\n";
    }

tablero::~tablero() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (casillas[i][j] != nullptr) {
                delete casillas[i][j];
            }
        }
    }
}

personaje* tablero::getpersonajeen(int f, int c) const {
    if (f >= 0 && f < 9 && c >= 0 && c < 9) return casillas[f][c];
    return nullptr;
}

void tablero::colocarpersonaje(int f, int c, personaje* p) {
    if (f >= 0 && f < 9 && c >= 0 && c < 9) casillas[f][c] = p;
}

bool tablero::procesarmovimiento(int fori, int cori, int fdest, int cdest) {
    personaje* ficha = casillas[fori][cori];

    // Comprobamos si hay ficha y si su movimiento teorico es valido
    if (ficha != nullptr && ficha->esmovimientovalido(fori, cori, fdest, cdest)) {

        personaje* destino = casillas[fdest][cdest];

        //  si hay enemigo se abre el combate
        if (destino != nullptr && destino->getequipo() != ficha->getequipo()) {

            //  el arbitro calcula los bonus de VIDA y ATAQUE
            int vida_atacante = calcularbonusvida(ficha->getequipo());
            int danio_atacante = calcularbonusataque(ficha->getequipo());

            int vida_defensor = calcularbonusvida(destino->getequipo());
            int danio_defensor  = calcularbonusataque(destino->getequipo());

            std::cout << "\n[!] ¡CHOQUE EN LA CASILLA (" << fdest << "," << cdest << ")! [!]\n";

            if (vida_atacante > 0 || danio_atacante > 0) {
                std::cout << ">>> Atacante dopado: +" << vida_atacante << " HP | +" << danio_atacante << " Daño.\n";
            }
            if (vida_defensor > 0 || danio_defensor > 0) {
                std::cout << ">>> Defensor dopado: +" << vida_defensor << " HP | +" << danio_defensor << " Daño.\n";
            }

            std::cout << "-> Se iniciara el combate en la arena.\n";

            // de momento simulamos que el atacante gana siempre
            delete destino;
        }

        casillas[fdest][cdest] = ficha;
        casillas[fori][cori] = nullptr;
        return true;
    }
    return false;
}

void tablero::eliminarFicha(int f, int c) {
    if (casillas[f][c] != nullptr) {
        delete casillas[f][c];    // Liberamos la memoria
        casillas[f][c] = nullptr; // Dejamos la casilla vacía
    }
}

void tablero::dibujarconsola() const {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {

            if (casillas[i][j] != nullptr) {
                // si hay ficha, pintamos su letra
                std::cout << casillas[i][j]->getsimbolo() << " ";
            }
            else {
                // si esta vacia, le preguntamos al tablero si es especial
                if (escasilladepoder(i, j)) {
                    std::cout << "X "; // X marca la casilla de poder
                }
                else {
                    std::cout << ". "; // casilla normal vacia
                }
            }
        }
        std::cout << "\n";
    }
}
bool tablero::escasilladepoder(int f, int c) const {
    // comprobamos si las coordenadas coinciden con alguno de los 5 puntos
    if (f == 4 && c == 4) return true; // centro
    if (f == 0 && c == 4) return true; // arriba
    if (f == 8 && c == 4) return true; // abajo
    if (f == 4 && c == 0) return true; // izquierda
    if (f == 4 && c == 8) return true; // derecha

    return false; 
}
bando tablero::comprobarvictoria() const {

    // ANIQUILACIÓN TOTAL 
    int tropasplanta = 0;
    int tropaszombi = 0;

    // recorremos todo el tablero para hacer el censo
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (casillas[i][j] != nullptr) {
                if (casillas[i][j]->getequipo() == bando::planta) tropasplanta++;
                else if (casillas[i][j]->getequipo() == bando::zombi) tropaszombi++;
            }
        }
    }

    // si un bando se queda a cero, el otro gana automaticamente
    if (tropasplanta == 0 && tropaszombi > 0) return bando::zombi;
    if (tropaszombi == 0 && tropasplanta > 0) return bando::planta;


    //  DOMINIO DE LOS PUNTOS DE PODER 
   
    personaje* centro = casillas[4][4];
    personaje* arriba = casillas[0][4];
    personaje* abajo = casillas[8][4];
    personaje* izq = casillas[4][0];
    personaje* der = casillas[4][8];

    
    if (centro != nullptr && arriba != nullptr && abajo != nullptr &&
        izq != nullptr && der != nullptr) {

        // si todas están ocupadas, miramos si son todas del mismo equipo que la del centro
        bando equipodominante = centro->getequipo();

        if (arriba->getequipo() == equipodominante &&
            abajo->getequipo() == equipodominante &&
            izq->getequipo() == equipodominante &&
            der->getequipo() == equipodominante) {

            
            return equipodominante;
        }
    }

    // si no se cumple ninguna condicion, la guerra continua
    return bando::ninguno;
}
int tablero::calcularbonusvida(bando equipo) const {
    estadociclo cicloactual = getestadociclo();

    // si son plantas y hace sol
    if (equipo == bando::planta && cicloactual == estadociclo::luz_maxima) {
        return 20; // +20 de vida extra
    }

    // si son zombis y es de noche
    if (equipo == bando::zombi && cicloactual == estadociclo::oscuridad_maxima) {
        return 20; // +20 de vida extra
    }

    // si es neutral o no te favorece el clima, no hay bonus
    return 0;
}
int tablero::calcularbonusataque(bando equipo) const {
    estadociclo cicloactual = getestadociclo();

    // si son plantas y hace sol, +5 de ataque
    if (equipo == bando::planta && cicloactual == estadociclo::luz_maxima) {
        return 5;
    }

    // si son zombis y es de noche, +5 de ataque
    if (equipo == bando::zombi && cicloactual == estadociclo::oscuridad_maxima) {
        return 5;
    }

    // si no, se quedan como están
    return 0;
}
bool tablero::procesarhechizo(int fori, int cori, int hechizo_id, int fdest, int cdest) {
    personaje* ficha = casillas[fori][cori];

    // 1. ¿hay alguien en esa casilla origen?
    if (ficha != nullptr) {
        // 2. le pedimos a la ficha de tu colega que intente hacer su magia
        bool exito = ficha->lanzarhechizo(hechizo_id, fdest, cdest, *this);

        if (exito) {
            std::cout << "\n[!] ¡" << ficha->getsimbolo() << " ha lanzado un poderoso hechizo! [!]\n";
            return true; // el hechizo se gasto, el turno debe avanzar
        }
        else {
            std::cout << "\n[!] Esa ficha no sabe usar magia o el hechizo fallo.\n";
            return false;
        }
    }
    return false;
}

void tablero::inicializarpartida() {
    // primero nos aseguramos de que el tablero este vacio
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            casillas[i][j] = nullptr;
        }
    }

    
    for (int i = 0; i < 9; i++) {
        casillas[i][1] = new lanzaguisantes(0);
    }

 
    casillas[0][0] = new dronajo(0);         // esquina superior
    casillas[1][0] = new cactus(0);
    casillas[2][0] = new plantaCarnivora(0);
    casillas[3][0] = new pomelo(0);          // tanque protegiendo al lider
    casillas[4][0] = new dave(0);            // LÍDER EN EL CENTRO EXACTO (4,0)
    casillas[5][0] = new pomelo(0);          // tanque protegiendo al lider
    casillas[6][0] = new plantaCarnivora(0);
    casillas[7][0] = new cactus(0);
    casillas[8][0] = new dronajo(0);         // esquina inferior

   
   
    for (int i = 0; i < 9; i++) {
        casillas[i][7] = new zombidito(1);
    }

    
    casillas[0][8] = new loropirata(1);      // esquina superior
    casillas[1][8] = new soldado(1);
    casillas[2][8] = new superCerebroz(1);
    casillas[3][8] = new allstar(1);         // tanque protegiendo al lider
    casillas[4][8] = new drzomboss(1);       // LÍDER EN EL CENTRO EXACTO (4,8)
    casillas[5][8] = new allstar(1);         // tanque protegiendo al lider
    casillas[6][8] = new superCerebroz(1);
    casillas[7][8] = new soldado(1);
    casillas[8][8] = new loropirata(1);      // esquina inferior
}
bool tablero::hayObstaculoEnCamino(int fOri, int cOri, int fDest, int cDest) const {
    
    int dirFila = 0;
    if (fDest > fOri) dirFila = 1;
    else if (fDest < fOri) dirFila = -1;

    int dirCol = 0;
    if (cDest > cOri) dirCol = 1;
    else if (cDest < cOri) dirCol = -1;

    // Empezamos a mirar en la casilla SIGUIENTE a la de origen
    int fAct = fOri + dirFila;
    int cAct = cOri + dirCol;

    // Avanzamos paso a paso hasta llegar justo antes de la casilla de destino
    while (fAct != fDest || cAct != cDest) {
        if (getFicha(fAct, cAct) != nullptr) {
            return true; //choque
        }
        fAct += dirFila;
        cAct += dirCol;
    }

    return false; // Camino libre
}
void tablero::forzarMovimiento(int fOrig, int cOrig, int fDest, int cDest) {
  
    casillas[fDest][cDest] = casillas[fOrig][cOrig];
    casillas[fOrig][cOrig] = nullptr;
}
