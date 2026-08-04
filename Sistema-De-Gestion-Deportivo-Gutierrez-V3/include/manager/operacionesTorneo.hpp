#ifndef OPERACIONES_TORNEO_HPP
#define OPERACIONES_TORNEO_HPP

#include "../models/torneo.hpp"
#include <ctime>
#include <string>
#include <vector>

/**
 * @file OperacionesTorneo.hpp
 * @brief Declaración de funciones de utilidad estáticas para la gestión y formateo de torneos.
 */

/**
 * @class OperacionesTorneo
 * @brief Clase utilitaria orientada al procesamiento de fechas y mapeo de configuraciones de torneos.
 *
 * @details Esta clase provee un conjunto de métodos estáticos para realizar transformaciones
 * de formatos de fecha y búsquedas en la configuración del sistema.
 *
 * @note Esta clase **no puede ser instanciada** ni heredada debido a la eliminación de su constructor implícito.
 */
class OperacionesTorneo {
  private:
    /**
     * @brief Constructor privado eliminado para prevenir la instanciación de la clase utilitaria.
     */
    OperacionesTorneo() = delete;

  public:
    /**
     * @brief Convierte un objeto de tiempo de C (`time_t`) a una representación en texto (`std::string`).
     *
     * @details Mapea la fecha dada al formato legible definido por el sistema.
     *
     * @param[in] fecha Referencia constante al objeto `time_t` que representa la fecha y hora a convertir.
     *
     * @return `std::string` Cadena de texto con la fecha formateada. Retorna una cadena vacía en caso de que `fecha` no sea válida.
     *
     * @pre El valor de `fecha` debe ser un valor de tiempo POSIX válido mayor o igual a `0`.
     * @post El estado del sistema no se modifica (función pura/sin efectos secundarios).
     *
     * @code
     * time_t ahora = std::time(nullptr);
     * std::string fechaStr = OperacionesTorneo::convertirFechatimeAString(ahora);
     * @endcode
     */
    static std::string convertirFechaTimeAString(const time_t &fecha);

    /**
     * @brief Busca y recupera el nombre del deporte asociado a su identificador único.
     *
     * @param[in] id Identificador numérico único del deporte (`int`).
     *
     * @return `std::string` Nombre legible del deporte según la configuración. Retorna `"Desconocido"` si el `id` no existe.
     *
     * @warning Si la configuración del sistema no ha sido cargada previamente en memoria, esta función puede lanzar una excepción o retornar un valor por defecto.
     *
     * @code
     * int idFutbol = 1;
     * std::string nombre = OperacionesTorneo::buscarDeporteEnConfig(idFutbol);
     * // nombre -> "Fútbol"
     * @endcode
     */
    static std::string buscarDeporteEnConfig(const int idBuscado);

    /**
     * @brief Busca la posición de un jugador dada su ID de posición y el ID de su deporte correspondiente.
     *
     * @param[in] idPosicion Identificador numérico de la posición en el campo/cancha.
     * @param[in] idDeporte Identificador numérico del deporte al que pertenece la posición.
     *
     * @return `std::string` Descripción de la posición (ej. `"Portero"`, `"Base"`). Retorna una cadena vacía si la combinación no existe.
     *
     * @pre El `idDeporte` debe ser un identificador válido registrado en el sistema.
     *
     * @code
     * int idDeporte = 1; // Fútbol
     * int idPosicion = 1; // Portero
     * std::string pos = OperacionesTorneo::buscarPosicionJugadorEnConfig(idPosicion, idDeporte);
     * @endcode
     */
    static std::string buscarPosicionJugadorEnConfig(const int idPosicionBuscada, const int idDeporteBuscado);

    // Funcion para listar los nombres de los deportes en un vector
    static std::vector<std::string> listarNombresDeportes();

    // Funcion para listar las posiciones de un deportes en un vector
    static std::vector<std::string> listarNombresPosicionesPorDeporte(const int idDeporteBuscada);

    // Buscar el tipo de Formatos por su id
    static std::string buscarFormatoEnConfig(const int idFormatoBuscado);
};

#endif // OPERACIONES_TORNEO_HPP