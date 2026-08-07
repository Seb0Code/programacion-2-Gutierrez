/**
 * @file GestorArchivosBinarios.hpp
 * @brief Clase utilitaria estática para la administración de archivos binarios genéricos.
 * @details Proporciona operaciones de bajo nivel para persistencia binaria con soporte para cabeceras
 * (`ArchivoHeader`), acceso aleatorio directo mediante offsets en disco y plantillas (`templates`)
 * para la reutilización con diversas entidades del sistema.
 */

#ifndef GESTOR_ARCHIVOS_BINARIOS_HPP
#define GESTOR_ARCHIVOS_BINARIOS_HPP

#include "../../include/models/archivoHeader.hpp"
#include "../../include/models/torneo.hpp"
#include "../../include/utils/constantes.hpp"
#include "../../include/utils/formatos.hpp"
#include "../../include/utils/validaciones.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

/**
 * @class GestorArchivosBinarios
 * @brief Centraliza la persistencia física en disco de estructuras binarias mediante plantillas (`templates`).
 * @details Esta clase no es instanciable. Todos sus métodos son estáticos y operan de manera directa sobre el
 * sistema de archivos garantizando la integridad mediante verificaciones I/O y manejo de metadatos.
 */
class GestorArchivosBinarios {

    /**
     * @brief Constructor privado eliminado para prevenir la instanciación de la clase utilitaria.
     */
    GestorArchivosBinarios() = delete;

    // =======================================================================================
    // Métodos Auxiliares Privados (Control de Flujos y Archivos)
    // =======================================================================================

    /**
     * @brief Evalúa el estado de salud de un flujo de archivo (`stream`) tras realizar operaciones I/O.
     * @tparam tipoArchivo Tipo del flujo de archivo (`std::ifstream`, `std::ofstream` o `std::fstream`).
     * @param[in,out] archivo Referencia al flujo de archivo que se desea validar.
     * @return `true` si el flujo no presenta fallos (`failbit` o `badbit`).
     * @return `false` si ocurrió un error en la lectura/escritura (cierra el flujo automáticamente).
     */
    template <class tipoArchivo> static inline bool verificarOperacion(tipoArchivo &archivo) {
        if (archivo.fail() || archivo.bad()) {
            archivo.close();
            return false;
        }
        return true;
    }

    /**
     * @brief Abre un archivo binario en modo lectura/escritura simultánea (`std::ios::in | std::ios::out | std::ios::binary`).
     * @param[in] rutaArchivo Cadena de texto con la ruta relativa o absoluta del archivo.
     * @param[out] archivo Referencia al flujo `std::fstream` donde se asociará el archivo abierto.
     * @return `true` si el archivo pudo abrirse correctamente, `false` en caso contrario.
     */
    static bool abrirArchivoParaEscrituraYLectura(const std::string &rutaArchivo, std::fstream &archivo);

    /**
     * @brief Abre un archivo binario en modo solo escritura/anexado (`std::ios::out | std::ios::binary`).
     * @param[in] rutaArchivo Cadena con la ruta física del archivo.
     * @param[out] archivo Referencia al flujo `std::ofstream` a configurar.
     * @return `true` si la apertura fue exitosa, `false` en caso de fallo.
     */
    static bool abrirArchivoParaEscritura(const std::string &rutaArchivo, std::ofstream &archivo);

    /**
     * @brief Abre un archivo binario en modo lectura exclusiva (`std::ios::in | std::ios::binary`).
     * @param[in] rutaArchivo Ruta física del archivo.
     * @param[out] archivo Referencia al flujo `std::ifstream` a aperturar.
     * @return `true` si el archivo existe y se abrió para lectura, `false` en caso contrario.
     */
    static bool abrirArchivoParaLectura(const std::string &rutaArchivo, std::ifstream &archivo);

    /**
     * @brief Comprueba la existencia física de un archivo en el sistema de archivos local.
     * @param[in] rutaArchivo Ruta relativa o absoluta a verificar.
     * @return `true` si el archivo existe físicamente, `false` si no se localiza.
     */
    static bool validarExisteArchivo(const std::string &rutaArchivo);

    /**
     * @brief Evalúa si un archivo no contiene datos grabados (tamaño de `0` bytes).
     * @param[in] rutaArchivo Ruta del archivo a evaluar.
     * @return `true` si el tamaño del archivo es `0` bytes, `false` en caso contrario.
     */
    static bool validarArchivoVacio(const std::string &rutaArchivo);

    /**
     * @brief Valida si el archivo contiene únicamente la cabecera (`ArchivoHeader`) y ningún registro activo o histórico.
     * @param[in] rutaArchivo Ruta del archivo binario a comprobar.
     * @return `true` si solo existe el encabezado en el archivo, `false` si contiene datos adicionales.
     */
    static bool validarArchivoSinRegistros(const std::string &rutaArchivo);

    /**
     * @brief Crea un archivo binario en blanco y le escribe un `ArchivoHeader` por defecto en la posición `0`.
     * @param[in] rutaArchivo Ruta del archivo a inicializar.
     * @return `true` si la inicialización en disco se realizó con éxito, `false` de lo contrario.
     */
    static bool inicializarArchivo(const std::string &rutaArchivo);

    /**
     * @brief Calcula la posición en bytes (`std::streampos`) dentro del archivo para realizar un salto de puntero `seek`.
     * @param[in] indiceFisico Índice numérico base cero del registro (0, 1, 2...).
     * @param[in] tamRegistros Tamaño en bytes que ocupa la entidad física en disco.
     * @param[in] tamArchivoHeader Tamaño en bytes reservado para la cabecera del archivo.
     * @return Objeto `std::streampos` representando la posición absoluta en bytes dentro del archivo.
     */
    static std::streampos calcularPosicion(const size_t &indiceFisico, const size_t &tamRegistros, const size_t &tamArchivoHeader);

    /**
     * @brief Recorre secuencialmente el archivo binario para obtener el índice físico (offset ordenado) de un registro por su `ID`.
     * @tparam tipoRegistro Tipo de entidad sobre la que se realiza la lectura (debe implementar `getTamano()`).
     * @param[in] rutaArchivo Ruta del archivo binario.
     * @param[in] id Identificador único numérico del registro buscado.
     * @return `int` Posición física entera (`0`, `1`, `2`...) del registro dentro del archivo.
     * @return `constantes::ERROR_INT` (`-1`) Si el registro no se localiza, está ausente o falla el archivo.
     */
    template <class tipoRegistro> static int buscarIndiceFisicoPorId(const std::string &rutaArchivo, const int &id) {

        // Validamos que el archivo existe y que el id tambien existe
        if (!validarExisteArchivo(rutaArchivo) || !GestorDeValidaciones::validarId(id)) {
            return constantes::ERROR_INT;
        }

        // Abrimos el archivo en modo escritura
        std::ifstream archivo;
        if (!abrirArchivoParaLectura(rutaArchivo, archivo)) {
            return constantes::ERROR_INT;
        }

        // Movemos el puntero deespues de header
        archivo.seekg(ArchivoHeader::getTamano(), std::ios::beg);

        tipoRegistro registroTemporal;
        int contadorIndice = 0;

        // Leemos los registros uno a uno
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), tipoRegistro::getTamano())) {

            // verificamos si la operacion fue exitosa
            if (!verificarOperacion(archivo))
                break;

            // Si encontramos el id devolvemos el indice fisico
            if (registroTemporal.getId() == id) {
                archivo.close();
                return contadorIndice;
            }
            contadorIndice++;
        }

        // si no lo encontramos, devolvemos error
        archivo.close();
        return constantes::ERROR_INT;
    }

  public:
    // =======================================================================================
    // Métodos de Control General y Sistema
    // =======================================================================================

    /**
     * @brief Prepara el directorio y los archivos binarios requeridos para el funcionamiento del sistema.
     * @return `true` si todas las carpetas y archivos indispensables se encuentran creados o fueron inicializados.
     */
    static bool inicializarSistemaArchivos();

    /**
     * @brief Lee y recupera la estructura `ArchivoHeader` almacenada al inicio del archivo binario.
     * @param[in] rutaArchivo Ruta del archivo binario a consultar.
     * @return Instancia de `ArchivoHeader` cargada con los datos leídos de disco.
     */
    static ArchivoHeader obtenerHeader(const std::string &rutaArchivo);

    /**
     * @brief Sobrescribe la cabecera `ArchivoHeader` en los primeros bytes del archivo binario.
     * @param[in] rutaArchivo Ruta del archivo a modificar.
     * @param[in] header Instancia de `ArchivoHeader` con los nuevos datos a persistir.
     * @return `true` si la actualización de la cabecera fue exitosa, `false` en caso contrario.
     */
    static bool actualizarHeader(const std::string &rutaArchivo, const ArchivoHeader &header);

    /**
     * @brief Carga y retorna la configuración e información principal del torneo.
     * @return Objeto `Torneo` cargado desde el almacenamiento binario.
     */
    static Torneo obtenerInformacionTorneo();

    /**
     * @brief Persiste (sobrescribe) el único registro de `Torneo` en su archivo binario.
     * @details A diferencia de `guardarNuevoRegistro`, este archivo NO usa `ArchivoHeader`:
     * contiene un único registro `Torneo` escrito directamente desde el byte 0, tal como
     * lo crea `inicializarArchivo()`. Por eso requiere su propia lógica de guardado.
     * @param[in] torneo Objeto `Torneo` con los datos a persistir.
     * @return `true` si la escritura fue exitosa, `false` en caso contrario.
     */
    static bool guardarInformacionTorneo(const Torneo &torneo);

    // =======================================================================================
    // Métodos Genéricos Templatizados (CRUD y Consultas)
    // =======================================================================================

    /**
     * @brief Verifica si existe un registro activo (`eliminado == false`) asociado a un `ID` determinado.
     * @tparam tipoRegistro Tipo de entidad a consultar.
     * @param[in] rutaArchivo Ruta del archivo binario.
     * @param[in] id Identificador numérico a comprobar.
     * @return `true` si existe un registro activo correspondiente a ese `ID`.
     * @return `false` si no existe el registro, está marcado como eliminado o falla la lectura.
     */
    template <class tipoRegistro> static bool validarExisteRegistroConId(const std::string &rutaArchivo, const int id) {

        // Validamos si no existe el archivo o verificamos si esta vacio
        if (!validarExisteArchivo(rutaArchivo) || validarArchivoVacio(rutaArchivo) || !GestorDeValidaciones::validarId(id)) {
            return false;
        }

        // Abrimos el archivo
        std::ifstream archivo;
        if (!abrirArchivoParaLectura(rutaArchivo, archivo)) {
            return false;
        }

        // Movemos el puntero despues del header
        archivo.seekg(ArchivoHeader::getTamano(), std::ios::beg);
        tipoRegistro registroTemporal;

        // buscamos si existe el id recorriendo uno a uno
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), tipoRegistro::getTamano())) {

            // Verificamos si el archivo se leyó correctamente
            if (!verificarOperacion(archivo))
                return false;

            // Si encontramos el id y no esta eliminado devolvemos true
            if (!registroTemporal.getEliminado() && registroTemporal.getId() == id) {
                archivo.close();
                return true;
            }
        }

        // Si no false;
        archivo.close();
        return false;
    }

    /**
     * @brief Obtiene mediante acceso aleatorio directo una entidad específica buscando por su `ID`.
     * @tparam tipoRegistro Tipo de entidad a recuperar.
     * @param[in] rutaArchivo Ruta del archivo binario.
     * @param[out] registroBuscado Referencia donde se copiarán los datos leídos de la entidad.
     * @param[in] id Identificador numérico único de la entidad.
     * @return `true` si la entidad fue encontrada, está activa (`eliminado == false`) y se leyó correctamente.
     * @return `false` si el registro no existe, está borrado lógicamente o falla la lectura.
     * @code
     * Jugador j;
     * if (GestorArchivosBinarios::buscarRegistrosPorId("jugadores.dat", j, 10)) {
     * // Registro leído con éxito
     * }
     * @endcode
     */
    template <class tipoRegistro> static bool buscarRegistrosPorId(const std::string &rutaArchivo, tipoRegistro &registroBuscado, int id) {

        // Validamos si no existe el archivo o verificamos si esta vacio
        if (!validarExisteArchivo(rutaArchivo) || validarArchivoVacio(rutaArchivo) || !GestorDeValidaciones::validarId(id)) {
            return false;
        }

        // Buscamos el indice fisico
        int indice = buscarIndiceFisicoPorId<tipoRegistro>(rutaArchivo, id);
        if (indice == constantes::ERROR_INT) {
            return false;
        }

        // Abrimos el archivo
        std::ifstream archivo;
        if (!abrirArchivoParaLectura(rutaArchivo, archivo)) {
            return false;
        }

        // Calculamos la posicion
        std::streampos posicion = calcularPosicion(indice, tipoRegistro::getTamano(), ArchivoHeader::getTamano());
        archivo.seekg(posicion, std::ios::beg);
        archivo.read(reinterpret_cast<char *>(&registroBuscado), tipoRegistro::getTamano());

        // Verificamos que la lectura haya sido exitosa y que el registro no este eliminado
        if (!verificarOperacion(archivo) || registroBuscado.getEliminado()) {
            archivo.close();
            return false;
        }

        // Cerramos
        archivo.close();
        return true;
    }

    /**
     * @brief Busca registros cuya propiedad de nombre contenga una subcadena específica (insensible a mayúsculas/minúsculas).
     * @tparam tipoRegistro Tipo de entidad almacenada (debe contar con el miembro `.nombre` y `.eliminado`).
     * @param[in] rutaArchivo Ruta del archivo binario.
     * @param[in] subcadena Texto o fragmento de texto a buscar dentro del atributo `nombre`.
     * @return `std::vector<tipoRegistro>` Colección dinámica con todas las coincidencias activas encontradas.
     */
    template <class tipoRegistro> static std::vector<tipoRegistro> buscarRegistrosPorSubcadena(const std::string &rutaArchivo, const std::string &subcadena) {
        std::vector<tipoRegistro> resultados;

        // Validamos si no existe el archivo o verificamos si esta vacio
        if (!validarExisteArchivo(rutaArchivo) || validarArchivoVacio(rutaArchivo) || GestorDeValidaciones::validarCadenaVacia(subcadena)) {
            return resultados;
        }

        // Abrimos el arhcivo para lecrtura
        std::ifstream archivo;
        if (!abrirArchivoParaLectura(rutaArchivo, archivo)) {
            return resultados;
        }

        // Convertimos la subcadena a minus para no modificarla
        std::string subcadenaMinus = subcadena;
        Formatos::convertirTextoAMinus(subcadenaMinus);

        // Nos saltamos el header
        archivo.seekg(ArchivoHeader::getTamano(), std::ios::beg);
        tipoRegistro registroTemporal;

        // Buscamos uno a uno en el binario
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), tipoRegistro::getTamano())) {
            // Verificamos que se haya leido correctamente
            if (!verificarOperacion(archivo)) {
                break;
            }

            // si está eliminado nos lo saltamos para no agregarlo al vector
            if (registroTemporal.getEliminado()) {
                continue;
            }

            // Transformamos a string el parametro const char * para una mejor comparacion
            std::string nombreRegistro(registroTemporal.getNombre());
            Formatos::convertirTextoAMinus(nombreRegistro);

            // Comparamos
            if (nombreRegistro.find(subcadenaMinus) != std::string::npos) {
                // Agregamos al vector con .puch_back()
                resultados.push_back(registroTemporal);
            }
        }

        // Cerramos
        archivo.close();
        return resultados;
    }

    /**
     * @brief Lee secuencialmente el archivo binario y retorna todas las entidades activas (`eliminado == false`).
     * @tparam tipoRegistro Tipo de entidad a listar.
     * @param[in] rutaArchivo Ruta del archivo binario.
     * @return `std::vector<tipoRegistro>` Vector dinámico con la totalidad de registros activos recuperados.
     */
    template <class tipoRegistro> static std::vector<tipoRegistro> listarRegistros(const std::string &rutaArchivo) {
        std::vector<tipoRegistro> resultados;

        // Validamos si no existe el archivo o verificamos si esta vacio
        if (!validarExisteArchivo(rutaArchivo) || validarArchivoVacio(rutaArchivo)) {
            return resultados;
        }

        // Leemos el header i verificamos que se leyo bien
        ArchivoHeader header = obtenerHeader(rutaArchivo);
        if (header.getCantidadRegistros() <= 0) {
            return resultados;
        }

        // Abrimos el archivo
        std::ifstream archivo;
        if (!abrirArchivoParaLectura(rutaArchivo, archivo)) {
            return resultados;
        }

        // Nos saltamos el header
        archivo.seekg(ArchivoHeader::getTamano(), std::ios::beg);
        tipoRegistro registroTemporal;

        // Leemos uno a uno
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), tipoRegistro::getTamano())) {
            // Verificamos que se leyo correctamente
            if (!verificarOperacion(archivo)) {
                break;
            }

            // Si no está eliminado lo agregamos
            if (!registroTemporal.getEliminado()) {
                resultados.push_back(registroTemporal);
            }
        }

        // Cerramos
        archivo.close();
        return resultados;
    }

    /**
     * @brief Añade una nueva entidad al final del archivo binario y actualiza las métricas en la cabecera.
     * @tparam tipoRegistro Tipo de entidad a guardar.
     * @param[in] rutaArchivo Ruta del archivo binario.
     * @param[in] nuevoRegistro Objeto con la información a escribir en disco.
     * @return `true` si el registro se escribió y el `ArchivoHeader` se actualizó correctamente.
     * @return `false` si ocurrió una falla en la escritura del registro o de la cabecera.
     */
    template <class tipoRegistro> static bool guardarNuevoRegistro(const std::string &rutaArchivo, const tipoRegistro &nuevoRegistro) {

        // Validamos que el archivo exista
        if (!validarExisteArchivo(rutaArchivo)) {
            return false;
        }

        // Abrimos el archivo
        std::fstream archivo;
        if (!abrirArchivoParaEscrituraYLectura(rutaArchivo, archivo)) {
            return false;
        }

        // Nos posicionamos al final
        archivo.seekp(0, std::ios::end);
        archivo.write(reinterpret_cast<const char *>(&nuevoRegistro), tipoRegistro::getTamano());

        // Verificamos que se escribio correctamente
        if (!verificarOperacion(archivo)) {
            return false;
        }

        // Cerramos el archivo para evitar problemas con abrir el archivo dos veces
        archivo.close();

        // Actualizamos el header
        ArchivoHeader header = obtenerHeader(rutaArchivo);
        header.aumentarCantidadDeRegistros();
        header.aumentarProximoId();
        header.aumentarRegistrosActivos();

        // Devolvemos si se pudo actualizar el header (que si deberia)
        return actualizarHeader(rutaArchivo, header);
    }

    /**
     * @brief Reescribe un registro existente en su posición física exacta dentro del archivo buscando por su `ID`.
     * @tparam tipoRegistro Tipo de entidad a modificar.
     * @param[in] rutaArchivo Ruta del archivo binario.
     * @param[in] registro Objeto con los datos actualizados a persistir.
     * @param[in] id Identificador numérico del registro a sobrescribir.
     * @return `true` si la reescritura directa en disco fue exitosa.
     * @return `false` si no se localizó la posición física o falló el flujo de salida.
     */
    template <class tipoRegistro> static bool guardarRegistro(const std::string &rutaArchivo, const tipoRegistro &registro, int id) {

        // Validamos que exista el archivo
        if (!validarExisteArchivo(rutaArchivo)) {
            return false;
        }

        // calculamos el indice fisico
        int indiceFisico = buscarIndiceFisicoPorId<tipoRegistro>(rutaArchivo, id);
        if (indiceFisico == constantes::ERROR_INT) {
            return false;
        }

        // Abrimos ela rhcivo
        std::fstream archivo;
        if (!abrirArchivoParaEscrituraYLectura(rutaArchivo, archivo))
            return false;

        // Calculamos la posicion
        std::streampos posicion = calcularPosicion(indiceFisico, tipoRegistro::getTamano(), ArchivoHeader::getTamano());
        archivo.seekp(posicion, std::ios::beg);
        archivo.write(reinterpret_cast<const char *>(&registro), tipoRegistro::getTamano());

        // Verificamos que se escribio correctamente
        if (!verificarOperacion(archivo)) {
            return false;
        }

        archivo.close();
        return true;
    }
};

#endif // GESTOR_ARCHIVOS_BINARIOS_HPP