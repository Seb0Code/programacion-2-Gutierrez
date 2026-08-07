#include "../../include/ui/MenusReportes.hpp"

void MenuReportes::tablaTop10Anotadores() {

    // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
    ArchivoHeader headerJugadores = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerJugadores.getCantidadRegistros() == constantes::ERROR_INT || headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    std::vector<Jugador> listaDeJugadores = GestorReportes::tablaDeGoleadoresTop10();

    // Pedimos a la lógica los jugadores ordenados por goles (usa la función existente)


    if (listaDeJugadores.size() <= 0) {
        std::cout << "\n No hay jugadores disponibles \n";
        Formatos::pausarPrograma();
        return;
    }

    int limite = (listaDeJugadores.size() > 10) ? 10 : listaDeJugadores.size();

    // Mostramos la tabla de posiciones
    std::string nom_torneo(torneo.getNombre());
    Formatos::convertirTextoAMayus(nom_torneo);
    std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                             TABLA DE GOLEADORES TOP 10                                                      ║\n";
    std::cout << "║               " << std::left << std::setw(93) << nom_torneo << " ║\n";
    std::cout << "╠════╦══════════════════════════════════════════╦═════╦═════════════════════════╦═══════════╦═════╦════╦══════╣\n";
    std::cout << "║ #  ║ Jugadores                                ║ ID  ║    Nombre del Equipo    ║ ID Equipo ║  A  ║ PJ ║ Prom ║\n";
    std::cout << "╠════╬══════════════════════════════════════════╬═════╬═════════════════════════╬═══════════╬═════╬════╬══════╣\n";

    for (int e = 0; e < limite; e++) {
        // Buscamos el equipo al que pertenece
        Equipo equipoAuxiliar;
        bool existe = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, listaDeJugadores[e].getIdEquipo());
        if (!existe) {
            std::cerr << "\nError del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }
        float promedio = (((float)(listaDeJugadores[e].getNumAnotaciones())) / ((float)(equipoAuxiliar.getJugados())));
        std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(40) << listaDeJugadores[e].getNombre() << " ║ " << std::left << std::setw(3)
                  << listaDeJugadores[e].getId() << " ║ " << std::left << std::setw(23) << equipoAuxiliar.getNombre() << " ║ " << std::left << std::setw(9)
                  << listaDeJugadores[e].getIdEquipo() << " ║ " << std::left << std::setw(3) << listaDeJugadores[e].getNumAnotaciones() << " ║ " << std::right << std::setw(2)
                  << equipoAuxiliar.getJugados() << " ║ " << std::left << std::setw(4) << ((promedio == 0.0) ? 0.0 : promedio) << " ║ \n";
    }
    std::cout << "╚════╩══════════════════════════════════════════╩═════╩═════════════════════════╩═══════════╩═════╩════╩══════╝\n";
    std::cout << "\nReferencia: A = Anotaciones  PJ = PartidosJugados  Prom=Promedio\n\n";
    Formatos::pausarPrograma();
}

void MenuReportes::tablaTop10TarjetasAmarillas() {
    // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
    ArchivoHeader headerJugadores = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerJugadores.getCantidadRegistros() == constantes::ERROR_INT || headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    std::vector<Jugador> listaDeJugadores = GestorReportes::tablaDeTarjetasAmarillasTop10();

    int limite = (listaDeJugadores.size() > 10) ? 10 : listaDeJugadores.size();

    // Mostramos la tabla de posiciones
    std::string nom_torneo(torneo.getNombre());
    Formatos::convertirTextoAMayus(nom_torneo);
    std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                          TABLA DE TARJETAS AMARILLAS TOP 10                                                 ║\n";
    std::cout << "║               " << std::left << std::setw(93) << nom_torneo << " ║\n";
    std::cout << "╠════╦══════════════════════════════════════════╦═════╦═════════════════════════╦═══════════╦═════╦════╦══════╣\n";
    std::cout << "║ #  ║ Jugadores                                ║ ID  ║    Nombre del Equipo    ║ ID Equipo ║  T  ║ PJ ║ Prom ║\n";
    std::cout << "╠════╬══════════════════════════════════════════╬═════╬═════════════════════════╬═══════════╬═════╬════╬══════╣\n";

    for (int e = 0; e < limite; e++) {
        // Buscamos el equipo al que pertenece
        Equipo equipoAuxiliar;
        bool existe = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, listaDeJugadores[e].getIdEquipo());
        if (!existe) {
            std::cerr << "\nError del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }
        float promedio = (((float)(listaDeJugadores[e].getTarjetaAmarillas())) / ((float)(equipoAuxiliar.getJugados())));
        std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(40) << listaDeJugadores[e].getNombre() << " ║ " << std::left << std::setw(3)
                  << listaDeJugadores[e].getId() << " ║ " << std::left << std::setw(23) << equipoAuxiliar.getNombre() << " ║ " << std::left << std::setw(9)
                  << listaDeJugadores[e].getIdEquipo() << " ║ " << std::left << std::setw(3) << listaDeJugadores[e].getTarjetaAmarillas() << " ║ " << std::right << std::setw(2)
                  << equipoAuxiliar.getJugados() << " ║ " << std::left << std::setw(4) << ((promedio == 0.0) ? 0.0 : promedio) << " ║ \n";
    }
    std::cout << "╚════╩══════════════════════════════════════════╩═════╩═════════════════════════╩═══════════╩═════╩════╩══════╝\n";
    std::cout << "\nReferencia: T = Numero de Tarjetas  PJ = PartidosJugados  Prom=Promedio\n\n";
    Formatos::pausarPrograma();
}

void MenuReportes::tablaTop10TarjetasRojas() {

    // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
    ArchivoHeader headerJugadores = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerJugadores.getCantidadRegistros() == constantes::ERROR_INT || headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    std::vector<Jugador> listaDeJugadores = GestorReportes::tablaDeTarjetasAmarillasTop10();

    int limite = (listaDeJugadores.size() > 10) ? 10 : listaDeJugadores.size();

    // Mostramos la tabla de posiciones
    std::string nom_torneo(torneo.getNombre());
    Formatos::convertirTextoAMayus(nom_torneo);
    std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                   TABLA DE TARJETAS ROJAS TOP 10                                            ║\n";
    std::cout << "║               " << std::left << std::setw(93) << nom_torneo << " ║\n";
    std::cout << "╠════╦══════════════════════════════════════════╦═════╦═════════════════════════╦═══════════╦═════╦════╦══════╣\n";
    std::cout << "║ #  ║ Jugadores                                ║ ID  ║    Nombre del Equipo    ║ ID Equipo ║  T  ║ PJ ║ Prom ║\n";
    std::cout << "╠════╬══════════════════════════════════════════╬═════╬═════════════════════════╬═══════════╬═════╬════╬══════╣\n";

    for (int e = 0; e < limite; e++) {
        // Buscamos el equipo al que pertenece
        Equipo equipoAuxiliar;
        bool existe = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, listaDeJugadores[e].getIdEquipo());
        if (!existe) {
            std::cerr << "\nError del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }
        float promedio = (((float)(listaDeJugadores[e].getTarjetaAmarillas())) / ((float)(equipoAuxiliar.getJugados())));
        std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(40) << listaDeJugadores[e].getNombre() << " ║ " << std::left << std::setw(3)
                  << listaDeJugadores[e].getId() << " ║ " << std::left << std::setw(23) << equipoAuxiliar.getNombre() << " ║ " << std::left << std::setw(9)
                  << listaDeJugadores[e].getIdEquipo() << " ║ " << std::left << std::setw(3) << listaDeJugadores[e].getNumAnotaciones() << " ║ " << std::right << std::setw(2)
                  << equipoAuxiliar.getJugados() << " ║ " << std::left << std::setw(4) << ((promedio == 0.0) ? 0.0 : promedio) << " ║ \n";
    }
    std::cout << "╚════╩══════════════════════════════════════════╩═════╩═════════════════════════╩═══════════╩═════╩════╩══════╝\n";
    std::cout << "\nReferencia: T = Numero de Tarjetas  PJ = PartidosJugados  Prom=Promedio\n\n";
    Formatos::pausarPrograma();
}

void MenuReportes::fichaTecnica() {
    int error = -1;
    bool flagError = false;
    bool encontrado = false;
    bool cancelado = false;
    // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
    ArchivoHeader headerJugadores = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerJugadores.getCantidadRegistros() == constantes::ERROR_INT || headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << " No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    int idPartido = 0;
    Partido partidoBuscado;

    do {
        flagError = false;
        Formatos::limpiarPantalla();

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║               FICHA TECNICA               ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";


        // Recolectamos el ID
        if (!presentacion::funcionesInOut::ingresarDatos(idPartido, " Ingrese el ID del partido que desea consultar (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << " Operación Cancelada por el Usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idPartido)) {
            std::cerr << "\n Error: El ID de partido '" << idPartido << "' no es valido.\n";
            Formatos::esperarTiempo(2000);
            flagError = true;
            continue;
        }

        Formatos::limpiarPantalla();
        Formatos::esperarTiempo(500);

        // Si no existe, avisamos y salimos
        if (!GestorArchivosBinarios::buscarRegistrosPorId<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS, partidoBuscado, idPartido)) {
            std::cerr << "\n Error: El ID de partido '" << idPartido << "' no existe en el sistema.\n";
            Formatos::esperarTiempo(2000);
            flagError = true;
            continue;
        }
        Formatos::esperarTiempo(1200);
    } while (flagError);

    Equipo eqLocal, eqVisitante;

    // Buscamos el Local


    // Si no existe, avisamos y salimos
    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqLocal, partidoBuscado.getIdEquipoLocal()) ||
        !GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, partidoBuscado.getIdEquipoVisitante())) {
        std::cerr << "\n Error del Sistema!\n";
        Formatos::pausarPrograma();
        return;
    }

    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(500);
    std::cout << "\n Cargando Ficha Técnica... \n" << std::endl;
    Formatos::esperarTiempo(1200);
    Formatos::limpiarPantalla();

    // Mostrar ficha tecnica

    std::string nom_torneo(torneo.getNombre());
    Formatos::convertirTextoAMayus(nom_torneo);
    // Imprimimos la ficha con más detalle: goles, tarjetas y nombre del torneo
    std::cout << "\n╔════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                      FICHA TÉCNICA DEL PARTIDO                         ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Torneo: " << std::left << std::setw(60) << nom_torneo << "   ║\n";
    std::cout << "║ Partido ID: " << std::left << std::setw(10) << partidoBuscado.getId() << " Fecha: " << std::left << std::setw(14) << partidoBuscado.getFecha()
              << " Estado: " << std::left << std::setw(12) << partidoBuscado.getEstado() << "    ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ " << std::left << std::setw(28) << eqLocal.getNombre() << std::right << std::setw(3) << partidoBuscado.getAnotacionesLocal() << "  -  " << std::setw(3)
              << partidoBuscado.getAnotacionesVisitante() << "   " << std::left << std::setw(28) << eqVisitante.getNombre() << " ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Notas: " << std::left << std::setw(63) << partidoBuscado.getDescripcion() << " ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ GOLES:                                                                 ║\n";
    if (partidoBuscado.getNumAnotaciones() <= 0) {
        std::cout << "║   Ningun gol registrado                                                  ║\n";
    } else {
        for (int e = 0; e < partidoBuscado.getNumAnotaciones(); ++e) {
            const Anotacion *anotaciones = partidoBuscado.getAnotaciones();
            // Obtener nombre del jugador si existe
            char nombreJugador[constantes::TAMANO_NOMBRE] = "Jugador desconocido";
            if (anotaciones[e].getIdJugador() > 0) {
                Jugador jugadorAux;
                if (GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, anotaciones[e].getIdJugador())) {
                    std::strncpy(nombreJugador, jugadorAux.getNombre(), constantes::TAMANO_NOMBRE - 1);
                    nombreJugador[constantes::TAMANO_NOMBRE - 1] = '\0';
                }
            } else if (anotaciones[e].getIdJugador() == 0) {
                std::strncpy(nombreJugador, "AUTOGOL", constantes::TAMANO_NOMBRE);
            }
            std::string etiqueta = "[" + std::string(anotaciones[e].getEquipo()) + "]";
            std::cout << "║  " << std::left << std::setw(12) << etiqueta << " Min. " << std::right << std::setw(3) << anotaciones[e].getMinuto() << " - " << std::left
                      << std::setw(45) << nombreJugador << "║\n";
        }
    }
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ TARJETAS AMARILLAS:                                                     ║\n";
    if (partidoBuscado.getNumTarjetaAma() <= 0) {
        std::cout << "║   Ninguna tarjeta amarilla registrada                                   ║\n";
    } else {
        for (int e = 0; e < partidoBuscado.getNumTarjetaAma(); ++e) {
            TarjetaAmarilla tarjetaAma = partidoBuscado.getTarjetasAmarillas()[e];
            char nombreJugador[constantes::TAMANO_NOMBRE] = "Jugador desconocido";
            if (tarjetaAma.getIdJugador() > 0) {
                Jugador jugadorAux;
                if (GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, tarjetaAma.getIdJugador())) {
                    std::strncpy(nombreJugador, jugadorAux.getNombre(), constantes::TAMANO_NOMBRE - 1);
                    nombreJugador[constantes::TAMANO_NOMBRE - 1] = '\0';
                }
            }
            std::string etiqueta = "[" + std::string(tarjetaAma.getEquipo()) + "]";
            std::cout << "║  " << std::left << std::setw(10) << etiqueta << " Min. " << std::right << std::setw(3) << tarjetaAma.getMinuto() << " - " << std::left << std::setw(44)
                      << nombreJugador << "║\n";
        }
    }
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ TARJETAS ROJAS:                                                         ║\n";
    if (partidoBuscado.getNumTarjetasRojas() <= 0) {
        std::cout << "║   Ninguna tarjeta roja registrada                                       ║\n";
    } else {
        for (int e = 0; e < partidoBuscado.getNumTarjetasRojas(); ++e) {
            TarjetaRoja tarjetaRoja = partidoBuscado.getTarjetasRojas()[e];
            char nombreJugador[constantes::TAMANO_NOMBRE] = "Jugador desconocido";
            if (tarjetaRoja.getIdJugador() > 0) {
                Jugador jugadorAux;
                if (GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, tarjetaRoja.getIdJugador())) {
                    std::strncpy(nombreJugador, jugadorAux.getNombre(), constantes::TAMANO_NOMBRE - 1);
                    nombreJugador[constantes::TAMANO_NOMBRE - 1] = '\0';
                }
            }
            std::string etiqueta = "[" + std::string(tarjetaRoja.getEquipo()) + "]";
            std::cout << "║  " << std::left << std::setw(10) << etiqueta << " Min. " << std::right << std::setw(3) << tarjetaRoja.getMinuto() << " - " << std::left << std::setw(44)
                      << nombreJugador << "║\n";
        }
    }
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝\n\n";

    Formatos::pausarPrograma();
}
