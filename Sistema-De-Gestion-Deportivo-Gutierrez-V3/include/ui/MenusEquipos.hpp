#ifndef MENUS_EQUIPOS_HPP
#define MENUS_EQUIPOS_HPP

#include "../manager/operacionesEquipos.hpp"
#include "../models/equipos.hpp"
#include "../persistence/gestorArchivosBinarios.hpp"
#include "../utils/validaciones.hpp"
#include "funcionesInOut.hpp"
#include <cstring>
#include <iomanip>
#include <vector>

namespace equipos {

    inline void mensajeDefault();

    // ---------------------------------------------------------------------------------------------- //
    //   REGISTRAR EQUIPO
    // ---------------------------------------------------------------------------------------------- //
    void registrarEquipo();

    void buscarEquipoPorId();

    void buscarEquiposPorNombre();

    void listarEquipos();

    void mostrarTablaDePosiciones();

    void actualizarEquipo();

    void eliminarEquipo();
} // namespace equipos

#endif // MENUS_EQUIPOS_HPP