#include "../../include/manager/operacionesPartidos.hpp"
#include "../../include/manager/operacionesEquipos.hpp"
#include "../../include/manager/operacionesJugadores.hpp"
#include "../../include/models/equipos.hpp"
#include "../../include/persistence/gestorArchivosBinarios.hpp"
#include "../../include/persistence/gestorArchivosTexto.hpp"
#include "../../include/utils/constantes.hpp"
#include "../../include/utils/formatos.hpp"

std::string OperacionesPartidos::obtenerEstadoPorId(const int idEstado) {

    // Obtenemos los estados
    std::vector<std::vector<std::string>> listaEstadoPartidos = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_ESTADO_PARTIDOS_CONFIG, ',');
    if (listaEstadoPartidos.empty()) {
        return constantes::ERROR_STRING;
    }

    for (size_t e = 0; listaEstadoPartidos.size(); ++e) {

        std::vector<std::string> linea = listaEstadoPartidos[e];
        int idAuxiliar = Formatos::parsearValor<int>(linea[0], constantes::ERROR_INT);

        if (idAuxiliar == constantes::ERROR_INT) {
            return constantes::ERROR_STRING;
        }

        if (idEstado == idAuxiliar) {
            return linea[1];
        }
    }

    return constantes::ERROR_STRING;
}

int OperacionesPartidos::obtenerMinJugadoresPorDeporte() {

    // Declaramos la variable torneo y obtenemos su informacion de minimo de jugadores
    Torneo torneo;
    torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    // Devolvemos el minimo de jugadores
    return torneo.getReglasTorneo().getMinJugadores();
}

bool OperacionesPartidos::hayPartidoProgramadoEntre2(const Partido &partido) {

    // Declaramos y obtenemos la lista de partidos registrados
    std::vector<Partido> listaDePartidos;
    listaDePartidos = GestorArchivosBinarios::listarRegistros<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS);

    // Validamos que si hayan partidos
    if (listaDePartidos.size() <= 0) {
        return false;
    }

    // Por cada partido buscamos si hay partidos entre 2
    for (size_t e = 0; e < listaDePartidos.size(); ++e) {

        // Lo pasamos a una variable para manejarlo mejor
        Partido pAux = listaDePartidos[e];

        // verificamos la condicion de partido entre 2
        bool tienePartidoEntreSi = ((pAux.getIdEquipoLocal() == partido.getIdEquipoLocal()) && (pAux.getIdEquipoVisitante() == partido.getIdEquipoVisitante())) ||
                                   ((pAux.getIdEquipoLocal() == partido.getIdEquipoVisitante()) && (pAux.getIdEquipoVisitante() == partido.getIdEquipoLocal()));

        // Convertimos el const char* a un string con el constructor para mejorar la comparacion
        std::string estadoAux(pAux.getEstado());

        // Si hay partido entre si y el partido esta en estado programado devolvemos true
        if (tienePartidoEntreSi && (estadoAux == OperacionesPartidos::obtenerEstadoPorId(0))) {
            return true;
        }
    }

    // Si no encontramos nada devolvemos false
    return false;
}

// Retorna array de partidos con ese estado ("PROGRAMADO", "JUGADO", "CANCELADO")
std::vector<Partido> OperacionesPartidos::listarPartidosPorSuEstado(const char *estado) {

    std::vector<Partido> listaDePartidosPorEstado;

    // Declaramos y obetenemos la cantidad de partidos que hay
    std::vector<Partido> listaDePartidos;
    listaDePartidos = GestorArchivosBinarios::listarRegistros<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS);

    // Validamos que el vector no esté vacio por algun error
    if (listaDePartidos.size() <= 0) {
        return listaDePartidosPorEstado;
    }

    for (size_t e = 0; e < listaDePartidos.size(); ++e) {

        // Convertimos a una variable partido para facilitar la manipulacion
        Partido pAux = listaDePartidos[e];

        // Si el partido esta eliminado lo ignoramos
        if (pAux.getEliminado()) {
            continue;
        }

        // Si la cantidad de partidos por estado obtenidas aun no pasa la cantidad maxima de resultados
        if (listaDePartidosPorEstado.size() < constantes::MAX_RESULTADOS) {

            // Si tienen el mismo estado que el que nos piden lo guardamos en el vector
            if (std::strcmp(pAux.getEstado(), estado) == 0) {
                listaDePartidosPorEstado.push_back(pAux);
            }
            // si no  rompemos el bucle for
        } else {
            break;
        }

        // Devolvemos el vector
        return listaDePartidosPorEstado;
    }
}

bool OperacionesPartidos::programarPartido(Partido &nuevoPartido) {

    // * Validaciones

    // Leemos el header del archivo de Equipos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    // Verificamos que el header se haya leido correctamente (no devuelva -1)
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        return false;
    }

    // Si no hay al menos 2 equipos no se puede programar un partido
    if (headerEquipos.getRegistrosActivos() <= 1) {
        return false;
    }

    // No se puede programar un partido entre el mismo equipo
    if (nuevoPartido.getIdEquipoLocal() == nuevoPartido.getIdEquipoVisitante()) {
        return false;
    }

    Equipo eqLocal, eqVisitante;
    bool existeEquipoLocal = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqLocal, nuevoPartido.getIdEquipoLocal());
    bool existeEquipoVisitante = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, nuevoPartido.getIdEquipoVisitante());

    // Si no existen devolvemos false
    if ((!existeEquipoLocal) || (!existeEquipoVisitante)) {
        return false;
    }

    // Buscamos el archivo header de partidos
    ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);
    if (headerPartidos.getCantidadRegistros() == constantes::ERROR_INT) {
        return false;
    }

    // Verificamos que no haya un partido programado entre ellos 2
    if (hayPartidoProgramadoEntre2(nuevoPartido)) {
        return false;
    }

    // Obtenemos el minimo de jugadores por partido
    int minimoRequerido = OperacionesPartidos::obtenerMinJugadoresPorDeporte();
    /*
    // Verificar que ambos equipos tengan suficientes jugadores
    if (eqLocal.numJugadores < minimoRequerido || eqVisitante.numJugadores < minimoRequerido) {
        return false;
    }*/

    // * Procedemos a programar el partido

    // Asiganmos el proximo id
    bool flagSet = nuevoPartido.setId(headerPartidos.getProximoId());
    if (!flagSet) {
        return false;
    }

    // Asignamos los valores iniciales
    char estado[constantes::TAMANO_ESTADO];

    Formatos::copiarCadena(estado, (OperacionesPartidos::obtenerEstadoPorId(0)).c_str(), constantes::TAMANO_ESTADO);

    flagSet = nuevoPartido.setEstado(estado); // PROGRAMADO
    if (!flagSet) {
        return false;
    }

    // Asginamos el por fecto de goles de local
    flagSet = nuevoPartido.setAnotacionesLocal(0);
    if (!flagSet) {
        return false;
    }

    // asignamos el por defectro de goles del visitante
    flagSet = nuevoPartido.setAnotacionesVisitante(0);
    if (!flagSet) {
        return false;
    }

    /*flagSet = nuevoPartido.seta(0);
    if (!flagSet) {
        return false;
    }*/

    // Asignamos que no esta eliminado
    flagSet = nuevoPartido.setEliminado(false);
    if (!flagSet) {
        return false;
    }

    // Asignamos los valores de tiempo
    flagSet = nuevoPartido.setFechaCreacion(std::time(nullptr));
    if (!flagSet) {
        return false;
    }

    // Asiganmos la fecha de ultima modificaion
    flagSet = nuevoPartido.setFechaUltimaModificacion(std::time(nullptr));
    if (!flagSet) {
        return false;
    }

    bool guardado = GestorArchivosBinarios::guardarNuevoRegistro<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS, nuevoPartido);

    // si ocurrió un error en el guardado devolvemos false;
    if (!guardado) {
        return false;
    }
    return true;
}

// TODO:   Victoria local  → local  +3 pts, +1 victoria  / visitante +1 derrota
// TODO:   Empate          → ambos  +1 pt,  +1 empate
// TODO:   Victoria visit. → visit. +3 pts, +1 victoria  / local     +1 derrota
bool OperacionesPartidos::registrarResultado(Partido registroPartido) {

    Torneo torneo;
    torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    // Si el tamano es 0 quiere decir que no se leyó nada
    if (torneo.getTamano() == 0) {
        return false;
    }

    // * Validaciones

    // Leemos el header del archivo de Equipos
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);
    ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);

    // Verificamos que el header se haya leido correctamente
    if (headerEquipos.getCantidadRegistros() == constantes::ERROR_INT) {
        return false;
    }

    // Si no hay al menos 2 equipos no se puede registrar un partido
    if (headerEquipos.getRegistrosActivos() <= 1) {
        return false;
    }

    // Verificamos que el header se haya leido correctamente
    if (headerPartidos.getCantidadRegistros() == constantes::ERROR_INT) {
        return false;
    }

    // Si no hay partidos activos no hacemos nada
    if (headerPartidos.getRegistrosActivos() <= 0) {
        return false;
    }

    // * Por seguridad de que la ui no nos  haya enviado un registro con datos errados
    if (registroPartido.getAnotacionesLocal() < 0 || registroPartido.getAnotacionesVisitante() < 0) {
        return false;
    }

    // Ademas verificamos que el numAnotaciones no sea mayor que el maximo
    if (registroPartido.getNumAnotaciones() > constantes::MAX_ANOTACIONES) {
        return false;
    }

    // Verificamos que el numero de tarjetas amarillas no supere el maximo
    if (registroPartido.getNumTarjetaAma() > constantes::MAX_TARJETAS_AMARILLAS) {
        return false;
    }

    // Verificamos que el numero de tarjetas rojas no supere el maximo
    if (registroPartido.getNumTarjetasRojas() > constantes::MAX_TARJETAS_ROJAS) {
        return false;
    }

    // * 1. Leemos el partido y verificamos que exsite

    // verificamos que el partido si exista (en estado programado);

    Partido partidoProgramado;
    int contador = 0;

    bool encontrado = GestorArchivosBinarios::buscarRegistrosPorId<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS, partidoProgramado, registroPartido.getId());
    if (!encontrado) {
        return false;
    }

    // Si encontró el partido verificamos que esté en estado programado
    if (std::strcmp(partidoProgramado.getEstado(), OperacionesPartidos::obtenerEstadoPorId(0).c_str()) != 0) {
        return false;
    }

    // * 3. Determinamos el resultado del partido

    // Si ocurre un empate en deportes donde no se permiten empates retornamos false
    if ((registroPartido.getAnotacionesLocal() == registroPartido.getAnotacionesVisitante()) && !torneo.getReglasTorneo().getPermiteEmpate()) {
        return false;
    }

    // Si el equipo local ganó
    if (registroPartido.getAnotacionesLocal() > registroPartido.getAnotacionesVisitante()) {

        if (!OperacionesEquipos::modificarEstadisticas(partidoProgramado.getIdEquipoLocal(), 1, torneo.getReglasTorneo().getPermiteEmpate())) {
            return false;
        }
        if (!OperacionesEquipos::modificarEstadisticas(partidoProgramado.getIdEquipoVisitante(), -1, torneo.getReglasTorneo().getPermiteEmpate())) {
            return false;
        }

        // Si el equipo visitante ganó
    } else if (registroPartido.getAnotacionesVisitante() > registroPartido.getAnotacionesLocal()) {

        if (!OperacionesEquipos::modificarEstadisticas(partidoProgramado.getIdEquipoLocal(), -1, torneo.getReglasTorneo().getPermiteEmpate())) {
            return false;
        }
        if (!OperacionesEquipos::modificarEstadisticas(partidoProgramado.getIdEquipoVisitante(), 1, torneo.getReglasTorneo().getPermiteEmpate())) {
            return false;
        }

    } else {
        if (!OperacionesEquipos::modificarEstadisticas(partidoProgramado.getIdEquipoLocal(), 0, torneo.getReglasTorneo().getPermiteEmpate())) {
            return false;
        }
        if (!OperacionesEquipos::modificarEstadisticas(partidoProgramado.getIdEquipoVisitante(), 0, torneo.getReglasTorneo().getPermiteEmpate())) {
            return false;
        }
    }

    // * 4. Agregamos el id del partido al array de cada equipo y aumentamos el numero de partidos
    if (!OperacionesEquipos::modificarAnotaciones(registroPartido)) {
        return false;
    }

    // Actualizamos las anotaciones/tarjetas del partido
    partidoProgramado.setAnotacionesLocal(registroPartido.getAnotacionesLocal());
    partidoProgramado.setAnotacionesVisitante(registroPartido.getAnotacionesVisitante());

    partidoProgramado.setTarjetasAmaLocal(registroPartido.getTarjetasAmaLocal());
    partidoProgramado.setTarjetasAmaVisitante(registroPartido.getTarjetasAmaVisitante());

    partidoProgramado.setTarjetasRojasLocal(registroPartido.getTarjetasRojasLocal());
    partidoProgramado.setAnotacionesVisitante(registroPartido.getTarjetasRojasVisitante());

    // Copiamos los datos de estadisticas
    for (size_t e = 0; e < partidoProgramado.getNumAnotaciones(); ++e) {
        const Anotacion *anotaciones = partidoProgramado.getAnotaciones();
        partidoProgramado.setAnotacionPorIndice(e, anotaciones[e]);
    }

    for (size_t e = 0; e < partidoProgramado.getNumTarjetaAma(); ++e) {
        const TarjetaAmarilla *tarjetasAma = partidoProgramado.getTarjetasAmarillas();
        partidoProgramado.setTarjetaAmarillaPorIndice(e, tarjetasAma[e]);
    }

    for (size_t e = 0; e < partidoProgramado.getNumTarjetasRojas(); ++e) {
        const TarjetaRoja *tarjetasRoj = partidoProgramado.getTarjetasRojas();
        partidoProgramado.setTarjetaRojaPorIndice(e, tarjetasRoj[e]);
    }

    // * 6. Actualizamos las estadisticas de los jugadores por cada gol / tarjeta
    OperacionesJugadores::modificarEstadisticas(registroPartido);

    // * 7. Cambiamos el estado del partido a jugado
    char estado[constantes::TAMANO_ESTADO];
    Formatos::copiarCadena(estado, OperacionesPartidos::obtenerEstadoPorId(1).c_str(), constantes::TAMANO_ESTADO);
    if (!partidoProgramado.setEstado(estado)) {
        return false;
    }
    if (!partidoProgramado.setFechaUltimaModificacion(std::time(nullptr))) {
        return false;
    }

    // * 9. Guardamos el partido
    if (!GestorArchivosBinarios::guardarRegistro<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS, partidoProgramado, partidoProgramado.getId())) {
        return false;
    }
    return true;
}

std::vector<Partido> OperacionesPartidos::listarPartidosPorEquipo(const int idEquipo) {
    std::vector<Partido> listaDePartidosPorEquipo;

    std::vector<Partido> listaDePartidos;
    listaDePartidos = GestorArchivosBinarios::listarRegistros<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS);
    if (listaDePartidos.size() <= 0) {
        return listaDePartidos;
    }

    for (size_t e = 0; e < listaDePartidos.size(); ++e) {

        Partido pAux = listaDePartidos[e];

        // Si encontramos un archivo que esta eliminado lo saltamos
        if (pAux.getEliminado()) {
            continue;
        }

        // Si aún no llegamos a la cantidad maxima de registros hacemos la comparacion
        if (listaDePartidosPorEquipo.size() < constantes::MAX_RESULTADOS) {

            // comparamos con los estados
            if (pAux.getIdEquipoLocal() == idEquipo || pAux.getIdEquipoVisitante() == idEquipo) {
                listaDePartidosPorEquipo.push_back(pAux);
            }

        } else {
            // sino salimos del bucle
            break;
        }
    }

    // Devolvemos la lista de partidos por equipo
    return listaDePartidosPorEquipo;
}

bool OperacionesPartidos::cancelarPartido(const int idPartido) {

    // * Buscar y leer el Partido
    Partido partidoAuxiliar;
    if (!GestorArchivosBinarios::buscarRegistrosPorId<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS, partidoAuxiliar, idPartido)) {
        return false;
    }

    // Por seguridad (No sea el caso de que haya un bug o un error no se)
    if (partidoAuxiliar.getEliminado()) {
        return false;
    }

    // Leemos el torneo para saber si permitia empate o no
    Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

    // Si está jugado debemos revertir todo
    if (std::strcmp(partidoAuxiliar.getEstado(), OperacionesPartidos::obtenerEstadoPorId(1).c_str()) == 0) {

        // Si el equipo local ganó
        if (partidoAuxiliar.getAnotacionesLocal() > partidoAuxiliar.getAnotacionesVisitante()) {

            if (!OperacionesEquipos::modificarEstadisticas(partidoAuxiliar.getIdEquipoLocal(), 1, torneo.getReglasTorneo().getPermiteEmpate(), true)) {
                return false;
            }
            if (!OperacionesEquipos::modificarEstadisticas(partidoAuxiliar.getIdEquipoVisitante(), -1, torneo.getReglasTorneo().getPermiteEmpate(), true)) {
                return false;
            }
            // Si el equipo visitante ganó
        } else if (partidoAuxiliar.getAnotacionesVisitante() > partidoAuxiliar.getAnotacionesLocal()) {
            if (!OperacionesEquipos::modificarEstadisticas(partidoAuxiliar.getIdEquipoLocal(), -1, torneo.getReglasTorneo().getPermiteEmpate(), true)) {
                return false;
            }
            if (!OperacionesEquipos::modificarEstadisticas(partidoAuxiliar.getIdEquipoVisitante(), 1, torneo.getReglasTorneo().getPermiteEmpate(), true)) {
                return false;
            }
        } else {
            if (!OperacionesEquipos::modificarEstadisticas(partidoAuxiliar.getIdEquipoLocal(), 0, torneo.getReglasTorneo().getPermiteEmpate(), true)) {
                return false;
            }
            if (!OperacionesEquipos::modificarEstadisticas(partidoAuxiliar.getIdEquipoVisitante(), 0, torneo.getReglasTorneo().getPermiteEmpate(), true)) {
                return false;
            }
        }

        // * Revertimos las anotaciones de los equipos
        if (!OperacionesEquipos::modificarAnotaciones(partidoAuxiliar, true)) {
            return false;
        }

        // * 2. Buscamos los jugadores para revertir estadisticas

        OperacionesJugadores::modificarEstadisticas(partidoAuxiliar, true);

        if (!partidoAuxiliar.setAnotacionesLocal(0)) {
            return false;
        }
        if (!partidoAuxiliar.setAnotacionesVisitante(0)) {
            return false;
        }

        if (!partidoAuxiliar.setTarjetasAmaLocal(0)) {
            return false;
        }

        if (!partidoAuxiliar.setTarjetasAmaVisitante(0)) {
            return false;
        }

        if (!partidoAuxiliar.setTarjetasRojasLocal(0)) {
            return false;
        }
        if (!partidoAuxiliar.setTarjetasRojasVisitante(0)) {
            return false;
        }

        // Guardamos el partido


        // Colocamos el partido como cancelado
        char estado[constantes::TAMANO_ESTADO];
        Formatos::copiarCadena(estado, OperacionesPartidos::obtenerEstadoPorId(2).c_str(), constantes::TAMANO_ESTADO);

        if (!partidoAuxiliar.setEstado(estado)) {
            return false;
        }

        if (!partidoAuxiliar.setFechaUltimaModificacion(std::time(nullptr))) {
            return false;
        }
        // pAux.eliminado = true;

        // * Guardamos el partido
        if (!GestorArchivosBinarios::guardarRegistro(constantes::NOMBRE_ARCHIVO_PARTIDOS, partidoAuxiliar, idPartido)) {
            return false;
        }

        ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);
        if (headerPartidos.getCantidadRegistros() == -1) {
            return false;
        }

        headerPartidos.disminuirRegistrosActivos();
        GestorArchivosBinarios::actualizarHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS, headerPartidos);
        return true;

        return true;

        // Si está programado solo lo colocamos como eliminado
    } else if (std::strcmp(partidoAuxiliar.getEstado(), OperacionesPartidos::obtenerEstadoPorId(0).c_str()) == 0) {

        // Colocamos el partido como cancelado
        char estado[constantes::TAMANO_ESTADO];
        Formatos::copiarCadena(estado, OperacionesPartidos::obtenerEstadoPorId(2).c_str(), constantes::TAMANO_ESTADO);

        if (!partidoAuxiliar.setEstado(estado)) {
            return false;
        }

        // pAux.eliminado = true;
        if (!partidoAuxiliar.setFechaUltimaModificacion(std::time(nullptr))) {
            return false;
        }

        // Guardamos el partido
        if (!GestorArchivosBinarios::guardarRegistro(constantes::NOMBRE_ARCHIVO_PARTIDOS, partidoAuxiliar, idPartido)) {
            return false;
        }

        ArchivoHeader headerPartidos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS);
        if (headerPartidos.getCantidadRegistros() == -1) {
            return false;
        }

        headerPartidos.disminuirRegistrosActivos();
        GestorArchivosBinarios::actualizarHeader(constantes::NOMBRE_ARCHIVO_PARTIDOS, headerPartidos);
        return true;

    } else {
        return false; // Si está cancelado devolvemos error
    }
}
