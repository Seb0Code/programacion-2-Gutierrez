#ifndef EQUIPO_HPP
#define EQUIPO_HPP

#include "../utils/constantes.hpp"
#include "../utils/formatos.hpp"
#include <vector>

/**
 * @file Equipo.hpp
 * @brief Declaración de la clase modelo Equipo y sus operaciones de acceso y modificación.
 */

/**
 * @class Equipo
 * @brief Modelo de datos que representa a un equipo dentro del torneo, sus estadísticas y relaciones.
 *
 * @details Representa la entidad de un equipo participante, incluyendo su información administrativa,
 * métricas de desempeño (puntos, victorias, marcadores) y el histórico de partidos en los que interactúa.
 *
 * @note La clase utiliza buffers de caracteres de tamaño fijo (`char[]`) definidos en `constantes` para
 * facilitar la persistencia directa en binario o almacenamiento de longitud fija.
 */
class Equipo {

  private:
    // Datos básicos
    int id{};                                       ///< Identificador único del equipo.
    char nombre[constantes::TAMANO_NOMBRE]{};       ///< Cadena de caracteres para el nombre.
    char ciudad[constantes::TAMANO_NOMBRE]{};       ///< Cadena para la ciudad de origen.
    char entrenador[constantes::TAMANO_NOMBRE]{};   ///< Cadena para el nombre del entrenador.
    char fechaRegistro[constantes::TAMANO_FECHA]{}; ///< Cadena con la fecha de registro (`YYYY-MM-DD`).
    int numJugadores{};                             ///< Cantidad de jugadores registrados.

    // Estadísticas del torneo
    int puntos{};            ///< Puntos acumulados en la tabla del torneo.
    int jugados{};           ///< Cantidad total de partidos jugados.
    int victorias{};         ///< Cantidad de victorias obtenidas.
    int empates{};           ///< Cantidad de empates registrados.
    int derrotas{};          ///< Cantidad de derrotas acumuladas.
    int anotacionAFavor{};   ///< Puntos/Goles totales marcados por el equipo.
    int anotacionEnContra{}; ///< Puntos/Goles totales recibidos por el equipo.

    // Relaciones: IDs de partidos en que participó este equipo
    int partidosIDs[constantes::MAX_PARTIDOS_EQUIPOS]{}; ///< Arreglo de IDs de partidos asociados.
    int cantidadPartidos{};                              ///< Cantidad actual de partidos registrados en el arreglo.

    // Metadata de control
    bool eliminado{};                 ///< Bandera de borrado lógico (soft-delete).
    time_t fechaCreacion{};           ///< Timestamp Unix de creación del registro.
    time_t fechaUltimaModificacion{}; ///< Timestamp Unix de la última actualización.

  public:
    /**
     * @brief Obtiene el tamaño en bytes de la estructura interna del objeto `Equipo`.
     *
     * @details Devuelve la cantidad exacta de memoria que ocupa un objeto de tipo `Equipo`
     * (incluyendo alineación y relleno de estructura si aplica). Esta función es especialmente útil
     * para operaciones de serialización, persistencia en archivos binarios o cálculo de desplazamientos.
     *
     * @return `std::size_t` Tamaño de la clase `Equipo` expresado en bytes (`sizeof(Equipo)`).
     *
     * @post No altera el estado interno de la clase ni produce efectos secundarios.
     *
     * @code
     * std::size_t bytes = Equipo::getTamano();
     * // Permite reservar buffers o escribir registros directos en archivos binarios:
     * // archivo.write(reinterpret_cast<const char*>(&equipo), Equipo::getTamano());
     * @endcode
     */
    static size_t getTamano() { return sizeof(Equipo); }

    /**
     * @brief Constructor por defecto de la clase `Equipo`.
     * @details Inicializa todos los atributos miembros con sus valores por defecto (list-initialization `{}`).
     */
    Equipo() = default;

    /**
     * @brief Constructor parametrizado para instanciar un objeto `Equipo` con sus datos iniciales.
     *
     * @param[in] idE Identificador único del equipo.
     * @param[in] nom Cadena con el nombre del equipo.
     * @param[in] cdd Cadena con la ciudad de origen.
     * @param[in] ent Cadena con el nombre del entrenador.
     * @param[in] fechaR Cadena con la fecha de registro en formato `YYYY-MM-DD`.
     * @param[in] eli Estado de borrado lógico (`true` si está eliminado).
     * @param[in] fechaC Timestamp Unix de creación del registro.
     * @param[in] fechaUM Timestamp Unix de la última modificación.
     *
     * @pre Los punteros `nom`, `cdd`, `ent` y `fechaR` no deben ser `nullptr`.
     * @post Copia de forma segura las cadenas de texto a los buffers internos de tamaño fijo.
     *
     * @code
     * Equipo e(1, "Real Madrid", "Madrid", "Ancelotti", "2026-01-10", false, std::time(nullptr), std::time(nullptr));
     * @endcode
     */
    Equipo(int idE, const char *nom, const char *cdd, const char *ent, const char *fechaR, bool eli, time_t fechaC, time_t fechaUM);

    /**
     * @brief Destructor por defecto de la clase `Equipo`.
     */
    ~Equipo() = default;

    // ---------------------------------------------------------------------------------------//
    //    GETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Obtiene el identificador único del equipo.
     * @return `int` ID del equipo.
     */
    int getId() const;

    /**
     * @brief Obtiene el nombre del equipo.
     * @return `const char*` Puntero a la cadena de caracteres constante con el nombre.
     */
    const char *getNombre() const;

    /**
     * @brief Obtiene la ciudad de origen del equipo.
     * @return `const char*` Puntero a la cadena de caracteres constante con la ciudad.
     */
    const char *getCiudad() const;

    /**
     * @brief Obtiene el nombre del entrenador del equipo.
     * @return `const char*` Puntero a la cadena de caracteres constante con el entrenador.
     */
    const char *getEntrenador() const;

    /**
     * @brief Obtiene la fecha de registro en el sistema.
     * @return `const char*` Puntero a la cadena de caracteres constante en formato `YYYY-MM-DD`.
     */
    const char *getFechaRegistro() const;

    /**
     * @brief Obtiene el número actual de jugadores en la plantilla del equipo.
     * @return `int` Cantidad de jugadores.
     */
    int getNumJugadores() const;

    /**
     * @brief Obtiene los puntos acumulados en el torneo.
     * @return `int` Puntos totales.
     */
    int getPuntos() const;

    /**
     * @brief Obtiene la cantidad de partidos disputados por el equipo.
     * @return `int` Número de partidos jugados.
     */
    int getJugados() const;

    /**
     * @brief Obtiene la cantidad de victorias del equipo.
     * @return `int` Número de victorias.
     */
    int getVictorias() const;

    /**
     * @brief Obtiene la cantidad de empates del equipo.
     * @return `int` Número de empates.
     */
    int getEmpates() const;

    /**
     * @brief Obtiene la cantidad de derrotas del equipo.
     * @return `int` Número de derrotas.
     */
    int getDerrotas() const;

    /**
     * @brief Obtiene las anotaciones/goles a favor acumulados.
     * @return `int` Total de anotaciones a favor.
     */
    int getAnotacionAFavor() const;

    /**
     * @brief Obtiene las anotaciones/goles en contra acumulados.
     * @return `int` Total de anotaciones recibidas.
     */
    int getAnotacionEnContra() const;

    /**
     * @brief Devuelve un puntero constante al arreglo de IDs de partidos asociados al equipo.
     * @return `const int*` Puntero de solo lectura al arreglo interno `partidosIDs`.
     */
    const int *getPartidosIDs() const;

    /**
     * @brief Obtiene la cantidad de partidos actualmente registrados en la lista del equipo.
     * @return `int` Cantidad de partidos válidos guardados.
     */
    int getCantidadPartidos() const;

    /**
     * @brief Consulta el estado de borrado lógico del equipo.
     * @return `true` si el equipo está marcado como eliminado, `false` en caso contrario.
     */
    bool getEliminado() const;

    /**
     * @brief Obtiene la fecha y hora de creación del registro en el sistema.
     * @return `time_t` Timestamp de la fecha de creación.
     */
    time_t getFechaCreacion() const;

    /**
     * @brief Obtiene la fecha y hora de la última modificación del registro.
     * @return `time_t` Timestamp de la última modificación.
     */
    time_t getFechaUltimaModificacion() const;

    // ---------------------------------------------------------------------------------------//
    //    SETTERS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Establece el ID único del equipo.
     * @param[in] idE Identificador numérico a asignar.
     * @return `true` si el asignamiento fue exitoso, `false` si el ID es inválido (`<= 0`).
     */
    bool setId(const int idE);

    /**
     * @brief Asigna el nombre al equipo.
     *
     * @param[in] nom Cadena terminada en nulo (`\0`) con el nuevo nombre del equipo.
     *
     * @return `true` si se copió correctamente, `false` si el puntero es `nullptr` o excede el límite.
     *
     * @pre `nom` no debe ser un puntero `nullptr`.
     * @post La longitud máxima copiada estará delimitada por `constantes::TAMANO_NOMBRE`.
     */
    bool setNombre(const char *nom);

    /**
     * @brief Asigna la ciudad de origen al equipo.
     *
     * @param[in] cdd Cadena terminada en nulo (`\0`) con el nombre de la ciudad.
     *
     * @return `true` si se asignó con éxito, `false` en caso de error o si el puntero es `nullptr`.
     *
     * @pre `cdd` no debe ser `nullptr`.
     */
    bool setCiudad(const char *cdd);

    /**
     * @brief Asigna el nombre del entrenador del equipo.
     *
     * @param[in] ent Cadena terminada en nulo (`\0`) con el nombre del entrenador.
     *
     * @return `true` si la operación se realizó de forma correcta, `false` si `ent` es `nullptr`.
     *
     * @pre `ent` no debe ser `nullptr`.
     */
    bool setEntrenador(const char *ent);

    /**
     * @brief Asigna la fecha de registro del equipo.
     *
     * @param[in] fechaR Cadena terminada en nulo con formato `YYYY-MM-DD`.
     *
     * @return `true` si la fecha cumple con la longitud y formato correcto, `false` en caso contrario.
     *
     * @warning Debe asegurarse de pasar la cadena en el formato estandarizado `YYYY-MM-DD`.
     */
    bool setFechaRegistro(const char *fechaR);

    /**
     * @brief Asigna la cantidad de jugadores registrados en el equipo.
     *
     * @param[in] numJ Número entero que indica el total de jugadores.
     *
     * @return `true` si el valor es positivo o cero, `false` si el valor es negativo.
     */
    bool setNumJugadores(const int numJ);

    /**
     * @brief Establece la puntuación acumulada en la tabla general.
     *
     * @param[in] pts Puntos acumulados.
     * @return `true` si `pts >= 0`, `false` si se intenta pasar un valor negativo.
     */
    bool setPuntos(const int pts);

    /**
     * @brief Establece el número de partidos disputados.
     *
     * @param[in] jgd Total de partidos jugados.
     * @return `true` si `jgd >= 0`, `false` en caso de recibir un número negativo.
     */
    bool setJugados(const int jgd);

    /**
     * @brief Establece la cantidad de victorias alcanzadas.
     *
     * @param[in] vct Cantidad de victorias.
     * @return `true` si el valor es mayor o igual a `0`, `false` si es negativo.
     */
    bool setVictorias(const int vct);

    /**
     * @brief Establece la cantidad de empates logrados.
     *
     * @param[in] emp Cantidad de empates.
     * @return `true` si el valor es mayor o igual a `0`, `false` si es negativo.
     */
    bool setEmpates(const int emp);

    /**
     * @brief Establece la cantidad de derrotas del equipo.
     *
     * @param[in] drt Cantidad de derrotas.
     * @return `true` si el valor es mayor o igual a `0`, `false` si es negativo.
     */
    bool setDerrotas(const int drt);

    /**
     * @brief Establece el número de puntos/goles a favor acumulados.
     *
     * @param[in] antF Anotaciones a favor.
     * @return `true` si `antF >= 0`, `false` en caso contrario.
     */
    bool setAnotacionAFavor(const int antF);

    /**
     * @brief Establece el número de puntos/goles en contra acumulados.
     *
     * @param[in] antC Anotaciones en contra.
     * @return `true` si `antC >= 0`, `false` en caso contrario.
     */
    bool setAnotacionEnContra(const int antC);

    /**
     * @brief Agrega una nueva anotación al detalle del partido, si hay espacio disponible.
     * @param nuevoIdPartido Estructura Anotacion a agregar.
     * @return true si se agregó correctamente, false si el arreglo está lleno o los datos son inválidos.
     */
    bool agregarIdPartido(const int nuevoIdPartido);

    bool eliminarIdPartido(const int idPartido);

    /**
     * @brief Modifica una anotación existente en el detalle del partido, dado su índice.
     * @param indice Posición dentro del arreglo de anotaciones (0-based).
     * @param idPartidoModificado Estructura Anotacion con los nuevos datos.
     * @return true si el índice es válido y el cambio fue realizado, false en caso contrario.
     */
    bool setIdPartidoPorIndice(const int indice, const int idPartidoModificado);

    /**
     * @brief Cambia la marca de borrado lógico para el registro del equipo.
     *
     * @param[in] eli Estado lógico de borrado (`true` para eliminar, `false` para reactivar).
     * @return `true` siempre que se aplique el cambio exitosamente.
     */
    bool setEliminado(const bool eli);

    /**
     * @brief Asigna el timestamp Unix de la fecha de creación del registro.
     *
     * @param[in] fechaC Estructura `time_t` con la fecha.
     * @return `true` si `fechaC >= 0`, `false` si es una fecha inválida.
     */
    bool setFechaCreacion(const time_t fechaC);

    /**
     * @brief Asigna el timestamp Unix de la fecha de última modificación.
     *
     * @param[in] fechaUM Estructura `time_t` con la fecha actualizada.
     * @return `true` si `fechaUM >= 0`, `false` si es una fecha inválida.
     */
    bool setFechaUltimaModificacion(const time_t fechaUM);
};

#endif // EQUIPO_HPP