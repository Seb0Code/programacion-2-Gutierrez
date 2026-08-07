#include "../../include/manager/operacionesTorneo.hpp"
#include "../../include/persistence/gestorArchivosTexto.hpp"
#include "../../include/utils/constantes.hpp"
#include "../../include/utils/formatos.hpp"
#include "../../include/utils/validaciones.hpp"
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <sstream>

std::string OperacionesTorneo::convertirFechaTimeAString(const time_t &fecha) {
    // Si pasan una fecha invalida devolvemos N/A
    if (fecha == 0) {
        return "N/A";
    }

    // Transformamos el timepo a una variable tm para que los separe en dia hora año...
    // Dependiendo de la zona horaria local del usuario
    std::tm *tmPtr = std::localtime(&fecha);

    // Si la fecha no es valida Devolvemos fecha invalida
    if (!tmPtr) {
        return "Fecha Inválida";
    }

    std::ostringstream ss;

    // Convertimos el puntero a el formaro dia/mes/año Hora:minuto
    ss << std::put_time(tmPtr, "%d/%m/%Y %H:%M");
    return ss.str();
}

std::string OperacionesTorneo::buscarDeporteEnConfig(const int idBuscado) {

    // Validamos el id con nuestro gestor de validaciones
    if (!GestorDeValidaciones::validarId(idBuscado)) {
        return constantes::ERROR_STRING;
    }

    // Validamos que el archivo en el que vamos a buscar exista
    if (GestorArchivosTexto::validarArchivoVacio(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG) ||
        !GestorArchivosTexto::validarExisteArchivo(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG)) {
        return constantes::ERROR_STRING;
    }

    std::vector<std::vector<std::string>> listaDeDeportes;

    // Le pedimos al gestor de texto que nos devuelva todos los deportes
    listaDeDeportes = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG);

    // Buscamos el deporte por su id // Como tiene encabezado empezamos desde 1
    for (size_t e = 1; e < listaDeDeportes.size(); ++e) {
        // Obtenemos el id al inicio y lo transformamos a numero
        std::string idStr = listaDeDeportes[e][0];
        int id = std::stoi(idStr); // Esto debe realizarse con una funcion que se encargue de hacer parsing
        if (id == idBuscado) {
            return listaDeDeportes[e][1]; // Devolvemos el nombre del Deporte
        }
    }

    // Si no lo encontro devolver error
    return constantes::ERROR_STRING;
}

ReglasTorneo OperacionesTorneo::buscarReglasDeporteEnConfig(const int idBuscado) {
    ReglasTorneo reglas;

    if (!GestorDeValidaciones::validarId(idBuscado)) {
        return reglas;
    }

    if (GestorArchivosTexto::validarArchivoVacio(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG) ||
        !GestorArchivosTexto::validarExisteArchivo(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG)) {
        return reglas;
    }

    std::vector<std::vector<std::string>> listaDeDeportes;
    listaDeDeportes = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG);

    for (size_t e = 1; e < listaDeDeportes.size(); ++e) {
        if (listaDeDeportes[e].size() < 8) {
            continue;
        }

        int id = Formatos::parsearValor<int>(listaDeDeportes[e][0], constantes::ERROR_INT);
        if (id == constantes::ERROR_INT) {
            continue;
        }

        if (id == idBuscado) {
            bool permiteEmpate = (listaDeDeportes[e][6] == "1" || listaDeDeportes[e][6] == "true" || listaDeDeportes[e][6] == "TRUE");
            bool tarjetasHabilitadas = (listaDeDeportes[e][7] == "1" || listaDeDeportes[e][7] == "true" || listaDeDeportes[e][7] == "TRUE");
            return ReglasTorneo(listaDeDeportes[e][1], Formatos::parsearValor<int>(listaDeDeportes[e][2], constantes::ERROR_INT),
                                Formatos::parsearValor<int>(listaDeDeportes[e][3], constantes::ERROR_INT),
                                Formatos::parsearValor<int>(listaDeDeportes[e][4], constantes::ERROR_INT),
                                Formatos::parsearValor<int>(listaDeDeportes[e][5], constantes::ERROR_INT), permiteEmpate, tarjetasHabilitadas);
        }
    }

    return reglas;
}

ReglasTorneo OperacionesTorneo::buscarReglasDeporteEnConfig(const std::string &nombreDeporte) {
    ReglasTorneo reglas;

    if (nombreDeporte.empty()) {
        return reglas;
    }

    if (GestorArchivosTexto::validarArchivoVacio(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG) ||
        !GestorArchivosTexto::validarExisteArchivo(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG)) {
        return reglas;
    }

    std::vector<std::vector<std::string>> listaDeDeportes;
    listaDeDeportes = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG);

    for (size_t e = 1; e < listaDeDeportes.size(); ++e) {
        if (listaDeDeportes[e].size() < 8) {
            continue;
        }

        if (listaDeDeportes[e][1] == nombreDeporte) {
            bool permiteEmpate = (listaDeDeportes[e][6] == "1" || listaDeDeportes[e][6] == "true" || listaDeDeportes[e][6] == "TRUE");
            bool tarjetasHabilitadas = (listaDeDeportes[e][7] == "1" || listaDeDeportes[e][7] == "true" || listaDeDeportes[e][7] == "TRUE");
            return ReglasTorneo(listaDeDeportes[e][1], Formatos::parsearValor<int>(listaDeDeportes[e][2], constantes::ERROR_INT),
                                Formatos::parsearValor<int>(listaDeDeportes[e][3], constantes::ERROR_INT),
                                Formatos::parsearValor<int>(listaDeDeportes[e][4], constantes::ERROR_INT),
                                Formatos::parsearValor<int>(listaDeDeportes[e][5], constantes::ERROR_INT), permiteEmpate, tarjetasHabilitadas);
        }
    }

    return reglas;
}

std::string OperacionesTorneo::buscarPosicionJugadorEnConfig(const int idPosicionBuscada, const int idDeporteBuscado) {

    // Validamos los ids
    if (!GestorDeValidaciones::validarId(idPosicionBuscada) || !GestorDeValidaciones::validarId(idDeporteBuscado)) {
        return constantes::ERROR_STRING;
    }

    // Validamos que el archivo en el que vamos a buscar exista
    if (GestorArchivosTexto::validarArchivoVacio(constantes::RUTA_ARCHIVO_POSICIONES_DEPORTES) ||
        !GestorArchivosTexto::validarExisteArchivo(constantes::RUTA_ARCHIVO_POSICIONES_DEPORTES)) {
        return constantes::ERROR_STRING;
    }

    // Creamos y pedimos la lista de posiciones
    std::vector<std::vector<std::string>> listaDePosiciones;
    listaDePosiciones = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_POSICIONES_DEPORTES);

    // Buscamos el deporte por su id // Como tiene encabezado empezamos desde 1
    for (size_t e = 1; e < listaDePosiciones.size(); ++e) {

        // Obtenemos el id al inicio y lo transformamos a numero
        std::string idDeporteStr = listaDePosiciones[e][1];
        std::string idPosicionStr = listaDePosiciones[e][0];

        int idDeporteAux = std::stoi(idDeporteStr); // Esto debe realizarse con una funcion que se encargue de hacer parsing
        int idPosicionAux = std::stoi(idPosicionStr);

        // Si el id del deporte y el id de la posicion coincide devolvemos la posicion
        if ((idDeporteAux == idDeporteBuscado) && (idPosicionAux == idPosicionBuscada)) {
            return listaDePosiciones[e][3];
        }
    }

    // Si no lo encontro devolver error
    return constantes::ERROR_STRING;
}

std::vector<std::string> OperacionesTorneo::listarNombresDeportes() {
    std::vector<std::string> listaDeNombresDeportes;

    // Validamos que el archivo en el que vamos a buscar exista
    if (GestorArchivosTexto::validarArchivoVacio(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG) ||
        !GestorArchivosTexto::validarExisteArchivo(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG)) {
        return listaDeNombresDeportes; // Devolvemos un vector vacio
    }

    std::vector<std::vector<std::string>> listaDeDeportes;
    // Le pedimos al gestor de texto que nos devuelva todos los deportes
    listaDeDeportes = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG);

    for (size_t e = 1; e < listaDeDeportes.size(); ++e) {
        listaDeNombresDeportes.push_back(listaDeDeportes[e][1]);
    }

    return listaDeNombresDeportes;
}

std::vector<std::string> OperacionesTorneo::listarNombresPosicionesPorDeporte(const int idDeporteBuscado) {
    std::vector<std::string> listaDeNombresPosiciones;

    // Validamos los ids
    if (!GestorDeValidaciones::validarId(idDeporteBuscado)) {
        return listaDeNombresPosiciones;
    }

    // Validamos que el archivo en el que vamos a buscar exista
    if (GestorArchivosTexto::validarArchivoVacio(constantes::RUTA_ARCHIVO_POSICIONES_DEPORTES) ||
        !GestorArchivosTexto::validarExisteArchivo(constantes::RUTA_ARCHIVO_POSICIONES_DEPORTES)) {
        return listaDeNombresPosiciones;
    }

    // Creamos y pedimos la lista de posiciones
    std::vector<std::vector<std::string>> listaDePosiciones;
    listaDePosiciones = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_POSICIONES_DEPORTES);

    // Buscamos la posicon
    for (size_t e = 1; e < listaDePosiciones.size(); ++e) {

        // Obtenemos el id del deporte al inicio y lo transformamos a numero
        std::string idDeporteStr = listaDePosiciones[e][1];
        int idDeporteAux = std::stoi(idDeporteStr); // Esto debe realizarse con una funcion que se encargue de hacer parsing

        // Si el id del deporte y el id de la posicion coincide devolvemos la posicion
        if ((idDeporteAux == idDeporteBuscado)) {
            listaDeNombresPosiciones.push_back(listaDePosiciones[e][3]);
        }
    }

    // Devolvemos el vector con el nombre de las posiciones para el deporte pedido
    return listaDeNombresPosiciones;
}

std::string OperacionesTorneo::buscarFormatoEnConfig(const int idFormatoBuscado) {

    // Validamos los ids
    if (!GestorDeValidaciones::validarId(idFormatoBuscado)) {
        return constantes::ERROR_STRING;
    }

    // Validamos que el archivo en el que vamos a buscar exista
    if (GestorArchivosTexto::validarArchivoVacio(constantes::RUTA_ARCHIVO_FORMATOS_CONFIG) ||
        !GestorArchivosTexto::validarExisteArchivo(constantes::RUTA_ARCHIVO_FORMATOS_CONFIG)) {
        return constantes::ERROR_STRING;
    }

    // Creamos y pedimos la lista de posiciones
    std::vector<std::vector<std::string>> listaDeFormatos;
    listaDeFormatos = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_FORMATOS_CONFIG);

    for (size_t e = 1; e < listaDeFormatos.size(); ++e) {
        // Obtenemos el id del formato al inicio y lo transformamos a numero
        std::string idFormatoStr = listaDeFormatos[e][0];
        int idFormatoAux = std::stoi(idFormatoStr); // Esto debe realizarse con una funcion que se encargue de hacer parsing

        // Si el id del deporte y el id de la posicion coincide devolvemos la posicion
        if ((idFormatoAux == idFormatoBuscado)) {
            return listaDeFormatos[e][1];
        }
    }
    return constantes::ERROR_STRING;
}