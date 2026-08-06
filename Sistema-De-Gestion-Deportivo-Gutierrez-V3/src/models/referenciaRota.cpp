#include "../../include/models/referenciaRota.hpp"

ReferenciaRota::ReferenciaRota(const char *tipo, const int origen, const int referencia) {
    setTipoDeReferencia(tipo);
    setIdOrigen(origen);
    setIdReferenciaRota(referencia);
}

const char *ReferenciaRota::getTipoDeReferencia() const { return tipoDeReferencia; }

int ReferenciaRota::getIdOrigen() const { return idOrigen; }

int ReferenciaRota::getIdReferenciaRota() const { return idReferenciaRota; }

bool ReferenciaRota::esValida() const { return tipoDeReferencia[0] != '\0' && idOrigen > 0 && idReferenciaRota > 0; }

/** @brief Asigna el tipo de relación, sin truncar textos que no caben en el buffer. */
bool ReferenciaRota::setTipoDeReferencia(const char *tipo) {
    if (tipo == nullptr || tipo[0] == '\0' || std::strlen(tipo) >= sizeof(tipoDeReferencia)) {
        return false;
    }

    std::strncpy(tipoDeReferencia, tipo, sizeof(tipoDeReferencia) - 1);
    tipoDeReferencia[sizeof(tipoDeReferencia) - 1] = '\0';
    return true;
}

bool ReferenciaRota::setIdOrigen(const int origen) {
    if (origen <= 0) {
        return false;
    }
    idOrigen = origen;
    return true;
}

bool ReferenciaRota::setIdReferenciaRota(const int referencia) {
    if (referencia <= 0) {
        return false;
    }
    idReferenciaRota = referencia;
    return true;
}

void ReferenciaRota::limpiar() {
    tipoDeReferencia[0] = '\0';
    idOrigen = 0;
    idReferenciaRota = 0;
}

static std::size_t getTamano() { return sizeof(ReferenciaRota); }