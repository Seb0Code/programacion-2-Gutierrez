#include "../../include/ui/MenusEquipos.hpp"

void equipos::mensajeDefault() { std::cout << "Opción inválida.\n"; }

// ---------------------------------------------------------------------------------------------- //
//   REGISTRAR EQUIPO
// ---------------------------------------------------------------------------------------------- //
void equipos::registrarEquipo() {
    Formatos::limpiarPantalla();
    bool flagError = false;
    bool cancelado = false;
    char confirmacion;
    Equipo nuevoEquipo;

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    char nombre[constantes::TAMANO_NOMBRE];
    char entrenador[constantes::TAMANO_NOMBRE];
    char ciudad[constantes::TAMANO_NOMBRE];
    char fechaRegistro[constantes::TAMANO_FECHA];

    // Recolectamos el nombre del Equipo
    do {
        Formatos::limpiarPantalla();
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarCadena(nombre, constantes::TAMANO_NOMBRE, " Ingrese el nombre del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Registro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarNombreOApellido(nombre)) {
            std::cerr << " Error, el nombre '" << nombre << "' no es válido\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoEquipo.setNombre(nombre)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(1200);
    } while (flagError);

    // Recolectamos el nombre del Entrenador
    do {
        flagError = false;
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarCadena(entrenador, constantes::TAMANO_NOMBRE,
                                                          " Ingrese el nombre del Entrenador (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Registro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarNombreOApellido(entrenador)) {
            std::cerr << " Error, el nombre '" << entrenador << "' no es válido\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoEquipo.setEntrenador(entrenador)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(1200);
    } while (flagError);

    // Recolectamos la ciudad del Equipo
    do {
        flagError = false;
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarCadena(ciudad, constantes::TAMANO_NOMBRE,
                                                          " Ingrese el nombre de la Ciudad del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Registro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarNombreOApellido(ciudad)) {
            std::cerr << " Error, la ciudad '" << ciudad << "' no es válida\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoEquipo.setCiudad(ciudad)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(1200);
    } while (flagError);

    // Recolectamos la fecha de registro del equipo
    do {
        flagError = false;
        Formatos::limpiarPantalla();
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Fecha de Inicio del Torneo: " << torneo.getFechaInicio() << std::endl;

        if (!presentacion::funcionesInOut::ingresarCadena(fechaRegistro, constantes::TAMANO_FECHA,
                                                          " Ingrese la fecha de Registro del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Registro cancelado por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarFechaDeRegistroDeJugadorOEquipo(fechaRegistro, torneo.getFechaInicio())) {
            std::cerr << " Error, la fecha '" << fechaRegistro << "' ingresada no es válida \n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

        if (!nuevoEquipo.setFechaRegistro(fechaRegistro)) {
            std::cerr << "Error del Sistema!\n";
            Formatos::pausarPrograma();
            return;
        }

        Formatos::esperarTiempo(1200);
        Formatos::limpiarPantalla();
    } while (flagError);

    // Pedimos la confirmacion al usuario
    presentacion::funcionesInOut::ingresarDatos(confirmacion, " ¿Está seguro de que desea registrar este equipo? (S/N): ");

    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
        bool registrado = OperacionesEquipos::registrarEquipo(nuevoEquipo);
        Formatos::esperarTiempo(1200);
        Formatos::limpiarPantalla();

        // Si el equipo no se pudo registrar (p. ej. nombre o entrenador duplicado)
        if (!registrado) {
            std::cerr << " Se produjo un error a la hora de registrar el equipo.\n";
            std::cout << " Verifique que el nombre y el entrenador no estén ya registrados.\n";
            Formatos::pausarPrograma();
            return;
        }

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║        EQUIPO REGISTRADO CON ÉXITO        ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << " Torneo: " << torneo.getNombre() << std::endl;
        std::cout << " ID del Equipo: " << nuevoEquipo.getId() << std::endl;
        std::cout << " Nombre del Equipo: " << nuevoEquipo.getNombre() << std::endl;
        std::cout << " Entrenador del Equipo: " << nuevoEquipo.getEntrenador() << std::endl;
        std::cout << " Ciudad del Equipo: " << nuevoEquipo.getCiudad() << std::endl;
        std::cout << " Fecha de Registro del Equipo: " << nuevoEquipo.getFechaRegistro() << std::endl;
    } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
        std::cout << "\n Registro de Equipo Cancelado\n";
    } else {
        std::cerr << "\n Error: No se ingresó una opción correcta (S/N)\n";
        std::cout << "\n Registro de Equipo Cancelado\n";
    }
    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   BUSCAR EQUIPO POR ID
// ---------------------------------------------------------------------------------------------- //
void equipos::buscarEquipoPorId() {
    Formatos::limpiarPantalla();
    int id = 0;
    bool flagError = false;
    bool cancelado = false;
    Equipo equipoBuscado;

    // Leemos el header del archivo de equipos para saber el numero de registros activos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos activos registrados
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    do {
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║        BUSQUEDA DE EQUIPOS POR ID         ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarDatos(id, " Ingrese el ID (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Busqueda cancelada por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(id)) {
            std::cerr << " Error el id '" << id << "' no es válido\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

    } while (flagError);

    Formatos::esperarTiempo(800);
    Formatos::limpiarPantalla();
    std::cout << "\nBuscando...\n";
    Formatos::esperarTiempo(1500);
    Formatos::limpiarPantalla();

    // Si no fue encontrado enviamos mensaje de error, si se encontró mostramos los datos
    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, id)) {
        std::cerr << " El equipo de ID '" << id << "' no fue encontrado\n";
    } else {
        std::string nom_torneo(torneo.getNombre());

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║             EQUIPO ENCONTRADO             ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << "-------------------------------------------------------------\n";
        std::cout << "  Torneo:                " << Formatos::convertirTextoAMayus(nom_torneo) << "\n";
        std::cout << "-------------------------------------------------------------\n";
        std::cout << "  Información del Equipo:\n";
        std::cout << "    ID del Equipo:         " << equipoBuscado.getId() << "\n";
        std::cout << "    Nombre:                " << equipoBuscado.getNombre() << "\n";
        std::cout << "    Entrenador:            " << equipoBuscado.getEntrenador() << "\n";
        std::cout << "    Ciudad Origen:         " << equipoBuscado.getCiudad() << "\n";
        std::cout << "    Cantidad de Jugadores: " << equipoBuscado.getNumJugadores() << "\n";
        std::cout << "    Fecha de Registro:     " << equipoBuscado.getFechaRegistro() << "\n";
        std::cout << "-------------------------------------------------------------\n";
        std::cout << "  Estadísticas en el Torneo:\n";
        std::cout << "    Puntos Totales:        " << equipoBuscado.getPuntos() << "\n";
        std::cout << "    Partidos Jugados:      " << equipoBuscado.getJugados() << "\n";
        std::cout << "    Victorias:             " << equipoBuscado.getVictorias() << "\n";
        std::cout << "    Empates:               " << equipoBuscado.getEmpates() << "\n";
        std::cout << "    Derrotas:              " << equipoBuscado.getDerrotas() << "\n";
        std::cout << "    Anotaciones a Favor:   " << equipoBuscado.getAnotacionAFavor() << "\n";
        std::cout << "    Anotaciones en Contra: " << equipoBuscado.getAnotacionEnContra() << "\n";
        std::cout << "-------------------------------------------------------------\n";
    }
    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   BUSCAR EQUIPOS POR NOMBRE (SUBCADENA)
// ---------------------------------------------------------------------------------------------- //
void equipos::buscarEquiposPorNombre() {
    Formatos::limpiarPantalla();
    char subcadena[constantes::TAMANO_NOMBRE];
    bool cancelado = false;
    bool flagError = false;

    // Leemos el header del archivo de equipos para saber el numero de registros activos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos activos registrados
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    do {
        flagError = false;
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║      BUSQUEDA DE EQUIPOS POR NOMBRE       ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        if (!presentacion::funcionesInOut::ingresarCadena(subcadena, constantes::TAMANO_NOMBRE,
                                                          " Escribe el nombre (o parte del nombre) del equipo que buscas (ingresa 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Busqueda cancelada por el usuario.\n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarNombreOApellido(subcadena)) {
            std::cerr << " Error la subcadena '" << subcadena << "' no es válida\n";
            flagError = true;
            Formatos::esperarTiempo(2500);
            continue;
        }

    } while (flagError);

    Formatos::limpiarPantalla();
    std::cout << "\n Buscando... \n";
    Formatos::esperarTiempo(1500);
    Formatos::limpiarPantalla();

    // Llamamos a la funcion de busqueda por subcadena
    std::vector<Equipo> listaDeEquiposSubcadena = GestorArchivosBinarios::buscarRegistrosPorSubcadena<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, subcadena);

    // Si no se encontró ningún equipo
    if (listaDeEquiposSubcadena.empty()) {
        std::cout << "\n No se encontró ninguna coincidencia con: '" << subcadena << "'\n";
    } else {
        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║          RESULTADOS ENCONTRADOS           ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
        std::cout << "----------------------------------------------------------------------------\n";
        std::cout << " Se encontraron " << listaDeEquiposSubcadena.size() << " coincidencia(s):\n";
        std::cout << "----------------------------------------------------------------------------\n";

        for (size_t e = 0; e < listaDeEquiposSubcadena.size(); e++) {
            std::cout << std::endl << (e + 1) << ".\n";
            std::cout << "   Nombre:                   " << listaDeEquiposSubcadena[e].getNombre() << "\n";
            std::cout << "   ID:                       " << listaDeEquiposSubcadena[e].getId() << "\n";
            std::cout << "   Entrenador:               " << listaDeEquiposSubcadena[e].getEntrenador() << "\n";
            std::cout << "   Ciudad:                   " << listaDeEquiposSubcadena[e].getCiudad() << "\n";
            std::cout << "   Cantidad de Jugadores:    " << listaDeEquiposSubcadena[e].getNumJugadores() << "\n";
            std::cout << "   Cantidad de Partidos:     " << listaDeEquiposSubcadena[e].getCantidadPartidos() << "\n";
            std::cout << "   Fecha de Registro:        " << listaDeEquiposSubcadena[e].getFechaRegistro() << "\n";
        }
        std::cout << "---------------------------------------------------------------------------\n";
    }

    std::cout << "\n";
    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   LISTAR TODOS LOS EQUIPOS
// ---------------------------------------------------------------------------------------------- //
void equipos::listarEquipos() {
    Formatos::limpiarPantalla();

    // Leemos el header del archivo de equipos para saber el numero de registros activos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos registrados
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << " No hay equipos registrados en el sistema actualmente.\n";
        Formatos::pausarPrograma();
        return;
    }

    std::vector<Equipo> listaDeEquipos = GestorArchivosBinarios::listarRegistros<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
    std::cout << "       ║            LISTA DE EQUIPOS               ║\n";
    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
    std::cout << "----------------------------------------------------------------------------------------- \n";
    std::cout << " " << std::left << std::setw(4) << "N°" << " | " << std::setw(6) << "ID" << " | " << std::setw(35) << "Nombre" << " | " << std::setw(30) << "Ciudad" << " | "
              << "Puntos"
              << "\n";
    std::cout << "----------------------------------------------------------------------------------------- \n";

    for (size_t e = 0; e < listaDeEquipos.size(); e++) {
        std::cout << " " << std::left << std::setw(4) << (e + 1) << " | " << std::setw(6) << listaDeEquipos[e].getId() << " | " << std::setw(35) << listaDeEquipos[e].getNombre()
                  << " | " << std::setw(30) << listaDeEquipos[e].getCiudad() << " | " << listaDeEquipos[e].getPuntos() << "\n";
    }

    std::cout << "----------------------------------------------------------------------------------------- \n";
    std::cout << " Total de equipos registrados: " << listaDeEquipos.size() << "\n";

    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   TABLA DE POSICIONES
// ---------------------------------------------------------------------------------------------- //
void equipos::mostrarTablaDePosiciones() {
    Formatos::limpiarPantalla();
    Formatos::esperarTiempo(1200);

    // Leemos el header del archivo de equipos para saber el numero de registros activos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos disponibles
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "\nNo hay Equipos Disponibles\n";
        Formatos::pausarPrograma();
        return;
    }

    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    // Armamos la tabla de posiciones
    std::vector<Equipo> tablaDePosiciones = OperacionesEquipos::tablaDePosiciones();

    Formatos::esperarTiempo(500);
    std::cout << "\nCargando la tabla de posiciones...\n";
    Formatos::esperarTiempo(1200);
    Formatos::limpiarPantalla();

    // Si no hay resultados no mostramos nada, sino mostramos la tabla con los equipos
    if (tablaDePosiciones.empty()) {
        std::cout << "\nNo hay Equipos Disponibles\n";
        Formatos::pausarPrograma();
        return;
    }

    std::string nombreTorneo(torneo.getNombre());
    Formatos::convertirTextoAMayus(nombreTorneo);

    std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                             TABLA DE POSICIONES                                     ║\n";
    std::cout << "║               " << std::left << std::setw(70) << nombreTorneo << "║\n";
    std::cout << "╠════╦═══════════════════════════════════════════════╦═════╦═══╦═══╦═══╦════╦════╦════╣\n";
    std::cout << "║ #  ║ Equipo                                        ║ PTS ║ J ║ G ║ E ║ D  ║ GF ║ GC ║\n";
    std::cout << "╠════╬═══════════════════════════════════════════════╬═════╬═══╬═══╬═══╬════╬════╬════╣\n";

    for (size_t e = 0; e < tablaDePosiciones.size(); e++) {
        std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(45) << tablaDePosiciones[e].getNombre() << " ║ " << std::right << std::setw(3)
                  << tablaDePosiciones[e].getPuntos() << " ║ " << std::right << std::setw(1) << tablaDePosiciones[e].getJugados() << " ║ " << std::right << std::setw(1)
                  << tablaDePosiciones[e].getVictorias() << " ║ " << std::right << std::setw(1) << tablaDePosiciones[e].getEmpates() << " ║ " << std::right << std::setw(2)
                  << tablaDePosiciones[e].getDerrotas() << " ║ " << std::right << std::setw(2) << tablaDePosiciones[e].getAnotacionAFavor() << " ║ " << std::right << std::setw(2)
                  << tablaDePosiciones[e].getAnotacionEnContra() << " ║\n";
    }
    std::cout << "╚════╩═══════════════════════════════════════════════╩═════╩═══╩═══╩═══╩════╩════╩════╝\n";
    std::cout << "\nReferencia: PTS=Puntos  J=Jugados  G=Ganados  E=Empatados\n";
    std::cout << "            D=Derrotas  GF=Anotaciones a Favor  GC=Anotaciones en Contra\n\n";

    Formatos::pausarPrograma();
}

// ---------------------------------------------------------------------------------------------- //
//   ACTUALIZAR EQUIPO
// ---------------------------------------------------------------------------------------------- //
void equipos::actualizarEquipo() {
    Formatos::limpiarPantalla();

    // * Datos Actualizables:
    // * Nombre del Equipo, Nombre del Entrenador, Nombre de la Ciudad

    // Variables Editables
    char nombreAux[constantes::TAMANO_NOMBRE] = "";
    char entrenadorAux[constantes::TAMANO_NOMBRE] = "";
    char ciudadAux[constantes::TAMANO_NOMBRE] = "";

    // Demás Variables
    bool cancelado = false;
    bool flagError = false;
    bool salir = false;
    char confirmacion;
    int idEquipo = 0;
    int opcion = -1;
    Equipo equipoBuscado;

    // Leemos el header del archivo de equipos para saber el numero de registros activos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Busqueda Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos activos registrados
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    // Pedimos el ID del equipo a modificar
    do {
        Formatos::limpiarPantalla();
        flagError = false;

        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
        std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

        // Pedimos el ID del equipo que desean actualizar
        if (!presentacion::funcionesInOut::ingresarDatos(idEquipo, "Ingresa el ID del equipo que desea actualizar (ingrese 'cancelar' para cancelar): ", &cancelado)) {
            std::cout << "\n Operación Cancelada por el usuario. \n";
            Formatos::pausarPrograma();
            return;
        }

        if (!GestorDeValidaciones::validarId(idEquipo)) {
            std::cout << "Error el id '" << idEquipo << "' no es válido\n";
            Formatos::esperarTiempo(1500);
            flagError = true;
            continue;
        }

        // si no existe el ID
        if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, idEquipo)) {
            std::cout << "Error el ID '" << idEquipo << "' no pertenece a ningún equipo registrado\n";
            Formatos::esperarTiempo(1500);
            flagError = true;
            continue;
        }

    } while (flagError);

    // Mostramos el menu para que el usuario elija
    do {
        Formatos::limpiarPantalla();
        flagError = false;
        salir = false;

        // Presentamos un menu corto de los datos que puede actualizar para que elija
        std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
        std::cout << "   ║          MENÚ ACTUALIZAR EQUIPOS          ║\n";
        std::cout << "   ╠═══════════════════════════════════════════╣\n";
        std::cout << "   ║  1. Nombre del equipo                     ║\n";
        std::cout << "   ║  2. Nombre del Entrenador                 ║\n";
        std::cout << "   ║  3. Nombre de la Ciudad                   ║\n";
        std::cout << "   ║  0. Salir                                 ║\n";
        std::cout << "   ╚═══════════════════════════════════════════╝\n";
        std::cout << std::endl;

        // Pedimos la opción
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

            // Actualizar nombre
            case 1:
                do {
                    flagError = false;
                    Formatos::limpiarPantalla();
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║        ACTUALIZAR NOMBRE DEL EQUIPO       ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                    if (!presentacion::funcionesInOut::ingresarCadena(nombreAux, constantes::TAMANO_NOMBRE,
                                                                      "Ingrese el nuevo nombre del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                        std::cout << "\n Operación Cancelada por el Usuario \n";
                        Formatos::pausarPrograma();
                        return;
                    }

                    if (!GestorDeValidaciones::validarNombreOApellido(nombreAux)) {
                        std::cout << "El nombre '" << nombreAux << "' no es válido\n";
                        Formatos::esperarTiempo(1500);
                        flagError = true;
                        continue;
                    }

                    Formatos::esperarTiempo(1200);
                    Formatos::limpiarPantalla();
                } while (flagError);
                break;

            // Actualizar nombre del entrenador
            case 2:
                do {
                    flagError = false;
                    Formatos::limpiarPantalla();
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║     ACTUALIZAR NOMBRE DEL ENTRENADOR      ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                    if (!presentacion::funcionesInOut::ingresarCadena(entrenadorAux, constantes::TAMANO_NOMBRE,
                                                                      "Ingrese el nuevo nombre del Entrenador del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                        std::cout << "\n Operación Cancelada por el Usuario \n";
                        Formatos::pausarPrograma();
                        return;
                    }

                    if (!GestorDeValidaciones::validarNombreOApellido(entrenadorAux)) {
                        std::cout << "El nombre '" << entrenadorAux << "' no es válido\n";
                        Formatos::esperarTiempo(1500);
                        flagError = true;
                        continue;
                    }

                    Formatos::esperarTiempo(1200);
                    Formatos::limpiarPantalla();
                } while (flagError);
                break;

            // Actualizar nombre de la ciudad
            case 3:
                do {
                    flagError = false;
                    Formatos::limpiarPantalla();
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║       ACTUALIZAR NOMBRE DE LA CIUDAD      ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                    if (!presentacion::funcionesInOut::ingresarCadena(ciudadAux, constantes::TAMANO_NOMBRE,
                                                                      "Ingrese el nuevo nombre de la Ciudad del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                        std::cout << "\n Operación Cancelada por el Usuario \n";
                        Formatos::pausarPrograma();
                        return;
                    }

                    if (!GestorDeValidaciones::validarNombreOApellido(ciudadAux)) {
                        std::cout << "La ciudad '" << ciudadAux << "' no es válida\n";
                        Formatos::esperarTiempo(1500);
                        flagError = true;
                        continue;
                    }

                    Formatos::esperarTiempo(1200);
                    Formatos::limpiarPantalla();
                } while (flagError);
                break;

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

    // Si no se modificó ningún parámetro
    if (GestorDeValidaciones::validarCadenaVacia(nombreAux) && GestorDeValidaciones::validarCadenaVacia(entrenadorAux) && GestorDeValidaciones::validarCadenaVacia(ciudadAux)) {
        std::cout << "\n No se realizaron cambios.\n";
        Formatos::pausarPrograma();
        return;
    }

    // Pedimos la confirmación
    Formatos::limpiarPantalla();
    presentacion::funcionesInOut::ingresarDatos(confirmacion, "¿Está seguro de que desea aplicar estos cambios? (S/N): ");

    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

        // Llamamos a la lógica correspondiente de Equipos
        if (!OperacionesEquipos::actualizarEquipo(idEquipo, nombreAux, entrenadorAux, ciudadAux)) {
            std::cout << "Se produjo un error a la hora de actualizar el equipo\n";
            std::cout << "Verifique que el nombre y el entrenador no estén ya en uso por otro equipo\n";
        } else {
            std::cout << "------------------------------------------------------------------------------\n";
            std::cout << "           Equipo Actualizado con Éxito\n";
            std::cout << "------------------------------------------------------------------------------\n";

            if (!GestorDeValidaciones::validarCadenaVacia(nombreAux)) {
                std::cout << "Nuevo Nombre del Equipo: " << nombreAux << std::endl;
            }
            if (!GestorDeValidaciones::validarCadenaVacia(entrenadorAux)) {
                std::cout << "Nuevo Nombre del Entrenador del Equipo: " << entrenadorAux << std::endl;
            }
            if (!GestorDeValidaciones::validarCadenaVacia(ciudadAux)) {
                std::cout << "Nueva Ciudad del Equipo: " << ciudadAux << std::endl;
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

// ---------------------------------------------------------------------------------------------- //
//   ELIMINAR EQUIPO
// ---------------------------------------------------------------------------------------------- //
void equipos::eliminarEquipo() {
    Formatos::limpiarPantalla();

    bool eliminado = false;
    bool encontrado = false;
    bool cancelado = false;
    int id = 0;
    char confirmacion;
    Equipo equipoAux;

    // Leemos el header del archivo de equipos para saber el numero de registros activos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que la lectura del header fue correcta
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        std::cerr << "\nError del Sistema!\n";
        std::cout << "Eliminación Cancelada\n";
        Formatos::pausarPrograma();
        return;
    }

    // Si no hay equipos activos registrados
    if (headerEquipos.getRegistrosActivos() == 0) {
        std::cout << "No hay ningún equipo registrado actualmente\n";
        Formatos::pausarPrograma();
        return;
    }

    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
    std::cout << "       ║             ELIMINAR EQUIPOS              ║\n";
    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

    // Pedimos el ID del equipo que desean eliminar
    if (!presentacion::funcionesInOut::ingresarDatos(id, "Ingresa el ID del equipo que desea eliminar (ingresa 'cancelar' para cancelar): ", &cancelado)) {
        std::cout << "\nOperación cancelada por el usuario.\n";
        Formatos::pausarPrograma();
        return;
    }

    if (!GestorDeValidaciones::validarId(id)) {
        std::cerr << "Error el id '" << id << "' no es válido\n";
        Formatos::pausarPrograma();
        return;
    }

    // si no existe el ID
    encontrado = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAux, id);
    if (!encontrado) {
        std::cerr << "Error el ID '" << id << "' no pertenece a ningún equipo registrado\n";
        Formatos::pausarPrograma();
        return;
    }

    Formatos::esperarTiempo(1500);
    Formatos::limpiarPantalla();

    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
    std::cout << "       ║             ELIMINAR EQUIPOS              ║\n";
    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

    std::cout << "Equipo Seleccionado: \n\n";
    std::cout << "Nombre: " << equipoAux.getNombre() << std::endl;
    std::cout << "Entrenador: " << equipoAux.getEntrenador() << std::endl;
    std::cout << "Ciudad: " << equipoAux.getCiudad() << std::endl;
    std::cout << "Cantidad de Jugadores: " << equipoAux.getNumJugadores() << std::endl;
    std::cout << "Cantidad de Partidos: " << equipoAux.getCantidadPartidos() << std::endl;

    presentacion::funcionesInOut::ingresarDatos(confirmacion, "¿Desea eliminar el equipo? (S/N): ");
    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
        eliminado = OperacionesEquipos::eliminarEquipo(id);
        if (!eliminado) {
            std::cout << "Se produjo un error a la hora de eliminar el equipo\n";
            std::cout << "Por favor revise que el equipo a eliminar no tenga partidos asociados ni jugadores registrados\n";
        } else {
            std::cout << "------------------------------------------------------------------------------\n";
            std::cout << "           Equipo Eliminado con Éxito\n";
            std::cout << "------------------------------------------------------------------------------\n";
        }
    } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
        std::cout << "Eliminación de Equipo Cancelada\n";
    } else {
        std::cerr << "Error: No se ingresó una opción correcta (S/N)\n";
        std::cout << "Eliminación de Equipo Cancelada\n";
    }
    Formatos::pausarPrograma();
}