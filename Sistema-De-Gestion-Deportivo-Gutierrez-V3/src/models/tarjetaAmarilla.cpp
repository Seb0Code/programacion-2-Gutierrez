#include "../../include/models/tarjetaAmarilla.hpp"
#include "../../include/models/torneo.hpp"
#include "../../include/persistence/gestorArchivosBinarios.hpp"
#include "../../include/utils/validaciones.hpp"

int TarjetaAmarilla::getIdJugador() const { return idJugador; }

int TarjetaAmarilla::getMinuto() const { return minuto; }

const char *TarjetaAmarilla::getEquipo() const { return equipo; }

bool TarjetaAmarilla::setIdJugador(const int idJ) {
    if (!GestorDeValidaciones::validarId(idJ)) {
        return false;
    }
    idJugador = idJ;
    return true;
}

bool TarjetaAmarilla::setMinuto(const int min) {

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    if (!GestorDeValidaciones::validarMinuto(min, torneo.getReglasTorneo().getMinutoMinimo(), torneo.getReglasTorneo().getMinutoMaximo())) {
        return false;
    }

    minuto = min;
    return true;
}

bool TarjetaAmarilla::setEquipo(const char *eq) {
    if (GestorDeValidaciones::validarCadenaVacia(eq)) {
        return false;
    }
    Formatos::copiarCadena(equipo, eq, constantes::TAMANO_LOCAL_O_VISITANTE);
    return true;
}
