#include "../../include/manager/operacionesEquipos.hpp"
#include "../../include/models/archivoHeader.hpp"
#include "../../include/utils/constantes.hpp"
#include "../../include/utils/validaciones.hpp"

bool OperacionesEquipos::registrarEquipo(Equipo &nuevoEquipo) {

    ArchivoHeader header = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // colocamos el equipo como activo (es decir que no ha sido eliminado)
    nuevoEquipo.setEliminado(false);

    // Inicializamos las estadísticas
    nuevoEquipo.setJugados(0);
    nuevoEquipo.setVictorias(0);
    nuevoEquipo.setEmpates(0);
    nuevoEquipo.setDerrotas(0);
    nuevoEquipo.setPuntos(0);
    nuevoEquipo.setAnotacionAFavor(0);
    nuevoEquipo.setAnotacionEnContra(0);
    nuevoEquipo.setNumJugadores(0);
    // nuevoEquipo.setCantidadDePartidos(0);

    // Asignamos el ID
    nuevoEquipo.setId(header.getProximoId());

    // Asignamos la fecha de creacion del equipo
    nuevoEquipo.setFechaCreacion(time(nullptr));           // Esto toma la fecha de en que se creo la variable
    nuevoEquipo.setFechaUltimaModificacion(time(nullptr)); // colocamos la misma porque cuando fue cuando se creó

    bool guardado = GestorArchivosBinarios::guardarNuevoRegistro<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, nuevoEquipo);
    if (!guardado) {
        return false;
    }

    return true;
}

std::vector<Equipo> OperacionesEquipos::tablaDePosiciones() {
    std::vector<Equipo> listaDeEquipos;
    listaDeEquipos = GestorArchivosBinarios::listarRegistros<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    if (listaDeEquipos.size() <= 0) {
        return listaDeEquipos;
    }

    // inicializamos en 0 por si no pasa las validaciones
    int difPtsEq1 = 0, difPtsEq2 = 0;
    bool intercambiar = false;
    int cantidadDelistaDeEquipos = 0;
    int error = -1;

    // Ordenamos por cantidad de puntos de mayor a menor
    // Restamos 1 para no acceder a memoria indebida
    for (int e = 0; e < listaDeEquipos.size() - 1; ++e) {
        for (int r = 0; r < listaDeEquipos.size() - e - 1; ++r) {

            // incializamos esta bandera en false, se activa si los equipos
            // requieren que se interambien por los criterios de cada condicion
            intercambiar = false;

            //* Condicion 1
            // Si el equipo 1 tiene menos puntos que el equipo 2;
            if (listaDeEquipos[r].getPuntos() < listaDeEquipos[r + 1].getPuntos()) {
                intercambiar = true;

                // Si poseen igual cantidad de puntos
            } else if ((listaDeEquipos[r].getPuntos()) == (listaDeEquipos[r + 1].getPuntos())) {

                // Calculamos diferencia de puntos
                difPtsEq1 = (listaDeEquipos[r].getAnotacionAFavor()) - (listaDeEquipos[r].getAnotacionEnContra());
                difPtsEq2 = (listaDeEquipos[r + 1].getAnotacionAFavor()) - (listaDeEquipos[r + 1].getAnotacionEnContra());

                // *Condicion 2
                // Si el equipo de la izquierda tiene menor diferencia de puntos
                // lo ubicamos a la deracha es decir lo bajamos una posicion
                if (difPtsEq1 < difPtsEq2) {
                    intercambiar = true;

                    // Si la diferencia de goles es igual tambien
                } else if (difPtsEq1 == difPtsEq2) {

                    //* Condicion 3
                    // comparamos los puntos a favor
                    if (listaDeEquipos[r].getAnotacionAFavor() < listaDeEquipos[r + 1].getAnotacionAFavor()) {
                        intercambiar = true;

                        // Si los puntos a favor son iguales desempatamos por victorias
                    } else if (listaDeEquipos[r].getAnotacionAFavor() == listaDeEquipos[r + 1].getAnotacionAFavor()) {

                        // * Condicion 4
                        // comparamos las victorias
                        if (listaDeEquipos[r].getVictorias() < listaDeEquipos[r + 1].getVictorias()) {
                            intercambiar = true;
                        }
                    }
                }
            }

            // Si se cumple alguna condicion hacemos el intercambio
            if (intercambiar) {
                // Guardamos el equipo con menos puntos en una variable auxiliar
                Equipo registroTemporal = listaDeEquipos[r];

                // Luego movemos el equipo mayor a la posicion donde estaba el menor
                listaDeEquipos[r] = listaDeEquipos[r + 1];

                // colocamos en la nueva posicion al equipo con menos puntos
                listaDeEquipos[r + 1] = registroTemporal;
            }
        }
    }

    return listaDeEquipos; // Devolvemos la lista de equipos organizada
}

bool OperacionesEquipos::actualizarEquipo(const int id, const char *nombre, const char *entrenador, const char *ciudad) {

    // Utilizamos una variable auxiliar
    Equipo equipoAuxiliar;

    // Buscamos el registro con el ID
    bool existe = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, id);

    // Verificamos que lo haya encontrado
    if (!existe) {
        return false;
    }

    // Verificamos que no estén duplicados (si se proporciona un nuevo nombre/entrenador)
    if (!GestorDeValidaciones::validarCadenaVacia(nombre)) {
        if (cadenaDuplicada(nombre, &Equipo::getNombre, true, id)) {
            return false;
        }
    }

    if (!GestorDeValidaciones::validarCadenaVacia(entrenador)) {
        if (cadenaDuplicada(entrenador, &Equipo::getEntrenador, true, id)) {
            return false;
        }
    }

    // Si no está vacío lo actualizamos
    if (!GestorDeValidaciones::validarCadenaVacia(nombre)) {
        equipoAuxiliar.setNombre(nombre);
    }
    if (!GestorDeValidaciones::validarCadenaVacia(entrenador)) {
        equipoAuxiliar.setEntrenador(entrenador);
    }
    if (!GestorDeValidaciones::validarCadenaVacia(ciudad)) {
        equipoAuxiliar.setCiudad(ciudad);
    }

    // Modificamos la fecha de ultima modificacion del registro
    equipoAuxiliar.setFechaUltimaModificacion(std::time(nullptr));
    GestorArchivosBinarios::guardarRegistro(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, id);
    return true;
}

bool OperacionesEquipos::eliminarEquipo(const int id) {

    ArchivoHeader headerEquipos;
    Equipo equipo;

    // Buscar y validar header
    headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        return false;
    }

    // buscamos el el equipo
    bool encontrado = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipo, id);
    if (!encontrado) {
        return false;
    }

    // Lo eliminamos logicamente
    equipo.setEliminado(true);

    // Guardamos el registro modificado en el archivo
    if (!GestorArchivosBinarios::guardarRegistro(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipo, id)) {
        return false;
    }

    headerEquipos.disminuirRegistrosActivos();
    if (!GestorArchivosBinarios::actualizarHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS, headerEquipos)) {
        return false;
    }

    return true;
    /*
    // Abrimos el archivo de partidos para hacer la verificacion
    std::ifstream archivoPartidos;
    if (!abrirParaLectura(NOMBRE_ARCHIVO_PARTIDOS, archivo)) {
            return false;
    }


    Partido partidoTemporal;

    // * Verificamos que no tenga partidos asociados
    while (archivoPartidos.read(reinterpret_cast<char *>(&partidoTemporal), sizeof(Partido))) {
        if (archivoPartidos.fail()) {
            archivoPartidos.close();
            archivo.close();
            return false;
        }

        if ((partidoTemporal.idEquipoLocal == registroTemporal.ID) || (partidoTemporal.idEquipoVisitante == registroTemporal.ID)) {
            archivoPartidos.close();
            archivo.close();
            return false;
        }
    }

    archivoPartidos.close();

    // Abrimos el archivo de jugadores para hacer la verificacion
    std::ifstream archivoJugadores;
    archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary);

    if (!archivoJugadores.is_open()) {
        archivo.close();
        return false;
    }

    Jugador jugadorTemporal;

    // * Verificamos que no tenga jugadores asociados
    while (archivoJugadores.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {
        if (archivoJugadores.fail()) {
            archivoJugadores.close();
            archivo.close();
            return false;
        }

        if (jugadorTemporal.idEquipo == registroTemporal.ID) {
            archivoJugadores.close();
            archivo.close();
            return false;
        }
    }

    archivoJugadores.close();
    */
    // Reabrimos el archivo de equipos para actualizar el registro
}

bool OperacionesEquipos::modificarCantidadDeJugadores(const int idEquipo, const bool disminuir) {

    // Buscamos el equipo por su id
    Equipo equipoAuxiliar;

    // Si no lo encuentra devolvemos false
    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, idEquipo)) {
        return false;
    }

    // Pedimos la cantidad de jugadores del equipo
    int cantJugadores = equipoAuxiliar.getNumJugadores();

    // Verificamos que no se intente disminuir por debajo de cero
    if (cantJugadores < 0) {
        return false;
    }

    if (!disminuir) {
        equipoAuxiliar.aumentarNumeroDeJugadores();
    } else if (cantJugadores > 0) {
        equipoAuxiliar.disminuirNumeroDeJugadores();
    } else {
        equipoAuxiliar.setNumJugadores(0);
    }

    equipoAuxiliar.setFechaUltimaModificacion(std::time(nullptr));

    // Persistimos la modificación en el archivo binario
    return GestorArchivosBinarios::guardarRegistro<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, idEquipo);
}

bool OperacionesEquipos::modificarEstadisticas(const int idEquipo, const int resultadoParaEquipo, const bool permiteEmpate, const bool revertir) {

    Equipo equipoAuxiliar;

    // Buscamos el equipo para ver si existe
    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, idEquipo)) {
        return false;
    }

    // Verificamos que el resultado este entre el rango
    if (resultadoParaEquipo < -1 || resultadoParaEquipo > 1) {
        return false;
    }

    if (!revertir) {

        // Si el equipo gano
        if (resultadoParaEquipo == 1) {
            int victorias = 0, puntos = 0;

            // * Aumentamos las victorias
            victorias = equipoAuxiliar.getVictorias();
            ++victorias;
            if (!equipoAuxiliar.setVictorias(victorias)) {
                return false;
            }

            // * Aumentamos los puntos
            puntos = equipoAuxiliar.getPuntos();
            puntos += 3;
            if (!equipoAuxiliar.setPuntos(puntos)) {
                return false;
            }

            // si perdio
        } else if (resultadoParaEquipo == -1) {
            int derrotas = 0;

            // * Aumentamos las derrotas
            derrotas = equipoAuxiliar.getDerrotas();
            ++derrotas;
            if (!equipoAuxiliar.setDerrotas(derrotas)) {
                return false;
            }

            // Si empato
        } else {

            // Y ademas el deporte permite empate
            if (permiteEmpate) {
                int empates, puntos;

                // * Aumentamos los empates y puntos
                empates = equipoAuxiliar.getEmpates();
                ++empates;
                equipoAuxiliar.setEmpates(empates);
                puntos = equipoAuxiliar.getPuntos();
                ++puntos;
                equipoAuxiliar.setPuntos(puntos);

            } else {
                return false;
            }
        }

        // Si deseamos revertir
    } else {
        // Si el equipo gano
        if (resultadoParaEquipo == 1) {
            int victorias = 0, puntos = 0;

            // * Aumentamos las victorias
            victorias = equipoAuxiliar.getVictorias();
            --victorias;
            if (!equipoAuxiliar.setVictorias(victorias)) {
                return false;
            }

            // * Aumentamos los puntos
            puntos = equipoAuxiliar.getPuntos();
            puntos -= 3;
            if (!equipoAuxiliar.setPuntos(puntos)) {
                return false;
            }

            // si perdio
        } else if (resultadoParaEquipo == -1) {
            int derrotas = 0;

            // * Aumentamos las derrotas
            derrotas = equipoAuxiliar.getDerrotas();
            --derrotas;
            if (!equipoAuxiliar.setDerrotas(derrotas)) {
                return false;
            }

            // Si empato
        } else {

            // Y ademas el deporte permite empate
            if (permiteEmpate) {
                int empates, puntos;

                // * Aumentamos los empates y puntos
                empates = equipoAuxiliar.getEmpates();
                --empates;
                equipoAuxiliar.setEmpates(empates);
                puntos = equipoAuxiliar.getPuntos();
                --puntos;
                equipoAuxiliar.setPuntos(puntos);

            } else {
                return false;
            }
        }
    }

    if (!GestorArchivosBinarios::guardarRegistro<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, idEquipo)) {
        return false;
    }

    return true;
}

bool OperacionesEquipos::modificarAnotaciones(Partido registroPartido, const bool revertir) {

    Equipo equipoLocal, equipoVisitante;

    // Buscamos los equipos para ver si existen
    if (!GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoLocal, registroPartido.getIdEquipoLocal()) ||
        !GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoVisitante, registroPartido.getIdEquipoVisitante())) {
        return false;
    }

    int jugados = 0, anotacionAFavorLocal = 0, anotacionAFavorVisitante = 0, anotacionEnContraLocal = 0, anotacionEnContraVisitante = 0;

    if (!revertir) {

        // * Modificamos las anotaciones del equipo local

        // Anotaciones a favor
        anotacionAFavorLocal = equipoLocal.getAnotacionAFavor();
        anotacionAFavorLocal += registroPartido.getAnotacionesLocal();
        if (!equipoLocal.setAnotacionAFavor(anotacionAFavorLocal)) {
            return false;
        }

        // Anotaciones en contra
        anotacionEnContraLocal = equipoLocal.getAnotacionEnContra();
        anotacionEnContraLocal += registroPartido.getAnotacionesVisitante();
        if (!equipoLocal.setAnotacionEnContra(anotacionEnContraLocal)) {
            return false;
        }

        // Partidos jugados
        jugados = equipoLocal.getJugados();
        ++jugados;
        if (!equipoLocal.setJugados(jugados)) {
            return false;
        }

        // * Modificamos las anotaciones del equipo visitante
        jugados = 0;

        // Anotaciones a favor
        anotacionAFavorVisitante = equipoVisitante.getAnotacionAFavor();
        anotacionAFavorVisitante += registroPartido.getAnotacionesVisitante();
        if (!equipoVisitante.setAnotacionAFavor(anotacionAFavorVisitante)) {
            return false;
        }

        // Anotaciones en contra
        anotacionEnContraVisitante = equipoVisitante.getAnotacionEnContra();
        anotacionEnContraVisitante += registroPartido.getAnotacionesLocal();
        if (!equipoVisitante.setAnotacionEnContra(anotacionEnContraVisitante)) {
            return false;
        }

        // Partidos jugados
        jugados = equipoVisitante.getJugados();
        ++jugados;
        if (!equipoVisitante.setJugados(jugados)) {
            return false;
        }

        // * 4. Agregamos el id del partido al array de cada equipo y aumentamos el numero de partidos
        if (equipoLocal.getCantidadPartidos() >= 50 || equipoVisitante.getCantidadPartidos() >= 50) {
            return false;
        }

        if (!equipoLocal.agregarIdPartido(registroPartido.getId())) {
            return false;
        }
        if (!equipoVisitante.agregarIdPartido(registroPartido.getId())) {
            return false;
        }

        // ! NOTA: LA FUNCION AGREGAR PARTIDO MANEJA INTERNAMENTE EL AUMENTO DE LA CANTIDAD DE PARTIDOS

        // Agregamos la fecha de modificacion
        if (!equipoLocal.setFechaUltimaModificacion(std::time(nullptr))) {
            return false;
        }

        if (!equipoVisitante.setFechaUltimaModificacion(std::time(nullptr))) {
            return false;
        }

        // Guardamos los archivos
        if (!GestorArchivosBinarios::guardarRegistro<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoLocal, registroPartido.getIdEquipoLocal()) ||
            !GestorArchivosBinarios::guardarRegistro<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoVisitante, registroPartido.getIdEquipoVisitante())) {
            return false;
        }

    } else {

        // * Modificamos las anotaciones del equipo local

        // Anotaciones a favor
        anotacionAFavorLocal = equipoLocal.getAnotacionAFavor();
        anotacionAFavorLocal -= registroPartido.getAnotacionesLocal();
        if (!equipoLocal.setAnotacionAFavor(anotacionAFavorLocal)) {
            return false;
        }

        // Anotaciones en contra
        anotacionEnContraLocal = equipoLocal.getAnotacionEnContra();
        anotacionEnContraLocal -= registroPartido.getAnotacionesVisitante();
        if (!equipoLocal.setAnotacionEnContra(anotacionEnContraLocal)) {
            return false;
        }

        // Partidos jugados
        jugados = equipoLocal.getJugados();
        --jugados;
        if (!equipoLocal.setJugados(jugados)) {
            return false;
        }

        // * Modificamos las anotaciones del equipo visitante
        jugados = 0;

        // Anotaciones a favor
        anotacionAFavorVisitante = equipoVisitante.getAnotacionAFavor();
        anotacionAFavorVisitante -= registroPartido.getAnotacionesVisitante();
        if (!equipoVisitante.setAnotacionAFavor(anotacionAFavorVisitante)) {
            return false;
        }

        // Anotaciones en contra
        anotacionEnContraVisitante = equipoVisitante.getAnotacionEnContra();
        anotacionEnContraVisitante -= registroPartido.getAnotacionesLocal();
        if (!equipoVisitante.setAnotacionEnContra(anotacionEnContraVisitante)) {
            return false;
        }

        // Partidos jugados
        jugados = equipoVisitante.getJugados();
        --jugados;
        if (!equipoVisitante.setJugados(jugados)) {
            return false;
        }

        // * ¿Quitamos el id del partido del array?

        if (!equipoLocal.eliminarIdPartido(registroPartido.getId())) {
            return false;
        }
        if (!equipoVisitante.eliminarIdPartido(registroPartido.getId())) {
            return false;
        }

        // ! NOTA: LA FUNCION ELIMINAR ID PARTIDO MANEJA INTERNAMENTE EL AUMENTO DE LA CANTIDAD DE PARTIDOS

        // Agregamos la fecha de modificacion
        if (!equipoLocal.setFechaUltimaModificacion(std::time(nullptr))) {
            return false;
        }

        if (!equipoVisitante.setFechaUltimaModificacion(std::time(nullptr))) {
            return false;
        }

        // Guardamos los archivos
        if (!GestorArchivosBinarios::guardarRegistro<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoLocal, registroPartido.getIdEquipoLocal()) ||
            !GestorArchivosBinarios::guardarRegistro<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoVisitante, registroPartido.getIdEquipoVisitante())) {
            return false;
        }
    }

    return true;
}
