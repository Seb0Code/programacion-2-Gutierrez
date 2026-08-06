#ifndef TARJETA_AMARILLA_HPP
#define TARJETA_AMARILLA_HPP

#include "../utils/constantes.hpp"
#include "../utils/formatos.hpp"

/**
 * @file TarjetaAmarilla.hpp
 * @brief Declaración de la clase TarjetaAmarilla para el registro de amonestaciones durante un partido.
 */

/**
 * @class TarjetaAmarilla
 * @brief Clase de entidad que representa una amonestación (tarjeta amarilla) ocurrida en un partido.
 *
 * @details Almacena la información del evento de amonestación, incluyendo el identificador
 * del jugador amonestado, el minuto en el que ocurrió y la condición del equipo (local/visitante).
 */
class TarjetaAmarilla {
  private:
    int idJugador;                                     ///< ID del jugador amonestado (`0` para desconocido/no especificado).
    int minuto;                                        ///< Minuto del partido en el que ocurrió la amonestación.
    char equipo[constantes::TAMANO_LOCAL_O_VISITANTE]; ///< Cadena de texto indicando el equipo (`"LOCAL"` o `"VISITANTE"`).

  public:
    /**
     * @brief Constructor por defecto de la clase `TarjetaAmarilla`.
     * @details Inicializa los miembros con sus valores predeterminados.
     */
    TarjetaAmarilla() = default;

    /**
     * @brief Constructor parametrizado para instanciar un evento de amonestación.
     *
     * @param[in] idJ ID del jugador amonestado.
     * @param[in] min Minuto del partido en que se mostró la tarjeta.
     * @param[in] eq Cadena de caracteres que especifica si fue para el equipo local o visitante.
     *
     * @pre `eq` no debe ser un puntero `nullptr`.
     * @post Copia la cadena `eq` de forma segura dentro del arreglo `equipo`.
     *
     * @code
     * TarjetaAmarilla amonestacion(7, 23, "VISITANTE");
     * @endcode
     */
    TarjetaAmarilla(int idJ, int min, const char *eq) : idJugador(idJ), minuto(min) { Formatos::copiarCadena(equipo, eq, constantes::TAMANO_LOCAL_O_VISITANTE); }

    /**
     * @brief Destructor por defecto de la clase `TarjetaAmarilla`.
     */
    ~TarjetaAmarilla() = default;

    // ---------------------------------------------------------------------------------------//
    //    GETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Obtiene el identificador del jugador amonestado.
     * @return `int` Identificador único del jugador.
     */
    int getIdJugado() const;

    /**
     * @brief Obtiene el minuto del partido en el que se otorgó la tarjeta amarilla.
     * @return `int` Minuto de la amonestación.
     */
    int getMinuto() const;

    /**
     * @brief Obtiene la cadena con la condición del equipo amonestado.
     * @return `const char*` Puntero a la cadena que representa la condición (`"LOCAL"` o `"VISITANTE"`).
     */
    const char *getEquipo() const;

    // ---------------------------------------------------------------------------------------//
    //    SETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Establece el identificador del jugador amonestado.
     *
     * @param[in] idJ Identificador del jugador.
     * @return `true` si la asignación fue exitosa (`idJ >= 0`), `false` en caso de ID inválido.
     */
    bool setIdJugador(const int idJ);

    /**
     * @brief Establece el minuto en que ocurrió la amonestación.
     *
     * @param[in] min Minuto del partido.
     * @return `true` si el minuto es mayor o igual a `0`, `false` si es un valor negativo.
     *
     * @pre `min` debe estar dentro del rango permitido del encuentro.
     */
    bool setMinuto(const int min);

    /**
     * @brief Asigna la condición del equipo amonestado (`"LOCAL"` o `"VISITANTE"`).
     *
     * @param[in] eq Cadena de caracteres con la condición del equipo.
     *
     * @return `true` si la cadena no es nula y se asignó correctamente, `false` en caso contrario.
     *
     * @pre `eq` debe ser un puntero no nulo (`nullptr`).
     */
    bool setEquipo(const char *eq);
};

#endif // TARJETA_AMARILLA_HPP