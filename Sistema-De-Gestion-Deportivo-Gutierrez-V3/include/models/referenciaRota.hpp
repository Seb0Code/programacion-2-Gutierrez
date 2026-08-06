#ifndef REFERENCIA_ROTA_HPP
#define REFERENCIA_ROTA_HPP

#include "../utils/constantes.hpp"
#include <cstddef>
#include <cstring>

/**
 * @file REFERENCIA_ROTA_HPP
 * @brief Declaración de la clase ReferenciaRota para la auditoría de integridad referencial.
 */

/**
 * @class ReferenciaRota
 * @brief Representa una anomalía de integridad referencial donde una entidad apunta a un registro inexistente o eliminado.
 *
 * @details Almacena el tipo de entidad/relación evaluada (por ejemplo `"JUGADOR"`, `"PARTIDO_LOCAL"`, `"GOL"`),
 * el identificador de la entidad de origen y el identificador roto o huérfano que no pudo resolverse en la base de datos o sistema.
 */
class ReferenciaRota {
  private:
    char tipoDeReferencia[constantes::TAMANO_TIPO_REFERNCIA]{}; ///< Cadena identificadora del tipo de referencia (ej. `"JUGADOR"`).
    int idOrigen{};                                             ///< ID del registro contenedor que posee la referencia inválida.
    int idReferenciaRota{};                                     ///< ID huérfano o no existente que falló en resolverse.

  public:
    /**
     * @brief Obtiene el tamaño en bytes de la estructura interna del objeto `ReferenciaRota`.
     *
     * @details Devuelve la cantidad exacta de memoria ocupada por un objeto de tipo `ReferenciaRota`.
     * Útil para operaciones de serialización o escritura binaria directa.
     *
     * @return `std::size_t` Tamaño de la clase en bytes (`sizeof(ReferenciaRota)`).
     */
    static size_t getTamano() { return sizeof(ReferenciaRota); }

    /**
     * @brief Constructor por defecto de la clase `ReferenciaRota`.
     * @details Inicializa todos los atributos miembros con sus valores predeterminados.
     */
    ReferenciaRota() = default;

    /**
     * @brief Constructor parametrizado para la creación de un reporte de referencia rota.
     *
     * @param[in] tipo Cadena de caracteres con el tipo de relación evaluada.
     * @param[in] origen ID de la entidad origen que almacena el puntero/clave foránea.
     * @param[in] referencia ID de la entidad de destino que no pudo ser encontrada.
     *
     * @pre `tipo` no debe ser un puntero `nullptr`.
     * @post Copia de forma segura el texto de `tipo` respetando el tamaño máximo asignado.
     *
     * @code
     * ReferenciaRota error("PARTIDO_LOCAL", 102, 999);
     * @endcode
     */
    ReferenciaRota(const char *tipo, const int origen, const int referencia);

    /**
     * @brief Destructor por defecto de la clase `ReferenciaRota`.
     */
    ~ReferenciaRota() = default;

    // ---------------------------------------------------------------------------------------//
    //    GETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Obtiene el tipo de referencia o relación donde ocurrió la inconsistencia.
     * @return `const char*` Puntero a la cadena constante con el nombre de la relación.
     */
    const char *getTipoDeReferencia() const;

    /**
     * @brief Obtiene el identificador único del registro origen.
     * @return `int` ID de la entidad que contiene la referencia rota.
     */
    int getIdOrigen() const;

    /**
     * @brief Obtiene el identificador que generó el fallo de resolución.
     * @return `int` ID no encontrado en el sistema.
     */
    int getIdReferenciaRota() const;

    /**
     * @brief Indica si la referencia contiene los datos mínimos para un informe de auditoría.
     *
     * @details Comprueba que el tipo de referencia no esté vacío y que tanto `idOrigen`
     * como `idReferenciaRota` tengan valores válidos mayores a cero.
     *
     * @return `true` si la información guardada es consistente y válida, `false` en caso contrario.
     */
    bool esValida() const;

    // ---------------------------------------------------------------------------------------//
    //    SETTERS Y MÉTODOS DE LIMPIEZA
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Asigna el tipo de relación evaluada.
     *
     * @param[in] tipo Cadena terminada en nulo (`\0`) con el nombre del tipo de referencia.
     *
     * @return `true` si la cadena fue asignada con éxito, `false` si `tipo` es `nullptr` o si excede el tamaño del buffer.
     *
     * @pre `tipo` no debe ser un puntero `nullptr`.
     * @post No trunca el texto si excede el límite del buffer `constantes::TAMANO_TIPO_REFERNCIA`; en su lugar rechaza la operación.
     */
    bool setTipoDeReferencia(const char *tipo);

    /**
     * @brief Establece el ID de la entidad origen.
     *
     * @param[in] origen Identificador único de la entidad origen.
     * @return `true` si `origen > 0`, `false` si se ingresa un ID no válido.
     */
    bool setIdOrigen(const int origen);

    /**
     * @brief Establece el ID de la referencia no resuelta.
     *
     * @param[in] referencia Identificador numérico de la referencia rota.
     * @return `true` si `referencia > 0`, `false` si se ingresa un ID inválido.
     */
    bool setIdReferenciaRota(const int referencia);

    /**
     * @brief Limpia el estado interno del objeto, reajustando todos sus miembros a cero o vacío.
     *
     * @post El buffer `tipoDeReferencia` se limpia a ceros y los IDs se restablecen a `0`.
     */
    void limpiar();
};

#endif // REFERENCIA_ROTA_HPP