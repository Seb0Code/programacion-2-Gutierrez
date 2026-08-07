#ifndef GESTOR_REPORTES
#define GESTOR_REPORTES

#include "../models/jugador.hpp"
#include "../persistence/gestorArchivosBinarios.hpp"

namespace GestorReportes {

    std::vector<Jugador> tablaDeGoleadoresTop10();

    std::vector<Jugador> tablaDeTarjetasAmarillasTop10();

    std::vector<Jugador> tablaDeTarjetasRojasTop10();

} // namespace GestorReportes

#endif