#ifndef ARCHIVOHEADER_HPP
#define ARCHIVOHEADER_HPP

#include <iostream>

class ArchivoHeader {
  private:
    int cantidadRegistros; // Total histórico (incluyendo eliminados lógicamente) // 0
    int proximoId;         // Siguiente ID a asignar (autoincremental)  // 1
    int registrosActivos;  // Registros con eliminado == false  // 0
    int version;           // Control de versión del archivo (iniciar en 1) // 1

  public:
    // cosntructor
    ArchivoHeader(int cantRgt, int proxId, int rgtAct, int ver);

    // ---------------------------------------------------------------------------------------------- //
    //   GETERS                                                                                       //
    // ---------------------------------------------------------------------------------------------- //

    /**
     * @brief Retorna el tamaño de la entidad torneo.
     * @return Devuelve el tamaño de la entidad Torneo
     */
    int getCantidadRegistros();

    /**
     * @brief Retorna el tamaño de la entidad torneo.
     * @return Devuelve el tamaño de la entidad Torneo
     */
    int getProximoId();

    /**
     * @brief Retorna el tamaño de la entidad torneo.
     * @return Devuelve el tamaño de la entidad Torneo
     */
    int getRegistrosActivos();

    /**
     * @brief Retorna el tamaño de la entidad torneo.
     * @return Devuelve el tamaño de la entidad Torneo
     */
    int getVersion();

    /**
     * @brief Retorna el tamaño de la entidad torneo.
     * @return Devuelve el tamaño de la entidad Torneo
     */
    size_t getTamanoArchivoHeader();

    // ---------------------------------------------------------------------------------------------- //
    //   SETTERS                                                                                      //
    // ---------------------------------------------------------------------------------------------- //

    /**
     * @brief Aumenta la variable cantidadRegistro en una unidad.
     */
    void aumentarCantidadDeRegistros();

    /**
     * @brief Aumenta la variable proximoId en una unidad.
     */
    void aumentarProximoId();

    /**
     * @brief Aumenta la variable registrosActivos en una unidad.
     */
    void aumentarRegistrosActivos();

    /**
     * @brief Disminuye la variable registrosActivos en una unidad.
     */
    void disminuirRegistrosActivos();

    /**
     * @brief Aumenta la variable version en una unidad.
     */
    void aumentarVersion();
};

#endif