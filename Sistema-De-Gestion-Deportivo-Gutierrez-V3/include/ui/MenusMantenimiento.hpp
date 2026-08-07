#ifndef MENUS_MANTENIMIENTO_HPP
#define MENUS_MANTENIMIENTO_HPP

#include "../manager/gestorDeMantenimiento.hpp"
#include "../models/informeIntegridad.hpp"
#include "../models/referenciaRota.hpp"
#include "../utils/formatos.hpp"
#include "MenusPrincipales.hpp"
#include "funcionesInOut.hpp"

namespace MenuMantenimiento {

    void verificarIntegridadReferencial();

    void crearBackups(const char *nombreArchivo);
} // namespace MenuMantenimiento

#endif