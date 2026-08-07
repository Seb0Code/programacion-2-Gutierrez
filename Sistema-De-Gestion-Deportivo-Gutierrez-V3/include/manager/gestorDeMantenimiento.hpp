#ifndef GESTOR_MANTENIMIENTO_HPP
#define GESTOR_MANTENIMIENTO_HPP


#include "../../include/ui/MenusPrincipales.hpp"
#include "../../include/ui/funcionesInOut.hpp"
#include "../models/archivoHeader.hpp"
#include "../models/equipos.hpp"
#include "../models/informeIntegridad.hpp"
#include "../models/jugador.hpp"
#include "../models/partidos.hpp"
#include "../models/referenciaRota.hpp"
#include "../persistence/gestorArchivosBinarios.hpp"
#include "../utils/formatos.hpp"
#include <iomanip>
#include <iostream>

namespace GestorMantenimiento {

    bool copiarArchivoBinario(const char *rutaOriginal, const char *rutaDestino);

    // Lo coloqué bool para que retornemos false si ocurrió un error
    bool verificarIntegridadReferencial(InformeIntegridad &informe);

    bool crearBackup(std::string &nombreCarpeta);
} // namespace GestorMantenimiento

#endif