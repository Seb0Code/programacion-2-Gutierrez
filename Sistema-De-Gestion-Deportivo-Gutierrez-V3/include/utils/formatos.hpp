#ifndef FORMATOS_HPP
#define FORMATOS_HPP
#include <string>

/**
 * @file Formatos.hpp
 * @brief Definición de la clase estática Formatos para utilidades de consola y manipulación de texto.
 */

/**
 * @class Formatos
 * @brief Clase utilitaria con funciones auxiliares para formateo de texto y control de la consola.
 *
 * Proporciona métodos estáticos que facilitan la interacción con la consola (limpieza, pausas, idioma)
 * y la transformación de cadenas de texto (`char*` y `std::string`). No está diseñada para ser instanciada.
 */
class Formatos {

  private:
    /**
     * @brief Constructor privado eliminado para prevenir la instanciación de la clase.
     */
    Formatos() = delete;

  public:
    /**
     * @brief Pausa la ejecución del programa durante un intervalo de tiempo determinado.
     * * @param[in] tiempo Cantidad de tiempo a esperar expresada en milisegundos. Por defecto es `1000` ms (1 segundo).
     * * @note Utiliza internamente los mecanismos de espera del sistema operativo.
     */
    static void esperarTiempo(int tiempo = 1000);

    /**
     * @brief Limpia la salida visible de la consola de comandos.
     */
    static void limpiarPantalla();

    /**
     * @brief Pausa la ejecución del programa hasta que el usuario presione la tecla `Enter`.
     */
    static void pausarPrograma();

    /**
     * @brief Configura la codificación de la consola en UTF-8 para permitir tildes y caracteres especiales.
     */
    static void configurarIdioma();

    /**
     * @brief Convierte todos los caracteres de una cadena C (`char*`) a mayúsculas.
     * * @param[in,out] texto Puntero a la cadena de caracteres (`char*`) que se desea transformar.
     * @return Puntero a la misma cadena de texto `texto` convertida a mayúsculas.
     * * @pre El parámetro `texto` no debe ser `nullptr` ni estar vacío.
     * * @code
     * char mensaje[] = "hola mundo";
     * Formatos::convertirCadenaAMayus(mensaje);
     * // mensaje ahora contiene "HOLA MUNDO"
     * @endcode
     */
    static char *convertirCadenaAMayus_pchar(char *texto);

    /**
     * @brief Convierte todos los caracteres de un objeto `std::string` a mayúsculas.
     * * @param[in,out] texto Referencia al objeto `std::string` que se transformará.
     * @return Referencia o copia del `std::string` modificado en mayúsculas.
     * * @pre El parámetro `texto` no debe estar vacío.
     */
    static std::string convertirAMayus_string(std::string &texto);

    /**
     * @brief Convierte todos los caracteres de una cadena C (`char*`) a minúsculas.
     * * @param[in,out] texto Puntero a la cadena de caracteres (`char*`) que se desea transformar.
     * @return Puntero a la misma cadena de texto `texto` convertida a minúsculas.
     * * @pre El parámetro `texto` no debe ser `nullptr` ni estar vacío.
     */
    static char *convertirCadenaAMinus_pchar(char *texto);

    /**
     * @brief Convierte todos los caracteres de un objeto `std::string` a minúsculas.
     * * @param[in,out] texto Referencia al objeto `std::string` que se transformará.
     * @return Referencia o copia del `std::string` modificado en minúsculas.
     * * @pre El parámetro `texto` no debe estar vacío.
     */
    static std::string convertirAMinus_string(std::string &texto);
};

#endif