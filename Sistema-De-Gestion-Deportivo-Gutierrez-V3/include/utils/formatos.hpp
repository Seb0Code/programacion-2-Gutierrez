#ifndef FORMATOS_HPP
#define FORMATOS_HPP
#include "../../include/models/fecha.hpp"
#include <sstream>
#include <string>
#include <type_traits>

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
    static char *convertirTextoAMayus(char *texto);

    /**
     * @brief Convierte todos los caracteres de un objeto `std::string` a mayúsculas.
     * * @param[in,out] texto Referencia al objeto `std::string` que se transformará.
     * @return Referencia o copia del `std::string` modificado en mayúsculas.
     * * @pre El parámetro `texto` no debe estar vacío.
     */
    static std::string convertirTextoAMayus(std::string &texto);

    /**
     * @brief Convierte todos los caracteres de una cadena C (`char*`) a minúsculas.
     * * @param[in,out] texto Puntero a la cadena de caracteres (`char*`) que se desea transformar.
     * @return Puntero a la misma cadena de texto `texto` convertida a minúsculas.
     * * @pre El parámetro `texto` no debe ser `nullptr` ni estar vacío.
     */
    static char *convertirTextoAMinus(char *texto);

    /**
     * @brief Convierte todos los caracteres de un objeto `std::string` a minúsculas.
     * * @param[in,out] texto Referencia al objeto `std::string` que se transformará.
     * @return Referencia o copia del `std::string` modificado en minúsculas.
     * * @pre El parámetro `texto` no debe estar vacío.
     */
    static std::string convertirTextoAMinus(std::string &texto);

    /**
     * @brief Convierte una cadena de texto en formato C (`const char*`) a una estructura `Fecha`.
     *
     * Lee una cadena con el formato esperado `YYYY-MM-DD` extrae sus valores numéricos mediante
     * `std::stringstream` y los asigna a un objeto `Fecha`. Realiza validación de puntero nulo
     * y verifica que los separadores sean guiones (`-`).
     *
     * @param[in] fechaCStr Cadena de texto en formato C (`const char*`) que representa la fecha ("YYYY-MM-DD").
     * @return Estructura `Fecha` con el año, mes y día extraídos. Si la cadena es `nullptr`,
     * el formato es inválido o los separadores no son guiones, retorna una fecha por defecto `{0, 0, 0}`.
     *
     * @pre La cadena `fechaCStr` debe tener el formato estricto `"YYYY-MM-DD"`.
     *
     * @code
     * const char* textoFecha = "2026-07-26";
     * Fecha f = MiClase::stringAFecha(textoFecha);
     * // f.anio == 2026, f.mes == 7, f.dia == 26
     *
     * const char* fechaInvalida = "2026/07/26";
     * Fecha fErr = MiClase::stringAFecha(fechaInvalida);
     * // fErr.anio == 0, fErr.mes == 0, fErr.dia == 0 (Formato rechazado por el separador)
     * @endcode
     */
    static Fecha convertirTextoAFecha(const char *fechaCStr);

    static void copiarCadena(char *destino, const char *origen, const size_t tamano);

    /**
     * @brief Elimina espacios en blanco al inicio y al final de una cadena.
     */
    static std::string trim(const std::string &str);

    /**
     * @brief Convierte una cadena de texto a un tipo de dato genérico.
     * Maneja internamente cualquier error de parseo retornando un valor por defecto.
     */
    template <typename T> T static parsearValor(const std::string &str, T valorPorDefecto = T{}) {
        std::string textoLimpio = trim(str);
        if (textoLimpio.empty()) {
            return valorPorDefecto;
        }

        // Si T es un std::string, simplemente devolvemos la cadena limpia
        if constexpr (std::is_same_v<T, std::string>) {
            return textoLimpio;
        } else {
            // Para tipos numéricos o de otro tipo, usamos stringstream
            std::stringstream ss(textoLimpio);
            T resultado;
            ss >> resultado;

            if (ss.fail() || !ss.eof()) {
                return valorPorDefecto;
            }

            return resultado;
        }
    }
};

#endif