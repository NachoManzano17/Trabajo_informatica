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

int main() {
    std::cout << "iniciando el juego archon: plantas vs zombis..." << std::endl;

    // creamos un lanzaguisantes y le decimos que es del bando 0 (plantas)
    lanzaguisantes planta1(0);
    zombidito zombi1(1);
    cactus PlantaFranco(0);
    soldado ZombiFranco(1);
    plantaCarnivora plantaTP(0);
    superCerebroz zombiTP(1);
    dronajo plantaVol(0);
    loropirata zombiVol(1);
    pomelo plantaTanque(0);
    allstar zombiTanque(1);
    dave liderPlanta(0);
    drzomboss liderZombi(1);

    // le decimos que ataque
    planta1.atacarEnArena();
    zombi1.atacarEnArena();
    PlantaFranco.atacarEnArena();
    ZombiFranco.atacarEnArena();
    plantaTP.atacarEnArena();
    zombiTP.atacarEnArena();
    plantaTanque.atacarEnArena();
    zombiTanque.atacarEnArena();
    liderPlanta.atacarEnArena();
    liderZombi.atacarEnArena();

    return 0;
}