#include "../include/persistence/gestorArchivosBinarios.hpp"
#include "../include/ui/MenusEquipos.hpp"
#include "../include/ui/MenusJugadores.hpp"
#include "../include/ui/MenusMantenimiento.hpp"
#include "../include/ui/MenusPartidos.hpp"
#include "../include/ui/MenusPrincipales.hpp"
#include "../include/ui/MenusReportes.hpp"
#include "../include/ui/funcionesInOut.hpp"
#include "../include/utils/constantes.hpp"
#include "../include/utils/formatos.hpp"
#include <cctype>
#include <iostream>

int main() {

    Formatos::configurarIdioma();

    std::cout << "Inicializando sistema de torneo...\n";
    if (!GestorArchivosBinarios::inicializarSistemaArchivos()) {
        std::cerr << "No fue posible inicializar los archivos del sistema.\n";
        return 1;
    }

    std::cout << "Sistema listo. Archivos binarios inicializados correctamente.\n";
    Formatos::esperarTiempo(1200);

    presentacion::menu::datosInicialesTorneo();

    int opcionMenu = -1;
    int opcionSubMenu = -1;
    int opcionMenuBusq = -1;
    int opcionMenuListar = -1;
    char confirmacion = '\0';

    do {
        opcionMenu = -1;
        opcionSubMenu = -1;
        opcionMenuBusq = -1;
        opcionMenuListar = -1;

        presentacion::menu::Principal();
        presentacion::funcionesInOut::ingresarDatos(opcionMenu, " Seleccione una opcion: ");
        Formatos::limpiarPantalla();
        Formatos::esperarTiempo(800);

        switch (opcionMenu) {
            case 0:
                Formatos::limpiarPantalla();
                Formatos::esperarTiempo(500);
                presentacion::funcionesInOut::ingresarDatos(confirmacion, " ¿Está seguro de que desea salir del programa? (S/N): ");

                if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                    presentacion::menu::mensajeSalida();
                } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                    std::cout << " Salida del Programa Cancelada\n";
                    Formatos::esperarTiempo(2000);
                    Formatos::limpiarPantalla();
                    opcionMenu = -1;
                } else {
                    std::cerr << " Error: No se ingresó una opción correcta (S/N) \n";
                    std::cout << " Salida del Programa Cancelada \n";
                    Formatos::esperarTiempo(2000);
                    opcionMenu = -1;
                }
                break;

            case 1:
                Formatos::esperarTiempo(500);
                Formatos::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Gestión de Equipos... \n";
                Formatos::esperarTiempo(1200);

                do {
                    Formatos::limpiarPantalla();
                    opcionSubMenu = -1;
                    opcionMenuBusq = -1;
                    presentacion::menu::GestionDeEquipos();
                    presentacion::funcionesInOut::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ");

                    switch (opcionSubMenu) {
                        case 0:
                            presentacion::menu::mensajeMenuPrincipal();
                            break;
                        case 1:
                            equipos::registrarEquipo();
                            break;
                        case 2:
                            do {
                                Formatos::esperarTiempo(800);
                                opcionMenuBusq = -1;
                                presentacion::menu::menuBuscarEquipo();
                                presentacion::funcionesInOut::ingresarDatos(opcionMenuBusq, " Seleccione una opcion: ");

                                switch (opcionMenuBusq) {
                                    case 0:
                                        presentacion::menu::mensajeMenuAnterior();
                                        break;
                                    case 1:
                                        equipos::buscarEquipoPorId();
                                        break;
                                    case 2:
                                        equipos::buscarEquiposPorNombre();
                                        break;
                                    default:
                                        presentacion::menu::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);
                            break;
                        case 3:
                            equipos::actualizarEquipo();
                            break;
                        case 4:
                            equipos::listarEquipos();
                            break;
                        case 5:
                            equipos::eliminarEquipo();
                            break;
                        default:
                            presentacion::menu::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);
                break;

            case 2:
                Formatos::esperarTiempo(500);
                Formatos::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Gestión de Jugadores... \n";
                Formatos::esperarTiempo(1200);

                do {
                    Formatos::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::GestionDeJugadores();
                    presentacion::funcionesInOut::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ");

                    switch (opcionSubMenu) {
                        case 0:
                            presentacion::menu::mensajeMenuPrincipal();
                            break;
                        case 1:
                            jugadores::registrarJugador();
                            break;
                        case 2:
                            do {
                                opcionMenuBusq = -1;
                                presentacion::menu::menuBuscarJugador();
                                presentacion::funcionesInOut::ingresarDatos(opcionMenuBusq, " Seleccione una opcion: ");

                                switch (opcionMenuBusq) {
                                    case 0:
                                        presentacion::menu::mensajeMenuAnterior();
                                        break;
                                    case 1:
                                        jugadores::buscarJugadorPorId();
                                        break;
                                    case 2:
                                        jugadores::buscarJugadorPorNombre();
                                        break;
                                    default:
                                        presentacion::menu::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);
                            break;
                        case 3:
                            jugadores::actualizarJugador();
                            break;
                        case 4:
                            jugadores::mostrarListaDeJugadores();
                            break;
                        case 5:
                            jugadores::mostrarJugadoresPorEquipo();
                            break;
                        case 6:
                            jugadores::eliminarJugador();
                            break;
                        default:
                            presentacion::menu::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);
                break;

            case 3:
                Formatos::esperarTiempo(500);
                Formatos::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Gestión de Partidos... \n";
                Formatos::esperarTiempo(1200);

                do {
                    Formatos::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::GestionDePartidos();
                    presentacion::funcionesInOut::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ");

                    switch (opcionSubMenu) {
                        case 0:
                            presentacion::menu::mensajeMenuPrincipal();
                            break;
                        case 1:
                            partidos::programarPartido();
                            break;
                        case 2:
                            partidos::registrarResultado();
                            break;
                        case 3:
                            do {
                                opcionMenuBusq = -1;
                                presentacion::menu::buscarPartidos();
                                presentacion::funcionesInOut::ingresarDatos(opcionMenuBusq, " Seleccione una opcion: ");

                                switch (opcionMenuBusq) {
                                    case 0:
                                        presentacion::menu::mensajeMenuAnterior();
                                        break;
                                    case 1:
                                        partidos::buscarPartidosPorEquipo();
                                        break;
                                    case 2:
                                        partidos::buscarPartidoPorId();
                                        break;
                                    default:
                                        presentacion::menu::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);
                            break;
                        case 4:
                            do {
                                opcionMenuListar = -1;
                                presentacion::menu::listarPartidos();
                                presentacion::funcionesInOut::ingresarDatos(opcionMenuListar, " Seleccione una opcion: ");

                                switch (opcionMenuListar) {
                                    case 0:
                                        presentacion::menu::mensajeMenuAnterior();
                                        break;
                                    case 1:
                                        partidos::listarTodosLosPartidos();
                                        break;
                                    case 2:
                                        partidos::listarPartidosPorEstado();
                                        break;
                                    default:
                                        presentacion::menu::mensajeDefault();
                                }
                            } while (opcionMenuListar != 0);
                            break;
                        case 5:
                            partidos::cancelarPartido();
                            break;
                        default:
                            presentacion::menu::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);
                break;

            case 4:
                Formatos::esperarTiempo(500);
                Formatos::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Reportes... \n";
                Formatos::esperarTiempo(1200);

                do {
                    Formatos::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::Reportes();
                    presentacion::funcionesInOut::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ");

                    switch (opcionSubMenu) {
                        case 0:
                            presentacion::menu::mensajeMenuPrincipal();
                            break;
                        case 1:
                            equipos::mostrarTablaDePosiciones();
                            break;
                        case 2:
                            MenuReportes::tablaTop10Anotadores();
                            break;
                        case 3:
                            MenuReportes::tablaTop10TarjetasAmarillas();
                            break;
                        case 4:
                            MenuReportes::tablaTop10TarjetasRojas();
                            break;
                        case 5:
                            MenuReportes::fichaTecnica();
                            break;
                        default:
                            presentacion::menu::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);
                break;

            case 5:
                Formatos::esperarTiempo(500);
                Formatos::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Mantenimiento... \n";
                Formatos::esperarTiempo(1200);

                do {
                    Formatos::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::Mantenimiento();
                    presentacion::funcionesInOut::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ");

                    switch (opcionSubMenu) {
                        case 0:
                            presentacion::menu::mensajeMenuPrincipal();
                            break;
                        case 1:
                            MenuMantenimiento::verificarIntegridadReferencial();
                            break;
                        case 2:
                            MenuMantenimiento::crearBackups(constantes::RUTA_BACKUPS.c_str());
                            break;
                        default:
                            presentacion::menu::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);
                break;

            default:
                presentacion::menu::mensajeDefault();
        }
    } while (opcionMenu != 0);

    return 0;
}
