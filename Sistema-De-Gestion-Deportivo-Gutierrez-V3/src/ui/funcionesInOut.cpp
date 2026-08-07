#include "../../include/ui/funcionesInOut.hpp"

// Funciones auxiliares de cancelación
std::string presentacion::funcionesInOut::normalizarEntradaAMayus(const std::string &valor) {
    std::string copia = valor;
    std::transform(copia.begin(), copia.end(), copia.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return copia;
}

bool presentacion::funcionesInOut::esCancelacion(const std::string &valor) {
    std::string copia = normalizarEntradaAMayus(valor);
    return copia == "CANCELAR" || copia == "SALIR";
}
