#include "../../include/models/equipos.hpp"
#include "../../include/utils/formatos.hpp"
#include "../../include/utils/validaciones.hpp"
#include <cstring>
#include <ctime>

Equipo::Equipo(int idE, const char *nom, const char *cdd, const char *ent, const char *fechaR, bool eli, time_t fechaC, time_t fechaUM)
    : id(idE), eliminado(eli), fechaCreacion(fechaC), fechaUltimaModificacion(fechaUM) {
    setNombre(nom);
    setCiudad(cdd);
    setEntrenador(ent);
    setFechaRegistro(fechaR);
}

// *getters

int Equipo::getId() const { return id; }

const char *Equipo::getNombre() const { return nombre; }

const char *Equipo::getCiudad() const { return ciudad; }

const char *Equipo::getEntrenador() const { return entrenador; }

const char *Equipo::getFechaRegistro() const { return fechaRegistro; }

int Equipo::getNumJugadores() const { return numJugadores; }

int Equipo::getPuntos() const { return puntos; }

int Equipo::getJugados() const { return jugados; }

int Equipo::getVictorias() const { return victorias; }

int Equipo::getEmpates() const { return empates; }

int Equipo::getDerrotas() const { return derrotas; }

int Equipo::getAnotacionAFavor() const { return anotacionAFavor; }

int Equipo::getAnotacionEnContra() const { return anotacionEnContra; }

const int *Equipo::getPartidosIDs() const { return partidosIDs; }

int Equipo::getCantidadPartidos() const { return cantidadPartidos; }

bool Equipo::getEliminado() const { return eliminado; }

time_t Equipo::getFechaCreacion() const { return fechaCreacion; }

time_t Equipo::getFechaUltimaModificacion() const { return fechaUltimaModificacion; }

// *Setters

bool Equipo::setId(const int idE) {
    if (!GestorDeValidaciones::validarId(idE)) {
        return false;
    }
    id = idE;
    return true;
}

bool Equipo::setNombre(const char *nom) {
    if (!GestorDeValidaciones::validarNombreOApellido(nom)) {
        return false;
    }
    Formatos::copiarCadena(nombre, nom, constantes::TAMANO_NOMBRE);
    return true;
}

bool Equipo::setCiudad(const char *cdd) {
    if (!GestorDeValidaciones::validarNombreOApellido(cdd)) {
        return false;
    }
    Formatos::copiarCadena(ciudad, cdd, constantes::TAMANO_NOMBRE);
    return true;
}

bool Equipo::setEntrenador(const char *ent) {
    if (!GestorDeValidaciones::validarNombreOApellido(ent)) {
        return false;
    }
    Formatos::copiarCadena(entrenador, ent, constantes::TAMANO_NOMBRE);
    return true;
}

bool Equipo::setFechaRegistro(const char *fechaR) {
    if (fechaR == nullptr || !GestorDeValidaciones::validarFecha(fechaR)) {
        return false;
    }
    Formatos::copiarCadena(fechaRegistro, fechaR, constantes::TAMANO_FECHA);
    return true;
}

bool Equipo::setNumJugadores(const int numJ) {
    if (numJ < 0) {
        return false;
    }
    numJugadores = numJ;
    return true;
}

bool Equipo::setPuntos(const int pts) {
    if (pts < 0) {
        return false;
    }
    puntos = pts;
    return true;
}

bool Equipo::setJugados(const int jgd) {
    if (jgd < 0) {
        return false;
    }
    jugados = jgd;
    return true;
}

bool Equipo::setVictorias(const int vct) {
    if (vct < 0) {
        return false;
    }
    victorias = vct;
    return true;
}

bool Equipo::setEmpates(const int emp) {
    if (emp < 0) {
        return false;
    }
    empates = emp;
    return true;
}

bool Equipo::setDerrotas(const int drt) {
    if (drt < 0) {
        return false;
    }
    derrotas = drt;
    return true;
}

bool Equipo::setAnotacionAFavor(const int antF) {
    if (antF < 0) {
        return false;
    }
    anotacionAFavor = antF;
    return true;
}

bool Equipo::setAnotacionEnContra(const int antC) {
    if (antC < 0) {
        return false;
    }
    anotacionEnContra = antC;
    return true;
}

bool Equipo::agregarIdPartido(const int nuevoIdPartido) {
    if (!GestorDeValidaciones::validarId(nuevoIdPartido) || cantidadPartidos >= constantes::MAX_PARTIDOS_EQUIPOS) {
        return false;
    }
    partidosIDs[cantidadPartidos++] = nuevoIdPartido;
    return true;
}

bool Equipo::eliminarIdPartido(const int idPartido) {
    for (int e = 0; e < cantidadPartidos; ++e) {
        if (partidosIDs[e] == idPartido) {
            for (int r = e; r < cantidadPartidos - 1; ++r) {
                partidosIDs[r] = partidosIDs[r + 1];
            }
            partidosIDs[cantidadPartidos - 1] = 0;
            --cantidadPartidos;
            return true;
        }
    }
    return false;
}

bool Equipo::setIdPartidoPorIndice(const int indice, const int idPartidoModificado) {
    if (indice < 0 || indice >= cantidadPartidos || idPartidoModificado <= 0) {
        return false;
    }
    partidosIDs[indice] = idPartidoModificado;
    return true;
}

bool Equipo::setEliminado(const bool eli) {
    eliminado = eli;
    return true;
}

bool Equipo::setFechaCreacion(const time_t fechaC) {
    if (fechaC < 0) {
        return false;
    }
    fechaCreacion = fechaC;
    return true;
}

bool Equipo::setFechaUltimaModificacion(const time_t fechaUM) {
    if (fechaUM < 0) {
        return false;
    }
    fechaUltimaModificacion = fechaUM;
    return true;
}
