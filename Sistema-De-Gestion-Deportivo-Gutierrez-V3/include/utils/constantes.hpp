#ifndef CONSTANTES_HPP
#define CONSTANTES_HPP

#include <filesystem>
#include <string>

// Definicion de Variables globales constantes
namespace constantes {

    inline const std::filesystem::path PROYECTO_ROOT = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path();

    // Devolucion de Error en funciones de tipo int
    const int ERROR_INT = -1;

    // Devolucion de error en funciones tipo std::string
    inline const std::string ERROR_STRING = "ERROR";

    // Tamaños de Cadenas de Texto
    constexpr int TAMANO_MENSAJE_ERROR = 150;
    constexpr int TAMANO_NOMBRE = 100;
    constexpr int TAMANO_FECHA = 11;
    constexpr int TAMANO_DEPORTE = 50;
    constexpr int TAMANO_CEDULA = 20;
    constexpr int TAMANO_POSICION = 25;
    constexpr int TAMANO_ESTADO = 12;
    constexpr int TAMANO_FORMATO = 20;
    constexpr int TAMANO_DESCRIPCION = 200;
    constexpr int TAMANO_LOCAL_O_VISITANTE = 12;
    constexpr int TAMANO_TIPO_REFERNCIA = 20;

    // Rutas de Archivos Principales
    inline const std::string RUTA_DATOS_CARPETA = (PROYECTO_ROOT / "datos").string() + "/";
    inline const std::string NOMBRE_ARCHIVO_TORNEO = (PROYECTO_ROOT / "datos" / "torneo.bin").string();
    inline const std::string NOMBRE_ARCHIVO_JUGADORES = (PROYECTO_ROOT / "datos" / "jugadores.bin").string();
    inline const std::string NOMBRE_ARCHIVO_EQUIPOS = (PROYECTO_ROOT / "datos" / "equipos.bin").string();
    inline const std::string NOMBRE_ARCHIVO_PARTIDOS = (PROYECTO_ROOT / "datos" / "partidos.bin").string();
    inline const std::string RUTA_ARCHIVO_DEPORTES_CONFIG = (PROYECTO_ROOT / "config" / "deportes_config.csv").string();
    inline const std::string RUTA_ARCHIVO_POSICIONES_DEPORTES = (PROYECTO_ROOT / "config" / "posiciones_deportes.csv").string();
    inline const std::string RUTA_ARCHIVO_FORMATOS_CONFIG = (PROYECTO_ROOT / "config" / "formatos_config.csv").string();
    inline const std::string RUTA_ARCHIVO_ESTADO_PARTIDOS_CONFIG = (PROYECTO_ROOT / "config" / "estado_partidos_config.csv").string();

    // Rutas de Backups
    inline const std::string RUTA_BACKUPS = (PROYECTO_ROOT / "datos" / "backups").string() + "/";
    inline const std::string NOMBRE_BACKUPS_TORNEO = (PROYECTO_ROOT / "datos" / "backups" / "torneo.bin").string();
    inline const std::string NOMBRE_BACKUPS_JUGADORES = (PROYECTO_ROOT / "datos" / "backups" / "jugadores.bin").string();
    inline const std::string NOMBRE_BACKUPS_EQUIPOS = (PROYECTO_ROOT / "datos" / "backups" / "equipos.bin").string();
    inline const std::string NOMBRE_BACKUPS_PARTIDOS = (PROYECTO_ROOT / "datos" / "backups" / "partidos.bin").string();

    // Límites de Valores
    constexpr int MAX_RESULTADOS = 100;
    constexpr int MAX_ANOTACIONES = 22;
    constexpr int MAX_TARJETAS_AMARILLAS = 30;
    constexpr int MAX_TARJETAS_ROJAS = 8;
    constexpr int MINUTO_MINIMO = 1;
    constexpr int MINUTO_MAXIMO = 120;
    constexpr int MAX_PARTIDOS_EQUIPOS = 50;

} // namespace constantes

#endif // CONSTANTES_HPP
