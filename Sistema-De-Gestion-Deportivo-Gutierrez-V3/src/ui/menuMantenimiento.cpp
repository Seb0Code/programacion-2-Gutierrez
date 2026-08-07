#include "../../include/manager/gestorDeMantenimiento.hpp"
#include "../../include/ui/MenusMantenimiento.hpp"
#include <filesystem>

namespace fs = std::filesystem;

void MenuMantenimiento::verificarIntegridadReferencial() {
    Formatos::limpiarPantalla();
    std::cout << "\n Verificando integridad referencial...\n";
    Formatos::esperarTiempo(1000);

    InformeIntegridad informe;

    // Si no
    if (!GestorMantenimiento::verificarIntegridadReferencial(informe)) {
        std::cerr << "\n Error del Sistema!\n";
    }

    Formatos::limpiarPantalla();

    std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║          INFORME DE INTEGRIDAD REFERENCIAL            ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════╣\n";
    std::cout << "║  Equipos verificados   : " << std::left << std::setw(29) << informe.getEquiposVerificados() << "║\n";
    std::cout << "║  Jugadores verificados : " << std::left << std::setw(29) << informe.getJugadoresVerificados() << "║\n";
    std::cout << "║  Partidos verificados  : " << std::left << std::setw(29) << informe.getPartidosVerificados() << "║\n";
    std::cout << "║                                                       ║\n";
    std::cout << "║  Referencias rotas encontradas: " << std::left << std::setw(22) << informe.getTotalDeReferenciasRotas() << "║\n";

    if (informe.getTotalDeReferenciasRotas() == 0) {
        std::cout << "║  Estado: OK - SISTEMA INTEGRO                         ║\n";
    } else {
        std::cout << "║  Estado: ERROR - SE ENCONTRARON REFERENCIAS ROTAS     ║\n";
    }
    std::cout << "╚═══════════════════════════════════════════════════════╝\n\n";

    for (size_t e = 0; e < informe.getTotalDeReferenciasRotas(); e++) {
        std::cout << " ROTO: " << informe.getReferenciasRotas()[e].getTipoDeReferencia() << " ID " << informe.getReferenciasRotas()[e].getIdOrigen() << " apunta a ID "
                  << informe.getReferenciasRotas()[e].getIdReferenciaRota() << " (no existe)\n";
    }

    Formatos::pausarPrograma();
}

void MenuMantenimiento::crearBackups(const char *nombreArchivo) {
    (void)nombreArchivo;

    Formatos::limpiarPantalla();

    bool backupCreado = false;
    char confirmacion = '\0';

    std::cout << "\n   ╔════════════════════════════════════════════╗\n";
    std::cout << "   ║           CREAR BACKUP DE DATOS           ║\n";
    std::cout << "   ╚════════════════════════════════════════════╝\n\n";

    if (!presentacion::funcionesInOut::ingresarDatos(confirmacion, " ¿Desea crear un backup ahora? (S/N): ")) {
        std::cout << "\n Creación de Backup cancelada por el usuario.\n";
        Formatos::pausarPrograma();
        return;
    }

    Formatos::esperarTiempo(750);
    Formatos::limpiarPantalla();

    if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
        try {
            fs::path rutaBase = constantes::RUTA_BACKUPS;
            if (!fs::exists(rutaBase)) {
                fs::create_directories(rutaBase);
            }

            std::string nombreCarpeta, rutaBackup;
            backupCreado = GestorMantenimiento::crearBackup(nombreCarpeta);
            rutaBackup = constantes::RUTA_BACKUPS + nombreCarpeta;

            if (backupCreado) {
                std::cout << "\n Respaldo creado con éxito.\n";
                std::cout << " Backup guardado en: " << rutaBackup << std::endl;
            } else {
                std::cerr << " Error: Se produjo un error al crear el backup.\n";
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << " Error del sistema al preparar la carpeta de backup: " << e.what() << "\n";
        }
    } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
        std::cout << "\n Creación de Backup cancelada.\n";
    } else {
        std::cerr << "\n Error: Opción inválida (S/N).\n Creación de Backup cancelada.\n";
    }
    Formatos::pausarPrograma();
}
