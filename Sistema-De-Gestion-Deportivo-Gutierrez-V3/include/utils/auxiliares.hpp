#ifndef AUXILIARES_HPP
#define AUXILIARES_HPP

/**
 * @brief Contiene funciones auxiliares del programa que no inciden en su logica pero que facilitan
 * algunas acciones.
 */
class Auxiliares {

  private:
    // Para no dejar que la clase sea instanciada
    Auxiliares() = delete;

  public:
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