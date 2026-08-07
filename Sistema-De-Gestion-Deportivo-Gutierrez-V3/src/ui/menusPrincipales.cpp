#include "../../include/ui/MenusPrincipales.hpp"

namespace presentacion {
    namespace menu {

        bool datosInicialesTorneo() {
            Torneo verificarTorneo = GestorArchivosBinarios::obtenerInformacionTorneo();

            if (verificarTorneo.getInicializado()) {
                std::cout << "\n Iniciando el torneo " << verificarTorneo.getNombre() << "... \n";
                Formatos::esperarTiempo(1500);
                return true;
            }

            bool cancelado = false;
            bool deporteValido = false;
            bool opcionValida = false;
            Torneo torneo;

            char nombre[constantes::TAMANO_NOMBRE]{};
            char fechaInicio[constantes::TAMANO_FECHA]{};
            char fechaFin[constantes::TAMANO_FECHA]{};
            int idDeporte = 0;
            int opcionFormato = 0;

            Formatos::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!presentacion::funcionesInOut::ingresarCadena(nombre, constantes::TAMANO_NOMBRE, " Nombre del Torneo (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                              validadores::nombreTorneo)) {
                std::cout << "\n Operación Cancelada por el Usuario \n";
                Formatos::esperarTiempo(1000);
                std::cout << "\n Se omite la inicialización del torneo por el momento. \n";
                Formatos::esperarTiempo(1500);
                return false;
            }
            torneo.setNombre(nombre);
            Formatos::esperarTiempo(1500);

            do {
                deporteValido = false;

                Formatos::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                std::cout << " Deportes disponibles:\n";
                std::vector<std::vector<std::string>> listaDeDeportes = GestorArchivosTexto::leerCSV(constantes::RUTA_ARCHIVO_DEPORTES_CONFIG);

                if (listaDeDeportes.empty()) {
                    std::cerr << "\nError del Sistema!\n";
                    Formatos::pausarPrograma();
                    return false;
                }

                for (size_t e = 1; e < listaDeDeportes.size(); ++e) {
                    std::cout << " " << listaDeDeportes[e][0] << " - " << listaDeDeportes[e][1] << std::endl;
                }
                std::cout << std::endl;

                if (!presentacion::funcionesInOut::ingresarDatos(idDeporte, " Ingrese el ID del deporte del torneo (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                                 validadores::idDeporteValido)) {
                    std::cout << "\n Operación Cancelada por el Usuario \n";
                    Formatos::esperarTiempo(1000);
                    std::cout << "\n Se omite la inicialización del torneo por el momento. \n";
                    Formatos::esperarTiempo(1500);
                    return false;
                }

                if (!torneo.setDeporte(idDeporte)) {
                    std::cerr << " Error: El deporte ingresado no es válido.\n";
                    Formatos::esperarTiempo(2000);
                } else {
                    deporteValido = true;
                }

            } while (!deporteValido);

            Formatos::esperarTiempo(1500);

            do {
                opcionValida = true;
                Formatos::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " 1. Formato de Grupos (Todos contra todos)\n";
                std::cout << " 2. Formato de Eliminatoria Directa\n";
                std::cout << "--------------------------------------------------\n";
                if (!presentacion::funcionesInOut::ingresarDatos(opcionFormato, " Seleccione el formato (1 o 2) (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                                 validadores::idFormatoValido)) {
                    std::cout << "\n Operación Cancelada por el Usuario \n";
                    Formatos::esperarTiempo(1000);
                    std::cout << "\n Se omite la inicialización del torneo por el momento. \n";
                    Formatos::esperarTiempo(1500);
                    return false;
                }

                if (opcionFormato != 1 && opcionFormato != 2) {
                    std::cerr << " Opcion invalida. Intente de nuevo.\n";
                    opcionValida = false;
                    Formatos::pausarPrograma();
                } else if (!torneo.setFormato(opcionFormato)) {
                    std::cerr << " Error: El formato ingresado no es válido.\n";
                    opcionValida = false;
                    Formatos::pausarPrograma();
                }
            } while (!opcionValida);

            Formatos::esperarTiempo(1500);

            bool fechaInicioValida = false;
            do {
                Formatos::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!presentacion::funcionesInOut::ingresarCadena(fechaInicio, constantes::TAMANO_FECHA,
                                                                  " Fecha de Inicio del Torneo (YYYY-MM-DD) (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                                  validadores::fechaValida)) {
                    std::cout << "\n Operación Cancelada por el Usuario \n";
                    Formatos::esperarTiempo(1000);
                    std::cout << "\n Se omite la inicialización del torneo por el momento. \n";
                    Formatos::esperarTiempo(1500);
                    return false;
                }

                if (torneo.setFechaInicio(fechaInicio)) {
                    fechaInicioValida = true;
                } else {
                    std::cerr << " Error: La fecha de inicio ingresada es inválida.\n";
                    Formatos::pausarPrograma();
                }
            } while (!fechaInicioValida);
            Formatos::esperarTiempo(1500);

            bool fechaFinValida = false;
            do {
                Formatos::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                if (!presentacion::funcionesInOut::ingresarCadena(fechaFin, constantes::TAMANO_FECHA,
                                                                  " Fecha de Finalización del Torneo (YYYY-MM-DD) (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                                  validadores::fechaValida)) {
                    std::cout << "\n Operación Cancelada por el Usuario \n";
                    Formatos::esperarTiempo(1000);
                    std::cout << "\n Se omite la inicialización del torneo por el momento. \n";
                    Formatos::esperarTiempo(1500);
                    return false;
                }

                if (!torneo.setFechaFin(fechaFin)) {
                    std::cerr << "\n Error: La fecha ingresada debe ser posterior o igual a la fecha de inicio del torneo.\n";
                    Formatos::pausarPrograma();
                } else {
                    fechaFinValida = true;
                }
            } while (!fechaFinValida);

            torneo.setFechaCreacion(std::time(nullptr));
            torneo.setFechaUltimaModificacion(std::time(nullptr));
            torneo.setInicializado(true);

            if (!GestorArchivosBinarios::guardarInformacionTorneo(torneo)) {
                std::cerr << "\nError del Sistema!\n";
                Formatos::pausarPrograma();
                return false;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║       NUEVO TORNEO CREADO CON ÉXITO       ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << " Nombre: " << torneo.getNombre() << std::endl;
            std::cout << " Deporte: " << torneo.getDeporte() << std::endl;
            std::cout << " Formato: " << torneo.getFormato() << std::endl;
            std::cout << " Fecha de inicio del torneo: " << torneo.getFechaInicio() << std::endl;
            std::cout << " Fecha de Finalización del torneo: " << torneo.getFechaFin();

            Formatos::pausarPrograma();
            return true;
        }

        void Principal() {
            Formatos::limpiarPantalla();

            Torneo torneo = GestorArchivosBinarios::obtenerInformacionTorneo();

            if (!torneo.getInicializado()) {
                std::cout << "\n El torneo aún no ha sido inicializado. Iniciándolo ahora... \n";
                Formatos::esperarTiempo(1200);
                if (!datosInicialesTorneo()) {
                    std::cout << "\n No se pudo inicializar el torneo. Regrese más tarde. \n";
                    Formatos::pausarPrograma();
                    return;
                }
                torneo = GestorArchivosBinarios::obtenerInformacionTorneo();
            }

            std::cout << "\n   ╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "   ║                    Sport G&C Tournaments                     ║\n";
            std::cout << "   ║  Torneo:  " << std::left << std::setw(50) << (torneo.getNombre() ? torneo.getNombre() : "Sin definir") << " ║\n";
            std::cout << "   ║  Deporte: " << std::left << std::setw(18) << (torneo.getDeporte() ? torneo.getDeporte() : "Sin definir") << " | Formato: " << std::left
                      << std::setw(21) << (torneo.getFormato() ? torneo.getFormato() : "Sin definir") << "║\n";
            std::cout << "   ╠══════════════════════════════════════════════════════════════╣\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "1. Gestión de Equipos" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "2. Gestión de Jugadores" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "3. Gestión de Partidos" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "4. Reportes" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "5. Mantenimiento" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "0. Salir" << "║\n";
            std::cout << "   ╚══════════════════════════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void GestionDeEquipos() {
            Formatos::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║          GESTIÓN DE EQUIPOS               ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Registrar equipo                      ║\n";
            std::cout << "   ║  2. Buscar equipo                         ║\n";
            std::cout << "   ║  3. Actualizar equipo                     ║\n";
            std::cout << "   ║  4. Listar equipos                        ║\n";
            std::cout << "   ║  5. Eliminar equipo                       ║\n";
            std::cout << "   ║  0. Volver al menú principal              ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void menuBuscarEquipo() {
            Formatos::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║          BUSQUEDA DE EQUIPOS              ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Buscar equipo por ID                  ║\n";
            std::cout << "   ║  2. Buscar equipo por nombre              ║\n";
            std::cout << "   ║  0. Volver al menú anterior               ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void menuBuscarJugador() {
            Formatos::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║          BUSQUEDA DE JUGADORES            ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Buscar jugador por ID                 ║\n";
            std::cout << "   ║  2. Buscar jugador por nombre             ║\n";
            std::cout << "   ║  0. Volver al menú anterior               ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void listarPartidos() {
            Formatos::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║              LISTAR PARTIDOS              ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Listar todos los partidos             ║\n";
            std::cout << "   ║  2. Listar partidos por su estado         ║\n";
            std::cout << "   ║  0. Volver al menú anterior               ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void buscarPartidos() {
            Formatos::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║          BUSQUEDA DE PARTIDOS             ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Buscar partido por equipo             ║\n";
            std::cout << "   ║  2. Buscar partido por ID                 ║\n";
            std::cout << "   ║  0. Volver al menú anterior               ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void GestionDePartidos() {
            Formatos::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║        GESTIÓN DE PARTIDOS                ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Programar partido                     ║\n";
            std::cout << "   ║  2. Registrar resultado                   ║\n";
            std::cout << "   ║  3. Buscar partido                        ║\n";
            std::cout << "   ║  4. Listar partidos                       ║\n";
            std::cout << "   ║  5. Cancelar partido                      ║\n";
            std::cout << "   ║  0. Volver al menú principal              ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void GestionDeJugadores() {
            Formatos::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║        GESTIÓN DE JUGADORES               ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Registrar jugador                     ║\n";
            std::cout << "   ║  2. Buscar jugador                        ║\n";
            std::cout << "   ║  3. Actualizar jugador                    ║\n";
            std::cout << "   ║  4. Listar jugadores (todos)              ║\n";
            std::cout << "   ║  5. Listar jugadores por equipo           ║\n";
            std::cout << "   ║  6. Eliminar jugador                      ║\n";
            std::cout << "   ║  0. Volver al menú principal              ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void Reportes() {
            std::cout << " ╔═══════════════════════════════════════════╗\n";
            std::cout << " ║              REPORTES                     ║\n";
            std::cout << " ╠═══════════════════════════════════════════╣\n";
            std::cout << " ║  1. Tabla de posiciones                   ║\n";
            std::cout << " ║  2. Tabla de goleadores (Top 10)          ║\n";
            std::cout << " ║  3. Tabla de tarjetas amarillas (Top 10)  ║\n";
            std::cout << " ║  4. Tabla de tarjetas rojas (Top 10)      ║\n";
            std::cout << " ║  5. Ficha técnica de partido              ║\n";
            std::cout << " ║  0. Volver al menú principal              ║\n";
            std::cout << " ╚═══════════════════════════════════════════╝\n";
        }

        void Mantenimiento() {
            std::cout << " ╔═══════════════════════════════════════════╗\n";
            std::cout << " ║           MANTENIMIENTO                   ║\n";
            std::cout << " ╠═══════════════════════════════════════════╣\n";
            std::cout << " ║  1. Verificar integridad referencial      ║\n";
            std::cout << " ║  2. Crear backup de datos                 ║\n";
            std::cout << " ║  0. Volver                                ║\n";
            std::cout << " ╚═══════════════════════════════════════════╝\n";
        }

        bool crearCarpeta(fs::path rutaDatos, std::string nombre) {
            Formatos::limpiarPantalla();

            try {
                if (fs::create_directories(rutaDatos)) {
                    std::cout << "\n Carpeta '" << nombre << "' creada en: " << rutaDatos << std::endl;
                    return true;
                } else {
                    std::cout << "\n La carpeta '" << nombre << "' ya existe. No se realizaron cambios." << std::endl;
                    return true;
                }
            } catch (const fs::filesystem_error &e) {
                std::cerr << "\n Error de permisos o sistema: " << e.what() << std::endl;
                return false;
            }
        }

        void mensajeSalida() {
            Formatos::limpiarPantalla();
            std::cout << "\n Saliendo... \n";
            Formatos::esperarTiempo(2500);
        }

        void mensajeMenuPrincipal() {
            Formatos::limpiarPantalla();
            std::cout << "\n Volviendo al Menú Principal... \n";
            Formatos::esperarTiempo(2500);
        }

        void mensajeMenuAnterior() {
            Formatos::limpiarPantalla();
            std::cout << "\n Volviendo al menu anterior... \n";
            Formatos::esperarTiempo(2500);
        }

        void mensajeDefault() {
            Formatos::limpiarPantalla();
            std::cout << "\n Error: Has ingresado una opcion inválida. Intentalo nuevamente.\n";
            Formatos::esperarTiempo(2500);
        }

    } // namespace menu
} // namespace presentacion