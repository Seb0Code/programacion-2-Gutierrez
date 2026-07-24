#ifndef FORMATOS_HPP
#define FORMATOS_HPP

/**
 * @brief Contiene funciones que formatean o afectan la salida de mensajes en consola.
 * Contiene funciones auxiliares del programa que no inciden en su logica pero que facilitan
 * algunas acciones.
 */
class Formatos {

  private:
    // Se encarga de no permitir que la clase sea instanciada
    Formatos() = delete;

  public:
    /**
     * @brief Pausa el programa por cierta cantidad de tiempo, si no es indicada por el usuario el tiempo
     * de espera es de 1 segundo.
     * @param tiempo Cantidad de tiempo a esperar.
     */
    static void esperarTiempo(int tiempo = 1000);

    /**
     * @brief Limpia la salida en consola.
     */
    static void limpiarPantalla();

    /**
     * @brief Pausa el programa hasta que el usuario ingrese un enter por consola.
     */
    static void pausarPrograma();

    /**
     * @brief Configura el idioma de la salida de la consola a UTF-8 àra permitir acentos y carácteres especiales.
     */
    static void configurarIdioma();

    /**
     * @brief Transforma la cadena a mayusculas y devuelve la cadena
     * @param texto Cadena de texto a evaluar.
     * @return Devuelve la cadena convertida en mayusculas.
     * @pre El parámetro texto no debe estar vacio.
     */
    static char *convertirCadenaAMayus(char *texto);

    /**
     * @brief Transforma la cadena a minusculas y devuelve la cadena
     * @param texto Cadena de texto a evaluar.
     * @return Devuelve la cadena convertida en minusculas.
     * @pre El parámetro texto no debe estar vacio.
     */
    static char *convertirCadenaAMinus(char *texto);
};

#endif