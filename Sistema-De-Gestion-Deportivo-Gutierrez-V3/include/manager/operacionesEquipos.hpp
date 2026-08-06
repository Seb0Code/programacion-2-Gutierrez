#ifndef OPERACIONES_EQUIPOS_HPP
#define OPERACIONES_EQUIPOS_HPP
#include "../../include/models/partidos.hpp"
#include "../../include/persistence/gestorArchivosBinarios.hpp"
#include "../models/equipos.hpp"
#include "../utils/formatos.hpp"
#include <vector>

class OperacionesEquipos {

  private:
    OperacionesEquipos() = default;

    // Comprueba si existe una cadena duplicada en un miembro texto del modelo Equipo.
    // Usamos puntero a método getter (`const char * (Equipo::*)() const`) porque los campos
    // de `Equipo` son arreglos `char[]` y existen getters públicos que devuelven `const char*`.
    static bool cadenaDuplicada(const char *cadena, const char *(Equipo::*getter)() const, bool actualizar = false, int idRegistro = -1) {

        std::vector<Equipo> listaDeEquipos = GestorArchivosBinarios::listarRegistros<Equipo>(constantes::NOMBRE_ARCHIVO_EQUIPOS);

        if (listaDeEquipos.empty()) {
            return false;
        }

        std::string cadenaBuscada = cadena ? cadena : std::string();
        Formatos::convertirTextoAMinus(cadenaBuscada);

        for (const auto &eqAux : listaDeEquipos) {
            if (eqAux.getEliminado()) {
                continue;
            }

            if (actualizar && eqAux.getId() == idRegistro) {
                continue;
            }

            const char *miembroCStr = (eqAux.*getter)();
            std::string miembroStr = miembroCStr ? miembroCStr : std::string();
            Formatos::convertirTextoAMinus(miembroStr);

            if (cadenaBuscada == miembroStr) {
                return true;
            }
        }

        return false;
    }

  public:
    static bool registrarEquipo(Equipo &nuevoEquipo);

    static std::vector<Equipo> tablaDePosiciones();

    static bool actualizarEquipo(const int id, const char *nombre, const char *entrenador, const char *ciudad);

    static bool eliminarEquipo(const int id);

    static bool modificarCantidadDeJugadores(const int idEquipo, const bool disminuir = false);

    // -1 = derrota, 0 = empate, 1 = victoria
    static bool modificarEstadisticas(const int idEquipo, const int resultadoParaEquipo, const bool permiteEmpate, const bool revertir = false);

    static bool modificarAnotaciones(const Partido registroPartido, const bool revertir = false);
};

#endif