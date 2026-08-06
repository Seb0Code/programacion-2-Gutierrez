#ifndef ANOTACION_HPP
#define ANOTACION_HPP

#include "../utils/constantes.hpp"
#include "../utils/formatos.hpp"

/**
 * @file Anotacion.hpp
 * @brief Declaración de la clase Anotacion para el registro de puntos o goles en un partido.
 */

/**
 * @class Anotacion
 * @brief Clase de entidad que representa un evento de anotación (gol o punto) durante un partido.
 *
 * @details Almacena los datos requeridos para auditar una anotación, incluyendo el identificador
 * del jugador autor del punto, el minuto exacto del partido y la asignación del equipo (`LOCAL` o `VISITANTE`).
 */
class Anotacion {
  private:
    int idJugador;                                     ///< ID del jugador que anotó (`0` representa desconocido o autogol).
    int minuto;                                        ///< Minuto del partido en que se concretó la anotación.
    char equipo[constantes::TAMANO_LOCAL_O_VISITANTE]; ///< Condición del equipo (`"LOCAL"` o `"VISITANTE"`).

  public:
    /**
     * @brief Constructor por defecto de la clase `Anotacion`.
     * @details Inicializa la instancia con los valores por omisión de los miembros nativos.
     */
    Anotacion() = default;

    /**
     * @brief Constructor parametrizado para instanciar un registro de anotación.
     *
     * @param[in] idJ Identificador del jugador anotador (`0` si no se especifica o es en propia puerta).
     * @param[in] min Minuto del evento.
     * @param[in] eq Cadena que especifica la condición del equipo (`"LOCAL"` o `"VISITANTE"`).
     *
     * @pre `eq` no debe ser un puntero `nullptr`.
     * @post Asigna los atributos de forma segura copiando `eq` dentro del buffer `equipo`.
     *
     * @code
     * Anotacion gol(9, 88, "LOCAL");
     * @endcode
     */
    Anotacion(int idJ, int min, const char *eq) : idJugador(idJ), minuto(min) { Formatos::copiarCadena(equipo, eq, constantes::TAMANO_LOCAL_O_VISITANTE); }

    /**
     * @brief Destructor por defecto de la clase `Anotacion`.
     */
    ~Anotacion() = default;

    // ---------------------------------------------------------------------------------------//
    //    GETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Obtiene el identificador del jugador que realizó la anotación.
     * @return `int` ID del jugador (`0` en caso de gol en contra o valor no especificado).
     */
    int getIdJugador() const;

    /**
     * @brief Obtiene el minuto exacto en el que ocurrió la anotación.
     * @return `int` Minuto del partido.
     */
    int getMinuto() const;

    /**
     * @brief Obtiene el nombre del equipo asignado a la anotación.
     * @return `const char*` Puntero a la cadena constante con la condición (`"LOCAL"` o `"VISITANTE"`).
     */
    const char *getEquipo() const;

    // ---------------------------------------------------------------------------------------//
    //    SETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Establece el ID del jugador anotador.
     *
     * @param[in] idJ Identificador único del jugador.
     * @return `true` si el ID es válido (`idJ >= 0`), `false` en caso contrario.
     */
    bool setIdJugador(const int idJ);

    /**
     * @brief Establece el minuto en que se registró el punto o gol.
     *
     * @param[in] min Minuto del encuentro.
     * @return `true` si `min >= 0`, `false` si se pasa un entero negativo.
     *
     * @pre `min` debe estar alineado con los límites temporales del encuentro.
     */
    bool setMinuto(const int min);

    /**
     * @brief Asigna la condición del equipo responsable de la anotación.
     *
     * @param[in] eq Cadena de caracteres que indica `"LOCAL"` o `"VISITANTE"`.
     *
     * @return `true` si la cadena se copió exitosamente, `false` si `eq` es un puntero nulo.
     *
     * @pre `eq` no debe ser `nullptr`.
     */
    bool setEquipo(const char *eq);
};

#endif // ANOTACION_HPP