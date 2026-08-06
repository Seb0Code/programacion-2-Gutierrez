#include "../../include/models/informeIntegridad.hpp"

int InformeIntegridad::getEquiposVerificados() const { return equiposVerificados; }

int InformeIntegridad::getJugadoresVerificados() const { return jugadoresVerificados; }

int InformeIntegridad::getPartidosVerificados() const { return partidosVerificados; }

int InformeIntegridad::getTotalDeReferenciasRotas() const { return totalDeReferenciasRotas; }

bool InformeIntegridad::hayReferenciasOmitidas() const { return referenciasOmitidas; }

bool InformeIntegridad::esIntegro() const { return totalDeReferenciasRotas == 0; }

const ReferenciaRota *InformeIntegridad::getReferenciasRotas() const { return rotas; }

/** @brief Devuelve una referencia por índice o `nullptr` si está fuera del rango registrado. */
const ReferenciaRota *InformeIntegridad::getReferenciaRota(const int indice) const {
    if (indice < 0 || indice >= totalDeReferenciasRotas) {
        return nullptr;
    }
    return &rotas[indice];
}

void InformeIntegridad::incrementarEquiposVerificados() { ++equiposVerificados; }

void InformeIntegridad::incrementarJugadoresVerificados() { ++jugadoresVerificados; }

void InformeIntegridad::incrementarPartidosVerificados() { ++partidosVerificados; }

/**
 * @brief Registra una referencia rota válida.
 * @return `false` si la referencia es inválida o si el informe alcanzó su capacidad.
 */
bool InformeIntegridad::agregarReferenciaRota(const ReferenciaRota &referencia) {
    if (!referencia.esValida()) {
        return false;
    }
    if (totalDeReferenciasRotas >= constantes::MAX_RESULTADOS) {
        referenciasOmitidas = true;
        return false;
    }

    rotas[totalDeReferenciasRotas] = referencia;
    ++totalDeReferenciasRotas;
    return true;
}

/** @brief Restablece el informe para iniciar una nueva verificación. */
void InformeIntegridad::limpiar() {
    equiposVerificados = 0;
    jugadoresVerificados = 0;
    partidosVerificados = 0;
    totalDeReferenciasRotas = 0;
    referenciasOmitidas = false;

    for (ReferenciaRota &referencia : rotas) {
        referencia.limpiar();
    }
}