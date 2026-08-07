#ifndef MENUS_PARTIDOS_HPP
#define MENUS_PARTIDOS_HPP

#include "../manager/operacionesEquipos.hpp"
#include "../manager/operacionesJugadores.hpp"
#include "../manager/operacionesPartidos.hpp"
#include "../models/equipos.hpp"
#include "../models/jugador.hpp"
#include "../models/partidos.hpp"
#include "../persistence/gestorArchivosBinarios.hpp"
#include "../utils/validaciones.hpp"
#include "funcionesInOut.hpp"
#include <cstring>
#include <iomanip>
#include <vector>

namespace partidos {

    inline void mensajeDefault();

    // ---------------------------------------------------------------------------------------------- //
    //   PROGRAMAR PARTIDO
    // ---------------------------------------------------------------------------------------------- //
    void programarPartido();

    void registrarResultado();

    void buscarPartidoPorId();

    void listarTodosLosPartidos();

    void buscarPartidosPorEquipo();

    void listarPartidosPorEstado();

    void cancelarPartido();
} // namespace partidos

#endif // MENUS_PARTIDOS_HPP