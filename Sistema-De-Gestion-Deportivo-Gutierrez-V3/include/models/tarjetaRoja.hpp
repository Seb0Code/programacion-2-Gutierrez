#ifndef TARJETA_ROJA_HPP
#define TARJETA_ROJA_HPP

#include "../utils/constantes.hpp"
#include "../utils/formatos.hpp"

/**
 * @file TarjetaRoja.hpp
 * @brief Declaración de la clase TarjetaRoja para el registro de expulsiones durante un partido.
 */

/**
 * @class TarjetaRoja
 * @brief Clase de entidad que representa una expulsión (tarjeta roja) ocurrida en un partido.
 *
 * @details Almacena la información puntual del evento sancionatorio, incluyendo el identificador
 * del jugador expulsado, el minuto exacto del encuentro y la condición del equipo (local/visitante).
 */
class TarjetaRoja {
  private:
    int idJugador;                                     ///< ID del jugador sancionado (`0` para desconocido/no especificado).
    int minuto;                                        ///< Minuto del partido en el que ocurrió la sanción.
    char equipo[constantes::TAMANO_LOCAL_O_VISITANTE]; ///< Cadena de texto indicando el equipo (`"LOCAL"` o `"VISITANTE"`).

  public:
    /**
     * @brief Constructor por defecto de la clase `TarjetaRoja`.
     * @details Inicializa la instancia con los valores por omisión de los tipos nativos.
     */
    TarjetaRoja() = default;

    /**
     * @brief Constructor parametrizado para la creación directa de un evento de tarjeta roja.
     *
     * @param[in] idJ ID del jugador sancionado.
     * @param[in] min Minuto del partido en que se mostró la tarjeta.
     * @param[in] eq Cadena de caracteres que especifica si fue para el equipo local o visitante.
     *
     * @pre `eq` no debe ser un puntero `nullptr` y debe estar en un formato válido.
     * @post Copia de manera segura la cadena `eq` dentro del arreglo miembro `equipo`.
     *
     * @code
     * TarjetaRoja sancion(10, 45, "LOCAL");
     * @endcode
     */
    TarjetaRoja(int idJ, int min, const char *eq) : idJugador(idJ), minuto(min) { Formatos::copiarCadena(equipo, eq, constantes::TAMANO_LOCAL_O_VISITANTE); }

    /**
     * @brief Destructor por defecto de la clase `TarjetaRoja`.
     */
    ~TarjetaRoja() = default;

    // ---------------------------------------------------------------------------------------//
    //    GETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Obtiene el identificador del jugador sancionado.
     * @return `int` Identificador único del jugador.
     */
    int getIdJugado() const;

    /**
     * @brief Obtiene el minuto del partido en el que se mostró la tarjeta roja.
     * @return `int` Minuto de la sanción.
     */
    int getMinuto() const;

    /**
     * @brief Obtiene la cadena que indica la condición del equipo sancionado.
     * @return `const char*` Puntero a la cadena que representa la condición (`"LOCAL"` o `"VISITANTE"`).
     */
    const char *getEquipo() const;

    // ---------------------------------------------------------------------------------------//
    //    SETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Establece el identificador del jugador sancionado.
     *
     * @param[in] idJ Identificador del jugador.
     * @return `true` si la asignación fue exitosa (`idJ >= 0`), `false` si es un ID inválido.
     */
    bool setIdJugador(const int idJ);

    /**
     * @brief Establece el minuto de la sanción.
     *
     * @param[in] min Minuto en que ocurrió la tarjeta roja.
     * @return `true` si el minuto es válido (mayor o igual a `0`), `false` si es un valor negativo.
     *
     * @pre `min` debe pertenecer a un tiempo de juego coherente con el reglamento del deporte.
     */
    bool setMinuto(const int min);

    /**
     * @brief Asigna el equipo del jugador expulsado (`"LOCAL"` o `"VISITANTE"`).
     *
     * @param[in] eq Cadena de caracteres con la condición del equipo.
     *
     * @return `true` si la cadena no es nula y se pudo copiar correctamente, `false` en caso contrario.
     *
     * @pre `eq` debe ser un puntero no nulo (`nullptr`).
     * @warning Debe asegurarse de que `eq` coincida exactamente con las cadenas predeterminadas de constantes.
     */
    bool setEquipo(const char *eq);
};

#endif // TARJETA_ROJA_HPP