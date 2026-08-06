#ifndef JUGADORES_HPP
#define JUGADORES_HPP

#include "../utils/constantes.hpp"
#include "../utils/formatos.hpp"
#include <ctime>

/**
 * @file Jugador.hpp
 * @brief Declaración de la clase modelo Jugador y sus operaciones de gestión individual.
 */

/**
 * @class Jugador
 * @brief Modelo de datos que representa a un jugador registrado en el torneo.
 *
 * @details Almacena los datos personales y deportivos del atleta, pertenencia a equipo,
 * estadísticas acumuladas (goles, tarjetas) y metadata de control para auditoría.
 */
class Jugador {

  private:
    // Datos básicos
    int id{};                                       ///< Identificador único del jugador.
    int idEquipo{};                                 ///< Identificador del equipo al que pertenece.
    char nombre[constantes::TAMANO_NOMBRE]{};       ///< Nombre completo del jugador.
    char cedula[constantes::TAMANO_CEDULA]{};       ///< Documento de identidad / Cédula.
    char posicion[constantes::TAMANO_POSICION]{};   ///< Posición de juego en el campo/cancha.
    int edad{};                                     ///< Edad del jugador en años.
    int numeroDorsal{};                             ///< Número de camiseta/dorsal asignado.
    char fechaRegistro[constantes::TAMANO_FECHA]{}; ///< Fecha de registro en formato `YYYY-MM-DD`.

    // Estadísticas individuales
    int numAnotaciones{};   ///< Total de goles/puntos anotados.
    int tarjetaAmarillas{}; ///< Total de tarjetas amarillas recibidas.
    int tarjetasRojas{};    ///< Total de tarjetas rojas recibidas.

    // Metadata de control
    bool eliminado{};                 ///< Bandera de borrado lógico (soft-delete).
    time_t fechaCreacion{};           ///< Timestamp Unix de creación del registro.
    time_t fechaUltimaModificacion{}; ///< Timestamp Unix de la última modificación.

  public:
    /**
     * @brief Obtiene el tamaño en bytes de la estructura interna del objeto `Jugador`.
     *
     * @details Devuelve la cantidad exacta de memoria que ocupa un objeto de tipo `Jugador`
     * (incluyendo alineación y relleno de estructura si aplica). Esta función es especialmente útil
     * para operaciones de serialización, persistencia en archivos binarios o cálculo de desplazamientos.
     *
     * @return `std::size_t` Tamaño de la clase `Jugador` expresado en bytes (`sizeof(Jugador)`).
     *
     * @post No altera el estado interno de la clase ni produce efectos secundarios.
     *
     * @code
     * std::size_t bytes = Jugador::getTamano();
     * // Permite reservar buffers o escribir registros directos en archivos binarios:
     * // archivo.write(reinterpret_cast<const char*>(&jugador), Jugador::getTamano());
     * @endcode
     */
    static size_t getTamano() { return sizeof(Jugador); }

    /**
     * @brief Constructor por defecto de la clase `Jugador`.
     * @details Inicializa todos los miembros con sus valores por defecto (list-initialization `{}`).
     */
    Jugador() = default;

    /**
     * @brief Constructor parametrizado para instanciar un objeto `Jugador` completo.
     *
     * @param[in] idJ Identificador único del jugador.
     * @param[in] idEq Identificador del equipo asignado.
     * @param[in] nom Cadena con el nombre del jugador.
     * @param[in] ced Cadena con el documento de identidad / cédula.
     * @param[in] pos Cadena con la posición dentro del juego.
     * @param[in] ed Edad en años.
     * @param[in] dor Número de dorsal.
     * @param[in] fechaR Cadena con la fecha de registro en formato `YYYY-MM-DD`.
     * @param[in] eli Estado de borrado lógico (`true` si está eliminado).
     * @param[in] fechaC Timestamp Unix de creación.
     * @param[in] fechaUM Timestamp Unix de última actualización.
     *
     * @pre Los punteros `nom`, `ced`, `pos` y `fechaR` no deben ser `nullptr`.
     * @post Copia las cadenas hacia sus respectivos buffers internos delimitados.
     *
     * @code
     * Jugador j(1, 5, "Carlos Perez", "12345678", "Delantero", 24, 9, "2026-03-15", false, std::time(nullptr), std::time(nullptr));
     * @endcode
     */
    Jugador(int idJ, int idEq, const char *nom, const char *ced, const char *pos, int ed, int dor, const char *fechaR, bool eli, time_t fechaC, time_t fechaUM);

    /**
     * @brief Destructor por defecto de la clase `Jugador`.
     */
    ~Jugador() = default;

    // ---------------------------------------------------------------------------------------//
    //    GETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Obtiene el identificador único del jugador.
     * @return `int` ID del jugador.
     */
    int getId() const;

    /**
     * @brief Obtiene el identificador del equipo al que pertenece el jugador.
     * @return `int` ID del equipo.
     */
    int getIdEquipo() const;

    /**
     * @brief Obtiene el nombre completo del jugador.
     * @return `const char*` Puntero a la cadena constante del nombre.
     */
    const char *getNombre() const;

    /**
     * @brief Obtiene la cédula / documento de identidad del jugador.
     * @return `const char*` Puntero a la cadena constante del documento.
     */
    const char *getCedula() const;

    /**
     * @brief Obtiene la posición de juego del jugador.
     * @return `const char*` Puntero a la cadena constante de la posición.
     */
    const char *getPosicion() const;

    /**
     * @brief Obtiene la edad del jugador.
     * @return `int` Edad en años.
     */
    int getEdad() const;

    /**
     * @brief Obtiene el número de dorsal/camiseta del jugador.
     * @return `int` Número de dorsal.
     */
    int getNumeroDorsal() const;

    /**
     * @brief Obtiene la fecha de registro del jugador.
     * @return `const char*` Puntero a la cadena en formato `YYYY-MM-DD`.
     */
    const char *getFechaRegistro() const;

    /**
     * @brief Obtiene la cantidad total de anotaciones registradas.
     * @return `int` Total de goles/puntos acumulados.
     */
    int getNumAnotaciones() const;

    /**
     * @brief Obtiene la cantidad total de tarjetas amarillas.
     * @return `int` Total de tarjetas amarillas.
     */
    int getTarjetaAmarillas() const;

    /**
     * @brief Obtiene la cantidad total de tarjetas rojas.
     * @return `int` Total de tarjetas rojas.
     */
    int getTarjetasRojas() const;

    /**
     * @brief Consulta el estado de borrado lógico del jugador.
     * @return `true` si el registro está marcado como eliminado, `false` en caso contrario.
     */
    bool getEliminado() const;

    /**
     * @brief Obtiene el timestamp Unix de creación del registro.
     * @return `time_t` Timestamp de la fecha de creación.
     */
    time_t getFechaCreacion() const;

    /**
     * @brief Obtiene el timestamp Unix de la última modificación realizada.
     * @return `time_t` Timestamp de la última modificación.
     */
    time_t getFechaUltimaModificacion() const;

    // ---------------------------------------------------------------------------------------//
    //    SETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Asigna el identificador único del jugador.
     * @param[in] idJ Identificador numérico (`idJ > 0`).
     * @return `true` si es válido, `false` en caso contrario.
     */
    bool setId(const int idJ);

    /**
     * @brief Asigna el ID del equipo asociado al jugador.
     * @param[in] idEq Identificador del equipo.
     * @return `true` si `idEq > 0`, `false` en caso contrario.
     */
    bool setIdEquipo(const int idEq);

    /**
     * @brief Asigna el nombre del jugador.
     * @param[in] nom Cadena terminada en nulo (`\0`).
     * @return `true` si la copia fue exitosa, `false` si `nom` es `nullptr`.
     * @pre `nom` no debe ser un puntero `nullptr`.
     */
    bool setNombre(const char *nom);

    /**
     * @brief Asigna el documento de identidad / cédula.
     * @param[in] ced Cadena con el documento.
     * @return `true` si se asignó con éxito, `false` si `ced` es `nullptr`.
     * @pre `ced` no debe ser `nullptr`.
     */
    bool setCedula(const char *ced);

    /**
     * @brief Asigna la posición de juego.
     * @param[in] pos Cadena con la posición (ej. `"Portero"`, `"Delantero"`).
     * @return `true` si la operación se realizó correctamente, `false` si `pos` es `nullptr`.
     * @pre `pos` no debe ser `nullptr`.
     */
    bool setPosicion(const char *pos);

    /**
     * @brief Asigna la edad del jugador.
     * @param[in] ed Edad en años.
     * @return `true` si la edad es coherente (`ed > 0`), `false` en caso contrario.
     */
    bool setEdad(const int ed);

    /**
     * @brief Asigna el número de camiseta/dorsal.
     * @param[in] dor Número de dorsal.
     * @return `true` si `dor >= 0`, `false` si es un valor negativo.
     */
    bool setNumeroDorsal(const int dor);

    /**
     * @brief Asigna la fecha de registro en el sistema.
     * @param[in] fechaR Cadena en formato `YYYY-MM-DD`.
     * @return `true` si el formato/longitud es correcto, `false` en caso contrario.
     * @warning Debe mantenerse el formato estandarizado `YYYY-MM-DD`.
     */
    bool setFechaRegistro(const char *fechaR);

    /**
     * @brief Actualiza la cantidad de anotaciones/goles del jugador.
     * @param[in] anots Total de anotaciones a asignar.
     * @return `true` si `anots >= 0`, `false` si es un número negativo.
     */
    bool setNumAnotaciones(const int anots);

    /**
     * @brief Actualiza la cantidad acumulada de tarjetas amarillas.
     * @param[in] tAma Número de tarjetas amarillas.
     * @return `true` si `tAma >= 0`, `false` si es un valor negativo.
     */
    bool setTarjetaAmarillas(const int tAma);

    /**
     * @brief Actualiza la cantidad acumulada de tarjetas rojas.
     * @param[in] tRoj Número de tarjetas rojas.
     * @return `true` si `tRoj >= 0`, `false` si es un valor negativo.
     */
    bool setTarjetasRojas(const int tRoj);

    /**
     * @brief Establece el estado de borrado lógico para el jugador.
     * @param[in] eli Estado lógico (`true` para eliminar, `false` para reactivar).
     * @return `true` si el cambio fue aplicado exitosamente.
     */
    bool setEliminado(const bool eli);

    /**
     * @brief Asigna el timestamp Unix de la fecha de creación del registro.
     * @param[in] fechaC Estructura `time_t` con la fecha.
     * @return `true` si `fechaC >= 0`, `false` si es inválida.
     */
    bool setFechaCreacion(const time_t fechaC);

    /**
     * @brief Asigna el timestamp Unix de la fecha de última modificación.
     * @param[in] fechaUM Estructura `time_t` con la fecha actualizada.
     * @return `true` si `fechaUM >= 0`, `false` si es inválida.
     */
    bool setFechaUltimaModificacion(const time_t fechaUM);
};

#endif // JUGADORES_HPP