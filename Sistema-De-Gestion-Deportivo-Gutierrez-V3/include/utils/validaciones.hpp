#ifndef VALIDACIONES_HPP
#define VALIDACIONES_HPP

#include <cstddef> // Para que sea reconocido size_t
#include <string>

/**
 * @file Validaciones.hpp
 * @brief Definición de la clase utilitaria GestorDeValidaciones para la comprobación de reglas de negocio y tipos.
 */

/**
 * @class GestorDeValidaciones
 * @brief Proporciona métodos estáticos para validar la entrada de datos en el sistema.
 *
 * Esta clase no permite ser instanciada. Todos sus métodos son estáticos y devuelven `true`
 * si el dato de entrada cumple con la regla de validación o `false` en caso contrario.
 */
class GestorDeValidaciones {

  private:
    /**
     * @brief Constructor privado eliminado para evitar la instanciación de la clase.
     */
    GestorDeValidaciones() = delete;

  public:
    // =======================================================================================================
    // SOBRECARGAS PARA TEXTOS (compatibles con punteros char y std::string)
    // =======================================================================================================

    /**
     * @brief Verifica si una cadena de texto estilo C (`const char*`) está vacía o es `nullptr`.
     * @param[in] texto Puntero a la cadena de caracteres a evaluar.
     * @return `true` si es `nullptr` o su primer carácter es `'\0'`, `false` de lo contrario.
     */
    static bool validarCadenaVacia(const char *texto);

    /**
     * @brief Verifica si un objeto `std::string` está vacío.
     * @param[in] texto Referencia constante a la cadena a evaluar.
     * @return `true` si la longitud de la cadena es `0`, `false` en caso contrario.
     */
    static bool validarCadenaVacia(const std::string &texto);

    /**
     * @brief Comprueba si una cadena estilo C está vacía, es `nullptr` o contiene únicamente espacios en blanco.
     * @param[in] texto Puntero a la cadena de caracteres a evaluar.
     * @return `true` si no contiene caracteres imprimibles visibles, `false` en caso contrario.
     */
    static bool validarEsCadenaVaciaOSoloEspacios(const char *texto);

    /**
     * @brief Comprueba si un `std::string` está vacío o contiene únicamente espacios en blanco.
     * @param[in] texto Referencia constante a la cadena a evaluar.
     * @return `true` si no contiene caracteres imprimibles visibles, `false` en caso contrario.
     */
    static bool validarEsCadenaVaciaOSoloEspacios(const std::string &texto);

    /**
     * @brief Comprueba si una cadena estilo C contiene exclusivamente dígitos numéricos (`'0'` al `'9'`).
     * @param[in] texto Puntero a la cadena de caracteres a evaluar.
     * @return `true` si todos los caracteres son dígitos, `false` si contiene letras o símbolos.
     */
    static bool validarSoloNumeros(const char *texto);

    /**
     * @brief Comprueba si un `std::string` contiene exclusivamente dígitos numéricos (`'0'` al `'9'`).
     * @param[in] texto Referencia constante a la cadena a evaluar.
     * @return `true` si todos los caracteres son dígitos, `false` de lo contrario.
     */
    static bool validarSoloNumeros(const std::string &texto);

    /**
     * @brief Valida que una cadena estilo C contenga únicamente caracteres alfabéticos y espacios.
     * @param[in] texto Puntero a la cadena de caracteres a evaluar.
     * @return `true` si contiene solo letras (mayúsculas/minúsculas) y espacios, `false` en caso contrario.
     */
    static bool validarSoloLetras(const char *texto);

    /**
     * @brief Valida que un `std::string` contenga únicamente caracteres alfabéticos y espacios.
     * @param[in] texto Referencia constante a la cadena a evaluar.
     * @return `true` si contiene solo letras y espacios, `false` en caso contrario.
     */
    static bool validarSoloLetras(const std::string &texto);

    /**
     * @brief Verifica si una cadena estilo C contiene únicamente caracteres alfanuméricos y espacios.
     * @param[in] texto Puntero a la cadena de caracteres a evaluar.
     * @return `true` si está compuesta por letras, números o espacios; `false` si incluye símbolos especiales.
     */
    static bool validarEsAlfanumericoConEspacios(const char *texto);

    /**
     * @brief Verifica si un `std::string` contiene únicamente caracteres alfanuméricos y espacios.
     * @param[in] texto Referencia constante a la cadena a evaluar.
     * @return `true` si está compuesta por letras, números o espacios; `false` de lo contrario.
     */
    static bool validarEsAlfanumericoConEspacios(const std::string &texto);

    // =======================================================================================================
    // VALIDACIONES DE ATRIBUTOS DE CLASE Y ENTIDADES DE NEGOCIO
    // =======================================================================================================

    /**
     * @brief Evalúa si la longitud de una cadena estilo C no excede o coincide con un tamaño límite.
     * @param[in] texto Puntero a la cadena de caracteres.
     * @param[in] tamano Longitud máxima permitida o esperada.
     * @return `true` si la cadena cumple el criterio de tamaño, `false` de lo contrario.
     */
    static bool validarTamano(const char *texto, const size_t tamano);

    /**
     * @brief Evalúa si la longitud de una cadena de tipo std::string no excede o coincide con un tamaño límite.
     * @param[in] texto Variable de tipo `std::string` que contiene la cadena de caracteres.
     * @param[in] tamano Longitud máxima permitida o esperada.
     * @return `true` si la cadena cumple el criterio de tamaño, `false` de lo contrario.
     */
    static bool validarTamano(const std::string &texto, const size_t tamano);

    /**
     * @brief Valida si el minuto registrado de un evento en el partido es coherente (ej. en rango `0` a `120`).
     * @param[in] variable Valor entero que representa el minuto del evento.
     * @return `true` si el minuto es válido, `false` en caso contrario.
     */
    static bool validarMinuto(const int minuto, const int minutoMin, const int minutoMax);

    /**
     * @brief Determina si un número entero es estrictamente mayor que cero.
     * @param[in] num Valor entero a evaluar.
     * @return `true` si `num > 0`, `false` en caso de ser cero o negativo.
     */
    static bool validarEsPositivo(const int num);

    /**
     * @brief Valida que un identificador (`ID`) cumpla con el formato numérico del sistema.
     * @param[in] id Identificador numérico a comprobar.
     * @return `true` si el `ID` es un número entero válido y positivo, `false` en caso contrario.
     */
    static bool validarId(const int id);

    /**
     * @brief Valida que un `ID` sea apto para la asignación de un autogol.
     * @param[in] id Identificador numérico a evaluar.
     * @return `true` si el `ID` es válido en el contexto de autogol, `false` en caso contrario.
     */
    static bool validarIdParaAutogol(const int id);

    /**
     * @brief Evalúa si la edad ingresada está dentro del rango legal/permitido para participar.
     * @param[in] edad Número entero que representa la edad.
     * @return `true` si está dentro de los límites aceptados, `false` en caso contrario.
     */
    static bool validarEdad(const int edad);

    /**
     * @brief Valida que el número de camiseta/dorsal de un jugador esté dentro del rango permitido (`1` a `99`).
     * @param[in] dorsal Número de dorsal a evaluar.
     * @return `true` si el dorsal se encuentra entre `1` y `99` (incluidos), `false` de lo contrario.
     */
    static bool validarDorsal(const int dorsal);

    /**
     * @brief Valida la sintaxis y formato de una cédula de identidad o documento legal.
     * @param[in] cedula Cadena de texto con el número de cédula.
     * @return `true` si la cédula es válida en formato, `false` en caso contrario.
     */
    static bool validarCedula(const char *cedula);

    /**
     * @brief Valida la sintaxis y formato de una cédula de identidad o documento legal.
     * @param[in] texto Variable de tipo `std::string` a la cadena de caracteres.
     * @return `true` si la cadena cumple el criterio de tamaño, `false` de lo contrario.
     */
    static bool validarCedula(const std::string &cedula);

    /**
     * @brief Valida que un nombre o apellido no contenga caracteres numéricos ni símbolos no permitidos.
     * @param[in] nombre Cadena de texto con el nombre o apellido a evaluar.
     * @return `true` si la cadena representa un nombre o apellido válido, `false` en caso contrario.
     */
    static bool validarNombreOApellido(const char *nombre);

    /**
     * @brief Valida que un nombre o apellido no contenga caracteres numéricos ni símbolos no permitidos.
     * @param[in] nombre Cadena de texto con el nombre o apellido a evaluar.
     * @return `true` si la cadena representa un nombre o apellido válido, `false` en caso contrario.
     */
    static bool validarNombreOApellido(const std::string &nombre);

    /**
     * @brief Valida que el nombre de un torneo cumpla las políticas de formato del sistema.
     * @param[in] nombreTorneo Cadena de texto con el nombre del torneo.
     * @return `true` si es un nombre de torneo válido, `false` de lo contrario.
     */
    static bool validarNombreTorneo(const char *nombreTorneo);

    /**
     * @brief Valida que el nombre de un torneo cumpla las políticas de formato del sistema.
     * @param[in] nombreTorneo Cadena de texto con el nombre del torneo.
     * @return `true` si es un nombre de torneo válido, `false` de lo contrario.
     */
    static bool validarNombreTorneo(const std::string &nombreTorneo);

    // =======================================================================================================
    // LOGICA DE REGLAS DE TIEMPO Y FECHAS
    // =======================================================================================================

    /**
     * @brief Determina si un año específico es bisiesto.
     * @param[in] año Valor numérico del año a evaluar.
     * @return `true` si el año es bisiesto, `false` de lo contrario.
     */
    static bool validarEsBisiesto(const int año);

    /**
     * @brief Verifica que una cadena contenga una fecha válida y con la estructura esperada ("YYYY-MM-DD").
     * @param[in] fecha Cadena de texto que representa la fecha.
     * @return `true` si la fecha es sintáctica y calendáricamente válida, `false` de lo contrario.
     */
    static bool validarFecha(const char *fecha);

    /**
     * @brief Verifica que una cadena contenga una fecha válida y con la estructura esperada ("YYYY-MM-DD").
     * @param[in] fecha Cadena de texto que representa la fecha.
     * @return `true` si la fecha es sintáctica y calendáricamente válida, `false` de lo contrario.
     */
    static bool validarFecha(const std::string &fecha);

    /**
     * @brief Valida que la fecha de finalización sea cronológicamente posterior o igual a la fecha de inicio.
     * @param[in] fechaFin Cadena de texto con la fecha final.
     * @param[in] fechaInicio Cadena de texto con la fecha de inicio.
     * @return `true` si `fechaFin >= fechaInicio`, `false` si es anterior.
     */
    static bool validarFechaFin(const char *fechaFin, const char *fechaInicio);

    /**
     * @brief Valida que la fecha de finalización sea cronológicamente posterior o igual a la fecha de inicio.
     * @param[in] fechaFin Cadena de texto con la fecha final.
     * @param[in] fechaInicio Cadena de texto con la fecha de inicio.
     * @return `true` si `fechaFin >= fechaInicio`, `false` si es anterior.
     */
    static bool validarFechaFin(const std::string &fechaFin, const std::string &fechaInicio);

    /**
     * @brief Valida si la fecha en que se programa/juega un partido se encuentra dentro del rango oficial del torneo.
     * @param[in] fechaPartido Cadena con la fecha fijada para el partido.
     * @param[in] fechaInicioTorneo Cadena con la fecha de inicio oficial del torneo.
     * @param[in] fechaFinTorneo Cadena con la fecha de clausura del torneo.
     * @return `true` si `fechaInicioTorneo <= fechaPartido <= fechaFinTorneo`, `false` en caso contrario.
     */
    static bool validarFechaDeRegistroDePartidos(const char *fechaPartido, const char *fechaInicioTorneo, const char *fechaFinTorneo);

    /**
     * @brief Valida si la fecha en que se programa/juega un partido se encuentra dentro del rango oficial del torneo.
     * @param[in] fechaPartido Cadena con la fecha fijada para el partido.
     * @param[in] fechaInicioTorneo Cadena con la fecha de inicio oficial del torneo.
     * @param[in] fechaFinTorneo Cadena con la fecha de clausura del torneo.
     * @return `true` si `fechaInicioTorneo <= fechaPartido <= fechaFinTorneo`, `false` en caso contrario.
     */
    static bool validarFechaDeRegistroDePartidos(const std::string &fechaPartido, const std::string &fechaInicioTorneo, const std::string &fechaFinTorneo);

    /**
     * @brief Valida que la fecha de inscripción/registro de un jugador o equipo sea anterior a la fecha de inicio del torneo.
     * @param[in] fechaRegistro Cadena con la fecha de inscripción.
     * @param[in] fechaInicioTorneo Cadena con la fecha de inicio del torneo.
     * @return `true` si la fecha de registro es oportuna y válida, `false` de lo contrario.
     */
    static bool validarFechaDeRegistroDeJugadorOEquipo(const char *fechaRegistro, const char *fechaInicioTorneo);

    /**
     * @brief Valida que la fecha de inscripción/registro de un jugador o equipo sea anterior a la fecha de inicio del torneo.
     * @param[in] fechaRegistro Cadena con la fecha de inscripción.
     * @param[in] fechaInicioTorneo Cadena con la fecha de inicio del torneo.
     * @return `true` si la fecha de registro es oportuna y válida, `false` de lo contrario.
     */
    static bool validarFechaDeRegistroDeJugadorOEquipo(const std::string &fechaRegistro, const std::string &fechaInicioTorneo);
};

#endif // VALIDACIONES_HPP