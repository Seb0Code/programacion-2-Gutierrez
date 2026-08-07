#include "../../include/ui/MenusPartidos.hpp"

void partidos::mensajeDefault() { std::cout << "Opción inválida.\n"; }

// ---------------------------------------------------------------------------------------------- //
//   PROGRAMAR PARTIDO
// ---------------------------------------------------------------------------------------------- //
void partidos::programarPartido() {
    Formatos::limpiarPantalla();
    bool flagError = false;
    bool cancelado = false;
    char confirmacion;
    Equipo eqLocal, eqVisitante;
    Partido nuevoPartido;

    // Leemos el header del archivo de equipos para saber el numero de registros activos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Programación Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos activos registrados
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "\nNo hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay al menos 2 equipos no se puede programar un partido
    if (headerEquipos.getRegistrosActivos() <= 1) {
        std::cout << "\nNo es posible programar un partido con solo un equipo\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    int idLocal = 0;
    int idVisitante = 0;

    // Recolectamos el ID del equipo local
    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(idLocal, " Ingrese el ID del equipo local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idLocal)) {
            std::cerr << " Error el id '" << idLocal << "' no es válido\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqLocal, idLocal)) {
            std::cerr << " Error el ID '" << idLocal << "' no está asociado a ningún equipo\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

    } while (flagError);

    // Recolectamos el ID del equipo visitante
    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(idVisitante, " Ingrese el ID del equipo visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idVisitante)) {
            std::cerr << " Error el id '" << idVisitante << "' no es válido\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        // No se puede programar un partido entre el mismo equipo
        if (idLocal == idVisitante) {
            std::cerr << " Error: no se puede programar un partido entre un mismo equipo\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, idVisitante)) {
            std::cerr << " Error el ID '" << idVisitante << "' no está asociado a ningún equipo\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

    } while (flagError);

    if (!nuevoPartido.setIdEquipoLocal(idLocal) || !nuevoPartido.setIdEquipoVisitante(idVisitante)) {
        std::cerr << "Error del Sistema!\n";
        Formatos::pausarPrograma();
        return;
    }

    // Verificamos que no haya un partido programado entre ambos equipos
    if (OperacionesPartidos::hayPartidoProgramadoEntre2(nuevoPartido)) {
        std::cerr << "\n Error ya hay un partido programado entre '" << eqLocal.getNombre() << "' y '" << eqVisitante.getNombre() << "'\n";
        Formatos::pausarPrograma();
        return;
    }

    // Recolectamos la fecha del partido
    char fecha[constantes::TAMANO_FECHA];
    do {
        flagError = false;
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Fecha de Inicio del Torneo: " << torneo.getFechaInicio() << "\n";
        std::cout << " Fecha de Fin del Torneo:    " << torneo.getFechaFin() << "\n\n";

        if (!presentacion::funcionesInOut::ingresarCadena(fecha, constantes::TAMANO_FECHA,
                                                          " Ingrese la fecha del partido (YYYY-MM-DD) (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarFechaDeRegistroDePartidos(fecha, torneo.getFechaInicio(), torneo.getFechaFin())) {
            std::cerr << " Error la fecha '" << fecha << "' ingresada no es válida o está fuera del rango del torneo\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoPartido.setFecha(fecha)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(1500);
    } while (flagError);

    // Recolectamos la descripción del partido
    char descripcion[constantes::TAMANO_DESCRIPCION];
    do {
        flagError = false;
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarCadena(descripcion, constantes::TAMANO_DESCRIPCION,
                                                          " Ingrese la descripción del partido (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarNombreTorneo(descripcion)) {
            std::cerr << " Error la descripción '" << descripcion << "' no es válida\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoPartido.setDescripcion(descripcion)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(1500);
    } while (flagError);

    // Pedimos la confirmación al usuario
    Formatos::limpiarPantalla();
    presentacion::funcionesInOut::ingresarDatos(confirmacion, " Confirme la programación del partido (S/N): ");

    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

        bool programado = OperacionesPartidos::programarPartido(nuevoPartido);
        Formatos::esperarTiempo(1200);
        Formatos::limpiarPantalla();

        if (!programado) {
            std::cerr << "\n Se produjo un error a la hora de programar el partido.\n";
            Formatos::pausarPrograma();
            return;
        }

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║       PARTIDO PROGRAMADO CON ÉXITO        ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " ID Asignado: " << nuevoPartido.getId() << "\n";
        std::cout << " Encuentro:   " << eqLocal.getNombre() << "  VS  " << eqVisitante.getNombre() << "\n";
        std::cout << " Fecha:       " << nuevoPartido.getFecha() << "\n";
        std::cout << " Descripción: " << nuevoPartido.getDescripcion() << "\n";
        std::cout << " Estado:      " << nuevoPartido.getEstado() << "\n";

    } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
        std::cout << "\n La programación del partido ha sido cancelada.\n";
    } else {
        std::cerr << "\n Error: Opción inválida (S/N).\nLa programación del partido ha sido cancelada.\n";
    }

    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   REGISTRAR RESULTADO
// ---------------------------------------------------------------------------------------------- //
void partidos::registrarResultado() {
    Formatos::limpiarPantalla();
    bool flagError = false;
    bool cancelado = false;
    char confirmacion;
    Partido registroPartido, partidoAux;

    // Leemos los headers para saber el numero de registros activos
    ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura de los headers fue correcta
    if (headerPartidos.getCantidadRegistros() == constantes::ERROR_INT || headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\n Error del Sistema!\n";
        std::cout << " Operación Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay partidos activos registrados
    if (headerPartidos.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún partido programado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    int idPartido = 0;

    // Recolectamos el ID del partido a registrar
    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(idPartido, " Ingrese el ID del partido a registrar (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idPartido)) {
            std::cerr << " Error el id '" << idPartido << "' no es válido\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (!GestorArchivosBinarios::buscarRegistrosPorId<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS, partidoAux, idPartido)) {
            std::cerr << " Error: El ID de partido '" << idPartido << "' no está asociado a ningún partido.\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (std::strcmp(partidoAux.getEstado(), OperacionesPartidos::obtenerEstadoPorId(0).c_str()) != 0) {
            std::cerr << " Error: El partido ya fue JUGADO/CANCELADO o no se encuentra en estado PROGRAMADO.\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

    } while (flagError);

    if (!registroPartido.setId(idPartido)) {
        std::cerr << "Error del Sistema!\n";
        Formatos::pausarPrograma();
        return;
    }

    // Buscamos los equipos involucrados
    Equipo eqLocal, eqVisitante;

    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqLocal, partidoAux.getIdEquipoLocal())) {
        std::cerr << "\n Error del Sistema!\n";
        std::cout << " Operación Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, partidoAux.getIdEquipoVisitante())) {
        std::cerr << "\n Error del Sistema!\n";
        std::cout << " Operación Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    int anotacionesLocal = 0;
    int anotacionesVisitante = 0;

    // Recolectamos las anotaciones del partido
    do {
        flagError = false;
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           REGISTRAR ANOTACIONES           ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Deporte Actual del Torneo: " << torneo.getDeporte() << "\n";
        std::cout << " Partido: " << eqLocal.getNombre() << " VS " << eqVisitante.getNombre() << "\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(anotacionesLocal, " Número de Anotaciones del Equipo Local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << " Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(500);
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           REGISTRAR ANOTACIONES           ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Deporte Actual del Torneo: " << torneo.getDeporte() << "\n";
        std::cout << " Partido: " << eqLocal.getNombre() << " VS " << eqVisitante.getNombre() << "\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(anotacionesVisitante, " Número de Anotaciones del Equipo Visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << " Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        // Verificamos que sean positivos
        if (anotacionesLocal < 0 || anotacionesVisitante < 0) {
            std::cerr << " Error: El número de anotaciones no puede ser un valor negativo.\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        // Validamos el empate
        if (anotacionesLocal == anotacionesVisitante) {
            std::string deporteActual(torneo.getDeporte());

            if (deporteActual == "BALONCESTO" || deporteActual == "TENIS" || deporteActual == "VOLEIBOL" || deporteActual == "BEISBOL" || deporteActual == "SOFTBOL") {
                std::cerr << " Error: En el deporte " << torneo.getDeporte() << " no se permiten empates. Registre el marcador final con prórroga.\n";
                flagError = true;
                Formatos::esperarTiempo(3000);
                continue;
            }
        }

        // Verificamos que el numero de anotaciones no supere el máximo permitido
        if ((anotacionesLocal + anotacionesVisitante) > constantes::MAX_ANOTACIONES) {
            std::cerr << " Error: El número de anotaciones no puede ser mayor al máximo permitido (" << constantes::MAX_ANOTACIONES << ").\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!registroPartido.setAnotacionesLocal(anotacionesLocal) || !registroPartido.setAnotacionesVisitante(anotacionesVisitante)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        // * Registramos el detalle de cada gol del equipo local
        for (int e = 0; e < anotacionesLocal; ++e) {
            Formatos::esperarTiempo(300);
            Formatos::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " ---------- Detalle de las Anotaciones del Equipo Local (" << eqLocal.getNombre() << ") ---------- \n";
            std::cout << " Anotación " << (e + 1) << "/" << anotacionesLocal << "\n";

            int minuto = 0;
            bool flagErrorDetalle = false;

            do {
                flagErrorDetalle = false;
                if (!presentacion::funcionesInOut::ingresarDatos(minuto, " Ingrese el minuto en el que se anotó (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarMinuto(minuto, constantes::MINUTO_MINIMO, constantes::MINUTO_MAXIMO)) {
                    std::cerr << " Error el minuto '" << minuto << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                }
            } while (flagErrorDetalle);

            int idJugador = 0;
            do {
                idJugador = 0;
                flagErrorDetalle = false;

                if (!presentacion::funcionesInOut::ingresarDatos(idJugador,
                                                                 " Ingrese el ID del jugador que anotó (ingrese '0' si fue autogol, 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarIdParaAutogol(idJugador)) {
                    std::cerr << " Error el ID '" << idJugador << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                // Si el id del jugador no corresponde a un autogol lo validamos
                if (idJugador != 0) {
                    Jugador jugadorAux;
                    if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador)) {
                        std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                        flagErrorDetalle = true;
                        Formatos::esperarTiempo(2000);
                        continue;
                    }

                    if (jugadorAux.getIdEquipo() != eqLocal.getId()) {
                        std::cerr << "\n Error: El jugador '" << jugadorAux.getNombre() << "' de ID '" << idJugador << "' no pertenece al equipo local (" << eqLocal.getNombre()
                                  << ")\n";
                        flagErrorDetalle = true;
                        Formatos::esperarTiempo(2000);
                        continue;
                    }
                }
            } while (flagErrorDetalle);

            Anotacion nuevaAnotacion(idJugador, minuto, "LOCAL");
            if (!registroPartido.agregarAnotacion(nuevaAnotacion)) {
                std::cerr << "Error del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }
        }

        // * Registramos el detalle de cada gol del equipo visitante
        for (int e = 0; e < anotacionesVisitante; ++e) {
            Formatos::esperarTiempo(300);
            Formatos::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " ---------- Detalle de las Anotaciones del Equipo Visitante (" << eqVisitante.getNombre() << ") ---------- \n";
            std::cout << " Anotación " << (e + 1) << "/" << anotacionesVisitante << "\n";

            int minuto = 0;
            bool flagErrorDetalle = false;

            do {
                flagErrorDetalle = false;
                if (!presentacion::funcionesInOut::ingresarDatos(minuto, " Ingrese el minuto en el que se anotó (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarMinuto(minuto, constantes::MINUTO_MINIMO, constantes::MINUTO_MAXIMO)) {
                    std::cerr << " Error el minuto '" << minuto << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                }
            } while (flagErrorDetalle);

            int idJugador = 0;
            do {
                idJugador = 0;
                flagErrorDetalle = false;

                if (!presentacion::funcionesInOut::ingresarDatos(idJugador,
                                                                 " Ingrese el ID del jugador que anotó (ingrese '0' si fue autogol, 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarIdParaAutogol(idJugador)) {
                    std::cerr << " Error el ID '" << idJugador << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                if (idJugador != 0) {
                    Jugador jugadorAux;
                    if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador)) {
                        std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                        flagErrorDetalle = true;
                        Formatos::esperarTiempo(2000);
                        continue;
                    }

                    if (jugadorAux.getIdEquipo() != eqVisitante.getId()) {
                        std::cerr << "\n Error: El jugador '" << jugadorAux.getNombre() << "' de ID '" << idJugador << "' no pertenece al equipo visitante ("
                                  << eqVisitante.getNombre() << ")\n";
                        flagErrorDetalle = true;
                        Formatos::esperarTiempo(2000);
                        continue;
                    }
                }
            } while (flagErrorDetalle);

            Anotacion nuevaAnotacion(idJugador, minuto, "VISITANTE");
            if (!registroPartido.agregarAnotacion(nuevaAnotacion)) {
                std::cerr << "Error del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }
        }

    } while (flagError);

    int tarjetasAmaLocal = 0;
    int tarjetasAmaVisitante = 0;

    // Recolectamos las tarjetas amarillas
    do {
        flagError = false;
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║             REGISTRAR TARJETAS            ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Deporte Actual del Torneo: " << torneo.getDeporte() << "\n";
        std::cout << " Partido: " << eqLocal.getNombre() << " VS " << eqVisitante.getNombre() << "\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(tarjetasAmaLocal, " Número de Tarjetas Amarillas del Equipo Local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << " Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(500);
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║             REGISTRAR TARJETAS            ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Deporte Actual del Torneo: " << torneo.getDeporte() << "\n";
        std::cout << " Partido: " << eqLocal.getNombre() << " VS " << eqVisitante.getNombre() << "\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(tarjetasAmaVisitante,
                                                         " Número de Tarjetas Amarillas del Equipo Visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << " Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (tarjetasAmaLocal < 0 || tarjetasAmaVisitante < 0) {
            std::cerr << " Error: El número de tarjetas amarillas no puede ser un valor negativo.\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if ((tarjetasAmaLocal + tarjetasAmaVisitante) > constantes::MAX_TARJETAS_AMARILLAS) {
            std::cerr << " Error: El número de tarjetas amarillas no puede ser mayor al máximo permitido (" << constantes::MAX_TARJETAS_AMARILLAS << ").\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (!registroPartido.setTarjetasAmaLocal(tarjetasAmaLocal) || !registroPartido.setTarjetasAmaVisitante(tarjetasAmaVisitante)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        // Detalle de las tarjetas amarillas del equipo local
        for (int e = 0; e < tarjetasAmaLocal; ++e) {
            Formatos::esperarTiempo(300);
            Formatos::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " ---------- Detalle de las Tarjetas Amarillas del Equipo Local (" << eqLocal.getNombre() << ") ---------- \n";
            std::cout << " Tarjeta A " << (e + 1) << "/" << tarjetasAmaLocal << "\n";

            int minuto = 0;
            bool flagErrorDetalle = false;

            do {
                flagErrorDetalle = false;
                if (!presentacion::funcionesInOut::ingresarDatos(minuto, " Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarMinuto(minuto, constantes::MINUTO_MINIMO, constantes::MINUTO_MAXIMO)) {
                    std::cerr << " Error el minuto '" << minuto << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                }
            } while (flagErrorDetalle);

            int idJugador = 0;
            do {
                idJugador = 0;
                flagErrorDetalle = false;

                if (!presentacion::funcionesInOut::ingresarDatos(idJugador, " Ingrese el ID del jugador amonestado (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarId(idJugador)) {
                    std::cerr << " Error el ID '" << idJugador << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                Jugador jugadorAux;
                if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador)) {
                    std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                if (jugadorAux.getIdEquipo() != eqLocal.getId()) {
                    std::cerr << "\n Error: El jugador '" << jugadorAux.getNombre() << "' de ID '" << idJugador << "' no pertenece al equipo local (" << eqLocal.getNombre()
                              << ")\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }
            } while (flagErrorDetalle);

            TarjetaAmarilla nuevaTarjeta(idJugador, minuto, "LOCAL");
            if (!registroPartido.agregarTarjetaAmarilla(nuevaTarjeta)) {
                std::cerr << "Error del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }
        }

        // Detalle de las tarjetas amarillas del equipo visitante
        for (int e = 0; e < tarjetasAmaVisitante; ++e) {
            Formatos::esperarTiempo(300);
            Formatos::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " ---------- Detalle de las Tarjetas Amarillas del Equipo Visitante (" << eqVisitante.getNombre() << ") ---------- \n";
            std::cout << " Tarjeta A " << (e + 1) << "/" << tarjetasAmaVisitante << "\n";

            int minuto = 0;
            bool flagErrorDetalle = false;

            do {
                flagErrorDetalle = false;
                if (!presentacion::funcionesInOut::ingresarDatos(minuto, " Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarMinuto(minuto, constantes::MINUTO_MINIMO, constantes::MINUTO_MAXIMO)) {
                    std::cerr << " Error el minuto '" << minuto << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                }
            } while (flagErrorDetalle);

            int idJugador = 0;
            do {
                idJugador = 0;
                flagErrorDetalle = false;

                if (!presentacion::funcionesInOut::ingresarDatos(idJugador, " Ingrese el ID del jugador amonestado (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarId(idJugador)) {
                    std::cerr << " Error el ID '" << idJugador << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                Jugador jugadorAux;
                if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador)) {
                    std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                if (jugadorAux.getIdEquipo() != eqVisitante.getId()) {
                    std::cerr << "\n Error: El jugador '" << jugadorAux.getNombre() << "' de ID '" << idJugador << "' no pertenece al equipo visitante (" << eqVisitante.getNombre()
                              << ")\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }
            } while (flagErrorDetalle);

            TarjetaAmarilla nuevaTarjeta(idJugador, minuto, "VISITANTE");
            if (!registroPartido.agregarTarjetaAmarilla(nuevaTarjeta)) {
                std::cerr << "Error del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }
        }

    } while (flagError);

    int tarjetasRojasLocal = 0;
    int tarjetasRojasVisitante = 0;

    // Recolectamos las tarjetas rojas
    do {
        flagError = false;
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║             REGISTRAR TARJETAS            ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Deporte Actual del Torneo: " << torneo.getDeporte() << "\n";
        std::cout << " Partido: " << eqLocal.getNombre() << " VS " << eqVisitante.getNombre() << "\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(tarjetasRojasLocal, " Número de Tarjetas Rojas del Equipo Local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << " Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(500);
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║             REGISTRAR TARJETAS            ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Deporte Actual del Torneo: " << torneo.getDeporte() << "\n";
        std::cout << " Partido: " << eqLocal.getNombre() << " VS " << eqVisitante.getNombre() << "\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(tarjetasRojasVisitante,
                                                         " Número de Tarjetas Rojas del Equipo Visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << " Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (tarjetasRojasLocal < 0 || tarjetasRojasVisitante < 0) {
            std::cerr << " Error: El número de tarjetas rojas no puede ser un valor negativo.\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if ((tarjetasRojasLocal + tarjetasRojasVisitante) > constantes::MAX_TARJETAS_ROJAS) {
            std::cerr << " Error: El número de tarjetas rojas no puede ser mayor al máximo permitido (" << constantes::MAX_TARJETAS_ROJAS << ").\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (!registroPartido.setTarjetasRojasLocal(tarjetasRojasLocal) || !registroPartido.setTarjetasRojasVisitante(tarjetasRojasVisitante)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        // Detalle de las tarjetas rojas del equipo local
        for (int e = 0; e < tarjetasRojasLocal; ++e) {
            Formatos::esperarTiempo(300);
            Formatos::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " ---------- Detalle de las Tarjetas Rojas del Equipo Local (" << eqLocal.getNombre() << ") ---------- \n";
            std::cout << " Tarjeta R " << (e + 1) << "/" << tarjetasRojasLocal << "\n";

            int minuto = 0;
            bool flagErrorDetalle = false;

            do {
                flagErrorDetalle = false;
                if (!presentacion::funcionesInOut::ingresarDatos(minuto, " Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarMinuto(minuto, constantes::MINUTO_MINIMO, constantes::MINUTO_MAXIMO)) {
                    std::cerr << " Error el minuto '" << minuto << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                }
            } while (flagErrorDetalle);

            int idJugador = 0;
            do {
                idJugador = 0;
                flagErrorDetalle = false;

                if (!presentacion::funcionesInOut::ingresarDatos(idJugador, " Ingrese el ID del jugador expulsado (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarId(idJugador)) {
                    std::cerr << " Error el ID '" << idJugador << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                Jugador jugadorAux;
                if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador)) {
                    std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                if (jugadorAux.getIdEquipo() != eqLocal.getId()) {
                    std::cerr << "\n Error: El jugador '" << jugadorAux.getNombre() << "' de ID '" << idJugador << "' no pertenece al equipo local (" << eqLocal.getNombre()
                              << ")\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }
            } while (flagErrorDetalle);

            TarjetaRoja nuevaTarjeta(idJugador, minuto, "LOCAL");
            if (!registroPartido.agregarTarjetaRoja(nuevaTarjeta)) {
                std::cerr << "Error del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }
        }

        // Detalle de las tarjetas rojas del equipo visitante
        for (int e = 0; e < tarjetasRojasVisitante; ++e) {
            Formatos::esperarTiempo(300);
            Formatos::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " ---------- Detalle de las Tarjetas Rojas del Equipo Visitante (" << eqVisitante.getNombre() << ") ---------- \n";
            std::cout << " Tarjeta R " << (e + 1) << "/" << tarjetasRojasVisitante << "\n";

            int minuto = 0;
            bool flagErrorDetalle = false;

            do {
                flagErrorDetalle = false;
                if (!presentacion::funcionesInOut::ingresarDatos(minuto, " Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarMinuto(minuto, constantes::MINUTO_MINIMO, constantes::MINUTO_MAXIMO)) {
                    std::cerr << " Error el minuto '" << minuto << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                }
            } while (flagErrorDetalle);

            int idJugador = 0;
            do {
                idJugador = 0;
                flagErrorDetalle = false;

                if (!presentacion::funcionesInOut::ingresarDatos(idJugador, " Ingrese el ID del jugador expulsado (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    Formatos::pausarPrograma();
                    return;
                }

                if (!GestorDeValidaciones::validarId(idJugador)) {
                    std::cerr << " Error el ID '" << idJugador << "' no es válido\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                Jugador jugadorAux;
                if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador)) {
                    std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }

                if (jugadorAux.getIdEquipo() != eqVisitante.getId()) {
                    std::cerr << "\n Error: El jugador '" << jugadorAux.getNombre() << "' de ID '" << idJugador << "' no pertenece al equipo visitante (" << eqVisitante.getNombre()
                              << ")\n";
                    flagErrorDetalle = true;
                    Formatos::esperarTiempo(2000);
                    continue;
                }
            } while (flagErrorDetalle);

            TarjetaRoja nuevaTarjeta(idJugador, minuto, "VISITANTE");
            if (!registroPartido.agregarTarjetaRoja(nuevaTarjeta)) {
                std::cerr << "Error del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }
        }

    } while (flagError);

    // Mostramos el marcador final para confirmar
    Formatos::limpiarPantalla();
    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
    std::cout << "       ║          RESUMEN DEL MARCADOR             ║\n";
    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
    std::cout << " " << eqLocal.getNombre() << "  " << anotacionesLocal << "  -  " << anotacionesVisitante << "  " << eqVisitante.getNombre() << "\n\n";
    std::cout << " Tarjetas Amarillas: " << tarjetasAmaLocal << " (Local) / " << tarjetasAmaVisitante << " (Visitante)\n";
    std::cout << " Tarjetas Rojas:     " << tarjetasRojasLocal << " (Local) / " << tarjetasRojasVisitante << " (Visitante)\n\n";

    presentacion::funcionesInOut::ingresarDatos(confirmacion, " ¿Está seguro de registrar este resultado definitivo? (S/N): ");
    Formatos::limpiarPantalla();

    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

        bool registrado = OperacionesPartidos::registrarResultado(registroPartido);

        if (registrado) {
            std::cout << "\n------------------------------------------------------------------------------\n";
            std::cout << "                  ¡Resultado registrado con éxito!\n";
            std::cout << "------------------------------------------------------------------------------\n";
            std::cout << " Partido ID:  " << registroPartido.getId() << "\n";
            std::cout << " Estado:      JUGADO\n";
            std::cout << " Marcador:    " << eqLocal.getNombre() << "  " << anotacionesLocal << "  -  " << anotacionesVisitante << "  " << eqVisitante.getNombre() << "\n";
            std::cout << "------------------------------------------------------------------------------\n";
        } else {
            std::cerr << "\nError: No se pudo registrar el resultado del partido.\n";
        }
    } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
        std::cout << "\nRegistro de resultado cancelado.\n";
    } else {
        std::cerr << "\nError: Opción inválida (S/N).\nRegistro de resultado cancelado.\n";
    }

    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   BUSCAR PARTIDO POR ID
// ---------------------------------------------------------------------------------------------- //
void partidos::buscarPartidoPorId() {
    Formatos::limpiarPantalla();
    bool flagError = false;
    bool cancelado = false;
    Partido partidoBuscado;

    ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    if (headerPartidos.getCantidadRegistros() == constantes::ERROR_INT || headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    if (headerPartidos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún partido programado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    int idPartido = 0;

    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           BUSCAR PARTIDO POR ID           ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(idPartido, " Ingrese el ID del partido que desea consultar (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << " Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idPartido)) {
            std::cerr << " Error el id '" << idPartido << "' no es válido\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (!GestorArchivosBinarios::buscarRegistrosPorId<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS, partidoBuscado, idPartido)) {
            std::cerr << "\n Error: El ID de partido '" << idPartido << "' no existe en el sistema.\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

    } while (flagError);

    Equipo eqLocal, eqVisitante;

    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqLocal, partidoBuscado.getIdEquipoLocal())) {
        std::cerr << "\n Error del Sistema!\n";
        Formatos::pausarPrograma();
        return;
    }

    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, partidoBuscado.getIdEquipoVisitante())) {
        std::cerr << "\n Error del Sistema!\n";
        Formatos::pausarPrograma();
        return;
    }

    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(800);
    std::cout << "\nBuscando...\n";
    Formatos::esperarTiempo(1200);
    Formatos::limpiarPantalla();

    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
    std::cout << "       ║            DETALLE DE PARTIDO             ║\n";
    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
    std::cout << "-------------------------------------------------------------\n";
    std::cout << "  ID Partido:            " << partidoBuscado.getId() << "\n";
    std::cout << "  Estado:                " << partidoBuscado.getEstado() << "\n";
    std::cout << "  Fecha:                 " << partidoBuscado.getFecha() << "\n";
    std::cout << "  Descripción:           " << partidoBuscado.getDescripcion() << "\n";
    std::cout << "-------------------------------------------------------------\n";
    std::cout << "  " << eqLocal.getNombre() << "  " << partidoBuscado.getAnotacionesLocal() << "  -  " << partidoBuscado.getAnotacionesVisitante() << "  "
              << eqVisitante.getNombre() << "\n";
    std::cout << "      (Local)                          (Visitante)\n";
    std::cout << "-------------------------------------------------------------\n";
    std::cout << "  Tarjetas Amarillas:    " << partidoBuscado.getTarjetasAmaLocal() << " (Local) / " << partidoBuscado.getTarjetasAmaVisitante() << " (Visitante)\n";
    std::cout << "  Tarjetas Rojas:        " << partidoBuscado.getTarjetasRojasLocal() << " (Local) / " << partidoBuscado.getTarjetasRojasVisitante() << " (Visitante)\n";
    std::cout << "-------------------------------------------------------------\n";

    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   LISTAR TODOS LOS PARTIDOS
// ---------------------------------------------------------------------------------------------- //
void partidos::listarTodosLosPartidos() {
    Formatos::limpiarPantalla();

    ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    if (headerPartidos.getCantidadRegistros() == constantes::ERROR_INT || headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    if (headerPartidos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún partido programado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Formatos::esperarTiempo(1000);

    std::vector<Partido> listaDePartidos = GestorArchivosBinarios::listarRegistros<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS);

    if (listaDePartidos.empty()) {
        std::cout << " No existen partidos registrados en el sistema actualmente.\n";
    } else {

        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║            LISTADO DE PARTIDOS            ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado" << "Encuentro y Resultado\n";
        std::cout << "--------------------------------------------------------------------------------\n";

        int cantidadMostrados = 0;

        for (size_t e = 0; e < listaDePartidos.size() && cantidadMostrados < constantes::MAX_RESULTADOS; ++e) {

            if (listaDePartidos[e].getEliminado()) {
                continue;
            }

            Equipo eqLocal, eqVisitante;

            if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqLocal, listaDePartidos[e].getIdEquipoLocal())) {
                std::cerr << "Error del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }

            if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, listaDePartidos[e].getIdEquipoVisitante())) {
                std::cerr << "Error del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }

            std::cout << std::left << std::setw(6) << listaDePartidos[e].getId() << std::left << std::setw(14) << listaDePartidos[e].getFecha() << std::left << std::setw(13)
                      << listaDePartidos[e].getEstado();
            std::cout << eqLocal.getNombre() << " " << listaDePartidos[e].getAnotacionesLocal() << "  -  " << listaDePartidos[e].getAnotacionesVisitante() << " "
                      << eqVisitante.getNombre() << "\n";

            ++cantidadMostrados;
        }
        std::cout << "--------------------------------------------------------------------------------\n";
        std::cout << " Total de partidos registrados en el sistema: " << cantidadMostrados << "\n";
    }

    std::cout << "\n";
    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   BUSCAR PARTIDOS POR EQUIPO
// ---------------------------------------------------------------------------------------------- //
void partidos::buscarPartidosPorEquipo() {
    Formatos::limpiarPantalla();
    bool flagError = false;
    bool cancelado = false;

    ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    if (headerPartidos.getCantidadRegistros() == constantes::ERROR_INT || headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    if (headerPartidos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún partido programado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    int idEquipo = 0;
    Equipo equipoBuscado;

    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║        HISTORIAL DE PARTIDOS POR EQUIPO   ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(idEquipo, " Ingrese el ID del equipo a consultar (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idEquipo)) {
            std::cerr << " Error el id '" << idEquipo << "' no es válido\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, idEquipo)) {
            std::cerr << "El id '" << idEquipo << "' no le pertenece a ningún equipo\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

    } while (flagError);

    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(1000);
    std::cout << "\nBuscando...\n";
    Formatos::esperarTiempo(1000);
    Formatos::limpiarPantalla();

    std::vector<Partido> listaDePartidos = OperacionesPartidos::listarPartidosPorEquipo(idEquipo);

    if (listaDePartidos.empty()) {
        std::cout << "\n El equipo '" << equipoBuscado.getNombre() << "' no tiene partidos registrados todavía.\n\n";
    } else {
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           PARTIDOS ENCONTRADOS            ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n";
        std::cout << " Historial para: " << equipoBuscado.getNombre() << "\n\n";
        std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado" << "Encuentro y Resultado\n";
        std::cout << "--------------------------------------------------------------------------------\n";

        for (size_t e = 0; e < listaDePartidos.size(); ++e) {

            Equipo eqLocal, eqVisitante;

            if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqLocal, listaDePartidos[e].getIdEquipoLocal())) {
                std::cerr << "\nError del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }

            if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, listaDePartidos[e].getIdEquipoVisitante())) {
                std::cerr << "\nError del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }

            std::cout << std::left << std::setw(6) << listaDePartidos[e].getId() << std::left << std::setw(14) << listaDePartidos[e].getFecha() << std::left << std::setw(13)
                      << listaDePartidos[e].getEstado();
            std::cout << eqLocal.getNombre() << " " << listaDePartidos[e].getAnotacionesLocal() << "  -  " << listaDePartidos[e].getAnotacionesVisitante() << " "
                      << eqVisitante.getNombre() << "\n";
        }
        std::cout << "--------------------------------------------------------------------------------\n";
        std::cout << " Total de partidos encontrados: " << listaDePartidos.size() << "\n\n";
    }

    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   LISTAR PARTIDOS POR ESTADO
// ---------------------------------------------------------------------------------------------- //
void partidos::listarPartidosPorEstado() {
    Formatos::limpiarPantalla();
    bool flagError = false;
    bool cancelado = false;
    int opcion = -1;
    std::string estado;

    ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    if (headerPartidos.getCantidadRegistros() == constantes::ERROR_INT || headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    if (headerPartidos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún partido programado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║         BUSCAR PARTIDOS POR ESTADO        ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        std::cout << " Estados de partido consultables: \n\n";
        std::cout << "---------------------------------------------\n";
        std::cout << " 0. PROGRAMADO\n 1. JUGADO\n 2. CANCELADO\n";
        std::cout << "---------------------------------------------\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(opcion, " Ingrese el tipo de estado de partido que desea consultar (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (opcion < 0 || opcion > 2) {
            mensajeDefault();
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        estado = OperacionesPartidos::obtenerEstadoPorId(opcion);

        if (estado == constantes::ERROR_STRING) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

    } while (flagError);

    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(500);
    std::cout << "Buscando...\n";
    Formatos::esperarTiempo(1200);
    Formatos::limpiarPantalla();

    std::vector<Partido> listaDePartidos = OperacionesPartidos::listarPartidosPorSuEstado(estado.c_str());

    if (listaDePartidos.empty()) {
        std::cout << "\n No se encontró ningún partido en estado '" << estado << "' actualmente.\n\n";
    } else {
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           PARTIDOS ENCONTRADOS            ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n";
        std::cout << " Partidos en estado: " << estado << "\n\n";

        std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado" << "Encuentro y Resultado\n";
        std::cout << "--------------------------------------------------------------------------------\n";

        for (size_t e = 0; e < listaDePartidos.size(); ++e) {

            Equipo eqLocal, eqVisitante;

            if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqLocal, listaDePartidos[e].getIdEquipoLocal())) {
                std::cerr << "\nError del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }

            if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, listaDePartidos[e].getIdEquipoVisitante())) {
                std::cerr << "\nError del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }

            std::cout << std::left << std::setw(6) << listaDePartidos[e].getId() << std::left << std::setw(14) << listaDePartidos[e].getFecha() << std::left << std::setw(13)
                      << listaDePartidos[e].getEstado();
            std::cout << eqLocal.getNombre() << " " << listaDePartidos[e].getAnotacionesLocal() << "  -  " << listaDePartidos[e].getAnotacionesVisitante() << " "
                      << eqVisitante.getNombre() << "\n";
        }
        std::cout << "--------------------------------------------------------------------------------\n\n";
    }

    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   CANCELAR PARTIDO
// ---------------------------------------------------------------------------------------------- //
void partidos::cancelarPartido() {
    Formatos::limpiarPantalla();
    bool flagError = false;
    bool cancelado = false;
    char confirmacion;
    int idPartido = 0;
    Partido partidoAuxiliar;
    Equipo eqLocal, eqVisitante;

    ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);

    if (headerPartidos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    if (headerPartidos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún partido programado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║             CANCELAR PARTIDOS             ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(idPartido, " Ingrese el ID del Partido que desea cancelar (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el Usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idPartido)) {
            std::cerr << " Error el id '" << idPartido << "' no es válido\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (!GestorArchivosBinarios::buscarRegistrosPorId<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS, partidoAuxiliar, idPartido)) {
            std::cerr << "\n Error: El ID '" << idPartido << "' no pertenece a ningún partido registrado.\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

        if (std::strcmp(partidoAuxiliar.getEstado(), OperacionesPartidos::obtenerEstadoPorId(2).c_str()) == 0) {
            std::cerr << "\n Error: El partido '" << idPartido << "' ya se encuentra CANCELADO.\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
            continue;
        }

    } while (flagError);

    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqLocal, partidoAuxiliar.getIdEquipoLocal())) {
        std::cerr << "\n Error del Sistema!\n";
        Formatos::pausarPrograma();
        return;
    }

    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, partidoAuxiliar.getIdEquipoVisitante())) {
        std::cerr << "\n Error del Sistema!\n";
        Formatos::pausarPrograma();
        return;
    }

    Formatos::limpiarPantalla();
    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
    std::cout << "       ║             CANCELAR PARTIDOS             ║\n";
    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

    std::cout << " Se cancelará el siguiente partido:\n\n";
    std::cout << " -----------------------------------------------\n";
    std::cout << " ID:        " << partidoAuxiliar.getId() << "\n";
    std::cout << " Encuentro: " << eqLocal.getNombre() << "  " << partidoAuxiliar.getAnotacionesLocal() << "  -  " << partidoAuxiliar.getAnotacionesVisitante() << "  "
              << eqVisitante.getNombre() << "\n";
    std::cout << " Fecha:     " << partidoAuxiliar.getFecha() << "\n";
    std::cout << " Estado:    " << partidoAuxiliar.getEstado() << "\n";
    std::cout << " -----------------------------------------------\n\n";

    presentacion::funcionesInOut::ingresarDatos(confirmacion, " ¿Está seguro de cancelar este partido? (S/N): ");
    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(1000);

    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

        if (OperacionesPartidos::cancelarPartido(idPartido)) {
            std::cout << "\n------------------------------------------------------------------------------\n";
            std::cout << "           Partido Cancelado con Éxito\n";
            std::cout << "------------------------------------------------------------------------------\n";
        } else {
            std::cerr << "\n Error: No se pudo cancelar el Partido debido a un Error del Sistema.\n";
        }

    } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
        std::cout << " Operación Cancelada!\n";
    } else {
        std::cerr << " Error: No se ingresó una opción válida.\n Operación Cancelada!\n";
    }

    Formatos::pausarPrograma();
}
