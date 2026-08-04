#include "../../include/persistence/gestorArchivosBinarios.hpp"
#include "../../include/models/torneo.hpp"
#include <filesystem>

bool GestorArchivosBinarios::abrirArchivoParaEscrituraYLectura(const std::string &rutaArchivo, std::fstream &archivo) {
    archivo.open(rutaArchivo.c_str(), std::ios::in | std::ios::out | std::ios::binary);
    return archivo.is_open();
}

bool GestorArchivosBinarios::abrirArchivoParaEscritura(const std::string &rutaArchivo, std::ofstream &archivo) {
    archivo.open(rutaArchivo.c_str(), std::ios::binary);
    return archivo.is_open();
}

bool GestorArchivosBinarios::abrirArchivoParaLectura(const std::string &rutaArchivo, std::ifstream &archivo) {
    archivo.open(rutaArchivo.c_str(), std::ios::binary);
    return archivo.is_open();
}

bool GestorArchivosBinarios::validarExisteArchivo(const std::string &rutaArchivo) { return std::filesystem::exists(rutaArchivo); }

bool GestorArchivosBinarios::validarArchivoVacio(const std::string &rutaArchivo) {
    // Si el archivo existe y está vacío ( es igual a 0 su tamaño) retornamos true
    if (std::filesystem::exists(rutaArchivo) && std::filesystem::file_size(rutaArchivo) == 0) {
        return true;
    }
    return false;
}

bool GestorArchivosBinarios::validarArchivoSinRegistros(const std::string &rutaArchivo) {
    ArchivoHeader header = GestorArchivosBinarios::obtenerHeader(rutaArchivo);

    if (header.getRegistrosActivos() == 0 || std::filesystem::file_size(rutaArchivo) == ArchivoHeader::getTamano()) {
        return true;
    }
    return false;
}

bool GestorArchivosBinarios::inicializarArchivo(const std::string &rutaArchivo) {
    // Verificamos que existe el archivo
    if (validarExisteArchivo(rutaArchivo)) {
        return true;
    } else {
        std::ofstream archivo;
        if (!abrirArchivoParaEscritura(rutaArchivo, archivo)) {
            return false;
        }

        // Si el archivo es de torneo, inicializamos un registro Torneo vacío
        if (rutaArchivo == constantes::NOMBRE_ARCHIVO_TORNEO) {
            Torneo torneoVacio;
            const time_t ahora = std::time(nullptr);
            torneoVacio.setInicializado(false);
            torneoVacio.setFechaCreacion(ahora);
            torneoVacio.setFechaUltimaModificacion(ahora);

            archivo.seekp(0, std::ios::beg);
            archivo.write(reinterpret_cast<const char *>(&torneoVacio), Torneo::getTamano());
            if (!verificarOperacion(archivo)) {
                archivo.close();
                return false;
            }

            archivo.close();
            return true;
        }

        // inicializamos un archivo Header
        ArchivoHeader nuevo;

        // Movemos el puntero de escritura al inicio por seguridad
        archivo.seekp(0, std::ios::beg);

        // escribimos el header en el archivo binario
        archivo.write(reinterpret_cast<const char *>(&nuevo), ArchivoHeader::getTamano());

        // Verificamos que se haya escrito bien
        if (!verificarOperacion(archivo)) {
            archivo.close();
            return false;
        }

        // cerramos el archivo
        archivo.close();
        return true;
    }
}

std::streampos GestorArchivosBinarios::calcularPosicion(const size_t &indiceFisico, const size_t &tamRegistros, const size_t &tamArchivoHeader) {
    std::streampos posicion = (tamArchivoHeader + (indiceFisico * tamRegistros));
    return posicion;
}

// =======================================================================================
// Métodos Públicos de Control General y Sistema
// =======================================================================================

bool GestorArchivosBinarios::inicializarSistemaArchivos() {
    return GestorArchivosBinarios::inicializarArchivo(constantes::NOMBRE_ARCHIVO_TORNEO) && GestorArchivosBinarios::inicializarArchivo(constantes::NOMBRE_ARCHIVO_EQUIPOS) &&
           GestorArchivosBinarios::inicializarArchivo(constantes::NOMBRE_ARCHIVO_JUGADORES) && GestorArchivosBinarios::inicializarArchivo(constantes::NOMBRE_ARCHIVO_PARTIDOS);
}

ArchivoHeader GestorArchivosBinarios::obtenerHeader(const std::string &rutaArchivo) {

    // Lo declaramos en formato error
    ArchivoHeader header = {-1, -1, -1, -1};

    // Verificamos que exista y que no esté vacío
    if (!GestorArchivosBinarios::validarExisteArchivo(rutaArchivo) || validarArchivoVacio(rutaArchivo)) {
        return header;
    }

    // Abrimos el archivo para leerlo
    std::ifstream archivo;
    if (abrirArchivoParaLectura(rutaArchivo, archivo)) {

        // Posicionamos al principio
        archivo.seekg(0, std::ios::beg);

        // Lemos y verificamos que la lectura fue correcta
        archivo.read(reinterpret_cast<char *>(&header), ArchivoHeader::getTamano());
        if (!verificarOperacion(archivo)) {
            archivo.close();
            return header;
        }
    }
    archivo.close();
    return header;
}

bool GestorArchivosBinarios::actualizarHeader(const std::string &rutaArchivo, const ArchivoHeader &header) {

    // Verificamos que existe el archivo
    if (!validarExisteArchivo(rutaArchivo)) {
        return false;
    }

    // Abrimos en modo lectura escritura
    std::fstream archivo;
    if (!abrirArchivoParaEscrituraYLectura(rutaArchivo, archivo)) {
        return false;
    }

    // Nos posicionamos al principio y sobreescribimos el header
    archivo.seekp(0, std::ios::beg);
    archivo.write(reinterpret_cast<const char *>(&header), ArchivoHeader::getTamano());

    if (!verificarOperacion(archivo)) {
        archivo.close();
        return false;
    }
    archivo.close();
    return true;
}

Torneo GestorArchivosBinarios::obtenerInformacionTorneo() {
    Torneo torneo;
    // Validamos que existe el archivo
    if (!validarExisteArchivo(constantes::NOMBRE_ARCHIVO_TORNEO)) {
        return torneo;
    }

    // Lo abrimos en modo lectura
    std::ifstream archivo;
    if (abrirArchivoParaLectura(constantes::NOMBRE_ARCHIVO_TORNEO, archivo)) {

        // Posicionamos al principio
        archivo.seekg(0, std::ios::beg);

        // Leemos
        archivo.read(reinterpret_cast<char *>(&torneo), Torneo::getTamano());
        if (!verificarOperacion(archivo)) {
            archivo.close();
            return torneo;
        }
    }
    archivo.close();
    return torneo;
}
