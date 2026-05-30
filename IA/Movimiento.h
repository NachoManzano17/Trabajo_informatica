struct Movimiento {
    enum class Tipo { Mover, Hechizo, Pasar };

    Tipo tipo         = Tipo::Mover;
    int  filaOrigen   = -1;
    int  colOrigen    = -1;
    int  filaDestino  = -1;
    int  colDestino   = -1;
    int  idHechizo    = -1;
    int  filaObjetivo = -1;
    int  colObjetivo  = -1;

    bool operator==(const Movimiento& otro) const {
        return tipo == otro.tipo
            && filaOrigen == otro.filaOrigen && colOrigen == otro.colOrigen
            && filaDestino == otro.filaDestino && colDestino == otro.colDestino
            && idHechizo == otro.idHechizo
            && filaObjetivo == otro.filaObjetivo && colObjetivo == otro.colObjetivo;
    }
};
