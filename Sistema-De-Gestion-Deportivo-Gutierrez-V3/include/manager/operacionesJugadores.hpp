#ifndef OPERACIONES_JUGADORES_HPP
#define OPERACIONES_JUGADORES_HPP

#include "../manager/operacionesEquipos.hpp"
#include "../models/equipos.hpp"
#include "../models/jugador.hpp"
#include "../persistence/gestorArchivosBinarios.hpp"
#include "../utils/constantes.hpp"
#include <vector>

class OperacionesJugadores {

  private:
    OperacionesJugadores() = delete;

    static bool cadenaDuplicada(const char *cadena, const char *(Jugador::*getter)() const, bool actualizar = false, int idRegistro = -1);

  public:
    static bool esDorsalDuplicado(const int dorsal, const int idEquipo, bool actualizar = false, int idRegistro = -1);

    static bool registrarJugador(Jugador &nuevoJugador);

    static std::vector<Jugador> listarJugadoresPorEquipo(const int idEquipo);

    static bool actualizarJugador(const int id, const char *nombre, const int edad, const int numeroDorsal, const char *posicion, const char *cedula);

    static bool eliminarJugador(const int id);

    static bool modificarAnotaciones(Jugador &jugadorAuxiliar, const bool disminuir = false);

    static bool modificarTarjetasAma(Jugador &jugadorAuxiliar, const bool disminuir = false);

    static bool modificarTarjetasRojas(Jugador &jugadorAuxiliar, const bool disminuir = false);

    static bool modificarEstadisticas(Partido &registroPartido, const bool disminuir = false);
};

#endif