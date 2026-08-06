#include "../../include/models/jugador.hpp"
#include "../../include/utils/formatos.hpp"
#include "../../include/utils/validaciones.hpp"
#include <ctime>

Jugador::Jugador(int idJ, int idEq, const char *nom, const char *ced, const char *pos, int ed, int dor, const char *fechaR, bool eli, time_t fechaC, time_t fechaUM)
    : id(idJ), idEquipo(idEq), eliminado(eli), fechaCreacion(fechaC), fechaUltimaModificacion(fechaUM) {
    setNombre(nom);
    setCedula(ced);
    setPosicion(pos);
    setEdad(ed);
    setNumeroDorsal(dor);
    setFechaRegistro(fechaR);
}

int Jugador::getId() const { return id; }

int Jugador::getIdEquipo() const { return idEquipo; }

const char *Jugador::getNombre() const { return nombre; }

const char *Jugador::getCedula() const { return cedula; }

const char *Jugador::getPosicion() const { return posicion; }

int Jugador::getEdad() const { return edad; }

int Jugador::getNumeroDorsal() const { return numeroDorsal; }

const char *Jugador::getFechaRegistro() const { return fechaRegistro; }

int Jugador::getNumAnotaciones() const { return numAnotaciones; }

int Jugador::getTarjetaAmarillas() const { return tarjetaAmarillas; }

int Jugador::getTarjetasRojas() const { return tarjetasRojas; }

bool Jugador::getEliminado() const { return eliminado; }

time_t Jugador::getFechaCreacion() const { return fechaCreacion; }

time_t Jugador::getFechaUltimaModificacion() const { return fechaUltimaModificacion; }

bool Jugador::setId(const int idJ) {
    if (!GestorDeValidaciones::validarId(idJ)) {
        return false;
    }
    id = idJ;
    return true;
}

bool Jugador::setIdEquipo(const int idEq) {
    if (!GestorDeValidaciones::validarId(idEq)) {
        return false;
    }
    idEquipo = idEq;
    return true;
}

bool Jugador::setNombre(const char *nom) {
    if (!GestorDeValidaciones::validarNombreOApellido(nom)) {
        return false;
    }
    Formatos::copiarCadena(nombre, nom, constantes::TAMANO_NOMBRE);
    return true;
}

bool Jugador::setCedula(const char *ced) {
    if (ced == nullptr || !GestorDeValidaciones::validarCedula(ced)) {
        return false;
    }
    Formatos::copiarCadena(cedula, ced, constantes::TAMANO_CEDULA);
    return true;
}

bool Jugador::setPosicion(const char *pos) {
    if (pos == nullptr || !GestorDeValidaciones::validarTamano(pos, constantes::TAMANO_POSICION - 1)) {
        return false;
    }
    Formatos::copiarCadena(posicion, pos, constantes::TAMANO_POSICION);
    return true;
}

bool Jugador::setEdad(const int ed) {
    if (!GestorDeValidaciones::validarEdad(ed)) {
        return false;
    }
    edad = ed;
    return true;
}

bool Jugador::setNumeroDorsal(const int dor) {
    if (!GestorDeValidaciones::validarDorsal(dor)) {
        return false;
    }
    numeroDorsal = dor;
    return true;
}

bool Jugador::setFechaRegistro(const char *fechaR) {
    if (fechaR == nullptr || !GestorDeValidaciones::validarFecha(fechaR)) {
        return false;
    }
    Formatos::copiarCadena(fechaRegistro, fechaR, constantes::TAMANO_FECHA);
    return true;
}

bool Jugador::setNumAnotaciones(const int anots) {
    if (anots < 0) {
        return false;
    }
    numAnotaciones = anots;
    return true;
}

bool Jugador::setTarjetaAmarillas(const int tAma) {
    if (tAma < 0) {
        return false;
    }
    tarjetaAmarillas = tAma;
    return true;
}

bool Jugador::setTarjetasRojas(const int tRoj) {
    if (tRoj < 0) {
        return false;
    }
    tarjetasRojas = tRoj;
    return true;
}

bool Jugador::setEliminado(const bool eli) {
    eliminado = eli;
    return true;
}

bool Jugador::setFechaCreacion(const time_t fechaC) {
    if (fechaC < 0) {
        return false;
    }
    fechaCreacion = fechaC;
    return true;
}

bool Jugador::setFechaUltimaModificacion(const time_t fechaUM) {
    if (fechaUM < 0) {
        return false;
    }
    fechaUltimaModificacion = fechaUM;
    return true;
}
