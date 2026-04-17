#include "lanzaguisantes.h"
#include <iostream> // incluimos esto de momento para poder imprimir texto

// en el constructor, le pasamos los stats fijos al padre (terrestre).
// orden: vida, fuerza, velocidadAtaque, tiempoRecarga, radioMovimiento, bando
lanzaguisantes::lanzaguisantes(int bando)
    : terrestre(100, 20, 5, 2, 3, bando) {
    // el cuerpo del constructor puede quedar vacío, el padre ya hace el trabajo
}

void lanzaguisantes::atacarEnArena() {
    // de momento solo imprimimos un texto para comprobar que funciona.
    // más adelante, aquí irá el código gráfico para disparar el guisante.
    std::cout << "¡el lanzaguisantes dispara un guisante verde!" << std::endl;
}