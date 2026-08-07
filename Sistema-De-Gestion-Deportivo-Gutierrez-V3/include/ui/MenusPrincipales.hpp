#ifndef MENUS_PRINCIPALES_HPP
#define MENUS_PRINCIPALES_HPP

#include "../../include/manager/operacionesTorneo.hpp"
#include "../../include/models/torneo.hpp"
#include "../../include/persistence/gestorArchivosBinarios.hpp"
#include "../../include/persistence/gestorArchivosTexto.hpp"
#include "../../include/utils/constantes.hpp"
#include "../../include/utils/formatos.hpp"
#include "../../include/utils/validaciones.hpp"
#include "funcionesInOut.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace presentacion {
    namespace menu {

        namespace validadores {
            inline bool nombreTorneo(const char *texto) { return GestorDeValidaciones::validarNombreTorneo(texto); }

            inline bool idDeporteValido(const int valor) { return GestorDeValidaciones::validarId(valor); }

            inline bool idFormatoValido(const int valor) { return GestorDeValidaciones::validarId(valor); }

            inline bool fechaValida(const char *texto) { return GestorDeValidaciones::validarFecha(texto); }

            inline bool fechaFinValida(const char *fechaFin, const char *fechaInicio) { return GestorDeValidaciones::validarFechaFin(fechaFin, fechaInicio); }
        } // namespace validadores

        bool datosInicialesTorneo();

        void Principal();
        void GestionDeEquipos();
        void menuBuscarEquipo();
        void menuBuscarJugador();
        void listarPartidos();
        void buscarPartidos();
        void GestionDePartidos();
        void GestionDeJugadores();
        void Reportes();
        void Mantenimiento();
        bool crearCarpeta(fs::path rutaDatos, std::string nombre);
        void mensajeSalida();
        void mensajeMenuPrincipal();
        void mensajeMenuAnterior();
        void mensajeDefault();

    } // namespace menu
} // namespace presentacion

#endif
