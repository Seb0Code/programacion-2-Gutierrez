#include "../../include/manager/operacionesJugadores.hpp"
#include "../../include/persistence/gestorArchivosBinarios.hpp"

bool OperacionesJugadores::cadenaDuplicada(const char *cadena, const char *(Jugador::*getter)() const, bool actualizar, int idRegistro) {

    std::vector<Jugador> listaDeJugadores = GestorArchivosBinarios::listarRegistros<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES);

    if (listaDeJugadores.empty()) {
        return false;
    }

    std::string cadenaBuscada = cadena ? cadena : std::string();
    Formatos::convertirTextoAMinus(cadenaBuscada);

    for (const auto &jugadorAux : listaDeJugadores) {
        if (jugadorAux.getEliminado()) {
            continue;
        }

        if (actualizar && jugadorAux.getId() == idRegistro) {
            continue;
        }

        const char *miembroCStr = (jugadorAux.*getter)();
        std::string miembroStr = miembroCStr ? miembroCStr : std::string();
        Formatos::convertirTextoAMinus(miembroStr);

        if (cadenaBuscada == miembroStr) {
            return true;
        }
    }

    return false;
}

bool OperacionesJugadores::esDorsalDuplicado(const int dorsal, const int idEquipo, bool actualizar, int idRegistro) {

    std::vector<Jugador> listaDeJugadores = GestorArchivosBinarios::listarRegistros<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES);

    if (listaDeJugadores.empty()) {
        return false;
    }

    for (const auto &jugadorAux : listaDeJugadores) {
        if (jugadorAux.getEliminado()) {
            continue;
        }

        // Solo nos interesan los jugadores del mismo equipo
        if (jugadorAux.getIdEquipo() != idEquipo) {
            continue;
        }

        if (actualizar && jugadorAux.getId() == idRegistro) {
            continue;
        }

        if (dorsal == jugadorAux.getNumeroDorsal()) {
            return true;
        }
    }

    return false;
}

bool OperacionesJugadores::registrarJugador(Jugador &nuevoJugador) {

    ArchivoHeader header = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    if (header.getCantidadRegistros() == constantes::ERROR_INT) {
        return false;
    }

    // * Verificamos que no esten duplicados el nombre, la cedula o el dorsal dentro del equipo
    if (OperacionesJugadores::cadenaDuplicada(nuevoJugador.getNombre(), &Jugador::getNombre) ||
        OperacionesJugadores::cadenaDuplicada(nuevoJugador.getCedula(), &Jugador::getCedula) ||
        OperacionesJugadores::esDorsalDuplicado(nuevoJugador.getNumeroDorsal(), nuevoJugador.getIdEquipo())) {
        return false;
    }

    // * Inicializamos las estadisticas
    nuevoJugador.setNumAnotaciones(0);
    nuevoJugador.setTarjetaAmarillas(0);
    nuevoJugador.setTarjetasRojas(0);

    // * Inicializamos las fechas de creacion y ultima modificacion
    nuevoJugador.setFechaCreacion(std::time(nullptr));
    nuevoJugador.setFechaUltimaModificacion(std::time(nullptr));

    // * Colocamos el jugador como activo y le colocamos el ID
    nuevoJugador.setId(header.getProximoId());
    nuevoJugador.setEliminado(false);

    // * Guardamos el jugador
    if (!GestorArchivosBinarios::guardarNuevoRegistro(constantes::NOMBRE_ARCHIVO_JUGADORES, nuevoJugador)) {
        return false;
    }

    // * Aumentamos el numero de jugadores del equipo
    if (!OperacionesEquipos::modificarCantidadDeJugadores(nuevoJugador.getIdEquipo(), false)) {
        return false;
    }

    return true;
}

std::vector<Jugador> OperacionesJugadores::listarJugadoresPorEquipo(const int idEquipo) {
    std::vector<Jugador> listaDeJugadoresPorSuEquipo;

    std::vector<Jugador> listaDeJugadores = GestorArchivosBinarios::listarRegistros<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES);
    if (listaDeJugadores.empty()) {
        return listaDeJugadoresPorSuEquipo;
    }


    Equipo equipoTemporal;

    // Buscamos si el equipo solicitado realmente existe
    bool existe = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoTemporal, idEquipo);

    // Verificamos si el equipo existe
    if (!existe) {
        return listaDeJugadoresPorSuEquipo;
    }

    for (size_t e = 0; e < listaDeJugadores.size(); ++e) {
        if (listaDeJugadoresPorSuEquipo.size() < constantes::MAX_RESULTADOS) {
            Jugador jugadorAuxiliar = listaDeJugadores[e];

            // Si el jugador no pertence al equipo lo ignoramos
            if (jugadorAuxiliar.getIdEquipo() != idEquipo) {
                continue;
            }

            if (jugadorAuxiliar.getEliminado()) {
                continue;
            }

            // Guardamos el jugador
            listaDeJugadoresPorSuEquipo.push_back(jugadorAuxiliar);

        } else { // si no rompemos el bucle
            break;
        }
    }

    return listaDeJugadoresPorSuEquipo;
}

bool OperacionesJugadores::actualizarJugador(const int id, const char *nombre, const int edad, const int numeroDorsal, const char *posicion, const char *cedula) {

    bool existe = false;

    // Verifiquemos que los valores no esten vacios
    if (GestorDeValidaciones::validarCadenaVacia(nombre) && edad == 0 && numeroDorsal == 0 && GestorDeValidaciones::validarCadenaVacia(posicion) &&
        GestorDeValidaciones::validarCadenaVacia(cedula)) {
        return false;
    }

    // Utilizamos una variable auxiliar
    Jugador jugadorAuxiliar;

    // Buscamos el registro con el ID
    existe = GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, id);

    // Verificamos que lo haya encontrado
    if (!existe) {
        return false;
    }

    // Verificamos que no estén duplicados
    if (OperacionesJugadores::cadenaDuplicada(nombre, &Jugador::getNombre, true, id) || OperacionesJugadores::cadenaDuplicada(cedula, &Jugador::getCedula, true, id) ||
        OperacionesJugadores::esDorsalDuplicado(numeroDorsal, jugadorAuxiliar.getIdEquipo(), true, id)) {
        return false;
    }

    // Si no está vacío lo actualizamos
    if (!GestorDeValidaciones::validarCadenaVacia(nombre)) {
        if (!jugadorAuxiliar.setNombre(nombre)) {
            return false;
        }
    }
    if (GestorDeValidaciones::validarEdad(edad)) {
        if (!jugadorAuxiliar.setEdad(edad)) {
            return false;
        }
    }
    if (GestorDeValidaciones::validarDorsal(numeroDorsal)) {
        if (!jugadorAuxiliar.setNumeroDorsal(numeroDorsal)) {
            return false;
        }
    }
    if (!GestorDeValidaciones::validarCadenaVacia(posicion)) {
        if (!jugadorAuxiliar.setPosicion(posicion)) {
            return false;
        }
    }
    if (!GestorDeValidaciones::validarCadenaVacia(cedula)) {
        if (!jugadorAuxiliar.setCedula(cedula)) {
            return false;
        }
    }

    // Modificamos la fecha de ultima modificacion del registro
    jugadorAuxiliar.setFechaUltimaModificacion(std::time(nullptr));

    // Guardamos el registro
    if (!GestorArchivosBinarios::guardarRegistro<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, id)) {
        return false;
    }
    return true;
}

bool OperacionesJugadores::eliminarJugador(const int id) {

    // * Borramos el Jugador
    Jugador jugadorAuxiliar;

    // Primero buscamos al jugador para ver si existe
    if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, id)) {
        std::cout << "1";
        return false;
    }
    // * Disminuimos el contador de jugadores del equipo correspondiente si corresponde
    OperacionesEquipos::modificarCantidadDeJugadores(jugadorAuxiliar.getIdEquipo(), true);

    // Lo eliminamos
    jugadorAuxiliar.setEliminado(true);

    // Lo guardamos
    if (!GestorArchivosBinarios::guardarRegistro<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, id)) {
        std::cout << "1";
        return false;
    }

    // Obtenemos el header
    ArchivoHeader header = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    if (header.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cout << "1";
        return false;
    }

    // Reducimos el numero de jugadores activos
    header.disminuirRegistrosActivos();

    //  *Actualizamos el header
    if (!GestorArchivosBinarios::actualizarHeader(constantes::NOMBRE_ARCHIVO_JUGADORES, header)) {
        std::cout << "2";
        return false;
    }

    return true;
}

bool OperacionesJugadores::modificarEstadisticas(Partido &registroPartido, const bool disminuir) {

    // Recorremos de atras hacia adelante: al revertir (disminuir), eliminarAnotacion/
    // eliminarTarjetaX desplaza los elementos siguientes, por lo que iterar hacia
    // adelante saltaria registros y guardaria el jugador equivocado.
    for (int e = registroPartido.getNumAnotaciones() - 1; e >= 0; --e) {
        Jugador jugadorAuxiliar;
        const Anotacion *anotaciones = registroPartido.getAnotaciones();
        int idJugadorAnotacion = anotaciones[e].getIdJugador();

        if (idJugadorAnotacion == 0) {
            continue;
        }

        if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, idJugadorAnotacion)) {
            return false;
        }

        if (!disminuir) {
            if (!modificarAnotaciones(jugadorAuxiliar, false))
                return false;
        } else {
            if (!modificarAnotaciones(jugadorAuxiliar, true))
                return false;
            if (!registroPartido.eliminarAnotacion(e))
                return false;
        }

        if (!GestorArchivosBinarios::guardarRegistro<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, idJugadorAnotacion)) {
            return false;
        }
    }

    for (int e = registroPartido.getNumTarjetaAma() - 1; e >= 0; --e) {
        Jugador jugadorAuxiliar;
        const TarjetaAmarilla *tarjetaA = registroPartido.getTarjetasAmarillas();
        int idJugadorTarjeta = tarjetaA[e].getIdJugador();

        if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, idJugadorTarjeta)) {
            return false;
        }

        if (!disminuir) {
            if (!modificarTarjetasAma(jugadorAuxiliar, false))
                return false;
        } else {
            if (!modificarTarjetasAma(jugadorAuxiliar, true))
                return false;
            if (!registroPartido.eliminarTarjetaAmarilla(e))
                return false;
        }

        if (!GestorArchivosBinarios::guardarRegistro<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, idJugadorTarjeta)) {
            return false;
        }
    }

    for (int e = registroPartido.getNumTarjetasRojas() - 1; e >= 0; --e) {
        Jugador jugadorAuxiliar;
        const TarjetaRoja *tarjetaR = registroPartido.getTarjetasRojas();
        int idJugadorTarjeta = tarjetaR[e].getIdJugador();

        if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, idJugadorTarjeta)) {
            return false;
        }

        if (!disminuir) {
            if (!modificarTarjetasRojas(jugadorAuxiliar, false))
                return false;
        } else {
            if (!modificarTarjetasRojas(jugadorAuxiliar, true))
                return false;
            if (!registroPartido.eliminarTarjetaRoja(e))
                return false;
        }

        if (!GestorArchivosBinarios::guardarRegistro<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, idJugadorTarjeta)) {
            return false;
        }
    }

    return true;
}

bool OperacionesJugadores::modificarAnotaciones(Jugador &jugadorAuxiliar, const bool disminuir) {
    int anotaciones = jugadorAuxiliar.getNumAnotaciones();
    if (!disminuir) {
        ++anotaciones;
    } else {
        --anotaciones;
    }
    return jugadorAuxiliar.setNumAnotaciones(anotaciones);
}

bool OperacionesJugadores::modificarTarjetasAma(Jugador &jugadorAuxiliar, const bool disminuir) {
    int tarjetasAma = jugadorAuxiliar.getTarjetaAmarillas();
    if (!disminuir) {
        ++tarjetasAma;
    } else {
        --tarjetasAma;
    }
    return jugadorAuxiliar.setTarjetaAmarillas(tarjetasAma);
}

bool OperacionesJugadores::modificarTarjetasRojas(Jugador &jugadorAuxiliar, const bool disminuir) {
    int tarjetasRoj = jugadorAuxiliar.getTarjetasRojas();
    if (!disminuir) {
        ++tarjetasRoj;
    } else {
        --tarjetasRoj;
    }
    return jugadorAuxiliar.setTarjetasRojas(tarjetasRoj);
}
