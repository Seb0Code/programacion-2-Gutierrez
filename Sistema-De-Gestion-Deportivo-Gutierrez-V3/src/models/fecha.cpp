#include "../../include/models/fecha.hpp"
#include <tuple>

/**
 * @brief Sobrecarga del operador mayor que (`>`).
 *
 * Utiliza `std::tie` para empaquetar las variables (`anio`, `mes`, `dia`) en tuplas
 * de referencias temporales. Compara lexicográficamente en el siguiente orden de prioridad:
 * 1. Año
 * 2. Mes
 * 3. Día
 *
 * @param[in] otra Fecha contra la que se evalúa la comparación.
 * @return `true` si la fecha actual es cronológicamente mayor/posterior que `otra`, `false` en caso contrario.
 */
bool Fecha::operator>(const Fecha &otra) const { return std::tie(anio, mes, dia) > std::tie(otra.anio, otra.mes, otra.dia); }

bool Fecha::operator<(const Fecha &otra) const { return std::tie(anio, mes, dia) < std::tie(otra.anio, otra.mes, otra.dia); }

bool Fecha::operator==(const Fecha &otra) const { return std::tie(anio, mes, dia) == std::tie(otra.anio, otra.mes, otra.dia); }

// * MIS NOTAS

// std::tie enmpaqueta las fechas en una especie de contenedor por secciones en donde va comparando la primera seccion de cada
// contenedor luego la segunda y asi, y si encuentra que una es mayor que orta devuelve true
// si encuentra que son iguales pasa a comparar la siguiente seccion de contenedor en este caso si
/// encuentra que los años son iguales compara los meses luego dias y asi sucesivamente
// si encuentra que uno es menor que el otrodevuelve false
// esto es una formad e enseñarle al compilador a comparar un tipo de dato diferente a los primitivos, en este
// caso un tipo de dato fecha, que nos facilitara el trabajo a la hora de comprobar si una fecha es
// mayor o menor
// esto se conoce como sobrecarga de operadores y es parecido a la sobrecarga de funciones