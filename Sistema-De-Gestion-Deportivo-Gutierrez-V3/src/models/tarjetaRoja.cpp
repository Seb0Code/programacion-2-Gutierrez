#include "../../include/models/tarjetaRoja.hpp"
#include "../../include/models/torneo.hpp"
#include "../../include/persistence/gestorArchivosBinarios.hpp"
#include "../../include/utils/validaciones.hpp"

int TarjetaRoja::getIdJugador() const { return idJugador; }

int TarjetaRoja::getMinuto() const { return minuto; }

const char *TarjetaRoja::getEquipo() const { return equipo; }

bool TarjetaRoja::setIdJugador(const int idJ) {
    if (!GestorDeValidaciones::validarId(idJ)) {
        return false;
    }
    idJugador = idJ;
    return true;
}

bool TarjetaRoja::setMinuto(const int min) {
    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    if (!GestorDeValidaciones::validarMinuto(min, torneo.getReglasTorneo().getMinutoMinimo(), torneo.getReglasTorneo().getMinutoMaximo())) {
        return false;
    }
    minuto = min;
    return true;
}

bool TarjetaRoja::setEquipo(const char *eq) {
    if (GestorDeValidaciones::validarCadenaVacia(eq)) {
        return false;
    }
    Formatos::copiarCadena(equipo, eq, constantes::TAMANO_LOCAL_O_VISITANTE);
    return true;
}
