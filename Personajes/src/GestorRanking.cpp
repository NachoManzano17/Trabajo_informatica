#include "GestorRanking.h"

GestorRanking::GestorRanking() {
    cargarDatos();
}

void GestorRanking::registrarVictoria(const std::string& nombreGana) {
    puntuaciones[nombreGana]++;
    guardarDatos();
}

bool comparadorRanking(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
    return a.second > b.second;
}

std::vector<std::pair<std::string, int>> GestorRanking::obtenerTop() {
    std::vector<std::pair<std::string, int>> top(puntuaciones.begin(), puntuaciones.end());
    std::sort(top.begin(), top.end(), comparadorRanking);
    return top;
}

void GestorRanking::cargarDatos() {
    std::ifstream archivo(rutaArchivo);
    if (archivo.is_open()) {
        std::string nombre;
        int puntos;
        while (archivo >> nombre >> puntos) {
            puntuaciones[nombre] = puntos;
        }
        archivo.close();
    }
}

void GestorRanking::guardarDatos() {
    std::ofstream archivo(rutaArchivo);
    if (archivo.is_open()) {
        for (const auto& par : puntuaciones) {
            archivo << par.first << " " << par.second << "\n";
        }
        archivo.close();
    }
}