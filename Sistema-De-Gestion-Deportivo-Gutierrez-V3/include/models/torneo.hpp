#ifndef TORNEO_HPP
#define TORNEO_HPP

#include "../../include/models/reglasTorneo.hpp"
#include "../../include/utils/constantes.hpp"
#include <cstring>
#include <ctime>
#include <string>

/**
 * @brief Modelo de la entidad de objeto Torneo. Contiene su estructura y sus funciones básicas
 */
class Torneo {

  private:
    char nombre[constantes::TAMANO_NOMBRE]{};     // Nombre del torneo
    char deporte[constantes::TAMANO_DEPORTE]{};   // Deporte del torneo
    char formato[constantes::TAMANO_FORMATO]{};   // "GRUPOS" o "ELIMINATORIA"
    char fechaInicio[constantes::TAMANO_FECHA]{}; // Formato YYYY-MM-DD
    char fechaFin[constantes::TAMANO_FECHA]{};    // Formato YYYY-MM-DD
    ReglasTorneo reglas{};

    // Metadata de control
    time_t fechaCreacion{};
    time_t fechaUltimaModificacion{};

    // Variable para ver si los datos ya están inicializados
    bool inicializado{};

    // Reglas del torneo


  public:
    /**
     * @brief Constructor por defecto de la clase Torneo.
     */
    Torneo() = default;

    /**
     * @brief Constructor parametrizado para inicializar un torneo con sus datos principales.
     * @param nom Puntero al arreglo de caracteres con el nombre del torneo.
     * @param dep Puntero al arreglo de caracteres con el deporte del torneo.
     * @param form Puntero al arreglo de caracteres con el formato del torneo ("GRUPOS" o "ELIMINATORIA").
     * @param fechaI Puntero al arreglo de caracteres con la fecha de inicio (YYYY-MM-DD).
     * @param fechaF Puntero al arreglo de caracteres con la fecha de fin (YYYY-MM-DD).
     */
    Torneo(const char *nom, const char *dep, const char *form, const char *fechaI, const char *fechaF);

    /**
     * @brief Destructor de la clase Torneo.
     */
    ~Torneo() = default;

    // ---------------------------------------------------------------------------------------------- //
    //   GETERS                                                                                       //
    // ---------------------------------------------------------------------------------------------- //
    /**
     * @brief Retorna el nombre del torneo.
     * @return Devuelve un puntero a un array de char que contiene el nombre del torneo.
     */
    const char *getNombre() const;

    /**
     * @brief Retorna el deporte del torneo.
     * @return Devuelve un puntero a un array de char que contiene el deporte del torneo.
     */
    const char *getDeporte() const;

    /**
     * @brief Retorna el formato del torneo.
     * @return Devuelve un puntero a un array de char que contiene el foirmato del torneo.
     */
    const char *getFormato() const;

    /**
     * @brief Retorna la fecha de inicio del torneo.
     * @return Devuelve un puntero a un array de char que contiene la fecha de inicio del torneo.
     */
    const char *getFechaInicio() const;

    /**
     * @brief Retorna la fecha de finalización del torneo.
     * @return Devuelve un puntero a un array de char que contiene la fecha de finalización del torneo.
     */
    const char *getFechaFin() const;

    ReglasTorneo getReglasTorneo() const;

    /**
     * @brief Retorna la fecha de creación del registro en formato crudo (timestamp).
     * @return Devuelve un valor de tipo time_t con la marca de tiempo de creación.
     */
    time_t getFechaCreacion() const;

    /**
     * @brief Retorna la fecha de la última modificación en formato crudo (timestamp).
     * @return Devuelve un valor de tipo time_t con la marca de tiempo del último cambio.
     */
    time_t getFechaUltimaModificacion() const;

    /**
     * @brief Retorna la fecha de creación del torneo formateada para lectura humana.
     * @return Devuelve un objeto std::string con la fecha formateada (ej. DD/MM/AAAA HH:MM:SS).
     */
    std::string getFechaCreacionFormateada() const;

    /**
     * @brief Retorna la fecha de última modificación formateada para lectura humana.
     * @return Devuelve un objeto std::string con la fecha formateada (ej. DD/MM/AAAA HH:MM:SS).
     */
    std::string getFechaUltimaModificacionFormateada() const;

    /**
     * @brief Retorna si el objeto Torneo ha sido inicializado.
     * @return true si el torneo ya está inicializado, false en caso contrario.
     */
    bool getInicializado() const;

    /**
     * @brief Retorna el tamaño de la entidad torneo.
     * @return Devuelve el tamaño de la entidad Torneo
     */
    static size_t getTamano();

    // ---------------------------------------------------------------------------------------------- //
    //   SETTERS                                                                                      //
    // ---------------------------------------------------------------------------------------------- //

    /**
     * @brief Establece el nombre del torneo.
     * @param nom Puntero al arreglo de caracteres con el nuevo nombre.
     */
    bool setNombre(const char *nom);

    /**
     * @brief Establece el deporte del torneo.
     * @param dep Puntero al arreglo de caracteres con el nuevo deporte.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setDeporte(const int idDep);

    /**
     * @brief Establece el formato del torneo ("GRUPOS" o "ELIMINATORIA").
     * @param form Puntero al arreglo de caracteres con el nuevo formato.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setFormato(const int idForm);

    /**
     * @brief Establece la fecha de inicio del torneo.
     * @param fechaI Puntero al arreglo de caracteres con la nueva fecha (YYYY-MM-DD).
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setFechaInicio(const char *fechaI);

    /**
     * @brief Establece la fecha de finalización del torneo.
     * @param fechaF Puntero al arreglo de caracteres con la nueva fecha (YYYY-MM-DD).
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setFechaFin(const char *fechaF);

    /**
     * @brief Establece la fecha de creación del registro.
     * @param fechaC Marca de tiempo de tipo time_t que representa la creación.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setFechaCreacion(time_t fechaC);

    /**
     * @brief Establece la fecha de la última modificación del registro.
     * @param fechaUM Marca de tiempo de tipo time_t que representa la modificación.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setFechaUltimaModificacion(time_t fechaUM); // Corrección: Eliminada la tilde de 'Modificación' para evitar errores de compilación

    /**
     * @brief Define el estado de inicialización del torneo.
     * @param ini Valor booleano que indica si el registro ha sido incializado.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setInicializado(bool ini);
};

#endif
