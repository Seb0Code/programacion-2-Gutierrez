#ifndef VALIDACIONES_HPP
#define VALIDACIONES_HPP

#include <iostream>

/**
 * @brief Gestor de Validaciones Generales.
 * * Proporciona métodos estáticos/útiles para validar la entrada de datos en el sistema.
 * La mayoría de las funciones devuelven true si la validación es exitosa y false si no lo es,
 * enviando mensajes explicativos mediante parámetros de salida cuando aplica.
 */
class GestorDeValidaciones {

  private:
    // Constructor eliminado para evitar la instanciación (clase utilitaria estática)
    GestorDeValidaciones() = delete;

  public:
    // * ====================================================================================//
    // * Validaciones Auxiliares y de Formato Básico                                         //
    // * ====================================================================================//

    /**
     * @brief Compara si la longitud de una cadena coincide exactamente con un tamaño dado.
     * @param texto Cadena de texto a evaluar.
     * @param tamano Longitud esperada en número de caracteres.
     * @return true si el tamaño coincide, false en caso contrario.
     * @pre El parámetro tamano debe ser positivo (tamano > 0).
     */
    static bool validarTamano(const char *texto, const size_t tamano);

    /**
     * @brief Verifica si una cadena de texto está vacía o es nula.
     * @param texto Cadena de texto a evaluar.
     * @return true si está vacía o es nullptr, false en caso contrario.
     */
    static inline bool validarCadenaVacia(const char *texto);

    /**
     * @brief Comprueba si la cadena contiene únicamente caracteres numéricos.
     * @param texto Cadena de texto a evaluar.
     * @return true si contiene solo dígitos, false en caso contrario.
     */
    static bool validarSoloNumeros(const char *texto);

    /**
     * @brief Comprueba si la cadena contiene únicamente letras (y caracteres alfabéticos).
     * @param texto Cadena de texto a evaluar.
     * @return true si contiene solo letras, false en caso contrario.
     */
    static bool validarSoloLetras(const char *texto);

    /**
     * @brief Valida que un valor numérico entero sea mayor a cero.
     * @param num Número a evaluar.
     * @return true si el valor es estrictamente positivo (> 0), false en caso contrario.
     */
    static bool validarEsPositivo(const int num);



    /**
     * @brief Convierte los componentes de una fecha en un número entero único para facilitar comparaciones (ej. YYYYMMDD).
     * @param anio Año de la fecha.
     * @param mes Mes de la fecha.
     * @param dia Día de la fecha.
     * @return Entero representativo de la fecha ordenable cronológicamente.
     */
    static int convertirFechaANumeroEntero(int anio, int mes, int dia);

    /**
     * @brief Verifica si la cadena contiene solo caracteres alfanuméricos y espacios.
     * @param texto Cadena de texto a evaluar.
     * @return true si es alfanumérica (con o sin espacios), false si contiene caracteres especiales.
     */
    static bool validarEsAlfanumericoConEspacios(const char *texto);

    // ====================================================================================//
    //  Validaciones De Negocio                                                            //
    // ====================================================================================//

    /**
     * @brief Evalúa si un año determinado es bisiesto.
     * @param anio Año a evaluar (ej. 2024).
     * @return true si es bisiesto, false en caso contrario.
     */
    static bool validarEsBisiesto(int anio);

    /**
     * @brief Parsea una cadena con formato de fecha (YYYY-MM-DD) extrayendo sus componentes numéricos.
     * @param fecha Cadena de texto con la fecha original en formato "YYYY-MM-DD".
     * @param[out] anio Variable de referencia donde se almacenará el año resultante.
     * @param[out] mes Variable de referencia donde se almacenará el mes resultante.
     * @param[out] dia Variable de referencia donde se almacenará el día resultante.
     */
    static void separarFechaEnPartes(const char *fecha, int &anio, int &mes, int &dia);

    /**
     * @brief Valida que el minuto de un evento/partido esté dentro de un rango permitido.
     * @param minuto Minuto a validar.
     * @param[out] mensajeError Búfer donde se copia el detalle del error en caso de fallo.
     * @return true si el minuto es válido, false en caso contrario.
     */
    static bool validarMinuto(const int minuto);

    /**
     * @brief Valida que un Identificador (ID) cumpla con los formatos del sistema.
     * @param id Identificador numérico a evaluar.
     * @return true si el ID es válido, false en caso contrario.
     */
    static bool validarId(const int id);

    /**
     * @brief Valida si un ID es válido en el contexto específico de anotación de un autogol.
     * @param id Identificador numérico a evaluar.
     * @return true si el ID es apto para autogol, false en caso contrario.
     */
    static bool validarIdParaAutogol(const int id);

    /**
     * @brief Valida si un valor de edad está dentro de rangos lógicos/permitidos para el torneo.
     * @param edad Edad en años.
     * @return true si la edad es válida, false en caso contrario.
     */
    static bool validarEdad(const int edad);

    /**
     * @brief Valida el número de dorsal/camiseta de un jugador.
     * @param dorsal Número de dorsal a evaluar.
     * @return true si el dorsal es válido, false en caso contrario.
     */
    static bool validarDorsal(const int dorsal);

    /**
     * @brief Valida la sintaxis y coherencia del calendario de una fecha dada.
     * @param fecha Cadena de texto que representa la fecha.
     * @return true si la fecha es válida, false en caso contrario.
     */
    static bool validarFecha(const char *fecha);

    /**
     * @brief Comprueba que la fecha de finalización dada no sea menor/anterior en el tiempo que la fecha base.
     * @param fechaInicio Cadena con la fecha final a validar.
     * @param fechaFin Cadena con la fecha final a validar.
     * @return true si es una fecha fin coherente, false en caso contrario.
     */
    static bool validarFechaFin(const char *fechaInicio, const char *fechaFin);

    /**
     * @brief Valida que la fecha asignada a un partido esté dentro de los rangos permitidos del torneo.
     * @param fechaPartido Cadena con la fecha del encuentro.
     * @param fechaInicioTorneo Cadena con la decha del inicio del torneo.
     * @param fechaFinTorneo Cadena con la fecha de finalización del torneo.
     * @return true si la fecha del partido es aceptable, false en caso contrario.
     */
    static bool validarFechaDeRegistroDePartidos(const char *fechaPartido, const char *fechaInicioTorneo, const char *fechaFinTorneo);

    /**
     * @brief Valida que la fecha de registro para un jugador o equipo cumpla los límites estipulados.
     * @param fechaRegistro Cadena con la fecha de inscripción/registro.
     * @return true si la fecha de registro es válida, false en caso contrario.
     */
    static bool validarFechaDeRegistroDeJugadorOEquipo(const char *fechaRegistro, const char *fehcaInicioTorneo);

    /**
     * @brief Valida el formato y la autenticidad del documento de identidad (Cédula).
     * @param cedula Cadena con la cédula/DNI a verificar.
     * @return true si la cédula cumple el formato, false en caso contrario.
     */
    static bool validarCedula(const char *cedula);

    /**
     * @brief Valida que una cadena sea un nombre o apellido válido (longitud, caracteres alfabéticos, etc.).
     * @param nombre Cadena con el nombre o apellido a evaluar.
     * @return true si el nombre cumple con los criterios, false en caso contrario.
     */
    static bool validarNombreOApellido(const char *nombre);

    /**
     * @brief Valida que el nombre asignado a un torneo sea correcto en sintaxis y formato.
     * @param nombreTorneo Cadena con el nombre del torneo.
     * @return true si el nombre es aceptable, false en caso contrario.
     */
    static bool validarNombreTorneo(const char *nombreTorneo);
};

#endif // VALIDACIONES_HPP