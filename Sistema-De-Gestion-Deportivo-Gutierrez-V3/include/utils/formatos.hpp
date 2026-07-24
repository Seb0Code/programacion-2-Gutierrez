#ifndef FORMATOS_HPP
#define FORMATOS_HPP

/**
 * @brief Contiene funciones que formatean o afectan la salida de mensajes en consola.
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
};

#endif