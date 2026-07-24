#ifndef CONSTANTES_HPP
#define CONSTANTES_HPP

#include <string>

// Definicion de Variables globales constantes
namespace constantes {

    // Devolucion de Error en funciones de tipo int
    const int MI_ERROR = -1;

    // Tamaños de Cadenas de Texto
    const int TAMANO_MENSAJE_ERROR = 150;
    constexpr int TAMANO_NOMBRE = 100;
    constexpr int TAMANO_FECHA = 11;
    constexpr int TAMANO_DEPORTE = 50;
    constexpr int TAMANO_CEDULA = 20;
    constexpr int TAMANO_POSICION = 25;
    constexpr int TAMANO_ESTADO = 12;
    constexpr int TAMANO_FORMATO = 20;
    constexpr int TAMANO_DESCRIPCION = 200;
    constexpr int TAMANO_LOCAL_O_VISITANTE = 12;

    // Rutas de Archivos Principales
    inline const std::string RUTA_DATOS_CARPETA = "datos/";
    inline const std::string NOMBRE_ARCHIVO_TORNEO = "datos/torneo.bin";
    inline const std::string NOMBRE_ARCHIVO_JUGADORES = "datos/jugadores.bin";
    inline const std::string NOMBRE_ARCHIVO_EQUIPOS = "datos/equipos.bin";
    inline const std::string NOMBRE_ARCHIVO_PARTIDOS = "datos/partidos.bin";

    // Rutas de Backups
    inline const std::string RUTA_BACKUPS = "datos/backups/";
    inline const std::string NOMBRE_BACKUPS_TORNEO = "datos/backups/torneo.bin";
    inline const std::string NOMBRE_BACKUPS_JUGADORES = "datos/backups/jugadores.bin";
    inline const std::string NOMBRE_BACKUPS_EQUIPOS = "datos/backups/equipos.bin";
    inline const std::string NOMBRE_BACKUPS_PARTIDOS = "datos/backups/partidos.bin";

    // Límites de Valores
    constexpr int MAX_RESULTADOS = 100;
    constexpr int MAX_ANOTACIONES = 22;
    constexpr int MAX_TARJETAS_AMARILLAS = 30;
    constexpr int MAX_TARJETAS_ROJAS = 8;
    constexpr int MINUTO_MINIMO = 1;
    constexpr int MINUTO_MAXIMO = 120;

} // namespace constantes

#endif // CONSTANTES_HPP
