 #include "volador.h"
 #include <cmath>
 #include <algorithm> 


 
 volador::volador(int v, int f, int va, int tr, int rm, int b)
     : personaje(v, f, va, tr, rm, b) {
 }

 
 void volador::moverEnTablero() {
     
 }


 bool volador::esmovimientovalido(int forigen, int corigen, int fdestino, int cdestino) {
     int distfila = std::abs(forigen - fdestino);
     int distcol = std::abs(corigen - cdestino);

     // Regla 0: No quedarse quieto
     if (distfila == 0 && distcol == 0) {
         return false;
     }

     // Regla 1: Movimiento de ALFIL (solo en diagonal)
     // Para que sea diagonal pura, avanzar en filas debe ser igual que avanzar en columnas
     if (distfila != distcol) {
         return false;
     }

     // Regla 2: No superar el radio de movimiento
     // Como ahora sabemos que distfila es igual a distcol, podemos comparar cualquiera de las dos
     if (distfila > radioMovimiento) {
         return false;
     }

     return true;
 }
