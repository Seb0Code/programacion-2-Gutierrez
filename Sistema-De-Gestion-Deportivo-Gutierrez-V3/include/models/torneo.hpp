#ifndef TORNEO_HPP
#define TORNEO_HPP

#include "../../include/utils/constantes.hpp"
#include <cstring>
#include <ctime>

/**
 * @brief Modelo de la entidad de objeto Torneo. Contiene su estructura y sus funciones básicas
 */
class Torneo {

  private:
    char nombre[constantes::TAMANO_NOMBRE];     // Nombre del torneo
    char deporte[constantes::TAMANO_DEPORTE];   // Deporte del torneo
    char formato[constantes::TAMANO_FORMATO];   // "GRUPOS" o "ELIMINATORIA"
    char fechaInicio[constantes::TAMANO_FECHA]; // Formato YYYY-MM-DD
    char fechaFin[constantes::TAMANO_FECHA];    // Formato YYYY-MM-DD

    // Metadata de control
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

    // Variable para ver si los datos ya están inicializados
    bool inicializado;

  public:
    // Constructor
    Torneo(const char *nom, const char *dep, const char *form, const char *fechaI, const char *fechaF);

    // Destructor
    ~Torneo();

    // ---------------------------------------------------------------------------------------------- //
    //   GETERS                                                                                       //
    // ---------------------------------------------------------------------------------------------- //
    /**
     * @brief Retorna el nombre del torneo.
     * @return Devuelve un puntero a un array de char que contiene el nombre del torneo.
     */
    const char *getNombre();

    /**
     * @brief Retorna el deporte del torneo.
     * @return Devuelve un puntero a un array de char que contiene el deporte del torneo.
     */
    const char *getDeporte();

    /**
     * @brief Retorna el formato del torneo.
     * @return Devuelve un puntero a un array de char que contiene el foirmato del torneo.
     */
    const char *getFormato();

    /**
     * @brief Retorna la fecha de inicio del torneo.
     * @return Devuelve un puntero a un array de char que contiene la fecha de inicio del torneo.
     */
    const char *getFechaInicio();

    /**
     * @brief Retorna la fecha de finalización del torneo.
     * @return Devuelve un puntero a un array de char que contiene la fecha de finalización del torneo.
     */
    const char *getFechaFin();

    /**
     * @brief Retorna el tamaño de la entidad torneo.
     * @return Devuelve el tamaño de la entidad Torneo
     */
    size_t getTamanoTorneo();

    // ---------------------------------------------------------------------------------------------- //
    //   SETTERS                                                                                      //
    // ---------------------------------------------------------------------------------------------- //
};

#endif