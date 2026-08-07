#include "../../include/models/partidos.hpp"
#include "../../include/models/torneo.hpp"
#include "../../include/persistence/gestorArchivosBinarios.hpp"
#include "../../include/utils/formatos.hpp"
#include "../../include/utils/validaciones.hpp"
#include <cstring>
#include <ctime>

Partido::Partido(int idP, int idLocal, int idVisitante, const char *fechaP, const char *estadoP, const char *descripcionP)
    : id(idP), idEquipoLocal(idLocal), idEquipoVisitante(idVisitante), anotacionesLocal(0), anotacionesVisitante(0), tarjetasAmaLocal(0), tarjetasAmaVisitante(0),
      tarjetasRojasLocal(0), tarjetasRojasVisitante(0), numAnotaciones(0), numtarjetaAma(0), numTarjetasRojas(0), eliminado(false) {
    setFecha(fechaP);
    setEstado(estadoP);
    setDescripcion(descripcionP);
    fechaCreacion = std::time(nullptr);
    fechaUltimaModificacion = fechaCreacion;
}

int Partido::getId() const { return id; }

int Partido::getIdEquipoLocal() const { return idEquipoLocal; }

int Partido::getIdEquipoVisitante() const { return idEquipoVisitante; }

const char *Partido::getFecha() const { return fecha; }

const char *Partido::getEstado() const { return estado; }

const char *Partido::getDescripcion() const { return descripcion; }

int Partido::getAnotacionesLocal() const { return anotacionesLocal; }

int Partido::getAnotacionesVisitante() const { return anotacionesVisitante; }

int Partido::getTarjetasAmaLocal() const { return tarjetasAmaLocal; }

int Partido::getTarjetasAmaVisitante() const { return tarjetasAmaVisitante; }

int Partido::getTarjetasRojasLocal() const { return tarjetasRojasLocal; }

int Partido::getTarjetasRojasVisitante() const { return tarjetasRojasVisitante; }

const Anotacion *Partido::getAnotaciones() const { return anotaciones; }

const TarjetaAmarilla *Partido::getTarjetasAmarillas() const { return tarjetaA; }

const TarjetaRoja *Partido::getTarjetasRojas() const { return tarjetaR; }

int Partido::getNumAnotaciones() const { return numAnotaciones; }

int Partido::getNumTarjetaAma() const { return numtarjetaAma; }

int Partido::getNumTarjetasRojas() const { return numTarjetasRojas; }

bool Partido::getEliminado() const { return eliminado; }

time_t Partido::getFechaCreacion() const { return fechaCreacion; }

time_t Partido::getFechaUltimaModificacion() const { return fechaUltimaModificacion; }

std::string Partido::getFechaCreacionFormateada() const { return std::string(ctime(&fechaCreacion)); }

std::string Partido::getFechaUltimaModificacionFormateada() const { return std::string(ctime(&fechaUltimaModificacion)); }

size_t Partido::getTamano() { return sizeof(Partido); }

bool Partido::setId(int idP) {
    if (!GestorDeValidaciones::validarId(idP)) {
        return false;
    }
    id = idP;
    return true;
}

bool Partido::setIdEquipoLocal(int idLocal) {
    if (!GestorDeValidaciones::validarId(idLocal)) {
        return false;
    }
    idEquipoLocal = idLocal;
    return true;
}

bool Partido::setIdEquipoVisitante(int idVisitante) {
    if (!GestorDeValidaciones::validarId(idVisitante)) {
        return false;
    }
    idEquipoVisitante = idVisitante;
    return true;
}

bool Partido::setFecha(const char *fechaP) {

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    if (fechaP == nullptr || !GestorDeValidaciones::validarFecha(fechaP) ||
        !GestorDeValidaciones::validarFechaDeRegistroDePartidos(fechaP, torneo.getFechaInicio(), torneo.getFechaFin())) {
        return false;
    }
    Formatos::copiarCadena(fecha, fechaP, constantes::TAMANO_FECHA);
    return true;
}

bool Partido::setEstado(const char *estadoP) {
    if (estadoP == nullptr) {
        return false;
    }
    Formatos::copiarCadena(estado, estadoP, constantes::TAMANO_ESTADO);
    return true;
}

bool Partido::setDescripcion(const char *descripcionP) {
    if (descripcionP == nullptr) {
        return false;
    }
    Formatos::copiarCadena(descripcion, descripcionP, constantes::TAMANO_DESCRIPCION);
    return true;
}

bool Partido::setAnotacionesLocal(int anotaciones) {
    if (anotaciones < 0) {
        return false;
    }
    anotacionesLocal = anotaciones;
    return true;
}

bool Partido::setAnotacionesVisitante(int anotaciones) {
    if (anotaciones < 0) {
        return false;
    }
    anotacionesVisitante = anotaciones;
    return true;
}

bool Partido::setTarjetasAmaLocal(int tarjetas) {
    if (tarjetas < 0) {
        return false;
    }
    tarjetasAmaLocal = tarjetas;
    return true;
}

bool Partido::setTarjetasAmaVisitante(int tarjetas) {
    if (tarjetas < 0) {
        return false;
    }
    tarjetasAmaVisitante = tarjetas;
    return true;
}

bool Partido::setTarjetasRojasLocal(int tarjetas) {
    if (tarjetas < 0) {
        return false;
    }
    tarjetasRojasLocal = tarjetas;
    return true;
}

bool Partido::setTarjetasRojasVisitante(int tarjetas) {
    if (tarjetas < 0) {
        return false;
    }
    tarjetasRojasVisitante = tarjetas;
    return true;
}

bool Partido::agregarAnotacion(const Anotacion &nuevaAnotacion) {
    if (numAnotaciones >= constantes::MAX_ANOTACIONES) {
        return false;
    }
    anotaciones[numAnotaciones++] = nuevaAnotacion;
    return true;
}

bool Partido::eliminarAnotacion(const int indice) {
    if (indice < 0 || indice >= numAnotaciones) {
        return false;
    }
    for (int e = indice; e < numAnotaciones - 1; ++e) {
        anotaciones[e] = anotaciones[e + 1];
    }
    --numAnotaciones;
    return true;
}

bool Partido::agregarTarjetaAmarilla(const TarjetaAmarilla &nuevaTarjeta) {
    if (numtarjetaAma >= constantes::MAX_TARJETAS_AMARILLAS) {
        return false;
    }
    tarjetaA[numtarjetaAma++] = nuevaTarjeta;
    return true;
}

bool Partido::eliminarTarjetaAmarilla(const int indice) {
    if (indice < 0 || indice >= numtarjetaAma) {
        return false;
    }
    for (int e = indice; e < numtarjetaAma - 1; ++e) {
        tarjetaA[e] = tarjetaA[e + 1];
    }
    --numtarjetaAma;
    return true;
}

bool Partido::agregarTarjetaRoja(const TarjetaRoja &nuevaTarjeta) {
    if (numTarjetasRojas >= constantes::MAX_TARJETAS_ROJAS) {
        return false;
    }
    tarjetaR[numTarjetasRojas++] = nuevaTarjeta;
    return true;
}

bool Partido::eliminarTarjetaRoja(const int indice) {
    if (indice < 0 || indice >= numTarjetasRojas) {
        return false;
    }
    for (int e = indice; e < numTarjetasRojas - 1; ++e) {
        tarjetaR[e] = tarjetaR[e + 1];
    }
    --numTarjetasRojas;
    return true;
}

bool Partido::setAnotacionPorIndice(const int indice, const Anotacion &anotacionModificada) {
    if (indice < 0 || indice >= numAnotaciones) {
        return false;
    }
    anotaciones[indice] = anotacionModificada;
    return true;
}

bool Partido::setTarjetaAmarillaPorIndice(const int indice, const TarjetaAmarilla &tarjetaModificada) {
    if (indice < 0 || indice >= numtarjetaAma) {
        return false;
    }
    tarjetaA[indice] = tarjetaModificada;
    return true;
}

bool Partido::setTarjetaRojaPorIndice(const int indice, const TarjetaRoja &tarjetaModificada) {
    if (indice < 0 || indice >= numTarjetasRojas) {
        return false;
    }
    tarjetaR[indice] = tarjetaModificada;
    return true;
}

bool Partido::setEliminado(const bool elim) {
    eliminado = elim;
    return true;
}

bool Partido::setFechaCreacion(const time_t fechaC) {
    if (fechaC < 0) {
        return false;
    }
    fechaCreacion = fechaC;
    return true;
}

bool Partido::setFechaUltimaModificacion(const time_t fechaUM) {
    if (fechaUM < 0) {
        return false;
    }
    fechaUltimaModificacion = fechaUM;
    return true;
}

bool Partido::restaurarFecha(const char *fechaP) {
    if (fechaP == nullptr || !GestorDeValidaciones::validarFecha(fechaP)) {
        return false;
    }
    Formatos::copiarCadena(fecha, fechaP, constantes::TAMANO_FECHA);
    return true;
}