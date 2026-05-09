#pragma once
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>

class GestorRanking {
private:
    std::map<std::string, int> puntuaciones;
    std::string rutaArchivo = "ranking.txt";

public:
    GestorRanking();
    void registrarVictoria(const std::string& nombreGana);
    std::vector<std::pair<std::string, int>> obtenerTop();
    void cargarDatos();

private:
    void guardarDatos();
};
