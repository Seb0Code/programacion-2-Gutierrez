#include "../../include/ui/MenusJugadores.hpp"

void jugadores::mensajeDefault() { std::cout << "Opción inválida.\n"; }

std::vector<std::string> jugadores::obtenerPosicionesDisponiblesParaDeporte(const char *deporte) {
    std::vector<std::string> posicionesDisponibles;
    std::vector<std::vector<std::string>> listaDePosiciones = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_POSICIONES_DEPORTES);

    for (const auto &fila : listaDePosiciones) {
        if (fila.size() > 3 && std::strcmp(fila[2].c_str(), deporte) == 0) {
            posicionesDisponibles.push_back(fila[3]);
        }
    }

    return posicionesDisponibles;
}

void jugadores::registrarJugador() {
    Formatos::limpiarPantalla();
    int error = -1;
    bool flagError = false;
    char confirmacion;
    bool cancelado = false;
    int opcion = 0;

    // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Registro Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos activos registrados
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Leemos el torneo
    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    // Variables de informacion
    int idEquipo;
    char nombre[constantes::TAMANO_NOMBRE];
    int edad;
    char posicion[constantes::TAMANO_POSICION];
    char cedula[constantes::TAMANO_CEDULA];
    char fechaRegistro[constantes::TAMANO_FECHA];
    int numeroDorsal = 0;
    Jugador nuevoJugador;

    // Recolectamos el ID del equipo
    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        if (!presentacion::funcionesInOut::ingresarDatos(idEquipo, "Ingrese el ID del equipo al que pertenece el jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\nRegistro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idEquipo)) {
            std::cout << "\n Error, el id '" << idEquipo << "' no es valido\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        Equipo equipoAuxiliar;

        /// Si el ID no existe dentro de los equipos
        if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, idEquipo)) {
            std::cout << "Error el ID '" << idEquipo << "' no pertenece a ningun equipo\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoJugador.setIdEquipo(idEquipo)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(1500);
    } while (flagError);

    // Recolectamos el nombre del Jugador
    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO JUGADOR           ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        if (!presentacion::funcionesInOut::ingresarCadena(nombre, constantes::TAMANO_NOMBRE, "Ingrese el nombre del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\nRegistro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarNombreOApellido(nombre)) {
            std::cerr << "Error el nombre '" << nombre << "' no es valido";
            Formatos::pausarPrograma();
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        // Validamos nombre duplicado
        if (OperacionesJugadores::cadenaDuplicada(nombre, &Jugador::getNombre, false, -1)) {
            std::cout << "Error, el nombre '" << nombre << "' ya está en uso.\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoJugador.setNombre(nombre)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }
        Formatos::esperarTiempo(2000);
    } while (flagError);

    do {
        flagError = false;

        // Recolectamos la Edad
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        if (!presentacion::funcionesInOut::ingresarDatos(edad, "Ingrese la edad del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Registro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarEdad(edad)) {
            std::cerr << "Error, la edad ingresada '" << edad << "' no es valida";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoJugador.setEdad(edad)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

    } while (flagError);

    Formatos::esperarTiempo(1500);

    // Recolectamos la cedula
    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        if (!presentacion::funcionesInOut::ingresarCadena(cedula, constantes::TAMANO_CEDULA, "Ingrese la cedula del jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Registro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarCedula(cedula)) {
            std::cerr << "Error, la cedula '" << cedula << "' no es valida";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        // Validamos cedula duplicado
        if (OperacionesJugadores::cadenaDuplicada(cedula, &Jugador::getCedula)) {
            std::cout << " Error, la cedula '" << cedula << "' ya le pertenece a otro jugador\n";
            flagError = true;
            Formatos::esperarTiempo(3000);
            continue;
        }

        if (!nuevoJugador.setCedula(cedula)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(2000);
    } while (flagError);

    // Recolectamos la Posicion
    do {
        flagError = false;
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Deporte actual del Torneo: " << torneo.getDeporte() << "\n\n";

        std::vector<std::string> posicionesDisponibles = obtenerPosicionesDisponiblesParaDeporte(torneo.getDeporte());
        if (posicionesDisponibles.empty()) {
            std::cerr << "\nError del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        std::cout << "\n Seleccione la posición del jugador:\n";
        for (size_t j = 0; j < posicionesDisponibles.size(); ++j) {
            std::cout << " " << (j + 1) << ". " << posicionesDisponibles[j] << "\n";
        }
        std::cout << "\n";

        if (!presentacion::funcionesInOut::ingresarDatos(opcion, "Seleccione una opción (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\nRegistro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (opcion >= 1 && opcion <= static_cast<int>(posicionesDisponibles.size())) {
            std::memset(posicion, '\0', sizeof(posicion));
            std::strncpy(posicion, posicionesDisponibles[opcion - 1].c_str(), constantes::TAMANO_POSICION - 1);
            posicion[constantes::TAMANO_POSICION - 1] = '\0';
        } else {
            std::cerr << "Error: Opción inválida. Por favor, intente nuevamente.\n";
            flagError = true;
            Formatos::esperarTiempo(2000);
        }

        if (!flagError && !nuevoJugador.setPosicion(posicion)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

    } while (flagError);


    do {
        flagError = false;
        // Recolectamos la fecha de registro del jugador
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Fecha de Inicio del Torneo: " << torneo.getFechaInicio() << std::endl;
        if (!presentacion::funcionesInOut::ingresarCadena(fechaRegistro, constantes::TAMANO_FECHA,
                                                          " Ingrese la fecha de Registro del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Registro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarFechaDeRegistroDeJugadorOEquipo(fechaRegistro, torneo.getFechaInicio())) {
            std::cerr << " Error la fecha '" << fechaRegistro << "' ingresada no es valida \n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoJugador.setFechaRegistro(fechaRegistro)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(2000);
        Formatos::limpiarPantalla();
    } while (flagError);

    // Recolectamos el dorsal del Jugador
    do {
        flagError = false;
        Equipo equipoBuscado;
        GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, idEquipo);
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(numeroDorsal, "Ingrese el Dorsal del jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\nRegistro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarDorsal(numeroDorsal)) {
            std::cerr << " Error el dorsal '" << numeroDorsal << "' no es valido\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (OperacionesJugadores::esDorsalDuplicado(numeroDorsal, idEquipo)) {
            std::cout << " Error el dorsal '" << numeroDorsal << "' ya está ocupado en el equipo '" << equipoBuscado.getNombre() << "'.\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoJugador.setNumeroDorsal(numeroDorsal)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(2000);
        Formatos::limpiarPantalla();
    } while (flagError);

    // Pedimos la confirmacion al usuario
    Formatos::limpiarPantalla();
    presentacion::funcionesInOut::ingresarDatos(confirmacion, "¿Está seguro de que desea registrar este jugador? (S/N): ");

    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
        // agregamos el jugador
        bool registrado = OperacionesJugadores::registrarJugador(nuevoJugador);
        Formatos::esperarTiempo(1200);
        Formatos::limpiarPantalla();
        // Si no se agregó
        if (!registrado) {
            std::cerr << "Error al registrar al jugador.\n";
            Formatos::pausarPrograma();
            return;
        }

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║       JUGADOR REGISTRADO CON ÉXITO        ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Torneo: " << torneo.getNombre();
        std::cout << "\n ID del Jugador: " << nuevoJugador.getId();
        std::cout << "\n Nombre del Jugador: " << nuevoJugador.getNombre();
        std::cout << "\n Cédula: " << nuevoJugador.getCedula();
        std::cout << "\n Edad: " << nuevoJugador.getEdad() << " años";
        std::cout << "\n Posición: " << nuevoJugador.getPosicion();
        std::cout << "\n Dorsal: " << nuevoJugador.getNumeroDorsal();
        std::cout << "\n ID del Equipo asignado: " << nuevoJugador.getIdEquipo();
        std::cout << "\n Fecha de Registro: " << nuevoJugador.getFechaRegistro();
    } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
        std::cout << " Registro de Jugador Cancelado.\n";
    } else {
        std::cerr << " ERROR: Opción incorrecta (S/N).\nRegistro de Jugador Cancelado.\n";
    }
    Formatos::pausarPrograma();
}

void jugadores::buscarJugadorPorId() {
    Formatos::limpiarPantalla();
    int id = 0;
    bool flagError = false;
    bool encontrado = false;
    bool cancelado = false;
    Jugador jugadorBuscado;

    // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
    ArchivoHeader headerJugadores = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerJugadores.getCantidadRegistros() == constantes::ERROR_INT | headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    do {
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║       BUSQUEDA DE JUGADORES POR ID        ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(id, "Ingrese el ID (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\nRegistro Cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(id)) {
            std::cerr << "Error el id '" << id << "' no es valido\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

    } while (flagError);

    Formatos::esperarTiempo(800);
    Formatos::limpiarPantalla();
    std::cout << "\nBuscando...\n";
    Formatos::esperarTiempo(1500);


    // si no encontro un jugador
    if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorBuscado, id)) {
        std::cerr << "Error no hay ningun jugador registrado con el ID '" << id << "'\n";
    } else {

        // Buscamos el equipo del jugador
        Equipo equipoBuscado;

        // Verificamos que el equipo fue enoncontrado
        if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, jugadorBuscado.getIdEquipo())) {
            std::cout << "\nError del Sistema!\n";
            std::cout << "Busqueda Cancelada\n";
            Formatos::pausarPrograma();
            return;
        }

        std::string nom_torneo(torneo.getNombre());

        // Listamos los datos
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║            JUGADOR ENCONTRADO             ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        std::cout << "-------------------------------------------------------------\n";
        std::cout << "  Torneo:                 " << Formatos::convertirTextoAMayus(nom_torneo) << "\n";
        std::cout << "  Deporte:                " << torneo.getDeporte() << "\n";
        std::cout << "-------------------------------------------------------------\n";
        std::cout << "  Informacion del Jugador: \n";
        std::cout << "    ID del Jugador:       " << jugadorBuscado.getId() << "\n";
        std::cout << "    Nombre:               " << jugadorBuscado.getNombre() << "\n";
        std::cout << "    Cédula:               " << jugadorBuscado.getCedula() << "\n";
        std::cout << "    Edad:                 " << jugadorBuscado.getEdad() << " años \n";
        std::cout << "    Posición:             " << jugadorBuscado.getPosicion() << "\n";
        std::cout << "    Dorsal:               " << jugadorBuscado.getNumeroDorsal() << "\n";
        std::cout << "    Fecha de Registro:    " << jugadorBuscado.getFechaRegistro() << "\n";
        std::cout << "-------------------------------------------------------------\n";
        std::cout << "  Informacion del Equipo: \n";
        std::cout << "    ID Equipo:            " << jugadorBuscado.getIdEquipo() << "\n";
        std::cout << "    Equipo:               " << equipoBuscado.getNombre() << "\n";
        std::cout << "-------------------------------------------------------------\n";
        std::cout << "  Estadísticas en el Torneo:\n";
        std::cout << "    Anotaciones:          " << jugadorBuscado.getNumAnotaciones() << "\n";
        std::cout << "    Tarjetas Amarillas:   " << jugadorBuscado.getTarjetaAmarillas() << "\n";
        std::cout << "    Tarjetas Rojas:       " << jugadorBuscado.getTarjetasRojas() << "\n";
        std::cout << "-------------------------------------------------------------\n";
    }
    Formatos::pausarPrograma();
}

void jugadores::buscarJugadorPorNombre() {
    Formatos::limpiarPantalla();
    char subcadena[constantes::TAMANO_NOMBRE];
    int cantJugadoresEncontrados = 0;
    bool cancelado = false;
    bool flagError = false;
    bool encontrado = false;

    // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
    ArchivoHeader headerJugadores = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerJugadores.getCantidadRegistros() == constantes::ERROR_INT | headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    do {
        flagError = false;

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║      BÚSQUEDA DE JUGADORES POR NOMBRE     ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarCadena(subcadena, 100,
                                                          "Escribe el nombre (o parte del nombre) del jugador que buscas (escribe 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\nRegistro Cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarNombreOApellido(subcadena)) {
            std::cerr << "Error la subcadena '" << subcadena << "' no es una subcadena valida\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

    } while (flagError);

    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(1200);
    std::cout << "Buscando..." << std::endl;
    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(500);

    // llamamos a la funcion buscar por nombre y almacenamos el resultado
    std::vector<Jugador> listaDeJugadoresSubcadena = GestorArchivosBinarios::buscarRegistrosPorSubcadena<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, subcadena);

    // Si no encontró nada
    if (listaDeJugadoresSubcadena.empty()) {
        std::cout << "\n No se encontraron jugadores que coincidan con '" << subcadena << "'.\n";
    } else {

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          COINCIDENCIAS ENCONTRADAS        ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Se encontraron " << listaDeJugadoresSubcadena.size() << " jugador(es):\n";

        for (int e = 0; e < listaDeJugadoresSubcadena.size(); e++) {

            // Buscamos el nombre del Equipo
            Equipo equipoBuscado;
            if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, listaDeJugadoresSubcadena[e].getIdEquipo())) {
                std::cerr << "Error del Sistema!";
                std::cout << "Busqueda Cancelada";
                Formatos::pausarPrograma();
                return;
            }

            std::cout << "-------------------------------------------------------------\n";
            std::cout << "  ID:                    " << listaDeJugadoresSubcadena[e].getId() << "\n";
            std::cout << "  Nombre:                " << listaDeJugadoresSubcadena[e].getNombre() << "\n";
            std::cout << "  Cédula:                " << listaDeJugadoresSubcadena[e].getCedula() << "\n";
            std::cout << "  Dorsal:                [" << listaDeJugadoresSubcadena[e].getNumeroDorsal() << "]\n";
            std::cout << "  Edad:                  " << listaDeJugadoresSubcadena[e].getEdad() << " años \n";
            std::cout << "  Posición:              " << listaDeJugadoresSubcadena[e].getPosicion() << "\n";
            std::cout << "  Nombre del Equipo:     " << equipoBuscado.getNombre() << "\n";
            std::cout << "  ID Equipo:             " << listaDeJugadoresSubcadena[e].getIdEquipo() << "\n";
            std::cout << "  Anotaciones:           " << listaDeJugadoresSubcadena[e].getNumAnotaciones() << "\n";
            std::cout << "  Tarjetas Amarillas:    " << listaDeJugadoresSubcadena[e].getTarjetaAmarillas() << "\n";
            std::cout << "  Tarjetas Rojas:    " << listaDeJugadoresSubcadena[e].getTarjetasRojas() << "\n";
        }
        std::cout << "-------------------------------------------------------------\n";
    }

    std::cout << "\n";
    Formatos::pausarPrograma();
}

void jugadores::mostrarJugadoresPorEquipo() {
    Formatos::limpiarPantalla();
    int idEquipo = 0;
    bool flagError = false;
    bool cancelado = false;

    // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
    ArchivoHeader headerJugadores = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerJugadores.getCantidadRegistros() == constantes::ERROR_INT | headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    do {
        flagError = false;
        // Pedimos el ID del equipo a consultar
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║      MOSTRAR JUGADORES POR EQUIPO         ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(idEquipo, "Ingrese el ID del Equipo (ingresa 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idEquipo)) {
            std::cerr << " Error el id '" << idEquipo << "' no es valido\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        // Buscamos el equipo primero
        Equipo equipoBuscado;
        if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, idEquipo)) {
            std::cerr << "\n Error: El equipo con ID '" << idEquipo << "' no existe.\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

    } while (flagError);

    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(1000);
    std::cout << "\n Buscando jugadores...\n\n";

    std::vector<Jugador> listaJugadoresPorEquipo = OperacionesJugadores::listarJugadoresPorEquipo(idEquipo);

    /*
            if (listaJugadoresPorEquipo.empty()) {
                std::cerr << "\n Error del Sistema! \n";
                std::cout << " Operacion Cancelada \n";
                Formatos::pausarPrograma();
                return;
            }*/

    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(1000);

    Equipo equipoBuscado;
    GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, idEquipo);

    // Si no obtenemos nada
    if (listaJugadoresPorEquipo.size() == 0) {
        std::cout << "El equipo '" << equipoBuscado.getNombre() << "' actualmente no tiene jugadores registrados.\n";
    } else {
        std::cout << "╔═════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║ EQUIPO: " << std::left << std::setw(71) << equipoBuscado.getNombre() << " ║\n";
        std::cout << "║ ID DEL EQUIPO: " << std::left << std::setw(64) << equipoBuscado.getId() << " ║\n";
        std::cout << "╠════╦══════════════════════════════════════════╦═══════════════╦═════╦═══════════╣\n";
        std::cout << "║ ID ║ Nombre                                   ║ Posición      ║ Edad║ Dorsal    ║\n";
        std::cout << "╠════╬══════════════════════════════════════════╬═══════════════╬═════╬═══════════╣\n";

        // Imprimimos los jugadores
        for (int e = 0; e < listaJugadoresPorEquipo.size(); ++e) {
            std::cout << "║ " << std::right << std::setw(2) << listaJugadoresPorEquipo[e].getId() << " ║ " << std::left << std::setw(40) << listaJugadoresPorEquipo[e].getNombre()
                      << " ║ " << std::left << std::setw(13) << listaJugadoresPorEquipo[e].getPosicion() << " ║ " << std::right << std::setw(3)
                      << listaJugadoresPorEquipo[e].getEdad() << " ║ [" << std::right << std::setw(2) << listaJugadoresPorEquipo[e].getNumeroDorsal() << "]      ║\n";
        }
        std::cout << "╚════╩══════════════════════════════════════════╩═══════════════╩═════╩═══════════╝\n";
        std::cout << " Total de jugadores en el equipo: " << listaJugadoresPorEquipo.size() << "\n";
    }

    std::cout << "\n";
    Formatos::pausarPrograma();
}

void jugadores::mostrarListaDeJugadores() {
    Formatos::limpiarPantalla();
    bool encontrado = false;

    // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
    ArchivoHeader headerJugadores = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerJugadores.getCantidadRegistros() == constantes::ERROR_INT | headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    Formatos::esperarTiempo(1000);

    // Llamamos a la función lógica
    std::vector<Jugador> listaDeJugadores = GestorArchivosBinarios::listarRegistros<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES);

    // Validamos si el sistema tiene jugadores cargados
    if (listaDeJugadores.empty()) {
        std::cout << " No existen jugadores registrados en el sistema actualmente.\n";
    } else {

        std::cout << " Cargando todos los jugadores...\n\n";
        Formatos::limpiarPantalla();
        Formatos::esperarTiempo(1000);

        std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║ SPORT G&C TOURNAMENTS                                                             ║\n";
        std::cout << "║ TORNEO: " << std::left << std::setw(73) << torneo.getNombre() << " ║\n";
        std::cout << "║ LISTA DE JUGADORES REGISTRADOS                                                    ║\n";
        std::cout << "╠════╦══════════════════════╦══════════════════════╦═══════════════╦═════╦══════════╣\n";
        std::cout << "║ ID ║ Nombre               ║ Equipo               ║ Posición      ║ Edad║ Dorsal   ║\n";
        std::cout << "╠════╬══════════════════════╬══════════════════════╬═══════════════╬═════╬══════════╣\n";

        // Imprimimos cada jugador en el sistema
        for (int e = 0; e < listaDeJugadores.size(); e++) {

            // Buscamos el equipo en cada iteracion
            Equipo equipoBuscado;
            if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, listaDeJugadores[e].getIdEquipo())) {
                Formatos::limpiarPantalla();
                std::cout << "Error del Sistema!\n";
                Formatos::pausarPrograma();
                return;
            }

            std::cout << "║ " << std::right << std::setw(2) << listaDeJugadores[e].getId() << " ║ " << std::left << std::setw(20) << listaDeJugadores[e].getNombre() << " ║ "
                      << std::left << std::setw(20) << equipoBuscado.getNombre() << " ║ " << std::left << std::setw(13) << listaDeJugadores[e].getPosicion() << " ║ " << std::right
                      << std::setw(3) << listaDeJugadores[e].getEdad() << " ║ [" << std::right << std::setw(2) << listaDeJugadores[e].getNumeroDorsal() << "]     ║\n";
        }
        std::cout << "╚════╩══════════════════════╩══════════════════════╩═══════════════╩═════╩══════════╝\n";
        std::cout << " Total de jugadores registrados en el sistema: " << listaDeJugadores.size() << "\n";
    }

    std::cout << "\n";
    Formatos::pausarPrograma();
}

void jugadores::actualizarJugador() {
    Formatos::limpiarPantalla();

    // * Datos Actualizables:
    // * Nombre, Edad, Dorsal, Posicion, Cédula

    // Variables Editables
    char nombreAux[constantes::TAMANO_NOMBRE] = "";
    char posicionAux[constantes::TAMANO_POSICION] = "";
    char cedulaAux[constantes::TAMANO_CEDULA] = "";
    int edadAux = 0;
    int dorsalAux = 0;

    // Demás Variables
    char confirmacion;
    bool flagError = false;
    bool cancelado = false;
    bool salir = false;
    int idJugador = 0;
    int opcion = -1;
    Jugador jugadorBuscado;

    // Leemos el header del archivo de jugadores para saber el numero de registros activos
    ArchivoHeader headerJugadores = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_JUGADORES);

    // Verificamos que la lectura del header fue correcta
    if (headerJugadores.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    // Pedimos el ID del jugador a modificar
    do {
        Formatos::limpiarPantalla();
        flagError = false;

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        // Pedimos el ID del jugador que desean actualizar
        if (!presentacion::funcionesInOut::ingresarDatos(idJugador,
                                                         " Ingresa el ID del jugador al que desea actualizarsus datos (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el usuario. \n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idJugador)) {
            std::cout << "Error el id '" << idJugador << "' no es valido\n";
            Formatos::pausarPrograma();
            flagError = true;
        }

        // si no Existe el ID
        if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorBuscado, idJugador)) {
            std::cout << "Error el id '" << idJugador << "' no pertenece a ningún jugador registrado\n";
            Formatos::pausarPrograma();
            flagError = true;
        }

    } while (flagError);

    // Mostramos el menu para que el usuario eliga
    do {
        Formatos::limpiarPantalla();
        flagError = false;
        salir = false;
        bool existe = false;
        Jugador jugadorAuxiliar;
        Equipo equipoAuxiliar;

        // Presentamos un menu corto de los datos que puede actualizar para que eliga
        Formatos::limpiarPantalla();
        std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
        std::cout << "   ║         MENÚ ACTUALIZAR JUGADORES         ║\n";
        std::cout << "   ╠═══════════════════════════════════════════╣\n";
        std::cout << "   ║  1. Nombre del Jugador                    ║\n";
        std::cout << "   ║  2. Edad del Jugador                      ║\n";
        std::cout << "   ║  3. Dorsal del Jugador                    ║\n";
        std::cout << "   ║  4. Posicion del Jugador                  ║\n";
        std::cout << "   ║  5. Cédula del Jugador                    ║\n";
        std::cout << "   ║  0. Salir                                 ║\n";
        std::cout << "   ╚═══════════════════════════════════════════╝\n";
        std::cout << std::endl;

        // Pedimos la opcion
        if (!presentacion::funcionesInOut::ingresarDatos(opcion, " Ingrese una opción: ")) {
            std::cout << "\n Operación Cancelada por el usuario. \n";
            Formatos::pausarPrograma();
            return;
        }

        // Estructura del switch
        switch (opcion) {

            // Salida
            case 0:
                salir = true;
                break;

            // Actualizar Nombre
            case 1:

                do {
                    flagError = false;
                    Formatos::limpiarPantalla();
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║             ACTUALIZAR NOMBRE             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                    if (!presentacion::funcionesInOut::ingresarCadena(nombreAux, constantes::TAMANO_NOMBRE,
                                                                      " Ingrese el nuevo nombre del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                        std::cout << "\n Operación Cancelada por el Usuario \n";
                        Formatos::pausarPrograma();
                        return;
                    }

                    if (!GestorDeValidaciones::validarNombreOApellido(nombreAux)) {
                        std::cout << " El nombre '" << nombreAux << "' no es valido\n";
                        Formatos::pausarPrograma();
                        flagError = true;
                    }

                    if (OperacionesJugadores::cadenaDuplicada(nombreAux, &Jugador::getNombre, true, idJugador)) {
                        std::cout << " El nombre '" << nombreAux << "' ya le pertence a otro Jugador\n";
                        Formatos::pausarPrograma();
                        flagError = true;
                    }

                    Formatos::esperarTiempo(1500);
                    Formatos::limpiarPantalla();
                } while (flagError);
                break;

            // Actualizar Edad
            case 2:
                do {
                    flagError = false;
                    Formatos::limpiarPantalla();

                    // Recolectamos la Edad
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║              ACTUALIZAR EDAD              ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                    if (!presentacion::funcionesInOut::ingresarDatos(edadAux, " Ingrese la nueva edad del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                        std::cout << "\n Operación Cancelada por el Usuario \n";
                        Formatos::pausarPrograma();
                        return;
                    }

                    if (!GestorDeValidaciones::validarEdad(edadAux)) {
                        std::cout << " La edad '" << nombreAux << "' no es valida\n";
                        Formatos::pausarPrograma();
                        flagError = true;
                    }

                    Formatos::esperarTiempo(1200);
                    Formatos::limpiarPantalla();
                } while (flagError);
                break;

            // Actualizar Numero de Dorsal
            case 3:

                // Recolectamos el dorsal
                do {
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║             ACTUALIZAR DORSAL             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                    // Pedimos el dorsal
                    if (!presentacion::funcionesInOut::ingresarDatos(dorsalAux, " Ingrese el nuevo dorsal del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                        std::cout << "\n Operación Cancelada por el Usuario \n";
                        Formatos::pausarPrograma();
                        return;
                    }
                    Formatos::esperarTiempo(1000);

                    if (!GestorDeValidaciones::validarDorsal(dorsalAux)) {
                        std::cout << " El dorsal '" << dorsalAux << "' no es valida\n";
                        Formatos::pausarPrograma();
                        flagError = true;
                    }

                    // Buscamos al jugador para obtener el id de su equipo
                    GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAuxiliar, idJugador);

                    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, jugadorAuxiliar.getIdEquipo())) {
                        std::cerr << "\n Error del Sistema! \n";
                        Formatos::pausarPrograma();
                        return;
                    }

                    // Verificamos dorsal duplicado
                    if (OperacionesJugadores::esDorsalDuplicado(dorsalAux, jugadorAuxiliar.getIdEquipo(), true, jugadorAuxiliar.getId())) {
                        std::cerr << " Error: El dorsal '" << dorsalAux << "' ya está en uso en el equipo '" << equipoAuxiliar.getNombre() << "'.\n";
                        Formatos::esperarTiempo(2000);
                        flagError = true;
                        continue;
                    }

                    Formatos::esperarTiempo(1200);
                    Formatos::limpiarPantalla();
                } while (flagError);
                break;

            // Actualizar Posicion del Jugador
            case 4: {

                // Recolectamos la posición
                do {
                    opcion = 0;
                    flagError = false;
                    Formatos::limpiarPantalla();
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║           ACTUALIZAR LA POSICIÓN          ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << " Deporte actual del Torneo: " << torneo.getDeporte() << "\n\n";

                    std::vector<std::string> posicionesDisponibles = obtenerPosicionesDisponiblesParaDeporte(torneo.getDeporte());
                    if (posicionesDisponibles.empty()) {
                        std::cerr << "\nError del Sistema!\n";
                        Formatos::pausarPrograma();
                        return;
                    }

                    std::cout << " Seleccione la posición del jugador: \n";
                    for (size_t j = 0; j < posicionesDisponibles.size(); ++j) {
                        std::cout << " " << (j + 1) << ". " << posicionesDisponibles[j] << "\n";
                    }
                    std::cout << "\n";

                    if (!presentacion::funcionesInOut::ingresarDatos(opcion, "Seleccione una opción: ", &cancelado)) {
                        std::cout << "\n Operación Cancelada por el usuario. \n";
                        Formatos::pausarPrograma();
                        return;
                    }

                    if (opcion >= 1 && opcion <= static_cast<int>(posicionesDisponibles.size())) {
                        std::memset(posicionAux, '\0', sizeof(posicionAux));
                        std::strncpy(posicionAux, posicionesDisponibles[opcion - 1].c_str(), constantes::TAMANO_POSICION - 1);
                        posicionAux[constantes::TAMANO_POSICION - 1] = '\0';
                    } else {
                        std::cerr << " Error: Opción inválida. Por favor, intente nuevamente.\n";
                        flagError = true;
                        Formatos::esperarTiempo(2000);
                    }
                } while (flagError);

                break;
            }
            // Actualizar Cédula
            case 5:

                do {
                    existe = false;
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            ACTUALIZAR LA CEDULA           ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                    // Pedimos la cedula
                    if (!presentacion::funcionesInOut::ingresarCadena(cedulaAux, constantes::TAMANO_CEDULA,
                                                                      " Ingrese la nueva cedula del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                        std::cout << "\n Operación Cancelada por el Usuario \n";
                        Formatos::pausarPrograma();
                        return;
                    }

                    if (!GestorDeValidaciones::validarCedula(cedulaAux)) {
                        std::cout << " La cédula '" << cedulaAux << "' no es valida";
                        Formatos::esperarTiempo(1500);
                        flagError = true;
                        continue;
                    }

                    if (OperacionesJugadores::cadenaDuplicada(cedulaAux, &Jugador::getCedula, true, jugadorAuxiliar.getId())) {
                        std::cout << " La cédula '" << cedulaAux << "' ya le pertence a otro jugador";
                        Formatos::esperarTiempo(1500);
                        flagError = true;
                        continue;
                    }

                    Formatos::esperarTiempo(1500);
                    Formatos::limpiarPantalla();

                } while (flagError);
                break;

                // Mensaje de Default
            default:
                mensajeDefault();
        }

    } while (opcion != 0);

    // Si pidió salir
    if (salir) {
        Formatos::limpiarPantalla();
        Formatos::esperarTiempo(500);
        std::cout << "\n Saliendo... \n";
        Formatos::esperarTiempo(1000);
        Formatos::limpiarPantalla();
    }

    // Si no se modificó ningun parametro
    if (GestorDeValidaciones::validarCadenaVacia(nombreAux) && edadAux == 0 && dorsalAux == 0 && GestorDeValidaciones::validarCadenaVacia(posicionAux) &&
        GestorDeValidaciones::validarCadenaVacia(cedulaAux)) {
        std::cout << "\n No se realizaron cambios.\n";
        Formatos::pausarPrograma();
        return;
    }

    // Confirmación de los cambios
    Formatos::limpiarPantalla();
    std::cout << "\n";
    presentacion::funcionesInOut::ingresarDatos(confirmacion, " ¿Está seguro de que desea actualizar el jugador? (S/N): ");

    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

        // Llamamos a la lógica correspondiente de Jugadores
        if (!OperacionesJugadores::actualizarJugador(idJugador, nombreAux, edadAux, dorsalAux, posicionAux, cedulaAux)) {
            std::cout << "\n Se produjo un error a la hora de actualizar el jugador\n";
        } else {
            std::cout << "------------------------------------------------------------------------------\n";
            std::cout << "           Jugador Actualizado con Éxito\n";
            std::cout << "------------------------------------------------------------------------------\n\n";
            if (!GestorDeValidaciones::validarCadenaVacia(nombreAux)) {
                std::cout << " Nuevo Nombre:   " << nombreAux << "\n";
            }
            if (edadAux <= 0) {
                std::cout << " Nueva Edad:     " << edadAux << " años" << "\n";
            }
            if (dorsalAux <= 0) {
                std::cout << " Nuevo Dorsal:   " << dorsalAux << "\n";
            }
            if (!GestorDeValidaciones::validarCadenaVacia(posicionAux)) {
                std::cout << " Nueva Posición: " << posicionAux << "\n";
            }
            if (!GestorDeValidaciones::validarCadenaVacia(cedulaAux)) {
                std::cout << " Nueva Cedula:   " << cedulaAux << "\n";
            }
        }
    } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
        std::cout << "Actualización de Datos Cancelada\n";
    } else {
        std::cerr << "Error: No se ingresó una opción correcta (S/N)\n";
        std::cout << "Actualización de Datos Cancelada\n";
    }
    Formatos::pausarPrograma();
}

void jugadores::eliminarJugador() {
    Formatos::limpiarPantalla();
    int ID = 0;
    char confirmacion;
    int error = -1;
    bool cancelado = false;
    Jugador jugadorBuscado;
    bool encontrado = false;
    bool flagError = false;

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
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay jugadores activos registrados
    if (headerJugadores.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún jugador registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    do {
        Formatos::limpiarPantalla();
        flagError = false;

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║            ELIMINAR JUGADOR               ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        // Recolectamos el ID
        if (!presentacion::funcionesInOut::ingresarDatos(ID, "Ingresa el ID del jugador que deseas eliminar (ingresa 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\nOperacion Cancelada por el usuario\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(ID)) {
            std::cout << " El id '" << ID << "' no es válido";
            Formatos::esperarTiempo(1500);
            flagError = true;
            continue;
        }

        // Si no encontro el jugador con ese ID
        if (!GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorBuscado, ID)) {
            std::cout << " El id '" << ID << "' no le pertence a ningun jugador";
            Formatos::esperarTiempo(1500);
            flagError = true;
            continue;
        }
    } while (flagError);

    /*
    // Verificamos que no tenga puntos anotados
    if (jugadorBuscado.anotaciones > 0) {
        std::cout << "\nError el jugador " << jugadorBuscado.nombre << " no puede tener anotaciones en el torneo\n";
    }*/

    // Buscamos el equipo al que pertenece
    Equipo equipoBuscado;

    // Si no ecnontro el equipo del jugador
    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, jugadorBuscado.getIdEquipo())) {
        std::cerr << "\nError del Sistema!\n";
        Formatos::pausarPrograma();
        return;
    }

    // Mostramos el jugador que se va a eliminar
    std::cout << "\n Se eliminará el siguiente jugador del sistema:\n";
    std::cout << " -----------------------------------------------\n";
    std::cout << " Nombre:     " << jugadorBuscado.getNombre() << "\n";
    std::cout << " Posición:   " << jugadorBuscado.getPosicion() << "\n";
    std::cout << " Dorsal:     " << jugadorBuscado.getNumeroDorsal() << "\n";
    std::cout << " Anotaciones:     " << jugadorBuscado.getNumAnotaciones() << "\n";
    std::cout << " Equipo:     " << equipoBuscado.getNombre() << "\n";
    std::cout << " -----------------------------------------------\n\n";

    presentacion::funcionesInOut::ingresarDatos(confirmacion, "¿Está seguro de eliminar este jugador? (S/N): ");
    Formatos::esperarTiempo(1500);
    Formatos::limpiarPantalla();
    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

        if (OperacionesJugadores::eliminarJugador(ID)) {
            std::cout << "\n------------------------------------------------------------------------------\n";
            std::cout << "           Jugador eliminado con éxito\n";
            std::cout << "------------------------------------------------------------------------------\n";
        } else {
            std::cerr << "\nError: No se pudo eliminar al jugador.\n";
        }
    } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
        std::cout << "\nElimnación de datos cancelada\n";
    } else {
        std::cerr << "\nError: Opción inválida (S/N).\nEliminación de datos cancelada.\n";
    }
    Formatos::pausarPrograma();
}
