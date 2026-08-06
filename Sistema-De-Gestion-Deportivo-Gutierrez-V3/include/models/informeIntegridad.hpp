#ifndef INFORME_INTEGRIDAD_HPP
#define INFORME_INTEGRIDAD_HPP

#include "../utils/constantes.hpp"
#include "referenciaRota.hpp"

/**
 * @file InformeIntegridad.hpp
 * @brief Declaración de la clase InformeIntegridad para el seguimiento y reporte de auditorías referenciales.
 */

/**
 * @class InformeIntegridad
 * @brief Resultado compilado de una verificación global de integridad referencial.
 *
 * @details Acumula las métricas de conteo para los registros auditados (equipos, jugadores, partidos)
 * y almacena una lista estática de hasta `constantes::MAX_RESULTADOS` anomalías detectadas.
 * Si la cantidad de referencias rotas excede este límite, la clase registra dicho desbordamiento
 * para notificar que el reporte ha sido truncado.
 */
class InformeIntegridad {
  private:
    int equiposVerificados{};                           ///< Contador de registros de equipos auditados.
    int jugadoresVerificados{};                         ///< Contador de registros de jugadores auditados.
    int partidosVerificados{};                          ///< Contador de registros de partidos auditados.
    int totalDeReferenciasRotas{};                      ///< Total general de incidencias/referencias rotas contabilizadas.
    ReferenciaRota rotas[constantes::MAX_RESULTADOS]{}; ///< Arreglo de tamaño fijo con las anomalías registradas.
    bool referenciasOmitidas{};                         ///< Indica si se superó la capacidad máxima del arreglo interno.

  public:
    /**
     * @brief Obtiene el tamaño en bytes de la estructura interna del objeto `InformeIntegridad`.
     *
     * @details Devuelve la memoria total que ocupa un objeto `InformeIntegridad` en bytes,
     * incluyendo el arreglo estático de referencias rotas y sus banderas de control.
     *
     * @return `std::size_t` Tamaño de la clase expresado en bytes (`sizeof(InformeIntegridad)`).
     */
    static size_t getTamano() { return sizeof(InformeIntegridad); }

    /**
     * @brief Constructor por defecto de la clase `InformeIntegridad`.
     * @details Inicializa los contadores a cero, la bandera de omisión en `false` y limpia el arreglo interno.
     */
    InformeIntegridad() = default;

    /**
     * @brief Destructor por defecto de la clase `InformeIntegridad`.
     */
    ~InformeIntegridad() = default;

    // ---------------------------------------------------------------------------------------//
    //    GETTERS Y CONSULTAS
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Obtiene la cantidad total de equipos verificados durante la auditoría.
     * @return `int` Número de registros de equipos evaluados.
     */
    int getEquiposVerificados() const;

    /**
     * @brief Obtiene la cantidad total de jugadores verificados durante la auditoría.
     * @return `int` Número de registros de jugadores evaluados.
     */
    int getJugadoresVerificados() const;

    /**
     * @brief Obtiene la cantidad total de partidos verificados durante la auditoría.
     * @return `int` Número de registros de partidos evaluados.
     */
    int getPartidosVerificados() const;

    /**
     * @brief Obtiene el número total de referencias rotas o inconsistencias detectadas.
     * @return `int` Total de referencias rotas contabilizadas.
     */
    int getTotalDeReferenciasRotas() const;

    /**
     * @brief Consulta si existieron referencias rotas omitidas por exceder la capacidad del informe.
     * @return `true` si se identificaron más inconsistencias que las almacenables en el buffer, `false` en caso contrario.
     */
    bool hayReferenciasOmitidas() const;

    /**
     * @brief Determina si la base de datos o sistema se encuentra completamente íntegro.
     * @return `true` si no se detectó ninguna referencia rota (`totalDeReferenciasRotas == 0`), `false` en caso contrario.
     */
    bool esIntegro() const;

    /**
     * @brief Obtiene un puntero constante al arreglo estático de referencias rotas.
     * @return `const ReferenciaRota*` Puntero de solo lectura al inicio del arreglo interno `rotas`.
     */
    const ReferenciaRota *getReferenciasRotas() const;

    /**
     * @brief Devuelve una referencia rota específica según su índice dentro del informe.
     *
     * @param[in] indice Posición dentro del arreglo de referencias rotas (`0` a `MAX_RESULTADOS - 1`).
     *
     * @return `const ReferenciaRota*` Puntero al objeto en el índice indicado, o `nullptr` si está fuera de rango.
     *
     * @pre `indice` debe ser mayor o igual a 0 y menor que el total de elementos registrados.
     */
    const ReferenciaRota *getReferenciaRota(const int indice) const;

    // ---------------------------------------------------------------------------------------//
    //    MODIFICADORES Y OPERACIONES
    // ---------------------------------------------------------------------------------------//

    /**
     * @brief Incrementa en una unidad el contador de equipos verificados.
     */
    void incrementarEquiposVerificados();

    /**
     * @brief Incrementa en una unidad el contador de jugadores verificados.
     */
    void incrementarJugadoresVerificados();

    /**
     * @brief Incrementa en una unidad el contador de partidos verificados.
     */
    void incrementarPartidosVerificados();

    /**
     * @brief Registra una referencia rota detectada dentro del informe.
     *
     * @param[in] referencia Objeto de tipo `ReferenciaRota` a agregar al listado.
     *
     * @return `true` si la referencia fue registrada con éxito en el arreglo.
     * @return `false` si la referencia no es válida (`esValida() == false`) o si el informe alcanzó su capacidad máxima.
     *
     * @post Si el número de anomalías supera `constantes::MAX_RESULTADOS`, marca `referenciasOmitidas` como `true`.
     *
     * @code
     * ReferenciaRota ref("JUGADOR", 5, 99);
     * informe.agregarReferenciaRota(ref);
     * @endcode
     */
    bool agregarReferenciaRota(const ReferenciaRota &referencia);

    /**
     * @brief Restablece por completo el informe a su estado inicial.
     *
     * @post Reinicia todos los contadores a cero, limpia el arreglo interno de referencias y coloca la bandera de omisión en `false`.
     */
    void limpiar();
};

#endif // INFORME_INTEGRIDAD_HPP