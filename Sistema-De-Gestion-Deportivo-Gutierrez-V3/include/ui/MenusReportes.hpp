#ifndef MENU_REPORTES
#define MENU_REPORTES

#include "../../include/ui/funcionesInOut.hpp"
#include "../manager/gestorDeReportes.hpp"
#include "../models/archivoHeader.hpp"
#include "../models/equipos.hpp"
#include "../models/jugador.hpp"
#include "../models/partidos.hpp"
#include "../persistence/gestorArchivosBinarios.hpp"
#include <iomanip>

namespace MenuReportes {

    void tablaTop10Anotadores();

    void tablaTop10TarjetasAmarillas();

    void tablaTop10TarjetasRojas();

    void fichaTecnica();
} // namespace MenuReportes

#endif