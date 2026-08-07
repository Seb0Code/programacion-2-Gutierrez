#ifndef MENUS_JUGADORES_HPP
#define MENUS_JUGADORES_HPP

#include "../manager/operacionesJugadores.hpp"
#include "../models/equipos.hpp"
#include "../models/jugador.hpp"
#include "../persistence/gestorArchivosBinarios.hpp"
#include "../persistence/gestorArchivosTexto.hpp"
#include "../utils/validaciones.hpp"
#include "funcionesInOut.hpp"
#include <cstring>
#include <iomanip>
#include <vector>

namespace jugadores {

    inline void mensajeDefault();

    inline std::vector<std::string> obtenerPosicionesDisponiblesParaDeporte(const char *deporte);

    void registrarJugador();

    void buscarJugadorPorId();

    void buscarJugadorPorNombre();

    void mostrarJugadoresPorEquipo();

    void mostrarListaDeJugadores();

    void actualizarJugador();

    void eliminarJugador();
} // namespace jugadores

#endif