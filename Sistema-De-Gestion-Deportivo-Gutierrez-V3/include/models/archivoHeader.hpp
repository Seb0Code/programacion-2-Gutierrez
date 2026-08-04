#ifndef ARCHIVO_HEADER_HPP
#define ARCHIVO_HEADER_HPP

#include <iostream>

/**
 * @file ArchivoHeader.hpp
 * @brief Definición de la clase ArchivoHeader para el control del encabezado de archivos binarios.
 */

/**
 * @class ArchivoHeader
 * @brief Representa la metainformación o cabecera grabada al inicio de un archivo de datos binario.
 *
 * Mantiene un registro persistente del total histórico de registros, contador autoincremental de IDs,
 * número de registros activos y la versión del formato del archivo para asegurar la integridad de lectura.
 */
class ArchivoHeader {
  private:
    int cantidadRegistros; /**< Total histórico de registros creados (incluye eliminados lógicamente). */
    int proximoId;         /**< Siguiente identificador `ID` autoincremental a asignar. */
    int registrosActivos;  /**< Cantidad actual de registros válidos (`eliminado == false`). */
    int version;           /**< Número de versión de la estructura del archivo. */

  public:
    /**
     * @brief Constructor por defecto.
     * Inicializa los valores base del encabezado: `cantidadRegistros = 0`, `proximoId = 1`,
     * `registrosActivos = 0` y `version = 1`.
     */
    ArchivoHeader();

    /**
     * @brief Constructor parametrizado para inicializar la cabecera con valores específicos.
     * @param[in] cantRgt Cantidad total histórica de registros.
     * @param[in] proxId Siguiente ID que será asignado.
     * @param[in] rgtAct Cantidad de registros activos en el archivo.
     * @param[in] ver Número de versión del archivo.
     */
    ArchivoHeader(int cantRgt, int proxId, int rgtAct, int ver);

    /**
     * @brief Destructor de la clase ArchivoHeader.
     */
    ~ArchivoHeader() = default;

    // ---------------------------------------------------------------------------------------------- //
    //   GETTERS                                                                                      //
    // ---------------------------------------------------------------------------------------------- //

    /**
     * @brief Obtiene la cantidad total de registros escritos históricamente.
     * @return Entero con la cantidad total de registros (activos e inactivos).
     */
    int getCantidadRegistros() const;

    /**
     * @brief Obtiene el valor del siguiente ID disponible para una nueva inserción.
     * @return Entero con el próximo `ID` autoincremental.
     */
    int getProximoId() const;

    /**
     * @brief Obtiene la cantidad actual de registros que no están marcados como eliminados.
     * @return Entero con el total de registros activos.
     */
    int getRegistrosActivos() const;

    /**
     * @brief Obtiene la versión actual de la estructura del archivo.
     * @return Entero representativo de la versión del archivo.
     */
    int getVersion() const;

    /**
     * @brief Retorna el tamaño de la entidad torneo.
     * @return Devuelve el tamaño de la entidad Torneo
     */
    static size_t getTamano();

    // ---------------------------------------------------------------------------------------------- //
    //   SETTERS                                                                                      //
    // ---------------------------------------------------------------------------------------------- //

    /**
     * @brief Calcula y retorna el tamaño en bytes que ocupa el encabezado en memoria/disco.
     * @return `size_t` con el tamaño en bytes obtenido mediante `sizeof(ArchivoHeader)`.
     */
    void aumentarCantidadDeRegistros();

    /**
     * @brief Aumenta la variable `proximoId` en una unidad.
     */
    void aumentarProximoId();

    /**
     * @brief Aumenta la variable `registrosActivos` en una unidad.
     */
    void aumentarRegistrosActivos();

    /**
     * @brief Disminuye la variable `registrosActivos` en una unidad.
     */
    void disminuirRegistrosActivos();

    /**
     * @brief Aumenta la variable `version` en una unidad.
     */
    void aumentarVersion();
};

#endif
