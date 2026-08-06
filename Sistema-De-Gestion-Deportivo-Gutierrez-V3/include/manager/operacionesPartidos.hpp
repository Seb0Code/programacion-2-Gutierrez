#ifndef OPERACIONES_PARTIDOS_HPP
#define OPERACIONES_PARTIDOS_HPP

#include "../models/partidos.hpp"
#include "../models/torneo.hpp"
#include "../persistence/gestorArchivosBinarios.hpp"
#include "../utils/constantes.hpp"
#include <string>
#include <vector>

class OperacionesPartidos {
  private:
    OperacionesPartidos() = delete;

  public:
    static std::string obtenerEstadoPorId(const int idEstado);

    static int obtenerMinJugadoresPorDeporte();

    static bool hayPartidoProgramadoEntre2(const Partido &nuevoPartido);

    static std::vector<Partido> listarPartidosPorSuEstado(const char *estado);

    static bool programarPartido(Partido &nuevoPartido);

    static bool registrarResultado(Partido registroPartido);

    static std::vector<Partido> listarPartidosPorEquipo(const int idEquipo);

    static bool cancelarPartido(const int idPartido);
};

#endif