#ifndef PARTIDO_HPP
#define PARTIDO_HPP

#include "../utils/constantes.hpp"
#include "anotacion.hpp"
#include "tarjetaAmarilla.hpp"
#include "tarjetaRoja.hpp"
#include <ctime>
#include <string>

/**
 * @brief Modelo de la entidad de objeto Partido. Contiene su estructura y sus funciones básicas.
 */
class Partido {

  private:
    // Datos básicos
    int id{};
    int idEquipoLocal{};
    int idEquipoVisitante{};
    char fecha[constantes::TAMANO_FECHA]{};
    char estado[constantes::TAMANO_ESTADO]{}; // "PROGRAMADO", "JUGADO", "CANCELADO"
    char descripcion[constantes::TAMANO_DESCRIPCION]{};

    // Marcador global (para acceso rápido sin recorrer el array de goles)
    int anotacionesLocal{};
    int anotacionesVisitante{};

    // Tarjetas por equipo
    int tarjetasAmaLocal{};
    int tarjetasAmaVisitante{};
    int tarjetasRojasLocal{};
    int tarjetasRojasVisitante{};

    // Detalle de goles
    Anotacion anotaciones[constantes::MAX_ANOTACIONES]{};           // Máximo 22 goles por partido
    TarjetaAmarilla tarjetaA[constantes::MAX_TARJETAS_AMARILLAS]{}; // Máximo 30 tarjetas A por partido
    TarjetaRoja tarjetaR[constantes::MAX_TARJETAS_ROJAS]{};         // Máximo 8 tarjetas R por partido
    int numAnotaciones{};
    int numtarjetaAma{};
    int numTarjetasRojas{};

    // Metadata de control
    bool eliminado{};
    time_t fechaCreacion{};
    time_t fechaUltimaModificacion{};

  public:
    /**
     * @brief Constructor por defecto de la clase Partido.
     */
    Partido() = default;

    /**
     * @brief Constructor parametrizado para inicializar un partido con sus datos principales.
     * @param idP Identificador único del partido.
     * @param idLocal Identificador del equipo local.
     * @param idVisitante Identificador del equipo visitante.
     * @param fechaP Puntero al arreglo de caracteres con la fecha del partido.
     * @param estadoP Puntero al arreglo de caracteres con el estado del partido.
     * @param descripcionP Puntero al arreglo de caracteres con la descripción del partido.
     */
    Partido(int idP, int idLocal, int idVisitante, const char *fechaP, const char *estadoP, const char *descripcionP);

    /**
     * @brief Destructor de la clase Partido.
     */
    ~Partido() = default;

    // ---------------------------------------------------------------------------------------------- //
    //   GETTERS                                                                                       //
    // ---------------------------------------------------------------------------------------------- //

    /**
     * @brief Retorna el ID del partido.
     * @return Devuelve el identificador único del partido.
     */
    int getId() const;

    /**
     * @brief Retorna el ID del equipo local.
     * @return Devuelve el identificador del equipo local.
     */
    int getIdEquipoLocal() const;

    /**
     * @brief Retorna el ID del equipo visitante.
     * @return Devuelve el identificador del equipo visitante.
     */
    int getIdEquipoVisitante() const;

    /**
     * @brief Retorna la fecha del partido.
     * @return Devuelve un puntero a un array de char con la fecha del partido.
     */
    const char *getFecha() const;

    /**
     * @brief Retorna el estado del partido.
     * @return Devuelve un puntero a un array de char con el estado del partido ("PROGRAMADO", "JUGADO", "CANCELADO").
     */
    const char *getEstado() const;

    /**
     * @brief Retorna la descripción del partido.
     * @return Devuelve un puntero a un array de char con la descripción del partido.
     */
    const char *getDescripcion() const;

    /**
     * @brief Retorna la cantidad de anotaciones del equipo local.
     * @return Devuelve el número de anotaciones del equipo local.
     */
    int getAnotacionesLocal() const;

    /**
     * @brief Retorna la cantidad de anotaciones del equipo visitante.
     * @return Devuelve el número de anotaciones del equipo visitante.
     */
    int getAnotacionesVisitante() const;

    /**
     * @brief Retorna la cantidad de tarjetas amarillas del equipo local.
     * @return Devuelve el número de tarjetas amarillas del equipo local.
     */
    int getTarjetasAmaLocal() const;

    /**
     * @brief Retorna la cantidad de tarjetas amarillas del equipo visitante.
     * @return Devuelve el número de tarjetas amarillas del equipo visitante.
     */
    int getTarjetasAmaVisitante() const;

    /**
     * @brief Retorna la cantidad de tarjetas rojas del equipo local.
     * @return Devuelve el número de tarjetas rojas del equipo local.
     */
    int getTarjetasRojasLocal() const;

    /**
     * @brief Retorna la cantidad de tarjetas rojas del equipo visitante.
     * @return Devuelve el número de tarjetas rojas del equipo visitante.
     */
    int getTarjetasRojasVisitante() const;

    /**
     * @brief Retorna un puntero de solo lectura al arreglo de anotaciones del partido.
     * @return Devuelve un puntero constante al primer elemento del arreglo de anotaciones.
     */
    const Anotacion *getAnotaciones() const;

    /**
     * @brief Retorna un puntero de solo lectura al arreglo de tarjetas amarillas del partido.
     * @return Devuelve un puntero constante al primer elemento del arreglo de tarjetas amarillas.
     */
    const TarjetaAmarilla *getTarjetasAmarillas() const;

    /**
     * @brief Retorna un puntero de solo lectura al arreglo de tarjetas rojas del partido.
     * @return Devuelve un puntero constante al primer elemento del arreglo de tarjetas rojas.
     */
    const TarjetaRoja *getTarjetasRojas() const;

    /**
     * @brief Retorna la cantidad de anotaciones registradas en el detalle del partido.
     * @return Devuelve el número de elementos usados en el arreglo de anotaciones.
     */
    int getNumAnotaciones() const;

    /**
     * @brief Retorna la cantidad de tarjetas amarillas registradas en el detalle del partido.
     * @return Devuelve el número de elementos usados en el arreglo de tarjetas amarillas.
     */
    int getNumTarjetaAma() const;

    /**
     * @brief Retorna la cantidad de tarjetas rojas registradas en el detalle del partido.
     * @return Devuelve el número de elementos usados en el arreglo de tarjetas rojas.
     */
    int getNumTarjetasRojas() const;

    /**
     * @brief Retorna si el partido ha sido marcado como eliminado (borrado lógico).
     * @return true si el partido está eliminado, false en caso contrario.
     */
    bool getEliminado() const;

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
     * @brief Retorna la fecha de creación del partido formateada para lectura humana.
     * @return Devuelve un objeto std::string con la fecha formateada.
     */
    std::string getFechaCreacionFormateada() const;

    /**
     * @brief Retorna la fecha de última modificación formateada para lectura humana.
     * @return Devuelve un objeto std::string con la fecha formateada.
     */
    std::string getFechaUltimaModificacionFormateada() const;

    /**
     * @brief Retorna el tamaño de la entidad partido.
     * @return Devuelve el tamaño de la entidad Partido.
     */
    static size_t getTamano();

    // ---------------------------------------------------------------------------------------------- //
    //   SETTERS                                                                                       //
    // ---------------------------------------------------------------------------------------------- //

    /**
     * @brief Establece el ID del partido.
     * @param idP Nuevo identificador único del partido.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setId(int idP);

    /**
     * @brief Establece el ID del equipo local.
     * @param idLocal Nuevo identificador del equipo local.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setIdEquipoLocal(int idLocal);

    /**
     * @brief Establece el ID del equipo visitante.
     * @param idVisitante Nuevo identificador del equipo visitante.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setIdEquipoVisitante(int idVisitante);

    /**
     * @brief Establece la fecha del partido.
     * @param fechaP Puntero al arreglo de caracteres con la nueva fecha.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setFecha(const char *fechaP);

    /**
     * @brief Establece el estado del partido ("PROGRAMADO", "JUGADO", "CANCELADO").
     * @param estadoP Puntero al arreglo de caracteres con el nuevo estado.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setEstado(const char *estadoP);

    /**
     * @brief Establece la descripción del partido.
     * @param descripcionP Puntero al arreglo de caracteres con la nueva descripción.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setDescripcion(const char *descripcionP);

    /**
     * @brief Establece la cantidad de anotaciones del equipo local.
     * @param anotaciones Nuevo número de anotaciones del equipo local.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setAnotacionesLocal(int anotaciones);

    /**
     * @brief Establece la cantidad de anotaciones del equipo visitante.
     * @param anotaciones Nuevo número de anotaciones del equipo visitante.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setAnotacionesVisitante(int anotaciones);

    /**
     * @brief Establece la cantidad de tarjetas amarillas del equipo local.
     * @param tarjetas Nuevo número de tarjetas amarillas del equipo local.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setTarjetasAmaLocal(int tarjetas);

    /**
     * @brief Establece la cantidad de tarjetas amarillas del equipo visitante.
     * @param tarjetas Nuevo número de tarjetas amarillas del equipo visitante.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setTarjetasAmaVisitante(int tarjetas);

    /**
     * @brief Establece la cantidad de tarjetas rojas del equipo local.
     * @param tarjetas Nuevo número de tarjetas rojas del equipo local.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setTarjetasRojasLocal(int tarjetas);

    /**
     * @brief Establece la cantidad de tarjetas rojas del equipo visitante.
     * @param tarjetas Nuevo número de tarjetas rojas del equipo visitante.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setTarjetasRojasVisitante(int tarjetas);

    /**
     * @brief Agrega una nueva anotación al detalle del partido, si hay espacio disponible.
     * @param nuevaAnotacion Estructura Anotacion a agregar.
     * @return true si se agregó correctamente, false si el arreglo está lleno o los datos son inválidos.
     */
    bool agregarAnotacion(const Anotacion &nuevaAnotacion);

    /**
     * @brief Agrega una nueva tarjeta amarilla al detalle del partido, si hay espacio disponible.
     * @param nuevaTarjeta Estructura tarjetaAmarilla a agregar.
     * @return true si se agregó correctamente, false si el arreglo está lleno o los datos son inválidos.
     */
    bool agregarTarjetaAmarilla(const TarjetaAmarilla &nuevaTarjeta);

    /**
     * @brief Agrega una nueva tarjeta roja al detalle del partido, si hay espacio disponible.
     * @param nuevaTarjeta Estructura tarjetaRoja a agregar.
     * @return true si se agregó correctamente, false si el arreglo está lleno o los datos son inválidos.
     */
    bool agregarTarjetaRoja(const TarjetaRoja &nuevaTarjeta);

    /**
     * @brief Modifica una anotación existente en el detalle del partido, dado su índice.
     * @param indice Posición dentro del arreglo de anotaciones (0-based).
     * @param anotacionModificada Estructura Anotacion con los nuevos datos.
     * @return true si el índice es válido y el cambio fue realizado, false en caso contrario.
     */
    bool setAnotacionPorIndice(const int indice, const Anotacion &anotacionModificada);

    /**
     * @brief Modifica una tarjeta amarilla existente en el detalle del partido, dado su índice.
     * @param indice Posición dentro del arreglo de tarjetas amarillas (0-based).
     * @param tarjetaModificada Estructura tarjetaAmarilla con los nuevos datos.
     * @return true si el índice es válido y el cambio fue realizado, false en caso contrario.
     */
    bool setTarjetaAmarillaPorIndice(const int indice, const TarjetaAmarilla &tarjetaModificada);

    /**
     * @brief Modifica una tarjeta roja existente en el detalle del partido, dado su índice.
     * @param indice Posición dentro del arreglo de tarjetas rojas (0-based).
     * @param tarjetaModificada Estructura tarjetaRoja con los nuevos datos.
     * @return true si el índice es válido y el cambio fue realizado, false en caso contrario.
     */
    bool setTarjetaRojaPorIndice(const int indice, const TarjetaRoja &tarjetaModificada);

    /**
     * @brief Establece el estado de eliminación (borrado lógico) del partido.
     * @param elim Valor booleano que indica si el partido ha sido eliminado.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setEliminado(const bool elim);

    /**
     * @brief Establece la fecha de creación del registro.
     * @param fechaC Marca de tiempo de tipo time_t que representa la creación.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setFechaCreacion(const time_t fechaC);

    /**
     * @brief Establece la fecha de la última modificación del registro.
     * @param fechaUM Marca de tiempo de tipo time_t que representa la modificación.
     * @return true si el cambio fue realizado correctamente, false en caso contrario.
     */
    bool setFechaUltimaModificacion(const time_t fechaUM);
};

#endif // PARTIDO_HPP