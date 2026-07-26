/**
 * @file GestorArchivosTexto.hpp
 * @brief Clase de utilidad estática para la manipulación y persistencia de archivos de texto (.txt, .csv, .log).
 */

#ifndef GESTOR_ARCHIVOS_TEXTO_HPP
#define GESTOR_ARCHIVOS_TEXTO_HPP

#include <fstream>
#include <string>
#include <vector>

/**
 * @class GestorArchivosTexto
 * @brief Proporciona métodos estáticos genéricos para la lectura, escritura y procesamiento de archivos de texto.
 * @details Esta clase no es instanciable (Static Utility Class) y maneja operaciones de archivos de forma segura.
 */
class GestorArchivosTexto {

  private:
    /** @brief Constructor privado eliminado para evitar la instanciación de la clase. */
    GestorArchivosTexto() = delete;

    // =======================================================================================
    // Métodos Auxiliares de Manejo de Flujos (Streams)
    // =======================================================================================

    /**
     * @brief Verifica si el estado del flujo del archivo ha sufrido alguna falla de I/O.
     * @tparam TipoArchivo Tipo del flujo de archivo (std::ifstream, std::ofstream o std::fstream).
     * @param archivo Referencia al flujo de archivo a comprobar.
     * @return true si el flujo está en buen estado; false si ocurrió una falla (cierra el archivo automáticamente).
     */
    template <typename TipoArchivo> static inline bool verificarOperacion(TipoArchivo &archivo) {
        if (archivo.fail() || archivo.bad()) {
            archivo.close();
            return false;
        }
        return true;
    }

    /**
     * @brief Limpia las banderas de estado de error de un flujo de archivo si este falló.
     * @tparam TipoArchivo Tipo del flujo de archivo.
     * @param archivo Referencia al flujo de archivo a limpiar.
     */
    template <typename TipoArchivo> static inline void limpiarEstadoDeError(TipoArchivo &archivo) {
        if (archivo.fail()) {
            archivo.clear();
        }
    }

    /**
     * @brief Abre un archivo de texto en modo lectura y escritura (std::fstream).
     * @param rutaArchivo Ruta del archivo a abrir.
     * @param archivo Referencia al flujo de archivo que será abierto.
     * @return true si el archivo se abrió correctamente; false en caso contrario.
     */
    static bool abrirArchivoParaEscrituraYLectura(const std::string &rutaArchivo, std::fstream &archivo);

    /**
     * @brief Abre un archivo de texto para escritura (std::ofstream).
     * @param rutaArchivo Ruta del archivo a abrir.
     * @param archivo Referencia al flujo de salida.
     * @param modoAppend Si es true, escribe al final del archivo; si es false, sobrescribe el contenido.
     * @return true si se abrió correctamente; false en caso contrario.
     */
    static bool abrirArchivoParaEscritura(const std::string &rutaArchivo, std::ofstream &archivo, bool modoAppend = true);

    /**
     * @brief Abre un archivo de texto en modo lectura (std::ifstream).
     * @param rutaArchivo Ruta del archivo a abrir.
     * @param archivo Referencia al flujo de entrada.
     * @return true si se abrió correctamente; false en caso contrario.
     */
    static bool abrirArchivoParaLectura(const std::string &rutaArchivo, std::ifstream &archivo);

  public:
    // =======================================================================================
    // Operaciones sobre Archivos de Texto (.txt, .log)
    // =======================================================================================

    /**
     * @brief Lee un archivo de texto completo y retorna sus líneas en un vector.
     * @param rutaArchivo Ruta del archivo a leer.
     * @return std::vector<std::string> Conteniendo cada línea del archivo como un elemento.
     */
    static std::vector<std::string> leerLineas(const std::string &rutaArchivo);

    /**
     * @brief Escribe una sola línea de texto en un archivo.
     * @param rutaArchivo Ruta del archivo de destino.
     * @param linea Cadena de texto a escribir.
     * @param modoAppend true para añadir al final (logs), false para sobrescribir todo el archivo.
     * @return true si la escritura fue exitosa; false en caso contrario.
     */
    static bool escribirLinea(const std::string &rutaArchivo, const std::string &linea, const bool modoAppend = true);

    /**
     * @brief Escribe un conjunto de líneas completas en un archivo de texto.
     * @param rutaArchivo Ruta del archivo de destino.
     * @param lineas Vector con los contenidos a escribir.
     * @param modoAppend true para añadir al final, false para sobrescribir.
     * @return true si todas las líneas se escribieron correctamente.
     */
    static bool escribirLineas(const std::string &rutaArchivo, const std::vector<std::string> &lineas, bool modoAppend = false);

    // =======================================================================================
    // Operaciones sobre Archivos CSV (.csv)
    // =======================================================================================

    /**
     * @brief Lee un archivo CSV y lo transforma en una matriz de cadenas (Filas x Columnas).
     * @param rutaArchivo Ruta del archivo CSV.
     * @param delimitador Carácter que separa cada columna (por defecto ',').
     * @return std::vector<std::vector<std::string>> Matriz bidimensional con el contenido del CSV.
     */
    static std::vector<std::vector<std::string>> leerCSV(const std::string &rutaArchivo, char delimitador = ',');

    /**
     * @brief Escribe una matriz completa de datos en un archivo CSV.
     * @param ruta Ruta del archivo de destino.
     * @param matriz Matriz de strings (Filas x Columnas) a guardar.
     * @param delimitador Carácter separador de columnas (por defecto ',').
     * @return true si se guardó con éxito; false en caso contrario.
     */
    static bool escribirCSV(const std::string &ruta, const std::vector<std::vector<std::string>> &matriz, char delimitador = ',');

    // =======================================================================================
    // Métodos de Validación y Utilidades de Archivo
    // =======================================================================================

    /**
     * @brief Divide una cadena de texto en subcadenas según un carácter delimitador (lo tokeniza / divide en tokens).
     * @param texto Cadena de texto a procesar.
     * @param delimitador Carácter por el cual dividir el texto.
     * @return std::vector<std::string> Lista de tokens obtenidos.
     */
    static std::vector<std::string> dividirTexto(const std::string &texto, char delimitador);

    /**
     * @brief Comprueba la existencia física de un archivo en el disco.
     * @param rutaArchivo Ruta del archivo a consultar.
     * @return true si el archivo existe; false si no.
     */
    static bool validarExisteArchivo(const std::string &rutaArchivo);

    /**
     * @brief Determina si un archivo existe pero no contiene datos (0 bytes).
     * @param rutaArchivo Ruta del archivo a validar.
     * @return true si el archivo tiene un tamaño de 0 bytes; false en caso contrario.
     */
    static bool validarArchivoVacio(const std::string &rutaArchivo);

    /**
     * @brief Borra todo el contenido de un archivo dejándolo totalmente vacío (0 bytes).
     * @param rutaArchivo Ruta del archivo a vaciar.
     * @return true si se limpió con éxito.
     */
    static bool limpiarArchivo(const std::string &rutaArchivo);

    /**
     * @brief Cuenta la cantidad total de líneas que tiene un archivo de texto.
     * @param rutaArchivo Ruta del archivo a contabilizar.
     * @return size_t Número de líneas encontradas (0 si no existe o está vacío).
     */
    static size_t contarLineas(const std::string &rutaArchivo);
};

#endif // GESTOR_ARCHIVOS_TEXTO_HPP