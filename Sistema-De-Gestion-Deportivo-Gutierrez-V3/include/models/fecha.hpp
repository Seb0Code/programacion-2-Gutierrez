// Fecha.hpp
#ifndef FECHA_H
#define FECHA_H

/**
 * @file Fecha.hpp
 * @brief Definición de la estructura Fecha y sus operadores de comparación.
 */

/**
 * @struct Fecha
 * @brief Estructura que representa una fecha del calendario (Año, Mes, Día).
 *
 * Diseñada para almacenar valores numéricos de una fecha y permitir
 * comparaciones cronológicas directas entre instancias utilizando los
 * operadores convencionales (`>`, `<`, `==`).
 */
struct Fecha {
    int anio = 0; /**< Año de la fecha (ej. 2024). */
    int mes = 0;  /**< Mes del año, en rango numérico de 1 a 12. */
    int dia = 0;  /**< Día del mes, en rango numérico de 1 a 31. */

    /**
     * @brief Evalúa si la fecha actual es cronológicamente posterior a otra.
     * @param[in] otra Referencia constante a la fecha contra la que se va a comparar.
     * @return `true` si la fecha actual ocurrió después que `otra`, `false` en caso contrario.
     * * @code
     * Fecha f1{2024, 5, 10};
     * Fecha f2{2023, 12, 25};
     * if (f1 > f2) {
     * // Verdadero: mayo de 2024 es posterior a diciembre de 2023
     * }
     * @endcode
     */
    bool operator>(const Fecha &otra) const;

    /**
     * @brief Evalúa si la fecha actual es cronológicamente anterior a otra.
     * @param[in] otra Referencia constante a la fecha contra la que se va a comparar.
     * @return `true` si la fecha actual ocurrió antes que `otra`, `false` en caso contrario.
     */
    bool operator<(const Fecha &otra) const;

    /**
     * @brief Comprueba si dos fechas corresponden exactamente al mismo día, mes y año.
     * @param[in] otra Referencia constante a la fecha contra la que se va a comparar.
     * @return `true` si año, mes y día coinciden exactamente, `false` de lo contrario.
     */
    bool operator==(const Fecha &otra) const;
};

#endif // FECHA_H