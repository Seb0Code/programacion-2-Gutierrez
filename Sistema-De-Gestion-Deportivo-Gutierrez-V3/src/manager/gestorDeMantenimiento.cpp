
#include "../../include/manager/gestorDeMantenimiento.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

bool GestorMantenimiento::copiarArchivoBinario(const char *rutaOriginal, const char *rutaDestino) {
    std::ifstream archivoOrigen(rutaOriginal, std::ios::binary);
    if (!archivoOrigen.is_open()) {
        return false;
    }

    std::ofstream archivoDestino(rutaDestino, std::ios::binary | std::ios::trunc);
    if (!archivoDestino.is_open()) {
        archivoOrigen.close();
        return false;
    }

    archivoDestino << archivoOrigen.rdbuf(); // Copia todo directamente
    archivoOrigen.close();
    archivoDestino.close();

    // return true;
    return !archivoOrigen.bad() && !archivoDestino.fail();
}

// Lo coloqué bool para que retornemos false si ocurrió un error
bool GestorMantenimiento::verificarIntegridadReferencial(InformeIntegridad &informe) {

    // * Paso 1. Recorrer ArchivoJugadores.bin

    /*// Abrimos el archivo de jugadores
    std::ifstream archivo;
    archivo.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary);*/
    ArchivoHeader headerEquipos = GestorArchivosBinarios::obtenerHeader(constantes::NOMBRE_ARCHIVO_EQUIPOS);

    /*// Verificamos que se haya abierto correctamente
    if (!archivo.is_open()) {
        return false;
    }*/

    if (headerEquipos.getRegistrosActivos() > 0) {
        for (size_t e = 0; e < headerEquipos.getRegistrosActivos(); ++e) {
            informe.incrementarEquiposVerificados();
        }
    }

    std::vector<Jugador> listaDeJugadores = GestorArchivosBinarios::listarRegistros<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES);

    // Por cada jugador
    for (size_t e = 0; e < listaDeJugadores.size(); ++e) {

        // Buscamos el equipo del jugador para ver si existe
        Equipo equipoAuxiliar;
        bool existeEquipo = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, listaDeJugadores[e].getIdEquipo());

        if (!existeEquipo && informe.getTotalDeReferenciasRotas() < constantes::MAX_RESULTADOS) {
            ReferenciaRota refAux;
            refAux.setTipoDeReferencia("JUGADOR");
            refAux.setIdOrigen(listaDeJugadores[e].getId());
            refAux.setIdReferenciaRota(listaDeJugadores[e].getIdEquipo());
            informe.agregarReferenciaRota(refAux);
            // informe.totalDeReferenciasRotas++;
        }

        informe.incrementarJugadoresVerificados();
    }

    // Ahora recorremos los partidos
    std::vector<Partido> listaDePartidos = GestorArchivosBinarios::listarRegistros<Partido>(constantes::NOMBRE_ARCHIVO_PARTIDOS);

    // Revismaos cada partido
    for (size_t e = 0; e < listaDePartidos.size(); ++e) {
        Equipo eqAux;
        bool existe = false;

        // Buscamos el equipo local
        existe = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqAux, listaDePartidos[e].getIdEquipoLocal());

        if (!existe && informe.getTotalDeReferenciasRotas() < constantes::MAX_RESULTADOS) {
            ReferenciaRota refAux;
            refAux.setTipoDeReferencia("PARTIDO_LOCAL");
            refAux.setIdOrigen(listaDePartidos[e].getId());
            refAux.setIdReferenciaRota(listaDePartidos[e].getIdEquipoLocal());
            informe.agregarReferenciaRota(refAux);
            // informe.totalDeReferenciasRotas++;
        }

        // Buscamos el equipo visitante
        existe = GestorArchivosBinarios::buscarRegistrosPorId<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS, eqAux, listaDePartidos[e].getIdEquipoVisitante());

        if (!existe && informe.getTotalDeReferenciasRotas() < constantes::MAX_RESULTADOS) {
            ReferenciaRota refAux;
            refAux.setTipoDeReferencia("PARTIDO_VISITANTE");
            refAux.setIdOrigen(listaDePartidos[e].getId());
            refAux.setIdReferenciaRota(listaDePartidos[e].getIdEquipoVisitante());
            informe.agregarReferenciaRota(refAux);
            // informe.totalDeReferenciasRotas++;
        }

        // Recorremos por cada gol
        for (size_t r = 0; r < listaDePartidos[e].getNumAnotaciones(); ++r) {

            // Si es un autogol por ejemplo
            if (listaDePartidos[e].getAnotaciones()[r].getIdJugador() <= 0) {
                continue;
            }

            Jugador jugadorAux;

            bool existe =
                GestorArchivosBinarios::buscarRegistrosPorId<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES, jugadorAux, listaDePartidos[e].getAnotaciones()[r].getIdJugador());

            // si no se encontró nada
            if (!existe && informe.getTotalDeReferenciasRotas() < constantes::MAX_RESULTADOS) {
                ReferenciaRota refAux;
                refAux.setTipoDeReferencia("GOL");
                refAux.setIdOrigen(listaDePartidos[e].getId());
                refAux.setIdReferenciaRota(listaDePartidos[e].getAnotaciones()[r].getIdJugador());
                informe.agregarReferenciaRota(refAux);
                // informe.totalDeReferenciasRotas++;
            }
        }

        informe.incrementarPartidosVerificados();
    }
    Formatos::pausarPrograma();
    return true;
}

bool GestorMantenimiento::crearBackup(std::string &nombreCarpeta) {

    // 1. Obtenemos el tiempo actual
    std::time_t tiempoActual = std::time(nullptr);
    std::tm *infoTiempo = std::localtime(&tiempoActual);

    // 2. Formatear el nombre de la carpeta (backup_YYYY-MM-DD_HH-MM)
    char bufferNombre[50];
    std::strftime(bufferNombre, sizeof(bufferNombre), "backup_%Y-%m-%d_%H-%M", infoTiempo);
    nombreCarpeta = bufferNombre;

    fs::path rutaBase = constantes::RUTA_BACKUPS;
    fs::path rutaBackup = rutaBase / nombreCarpeta;

    try {
        if (!fs::exists(rutaBase)) {
            fs::create_directories(rutaBase);
        }

        if (!fs::exists(rutaBackup)) {
            if (!fs::create_directory(rutaBackup)) {
                return false;
            }
        }
    } catch (const fs::filesystem_error &) {
        return false;
    }

    // Variable Auxiliar
    std::string rutaAux;

    // 3. Copiar los 4 archivos uno por uno
    rutaAux = (rutaBackup / "equipos.bin").string();
    bool ok1 = copiarArchivoBinario(constantes::NOMBRE_ARCHIVO_EQUIPOS.c_str(), rutaAux.c_str());
    rutaAux = (rutaBackup / "partidos.bin").string();
    bool ok2 = copiarArchivoBinario(constantes::NOMBRE_ARCHIVO_PARTIDOS.c_str(), rutaAux.c_str());
    rutaAux = (rutaBackup / "jugadores.bin").string();
    bool ok3 = copiarArchivoBinario(constantes::NOMBRE_ARCHIVO_JUGADORES.c_str(), rutaAux.c_str());
    rutaAux = (rutaBackup / "torneo.bin").string();
    bool ok4 = copiarArchivoBinario(constantes::NOMBRE_ARCHIVO_TORNEO.c_str(), rutaAux.c_str());

    // 4. Verificar e informar el resultado
    return ok1 && ok2 && ok3 && ok4;
}
