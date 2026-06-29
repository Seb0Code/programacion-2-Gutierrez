// Sport G&C Tournaments (SISTEMA DE GESTION DE TORNEOS DEPORTIVOS)

// ============================================//
//   1. INCLUDES Y DIRECTIVAS                   //
// ============================================//
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

// Definicion de Variables globales
const int TAMANO_MENSAJE_ERROR = 150;
const int TAMANO_NOMBRE = 100;
const int TAMANO_FECHA = 11;
const char *NOMBRE_ARCHIVO_TORNEO = "datos/torneo.bin";
const char *NOMBRE_ARCHIVO_JUGADORES = "datos/jugadores.bin";
const char *NOMBRE_ARCHIVO_EQUIPOS = "datos/equipos.bin";
const char *NOMBRE_ARCHIVO_PARTIDOS = "datos/partidos.bin";
const int MAX_RESULTADOS = 100;

// ============================================//
//   2. STRUCTS                                //
// ============================================//

struct ArchivoHeader {
    int cantidadRegistros; // Total histórico (incluyendo eliminados lógicamente)
    int proximoID;         // Siguiente ID a asignar (autoincremental)
    int registrosActivos;  // Registros con eliminado == false
    int version;           // Control de versión del archivo (iniciar en 1)
};

struct Jugador {
    // Datos básicos
    int ID;
    int idEquipo;
    char nombre[TAMANO_NOMBRE];
    char cedula[20];
    char posicion[25];
    int edad;
    int numeroDorsal;
    char fechaRegistro[TAMANO_FECHA]; // fecha de registro del jugador en formato YYYY-MM-DD
    // bool suspendido;

    // Estadísticas individuales (se actualizan al registrar goles en partidos)
    int golesAnotados;
    int tarjetasAmarillas;
    int tarjetasRojas;

    // Metadata de control
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Equipo {
    // Datos básicos
    int ID;
    char nombre[TAMANO_NOMBRE];
    char ciudad[TAMANO_MENSAJE_ERROR];
    char entrenador[TAMANO_NOMBRE];
    char fechaRegistro[TAMANO_FECHA]; // fecha de registro del equipo en Formato: YYYY-MM-DD
    int numJugadores = 0;             // Numero de jugadores del equipo

    // Estadísticas del torneo
    int puntos;
    int jugados = 0; // Partidos jugados
    int victorias;
    int empates;
    int derrotas;
    int anotacionAFavor;
    int anotacionEnContra;

    // Relaciones: IDs de partidos en que participó este equipo
    int partidosIDs[50]; // Máximo 50 partidos por equipo
    int cantidadPartidos;

    // Metadata de control
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Anotacion {
    int idJugador;   // ID del jugador que anotó (0 = desconocido / gol en contra)
    int minuto;      // Minuto del partido en que se anotó (1 - 120)
    char equipo[12]; // "LOCAL" o "VISITANTE"
};

struct tarjetaAmarilla {
    //
};

struct tarjetaRoja {
    //
};

struct Partido {
    // Datos básicos
    int ID;
    int idEquipoLocal;
    int idEquipoVisitante;
    char fecha[TAMANO_FECHA];
    char estado[12]; // "PROGRAMADO", "JUGADO", "CANCELADO"
    char descripcion[200];

    // Marcador global (para acceso rápido sin recorrer el array de goles)
    int AnotacionLocal;
    int golesVisitante;

    // Detalle de goles (NUEVO en Proyecto 2)
    Anotacion goles[22]; // Máximo 22 goles por partido
    // tarjetaAmarilla tarjetasA[30]; // Máximo 30 tarjetas A por partido
    // tarjetaRoja tarjetaR[8]; // Máximo 8 tarjetas R por partido
    int numAnotaciones;
    // int numTarjetasAma;
    // int numTarjetasRojas;

    // Metadata de control
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Torneo {
    char nombre[TAMANO_NOMBRE];     // Nombre del torneo
    char deporte[50];               // Deporte del torneo
    char formato[20];               // "GRUPOS" o "ELIMINATORIA"
    char fechaInicio[TAMANO_FECHA]; // Formato YYYY-MM-DD
    char fechaFin[TAMANO_FECHA];    // Formato YYYY-MM-DD
    // Metadata de control
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

// ============================================//
//   3. FUNCIONES AUXILIARES                   //
// ============================================//

// grupo de funciones que no inciden como tal en el sistema pero que mejoran su funcionamiento
namespace Auxiliares {

    // funcion que se encarga de configurar el idioma para aceptar caracteres del lenguaje español
    void configurarIdioma() {
        // Intentamos configurar el locale de forma segura
        try {
            std::locale::global(std::locale(""));
            std::cout.imbue(std::locale());
        } catch (const std::exception &e) {
            // Si Windows/MinGW da error con el locale vacío, forzamos el locale por defecto "C"
            std::locale::global(std::locale("C"));
            std::cout.imbue(std::locale());
        }

// Código específico para Windows (esto es lo que realmente arregla los acentos en tu terminal)
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#endif
    }

    // funcion que pausa el programa por un tiempo determinado
    void waitfor(int tiempo = 2500) { std::this_thread::sleep_for(std::chrono::milliseconds(tiempo)); }

    // funcion que limpia la consola
    void limpiarPantalla() {
        // Enviamos el comando de borrado. Si la terminal lo soporta, se limpiará al instante.
        std::cout << "\x1B[2J\x1B[H" << std::flush;

        // Si la terminal es antigua o no procesó el código ANSI, ejecutamos el comando nativo.
#ifdef _WIN32
        // Si estamos en Windows
        std::system("cls");
#else
        /**/ // Si estamos en Linux o macOS
        /**/ std::system("clear");
#endif
    }

    // esta funcion transforma el texto a mayuscula
    char *toMayus(char *texto) {
        if (texto == nullptr) {
            return nullptr;
        }

        int longitud = std::strlen(texto);
        std::transform(texto, texto + longitud, texto, ::toupper);
        return texto;
    }

    // esta funcion transforma el texto a minuscula
    char *toMinus(char *texto) {
        if (texto == nullptr) {
            return nullptr;
        }

        int longitud = std::strlen(texto);
        std::transform(texto, texto + longitud, texto, ::tolower);
        return texto;
    }

    // funcion para ingresar cualquier tipo de dato
    template <typename var1> //
    void ingresarDatos(var1 &variable, const char *mensaje, bool (*ptrValidador)(var1, char *) = nullptr) {
        // bandera que se activa si el usuario ingresa un tipo de dato incorrecto
        bool flag = false;
        char mensajeError[TAMANO_MENSAJE_ERROR];
        do {
            mensajeError[0] = '\0'; // Limpieza preventiva del error anterior
            flag = false;
            std::cout << mensaje << std::flush;
            std::cin >> variable;

            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                flag = true; // activamos la bandera
                std::cout << "Error Tipo de Dato Incorrecto\n\n";
                waitfor(3000);
            } else {
                // si el puntero no contiene la direccion de ninguna direccion se omite este bloque
                if (ptrValidador != nullptr) {
                    flag = !ptrValidador(variable, mensajeError); // si no es valido se activa la bandera
                    std::cout << mensajeError << std::endl << std::endl;
                }
                // Si la lectura fue exitosa, limpiamos el enter residual
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } while (flag);
    }

    // funcion para ingresar cadenas de texto
    void ingresarCadena(char *texto, size_t tamañoMaximo, const char *mensaje, bool (*ptrValidador)(const char *, char *) = nullptr) {
        const int TAMANO_MENSAJE_ERROR = 150;
        bool flag = false;
        char mensajeError[TAMANO_MENSAJE_ERROR];

        do {
            mensajeError[0] = '\0'; // Limpieza preventiva del error
            flag = false;

            // usamos std::flush para obligar a la pantalla a mostrar el mensaje
            std::cout << mensaje << std::flush;

            // Se lee toda la linea
            std::cin.getline(texto, tamañoMaximo);

            // Si falla
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                flag = true;
                std::cout << "ERROR: Excediste el limite de caracteres permitido (" << tamañoMaximo - 1 << "). Intente de nuevo.\n\n";
                Auxiliares::waitfor(3000);
                continue; // Saltamos directo a la siguiente iteración ya que no es necesario el validador
            }

            // Si la lectura no tuvo errores, pasamos el texto por el validador
            if (ptrValidador != nullptr) {
                if (!ptrValidador(texto, mensajeError)) {
                    flag = true; // Si el validador retorna false, la bandera se activa para repetir
                    std::cout << mensajeError << std::endl << std::endl;
                    waitfor(3500);
                }
            }
        } while (flag);
    }

    // funcion que se encarga de pausar el programa hasta que el usuario ingrese enter por la consola
    void pausarPrograma() {
        std::cout << std::endl << std::endl;
        // Ignora cualquier carácter sobrante en el búfer hasta encontrar el salto de línea
        if (std::cin.rdbuf()->in_avail() > 0) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        // Mostramos el mensaje
        std::cout << "\nPresione Enter para continuar...";

        // Esperaa a que el usuario presione la tecla Enter
        std::cin.get();
    }

} // namespace Auxiliares

// ============================================//
//   4. VALIDADORES                            //
// ============================================//

namespace Validadores {

    // =======================================================================================//
    // Declaracion de arrays y variables que serán usados para algunas validaciones          //
    // =======================================================================================//

    const char *Deportes[] = {"FUTBOL", "BALONCESTO", "TENIS", "VOLEIBOL", "RUGBY", "BEISBOL", "HOCKEY", "HANDBALL", "SOFTBOL"};

    // Definimos nuestros punteros a arrays de literales para cada deporte iniciando con el nombre del deporte
    const char *MatrizFutbol[] = {"FUTBOL", "PORTERO", "DEFENSA", "MEDIOCAMPISTA", "DELANTERO", nullptr};
    const char *MatrizBasket[] = {"BALONCESTO", "BASE", "ESCOLTA", "ALERO", "ALA-PIVOT", "PIVOT", nullptr};
    const char *MatrizVoleibol[] = {"VOLEIBOL", "COLOCADOR", "PUNTA", "CENTRAL", "LIBERO", "OPUESTO", nullptr};
    const char *MatrizBeisbol[] = {"BEISBOL", "LANZADOR", "RECEPTOR", "INFIELDER", "OUTFIELDER", nullptr};
    const char *MatrizSoftbol[] = {"SOFTBOL", "LANZADOR", "RECEPTOR", "INFIELDER", "OUTFIELDER", nullptr};
    const char *MatrizHandball[] = {"HANDBALL", "PORTERO", "CENTRAL", "LATERAL", "EXTREMO", "PIVOTE", nullptr};
    const char *MatrizHockey[] = {"HOCKEY", "PORTERO", "DEFENSA", "MEDIOCAMPISTA", "DELANTERO", nullptr};
    const char *MatrizRugby[] = {"RUGBY", "DELANTERO", "RETAGUARDIA", "MEDIO", nullptr};
    const char *MatrizTenis[] = {"TENIS", "INDIVIDUAL", "DOBLES", nullptr};

    const char **MapaDeportes[] = {MatrizFutbol, MatrizBasket, MatrizVoleibol, MatrizBeisbol, MatrizSoftbol, MatrizHandball, MatrizHockey, MatrizRugby, MatrizTenis};

    const size_t totalDeportes = sizeof(MapaDeportes) / sizeof(MapaDeportes[0]);

    // Arreglo para el minimo de jugadores para poder jugar un partido
    const int MinimoJugadoresPorDeporte[] = {
        11, // FUTBOL
        5,  // BALONCESTO
        1,  // TENIS
        6,  // VOLEIBOL
        15, // RUGBY
        9,  // BEISBOL
        6,  // HOCKEY
        7,  // HANDBALL
        9   // SOFTBOL
    };

    char deporteActual[50];
    char fechaDeIni[11];
    char fechaDeFin[11];

    // Función que se llamará una sola vez al crear el torneo para agilizar las validaciones
    void definirDeporteActual(const char *deporte) {
        std::strncpy(deporteActual, deporte, 50);
        // La aseguramos en mayúsculas de una vez
        Auxiliares::toMayus(deporteActual);
    }

    // definimos las fechas para las futuras validaciones de datos
    void definirFechaInicio(const char *fechaInicio) { std::strncpy(fechaDeIni, fechaInicio, 11); }

    void definirFechaFin(const char *fechaFin) { std::strncpy(fechaDeFin, fechaFin, 11); }

    // =======================================================================================//
    //  Validaciones auxiliares (no se debe poder acceder a ellas desde fuera del namespace)  //
    // =======================================================================================//

    // encapsula todas las funciones que solo funcionan como auxiliares a otras funciones Validadores principales
    namespace {
        // funcion para hallar el tamaño de un char que no tiene permitido modificar el char ni el parametro tamaño
        bool TamañoValido(const char *texto, const int tamañoCorrecto) {
            /*int tamañoAux = 0;
            for (size_t e = 0; texto[e] != '\0'; e++) {
                tamañoAux++;
            }*/
            size_t tamañoAux = strlen(texto);
            return tamañoAux == tamañoCorrecto; // si son iguales devuelve true, es decir cumple el tamaño, sino false
        }

        inline bool charVacio(const char *texto) { return (texto == nullptr || *texto == '\0'); }

        bool soloNumeros(const char *texto) {
            size_t e = 0;
            // validamos que no esté vacío
            if (charVacio(texto)) {
                return false; // si esta vacio devolvemos que la fecha no es válida
            }
            bool esNum = true;
            while (*(texto + e) != '\0') {
                esNum = std::isdigit(static_cast<unsigned char>(*(texto + e)));
                if (!esNum) {
                    return false;
                }
                e++;
            }
            return true;
        }

        bool soloLetras(const char *texto) {
            size_t e = 0;
            // validamos que no esté vacío
            if (charVacio(texto)) {
                return false; // si esta vacio devolvemos que la fecha no es válida
            }
            bool esLetra = true;
            bool esEspacio = true;
            while (*(texto + e) != '\0') {
                esLetra = std::isalpha(static_cast<unsigned char>(*(texto + e)));
                esEspacio = std::isspace(static_cast<unsigned char>(*(texto + e)));
                if (!esLetra && !esEspacio) {
                    return false;
                }
                e++;
            }
            return true;
        }

        bool tamañoMaximoValido(const char *cadena, size_t tamMaximo) {
            size_t e = 0;
            while (*(cadena + e) != '\0') {
                e++;
            }
            return (e <= tamMaximo); // devuelve false si 'e' es mayor que el tamaño maximo o true si 'e' es menor
        }

        bool esBisiesto(int año) {
            // Verificamos que no sea 0 o negativo
            if (año <= 0) {
                return false;
            }
            // Aplicamos la regla de divisibilidad del año bisiesto
            return (año % 4 == 0 && año % 100 != 0) || (año % 400 == 0);
        }

        // Convertit formato fecha YYYY-MM-DD de texto a numeros que se pueden comparar
        void FechaToNum(const char *fecha, int &año, int &mes, int &dia) {
            año = (fecha[0] - '0') * 1000 + (fecha[1] - '0') * 100 + (fecha[2] - '0') * 10 + (fecha[3] - '0');
            mes = (fecha[5] - '0') * 10 + (fecha[6] - '0');
            dia = (fecha[8] - '0') * 10 + (fecha[9] - '0');
        }

        // convierte los numeros por separado a un solo numero mas facil de comparar
        int FechaAEntero(int año, int mes, int dia) { return (año * 10000) + (mes * 100) + dia; }

        bool esAlfanumericoConEspacios(const char *texto) {
            size_t i = 0;
            while (texto[i] != '\0') {
                // revisa si es letra o número y si es espacio ' '
                if (!std::isalnum(texto[i]) && texto[i] != ' ') {
                    return false;
                }
                i++;
            }
            return true;
        }
    } // namespace

    // ====================================================================================//
    //  Validaciones principales                                                           //
    // ====================================================================================//

    bool Positivo(const int variable, char *mensajeError) {
        if (variable < 0) {
            std::strncpy(mensajeError, "El dato ingresado no puede ser negativo", TAMANO_MENSAJE_ERROR);
            return false;
        }
        return true;
    }

    bool IDvalido(const int id, char *mensajeError) {
        if (id <= 0) {
            std::strncpy(mensajeError, "Error: El ID debe ser mayor a 0", TAMANO_MENSAJE_ERROR);
            return false;
        }
        return true;
    }

    bool Edad(const int edad, char *mensajeError) {
        // la edad no puede ser negativa ni igual a 0, tampoco puede ser mayor a 120
        if (edad < 14 || edad > 50) {
            // asignamos la siguiente cadena de texto a el array de char
            std::strncpy(mensajeError, "La edad debe esta entre un rango de 14-50", TAMANO_MENSAJE_ERROR);
            return false;
        }
        return true;
    }

    bool Dorsal(const int dorsal, char *mensajeError) {
        if (dorsal < 1 || dorsal > 99) {
            // std:strncpy copia el mensaje del segundo parametro dentro de un const char*
            std::strncpy(mensajeError, "El dorsal debe esta entre un rango de 1-99", TAMANO_MENSAJE_ERROR);
            return false;
        }
        return true;
    }

    bool FechaValida(const char *fecha, char *mensajeError) {
        int dia = 0, mes = 0, año = 0;
        // array de los dias de cada mes
        int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        // validamos que no esté vacío
        if (charVacio(fecha)) {
            std::strncpy(mensajeError, "La fecha no debe estar vacía", TAMANO_MENSAJE_ERROR);
            return false; // si esta vacio devolvemos que la fecha no es válida
        }

        // Validación de tamaño y guiones (YYYY-MM-DD)
        if (!TamañoValido(fecha, 10) || fecha[4] != '-' || fecha[7] != '-') {
            std::strncpy(mensajeError, "Formato incorrecto. Debe usar YYYY-MM-DD (Ej: 2026-05-31).", TAMANO_MENSAJE_ERROR);
            return false;
        }

        // Verificamos que los demás caracteres sean numéricos
        for (size_t e = 0; e < 10; e++) {
            if (e == 4 || e == 7)
                continue;
            if (!std::isdigit(static_cast<unsigned char>(fecha[e]))) {
                std::strncpy(mensajeError, "La fecha no debe contener caracteres no numericos.", TAMANO_MENSAJE_ERROR);
                return false;
            }
        }

        FechaToNum(fecha, año, mes, dia);
        // si es año bisiesto febrero pasa a tener 29 dias
        if (mes == 2 && esBisiesto(año)) {
            diasPorMes[1] = 29;
        }

        // validar que los meses esten en el rango y que los años no sean negativos
        if (mes < 1 || mes > 12 || año < 1) {
            std::strncpy(mensajeError, "Los meses deben estar entre el rango de 1-12 o año inválido", TAMANO_MENSAJE_ERROR);
            return false;
        }

        // validar que el dia
        if (dia < 1 || dia > diasPorMes[mes - 1]) {
            std::strncpy(mensajeError, "El dia ingresado no existe para ese mes.", TAMANO_MENSAJE_ERROR);
            return false;
        }

        return true;
    }

    // Esta funcion se usa para validar que una fecha no sea menor en el tiempo que otra
    bool ValidarFechaFin(const char *fechaFin, char *mensajeError) {

        // si la fecha final no es valida
        if (!FechaValida(fechaFin, mensajeError)) {
            return false;
        }

        // declaramos los valores a comparar
        int añoFin, mesFin, diaFin;
        int añoIni, mesIni, diaIni;

        // Almacenamos las fechas en variables int
        FechaToNum(fechaFin, añoFin, mesFin, diaFin);
        FechaToNum(fechaDeIni, añoIni, mesIni, diaIni);

        int numIni = FechaAEntero(añoIni, mesIni, diaIni);
        int numFin = FechaAEntero(añoFin, mesFin, diaFin);

        if (numFin < numIni) {
            std::strncpy(mensajeError, "La fecha de Finalizacion no puede ser antes que la fecha de Inicio", TAMANO_MENSAJE_ERROR);
            return false;
        }
        return true;
    }

    bool fechaValidaRegistroDePartidos(const char *fechaPartido, char *mensajeError) {
        // si la fecha final no es valida
        if (!FechaValida(fechaPartido, mensajeError)) {
            return false;
        }

        // declaramos los valores a comparar
        int añoFin, mesFin, diaFin;
        int añoIni, mesIni, diaIni;
        int añoPtd, mesPtd, diaPtd;

        // Almacenamos las fechas en variables int
        FechaToNum(fechaDeFin, añoFin, mesFin, diaFin);
        FechaToNum(fechaDeIni, añoIni, mesIni, diaIni);
        FechaToNum(fechaPartido, añoPtd, mesPtd, diaPtd);

        // llevamos cada fecha a expresion de un solo numero
        int numIni = FechaAEntero(añoIni, mesIni, diaIni);
        int numFin = FechaAEntero(añoFin, mesFin, diaFin);
        int numPtd = FechaAEntero(añoPtd, mesPtd, diaPtd);

        // Validamos los limites
        if (numPtd < numIni || numPtd > numFin) {
            std::strncpy(mensajeError, "La fecha del Partido esta fuera del rango del torneo", TAMANO_MENSAJE_ERROR);
            return false;
        }

        return true;
    }

    bool fechaValidaRegistroDeJugadorOEquipo(const char *fechaRegistro, char *mensajeError) {

        // si la fecha final no es valida
        if (!FechaValida(fechaRegistro, mensajeError)) {
            return false;
        }

        // declaramos los valores a comparar
        int añoRgt, mesRgt, diaRgt;
        int añoIni, mesIni, diaIni;

        // Almacenamos las fechas en variables int
        FechaToNum(fechaRegistro, añoRgt, mesRgt, diaRgt);
        FechaToNum(fechaDeIni, añoIni, mesIni, diaIni);

        int numIni = FechaAEntero(añoIni, mesIni, diaIni);
        int numRgt = FechaAEntero(añoRgt, mesRgt, diaRgt);

        // Verificamos que el registro no sea despues de iniciar el torneo del torneo
        if (numRgt >= numIni) {
            std::strncpy(mensajeError, "Error, el registro debe hacerse antes del dia de inicio del torneo", TAMANO_MENSAJE_ERROR);
            return false;
        }

        // Calculamos la diferencia de tiempo en meses entre ambas fechas
        int difAños = añoIni - añoRgt;
        int difMeses = (mesIni - mesRgt);
        int totalDeMesesDiferencia = (difAños * 12) + difMeses;

        // Verificamos que el registro no sea mas de 6 meses antes
        if (std::abs(totalDeMesesDiferencia) > 6) {
            std::strncpy(mensajeError, "Error, el registro solo puede hacerse hasta maximo 6 meses antes del incio del torneo", TAMANO_MENSAJE_ERROR);
            return false;
        }
        return true;
    }

    bool Cedulas(const char *cedula, char *mensajeError) {
        const size_t tamañoMin = 7, tamañomax = 10;

        // validamos que no esté vacío
        if (charVacio(cedula)) {
            std::strncpy(mensajeError, "La cedula no puede estar vacía.", TAMANO_MENSAJE_ERROR);
            return false; // si esta vacio devolvemos que la fecha no es válida
        }

        // verificamos que solo tenga numeros
        if (!soloNumeros(cedula)) {
            std::strncpy(mensajeError, "La cedula no puede contener caracteres no númericos.", TAMANO_MENSAJE_ERROR);
            return false;
        }

        // medimos la longitud
        size_t longitud = strlen(cedula);

        // si la longitud esta fuera del rando
        if (longitud < tamañoMin || longitud > tamañomax) {
            std::strncpy(mensajeError, "Longitud de cedula invalida (Debe tener entre 7 y 10 digitos).", TAMANO_MENSAJE_ERROR);
            return false;
        }

        return true;
    }

    bool Nombres(const char *nombre, char *mensajeError) {
        // validamos que no esté vacío
        if (charVacio(nombre)) {
            std::strncpy(mensajeError, "El nombre no puede estar vacío", TAMANO_MENSAJE_ERROR);
            return false; // si esta vacio devolvemos que la fecha no es válida
        }

        // validamos que solo contenga letras
        if (!soloLetras(nombre)) {
            std::strncpy(mensajeError, "El nombre solo debe contener letras y espacios", TAMANO_MENSAJE_ERROR);
            return false;
        }
        return true;
    }

    bool nombreTorneo(const char *nombreTorneo, char *mensajeError) {
        // validamos que no esté vacío
        if (charVacio(nombreTorneo)) {
            std::strncpy(mensajeError, "El nombre no puede estar vacío", TAMANO_MENSAJE_ERROR);
            return false; // si esta vacio devolvemos que la fecha no es válida
        }

        //
        if (!esAlfanumericoConEspacios(nombreTorneo)) {
            std::strncpy(mensajeError, "El nombre solo debe contener caracteres alfanumericos o espacios", TAMANO_MENSAJE_ERROR);
            return false;
        }

        return true;
    }

    // Función para validar si un deporte está en la lista (usa mensaje de error estilo MiSistema)
    bool existeDeporte(const char *deporte, char *mensajeError) {
        if (charVacio(deporte)) {
            std::strncpy(mensajeError, "El Deporte ingresado no puede estar Vacio", TAMANO_MENSAJE_ERROR);
            return false;
        }

        // Creamos una copia porque no se puede modificar la MiSistema y la convertimos a mayuscula
        char copiaDeporte[50];
        std::strncpy(copiaDeporte, deporte, sizeof(Torneo::deporte));
        Auxiliares::toMayus(copiaDeporte);

        for (size_t e = 0; e < sizeof(Deportes) / sizeof(Deportes[0]); e++) {
            if (std::strcmp(copiaDeporte, Deportes[e]) == 0) {
                return true;
            }
        }
        std::strncpy(mensajeError, "El deporte ingresado no esta en la lista de deportes validos.", TAMANO_MENSAJE_ERROR);
        return false;
    }

    // Función para validar la posición del jugador según el deporte del torneo
    bool Posicion(const char *posicion, char *mensajeError) {
        if (charVacio(posicion)) {
            std::strncpy(mensajeError, "La posición no puede estar vacía.", TAMANO_MENSAJE_ERROR);
            return false;
        }

        if (charVacio(deporteActual)) {
            std::strncpy(mensajeError, "Error: No se ha definido el deporte del torneo todavía.", TAMANO_MENSAJE_ERROR);
            return false;
        }

        char copiaPosicion[50];
        std::strncpy(copiaPosicion, posicion, sizeof(Jugador::posicion));
        Auxiliares::toMayus(copiaPosicion);

        for (size_t i = 0; i < totalDeportes; i++) {
            if (std::strcmp(deporteActual, MapaDeportes[i][0]) == 0) {
                size_t j = 1;
                while (MapaDeportes[i][j] != nullptr) {
                    if (std::strcmp(copiaPosicion, MapaDeportes[i][j]) == 0) {
                        return true;
                    }
                    j++;
                }
                // Si encuentra el deporte pero la posición no coincide
                std::strncpy(mensajeError, "La posicion ingresada no es valida para este deporte.", TAMANO_MENSAJE_ERROR);
                return false;
            }
        }
        std::strncpy(mensajeError, "Error del sistema: El deporte actual no coincide con los registros.", TAMANO_MENSAJE_ERROR);
        return false;
    }

} // namespace Validadores

// ============================================//
//   5. CAPA DE LOGICA                         //
// ============================================//

namespace Logica {

    bool existeArchivo(const char *nombreArchivo) {
        // Creamos el archivo y le colocamos la cabecera
        std::ifstream comprobar;

        // intentamos abrir el archivo
        comprobar.open(nombreArchivo, std::ios::binary);

        // Si el archivo no se pudo leer/abrir significa que no existe
        if (!comprobar.good()) {
            comprobar.close();
            return false;
        }
        return true;
        comprobar.close();
    }

    // Crea el archivo si no existe y escribe un ArchivoHeader en cero
    // Retorna true si el archivo quedó listo para usar
    bool inicializarArchivo(const char *nombreArchivo) {

        // Si el archivo existe (se puede abrir para leer) simplemente devolvemos true y cerramos el archivo
        if (existeArchivo(nombreArchivo)) {
            return true;

        } else { // Si el archivo no existe lo creamos
            std::ofstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            if (!archivo.is_open()) {
                // Devolvemos false porque ya que el archivo NO quedó listo para usar (no existe)
                return false;
            }

            // inicializamos un archivo Header
            ArchivoHeader nuevo = {0, 1, 0, 1};

            // Movemos el puntero de escritura al inicio por seguridad
            archivo.seekp(0, std::ios::beg);

            // escribimos el header en el archivo binario
            archivo.write(reinterpret_cast<const char *>(&nuevo), sizeof(ArchivoHeader));

            // Verifcamos que se haya escrito bien
            if (archivo.fail()) {
                archivo.close();
                // Devolvemos false porque ya que el archivo NO quedó listo para usar (falló)
                return false;
            }

            // cerramos el archivo
            archivo.close();
            return true;
        }
    }

    // Abre el archivo y retorna el header si el archivo existe y si tiene header
    ArchivoHeader leerHeader(const char *nombreArchivo) {
        // si no existe el archivo devolvemos el header lleno de -1
        ArchivoHeader header, headerError = {-1, -1, -1, -1};
        if (!existeArchivo) {
            return headerError;
        } else { // Si existe el archivo
            // abrimos el archivo en modo lectura
            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que el archivo abrió correctamente
            if (!archivo.is_open()) {
                return headerError;
            }

            // ubicamos el puntero de lectura al principio por seguridad
            archivo.seekg(0, std::ios::beg);

            // Leemos solo el header
            archivo.read(reinterpret_cast<char *>(&header), sizeof(ArchivoHeader));

            // Verificamos si la lectura fue correcta
            if (!archivo.fail()) {
                archivo.close();
                return headerError;
            }

            archivo.close();
            return header;
        }
    }

    // recibe el header y se actualiza el header del archivo
    bool actualizarHeader(const char *nombreArchivo, ArchivoHeader header) {

        // Si no existe el archivo devolvemos false
        if (!existeArchivo(nombreArchivo)) {
            return false;

        } else { // Si existe el archivo cambiamos el header
            std::fstream archivo;

            // abrimos el archivo de esta forma para evitar el truncamiento de los datos
            archivo.open(nombreArchivo, std::ios::in | std::ios::out | std::ios::binary);

            // Verificamos si abrió el archivo
            if (!archivo.is_open()) {
                return false;
            }

            // Movemos el puntero de escritura al inicio por seguridad
            archivo.seekp(0, std::ios::beg);

            // Escribimos el nuevo header
            archivo.write(reinterpret_cast<const char *>(&header), sizeof(ArchivoHeader));

            // Verificamos si la escritura fue correcta
            if (!archivo.fail()) {
                archivo.close();
                return false;
            }

            archivo.close();
            return true;
        }
    }

    bool inicializarSistemaArchivos() {
        return inicializarArchivo(NOMBRE_ARCHIVO_TORNEO) && inicializarArchivo(NOMBRE_ARCHIVO_EQUIPOS) && inicializarArchivo(NOMBRE_ARCHIVO_JUGADORES) &&
               inicializarArchivo(NOMBRE_ARCHIVO_PARTIDOS);
    }

    template <class struct1> //
    int buscarIndicePorID(const char *nombreArchivo, int ID) {
        int indice = -1;

        // Si el archivo no existe devolvemos -1
        if (!existeArchivo(nombreArchivo)) {
            return indice;
        }
        std::ifstream archivo;

        // Abrimos el archivo en modo lectura
        archivo.open(nombreArchivo, std::ios::binary);

        // Verificamos que el archivo esté abierto
        if (!archivo.is_open()) {
            return indice;
        }

        // Movemos el puntero de lectura despues del header
        archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

        struct1 registroTemp;
        int contadorIndice = 0;
        bool encontrado = false;

        // Realizamos la busqueda
        while (archivo.read(reinterpret_cast<char *>(&registroTemp), sizeof(struct1))) {

            // Si el ID coincide activamos la bandera y rompemos el bucle
            if (registroTemp.ID == ID) {
                encontrado = true;
                break;
            }
            contadorIndice++;
        }

        // Verificamos si la lectura fue correcta
        if (!archivo.fail()) {
            archivo.close();
            return indice;
        }

        indice = contadorIndice;
        archivo.close();

        // Si lo encontramos devolvemos el indice, sino retornamos -1
        if (encontrado) {
            return indice;
        } else {
            return indice;
        }
    }

    template <class struct2> //
    bool existeID(const char *nombreArchivo, const int ID) {

        // Verificamos que existe el archivo
        if (!existeArchivo(nombreArchivo)) {
            return false;
        }

        std::ifstream archivo;
        archivo.open(nombreArchivo, std::ios::binary);
        struct2 registroTemporal;

        // verificamos que se abrió sino devolvemos false;
        if (!archivo.is_open()) {
            return false;
        }

        // movemos el puntero de indice despues del header
        archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

        // Buscamos el ID en los registros
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct2))) {

            // Verificamos si se produjo un fallo
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Si encontramos una coincidencia devolvemos true
            if (registroTemporal.ID == ID) {
                archivo.close();
                return true;
            }
        }

        archivo.close();
        return false;
    }

    // tipo5 es la estructura y tipo6 es el tipo de dato dentro de la estructura, uno de los miembros
    template <class struct3, class var2> //
    bool nombreDuplicado(const char *nombreArchivo, const char *nombre, var2 struct3::*miembro) {
        // * DESCRIPCION:
        // se lee como un puntero llamado miembro que apunta a una variable d etipo tipo6 que reside dentro de una
        // estructura de tipo tipo5

        // Verificamos si el archivo existe
        if (!existeArchivo(nombreArchivo)) {
            return false;
        }

        std::ifstream archivo;
        archivo.open(nombreArchivo, std::ios::binary);

        // Realizamos una copia de la variable para poder convertirla a minuscula sin problemas
        char nombreBusquedaAux[TAMANO_NOMBRE];
        std::strncpy(nombreBusquedaAux, nombre, TAMANO_NOMBRE);
        Auxiliares::toMinus(nombreBusquedaAux);
        struct3 registroTemporal;

        // movemos el puntero de indice despues del header
        archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

        // buscamos en el binario
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct3))) {

            // verificamos que no hayan errores en la lectura
            if (archivo.fail()) {
                return false;
            }

            // buscamos si el nombre está duplicado
            if (std::strcmp(registroTemporal.*miembro, nombre) == 0) {
                // * DESCRIPCION:
                // como *miembro es un puntero es decir guarda una direccion de memoria, la desreferenciamos con '*'
                // por lo tanto lo que indica es que dentro de la estructura real (que aun no sabemos cual es)
                // vamos a acceder al campo que nos indica miembro (el que pasaron como argumento)
                // si la estructura es equipo y el miembro es nombre entrenador entonces hace equipo::nombreEntrenador
                // es decir creamos una variable de tipo equipo y le pasamos como propiedad el nombre del entrenador
                archivo.close();
                return true;
            }
        }

        archivo.close();
        return false;
    }

    template <class struct4, class var3> //
    bool nombreDuplicadoParaActualizar(const char *nombreArchivo, const char *nombre, const int idEquipo, var3 struct4::*miembro) {
        // si el archivo no existe devolvemos false
        if (!existeArchivo(nombreArchivo)) {
            return false;
        }

        std::ifstream archivo;
        archivo.open(nombreArchivo, std::ios::binary);

        // verificamos si el archivo abrió
        if (!archivo.is_open()) {
            return false;
        }

        // creamos una copia para no modificar las variables reales
        char copiaNuevoNombre[TAMANO_NOMBRE];
        std::strncpy(copiaNuevoNombre, nombre, TAMANO_NOMBRE);
        Auxiliares::toMinus(copiaNuevoNombre);
        struct4 registroTemporal;

        // movemos el puntero de indice despues del header
        archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

        // Realizamos la busqueda
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct4))) {
            // verificamos si no ocurrio un fallo en la lectura
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Si estamos revisando el equipo al que pertenecemos lo ignoramos
            if (registroTemporal.idEquipo == idEquipo) {
                continue;
            }

            // creamos una copia del nombre real para hacer la comparacion en minusculas
            char nombreAux[TAMANO_NOMBRE];
            std::strncpy(nombreAux, registroTemporal.*miembro, TAMANO_NOMBRE);
            Auxiliares::toMinus(nombreAux);

            // comparamos los nombres si es igual devolmemos true sino false
            if (std::strcmp(nombreAux, copiaNuevoNombre) == 0) {
                return true;
            }
        }

        archivo.close();
        return false;
    }

    // Funcion para cualquier estructura
    template <class struct5> //
    bool buscarRegistrosPorId(const char *nombreArchivo, struct5 &buscado, const int ID) {
        // verificamos que si existe ese archivo
        if (!existeArchivo(nombreArchivo)) {
            return false;
        }

        std::ifstream archivo;
        archivo.open(nombreArchivo, std::ios::binary);

        // Verificamos que se haya abierto correctamente
        if (!archivo.is_open()) {
            return false;
        }

        // Buscamos el indice fisico
        size_t indice = buscarIndicePorID<struct5>(nombreArchivo, ID);

        // Si no encontró nada es decir el indice vale -1 retornamos false
        if (indice == -1) {
            return false;
        }

        // calculamos la posicion
        std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(struct5);

        // movemos el punero a esa posicion
        archivo.seekg(posicion, std::ios::beg);

        // Leemos el registro
        archivo.read(reinterpret_cast<char *>(&buscado), sizeof(struct5));

        // verificamos que no se hayan producido errores en la lectura
        if (archivo.fail()) {
            archivo.close();
            return false;
        }

        // verificamos que no esté eliminado
        if (buscado.eliminado) {
            return false;
        }

        archivo.close();
        return true;
    }

    template <class struct6> //
    int buscarRegistrosPorSucadena(const char nombreArchivo, struct6 resultados[], const char *subcadena, const int maxResultados) {
        int cantidadDeRegistrosEncontrados = 0;
        int error = -1;
        struct6 registroTemporal;

        // verificamos que si existe ese archivo
        if (!existeArchivo(nombreArchivo)) {
            return error;
        }

        std::ifstream archivo;
        archivo.open(nombreArchivo, std::ios::binary);

        // Verificamos que se haya abierto correctamente
        if (!archivo.is_open()) {
            return error;
        }

        // Realizamos copias para no dañar las variables originales
        char copiaRegistro[TAMANO_NOMBRE];
        char copiaSubcadena[TAMANO_NOMBRE];
        std::strncpy(copiaSubcadena, subcadena, TAMANO_NOMBRE);
        Auxiliares::toMinus(copiaSubcadena);

        // Ubicamos el puntero de posicion despues del header
        archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

        // Realizamos la lectura del archivo
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct6))) {

            // Verificamos que no haya error al hacer la lectura
            if (archivo.fail()) {
                return error;
            }

            // Si encontramos un registro eliminado lo saltamos
            if (registroTemporal.eliminado) {
                return error;
            }

            // Si aún no llegamos a la cantidad maxima de registros hacemos la comparacion
            if (cantidadDeRegistrosEncontrados < maxResultados) {

                // hacemos una copia del nombre del registro
                std::strncpy(copiaRegistro, registroTemporal.nombre);
                Auxiliares::toMinus(copiaRegistro);

                // lo pasamos a minus para comparar mejor
                // Buscamos si la subcadena coincide con la copia usando std::strstr
                if (std::strstr(copiaRegistro, copiaSubcadena) != nullptr) {
                    resultados[cantidadDeRegistrosEncontrados] = registroTemporal;
                    cantidadDeRegistrosEncontrados++;
                }

            } else {
                // sino salimos del bucle
                break;
            }
        }

        archivo.close();
        return cantidadDeRegistrosEncontrados;
    }

    template <class struct7> //
    int listarRegistros(const char *nombreArchivo, struct7 resultados[], const int maxRegistros) {
        int cantidadDeRegistrosEncontrados = 0;
        int error = -1;
        struct7 registroTemporal;
        ArchivoHeader header = leerHeader(nombreArchivo);

        // verificamos que si existe ese archivo
        if (!existeArchivo(nombreArchivo)) {
            return error;
        }

        std::ifstream archivo;
        archivo.open(nombreArchivo, std::ios::binary);

        // Verificamos que se haya abierto correctamente
        if (!archivo.is_open()) {
            return error;
        }

        // Verificamos que el header se haya leido correctamente
        if (header.cantidadRegistros == -1) {
            return false;
        }

        // Ubicamos el puntero de posicion despues del header
        archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

        // Realizamos la lectura del archivo
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct7))) {

            // Verificamos si no hubo un falló en la lectura
            if (archivo.fail()) {
                return error;
            }

            // Si encontramos un archivo que esta eliminado lo saltamos
            if (registroTemporal.eliminado) {
                continue;
            }

            // Si aún no llegamos a la cantidad maxima de registros hacemos la comparacion
            if (cantidadDeRegistrosEncontrados < maxRegistros) {

                resultados[cantidadDeRegistrosEncontrados] = registroTemporal;
                cantidadDeRegistrosEncontrados++;

            } else {
                // sino salimos del bucle
                break;
            }
        }

        // Verificamos que se hayan leido todos los registros
        if (header.registrosActivos > cantidadDeRegistrosEncontrados) {
            return error;
        }

        archivo.close();
        return cantidadDeRegistrosEncontrados;
    }

    namespace equipos {

        bool registrarEquipo(const char *nombreArchivo, Equipo &nuevoEquipo) {

            // verificamos primero que el archivo existe
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            // Leemos el header para obtener los datos
            ArchivoHeader header = leerHeader(nombreArchivo);

            // Validamos que el header no devuelva error
            if (header.cantidadRegistros == -1) {
                return false;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Si se produjo un error a la hora de abrir el archivo devolvemos false
            if (!archivo.is_open()) {
                return false;
            }

            // movemos el puntero de escritura al final
            archivo.seekp(0, std::ios::end);

            // colocamos el equipo como activo (es decir que no ha sido eliminado)
            nuevoEquipo.eliminado = false;

            // Inicializamos las estadísticas
            nuevoEquipo.jugados = 0;
            nuevoEquipo.victorias = 0;
            nuevoEquipo.empates = 0;
            nuevoEquipo.derrotas = 0;
            nuevoEquipo.puntos = 0;
            nuevoEquipo.anotacionAFavor = 0;
            nuevoEquipo.anotacionEnContra = 0;
            nuevoEquipo.numJugadores = 0;
            nuevoEquipo.cantidadPartidos = 0;

            // Asignamos el ID
            nuevoEquipo.ID = header.proximoID;

            // Asignamos la fecha de creacion del equipo
            nuevoEquipo.fechaCreacion = time(nullptr);           // Esto toma la fecha de en que se creo la variable
            nuevoEquipo.fechaUltimaModificacion = time(nullptr); // colocamos la misma porque cuando fue cuando se creó

            // Escribimos la estructura
            archivo.write(reinterpret_cast<const char *>(&nuevoEquipo), sizeof(Equipo));

            // Verificamos que no se haya producido un error
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Actualizamos el header
            header.cantidadRegistros++;
            header.proximoID++;
            header.registrosActivos++;
            // header.version; // ? Por el momento lo de dejaré asi pq nose que hacer
            actualizarHeader(nombreArchivo, header);

            archivo.close();
            return true;
        }

        /*Equipo **buscarEquipoPorSubCadena(, const char *subcadena, int *contEquiposEncontrados) {
            if (!sistemaEquiposValido(MiSistema)) {
                return nullptr;
            }

            char copiaRegistro[100];
            char copiaSubcadena[100];
            std::strncpy(copiaSubcadena, subcadena);
            Auxiliares::toMinus(copiaSubcadena);


            // incializamos el contador en 0
            *contEquiposEncontrados = 0;

            // creamos un array de punteros dinamico con tamaño maximo el numero de equipos que hay
            Equipo **arrayEquiposEncontrados = new Equipo *[MiSistema->numEquiposActuales];

            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                // hacemos una copia del nombre del equipo
                std::strncpy(copiaRegistro, MiSistema->Equipos[e].nombre);
                Auxiliares::toMinus(copiaRegistro);

                // lo pasamos a minus para comparar mejor
                // Buscamos si la subcadena coincide con la copia usando std::strstr
                if (std::strstr(copiaRegistro, copiaSubcadena) != nullptr) {
                    arrayEquiposEncontrados[*(contEquiposEncontrados)] = &(MiSistema->Equipos[e]);
                    (*contEquiposEncontrados)++;
                }
            }
            return arrayEquiposEncontrados;
        }*/

        /*int listarEquipos(const char *nombreArchivo, const int maxEquipos) {

            // verificamos primero que el archivo existe
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            // Leemos el header para obtener los datos
            ArchivoHeader header = leerHeader(nombreArchivo);

            // Validamos que el header no devuelva error
            if (header.cantidadRegistros == -1) {
                return false;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Si se produjo un error a la hora de abrir el archivo devolvemos false
            if (!archivo.is_open()) {
                return false;
            }

            // inicializamos en 0 por si no pasa las validaciones
            *cantEquipos = 0;

            if (!sistemaEquiposValido(MiSistema)) {
                return nullptr;
            }

            // definimos las variables y el tamaño de la lista
            *cantEquipos = MiSistema->numEquiposActuales;
            Equipo **listaDePtrAEquipos = new Equipo *[*cantEquipos];

            // recorremos el bucle para listar cada direccion de memoria de los equipos
            for (size_t e = 0; e < (*cantEquipos); e++) {
                listaDePtrAEquipos[e] = &(MiSistema->Equipos[e]);
            }

            return listaDePtrAEquipos;
        }*/

        int tablaDePosiciones(const char *nombreArchivo, Equipo registros[], const int maxEquipos) {
            // inicializamos en 0 por si no pasa las validaciones
            int difPtsEq1 = 0, difPtsEq2 = 0;
            bool intercambiar = false;
            int cantidadDeRegistros = 0;
            int error = -1;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return error;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que se haya abierto correctamente
            if (!archivo.is_open()) {
                return error;
            }

            // Mediante la funcion listarRegistros obtenemos los registros de equipos guardados y la cantidad
            cantidadDeRegistros = listarRegistros<Equipo>(nombreArchivo, registros, maxEquipos);

            // verificamos que la funcion no devuelva error
            if (cantidadDeRegistros == error) {
                return error;
            }

            // Ordenamos por cantidad de puntos de mayor a menor
            // Restamos 1 para no acceder a memoria indebida
            for (size_t e = 0; e < cantidadDeRegistros - 1; e++) {
                // restamos 1 por la misma razon y 'e' para no recorrer los elemento ya ordenados del final
                for (size_t r = 0; r < cantidadDeRegistros - e - 1; r++) {

                    // incializamos esta bandera en false, se activa si los equipos
                    // requieren que se interambien por los criterios de cada condicion
                    intercambiar = false;

                    //* Condicion 1
                    // Si el equipo 1 tiene menos puntos que el equipo 2;
                    if ((registros[r].puntos) < (registros[r].puntos)) {
                        intercambiar = true;

                        // Si poseen igual cantidad de puntos
                    } else if ((registros[r].puntos) == (registros[r + 1].puntos)) {

                        // Calculamos diferencia de puntos
                        difPtsEq1 = (registros[r].anotacionAFavor) - (registros[r].anotacionEnContra);
                        difPtsEq2 = (registros[r + 1].anotacionAFavor) - (registros[r + 1].anotacionEnContra);

                        // *Condicion 2
                        // Si el equipo de la izquierda tiene menor diferencia de puntos
                        // lo ubicamos a la deracha es decir lo bajamos una posicion
                        if (difPtsEq1 < difPtsEq2) {
                            intercambiar = true;

                            // Si la diferencia de goles es igual tambien
                        } else if (difPtsEq1 == difPtsEq2) {

                            //* Condicion 3
                            // comparamos los puntos a favor
                            if (registros[r].anotacionAFavor < registros[r + 1].anotacionAFavor) {
                                intercambiar = true;

                                // Si los puntos a favor son iguales desempatamos por victorias
                            } else if (registros[r].anotacionAFavor == registros[r + 1].anotacionAFavor) {

                                // * Condicion 4
                                // comparamos las victorias
                                if (registros[r].victorias < registros[r + 1].victorias) {
                                    intercambiar = true;
                                }
                            }
                        }
                    }

                    // Si se cumple alguna condicion hacemos el intercambio
                    if (intercambiar) {
                        // Guardamos el equipo con menos puntos en una variable auxiliar
                        Equipo registroTemporal = registros[r];

                        // Luego movemos el equipo mayor a la posicion donde estaba el menor
                        registros[r] = registros[r + 1];

                        // colocamos en la nueva posicion al equipo con menos puntos
                        registros[r + 1] = registroTemporal;
                    }
                }
            }

            archivo.close();
            return cantidadDeRegistros;
        }

        bool actualizarEquipo(const char *nombreArchivo, const int ID, const char *nombre, const char *entrenador, const char *ciudad) {

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (!sistemaEquiposValido(MiSistema)) {
                return false;
            }

            // Buscamos el equipo mediante el ID
            Equipo *ptrEquipo = buscarEquipoPorID(MiSistema, ID);

            // si no encontro ningun equipo
            if (ptrEquipo == nullptr) {
                return false;
            }

            // Verificamos los duplicados
            if (nombreDuplicadoParaActualizar(MiSistema, nombre, ID)) {
                return false;
            }

            if (nombreEntrenadorDuplicadoParaActualizar(MiSistema, entrenador, ID)) {
                return false;
            }

            // Actualizamos los datos
            std::strncpy(ptrEquipo->nombre, nombre);
            std::strncpy(ptrEquipo->entrenador, entrenador);
            std::strncpy(ptrEquipo->ciudad, ciudad);

            return true;
        }

        bool eliminarEquipo(const char *nombreArchivo, const int ID, bool &flag1, bool &flag2) {

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que se haya abierto correctamente
            if (!archivo.is_open()) {
                return false;
            }

            ArchivoHeader header = leerHeader(nombreArchivo);

            // Verificamos que el header se haya leido correctamente
            if (header.cantidadRegistros == -1) {
                return false;
            }

            // Buscamos el indice fisico del equipo
            size_t indice = buscarIndicePorID<Equipo>(nombreArchivo, ID);

            // Verificamos si no se encontró ningun equipo con ese ID
            if (indice == -1) {
                return false;
            }

            Equipo registroTemporal;

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Equipo);

            // Movemos el puntero de lectura al indice de posicion
            archivo.seekg(posicion, std::ios::beg);

            // Leemos el registro
            archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Equipo));

            if (archivo.fail()) {
                return false;
            }

            // Abrimos el archivo de partidos para hacer la verificacion
            std::ifstream archivoPartidos;
            archivoPartidos.open(NOMBRE_ARCHIVO_PARTIDOS, std::ios::binary);

            // Verificamos que se abrió
            if (!archivoPartidos.is_open()) {
                return false;
            }

            Partido partidoTemporal;

            // * Verificamos que no tenga partidos asociados
            while (archivoPartidos.read(reinterpret_cast<char *>(&partidoTemporal), sizeof(Partido))) {
                if ((partidoTemporal.idEquipoLocal == registroTemporal.ID) || (partidoTemporal.idEquipoVisitante == registroTemporal.ID)) {
                    flag1 = true;
                    return false;
                }
            }

            // Verificamos que la lectura haya sido correcta
            if (archivoPartidos.fail()) {
                archivoPartidos.close();
                return false;
            }

            // Cerramos el archivo de partidos
            archivoPartidos.close();

            // Abrimos el archivo de jugadores para hacer la verificacion
            std::ifstream archivoJugadores;
            archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary);

            // Verificamos que se abrió
            if (!archivoJugadores.is_open()) {
                return false;
            }

            Jugador jugadorTemporal;

            // * Verificamos que no tenga jugadores asociados
            while (archivoPartidos.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {
                if (jugadorTemporal.idEquipo == registroTemporal.ID) {
                    flag2 = true;
                    return false;
                }
            }

            // Verificamos que la lectura haya sido correcta
            if (archivoJugadores.fail()) {
                archivoJugadores.close();
                return false;
            }

            // Cerramos el archivo de jugadores
            archivoJugadores.close();

            // Eliminamos el registro con borrado logico
            registroTemporal.eliminado = true;

            // Disminuimos el numero de equipos activos
            header.registrosActivos--;
            return true;
        }

    } // namespace equipos

    namespace jugadores {

        bool DorsalDuplicado(const char *nombreArchivo, const int dorsal, const int idEquipo, bool &error) {

            // Verificamos que exista el archivo
            if (!existeArchivo(nombreArchivo)) {
                error = true;
                return false;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que se haya abierto
            if (!archivo.is_open()) {
                error = true;
                return false;
            }

            // Movemos el puntero de lectura despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            Jugador jugadorTemporal;

            // Leemos el archivo
            while (archivo.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {

                // Verificamos si no hubo error en la lectura
                if (archivo.fail()) {
                    error = true;
                    return false;
                }

                // Si encontramos coincidencia y pertence al mismo equipo
                if ((jugadorTemporal.numeroDorsal == dorsal) && (jugadorTemporal.idEquipo == idEquipo)) {
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        bool dorsalDuplicadoParaActualizar(, const int dorsal, const int IDJugador, const int idEquipo) {
            if (!sistemaJugadoresValido(MiSistema)) {
                return false;
            }

            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                // Saltamos al jugador actual
                if (MiSistema->Jugadores[e].ID == IDJugador)
                    continue;

                // Si otro jugador del mismo equipo ya tiene el dorsal en uso devuelvo true
                if (MiSistema->Jugadores[e].idEquipo == idEquipo && MiSistema->Jugadores[e].dorsal == dorsal) {
                    return true;
                }
            }
            return false;
        }

        bool CedulaRepetida(, const char *Cedula) {
            if (!sistemaJugadoresValido(MiSistema)) {
                return false;
            }
            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                // si hay una cedula que coincide
                if (std::strcmp(Cedula, MiSistema->Jugadores[e].cedula) == 0) {
                    return true;
                }
            }
            return false;
        }

        Jugador *agregarJugador(, const int idEquipo, const char *nombre, const char *cedula, const char *posicion, const int edad, const int numeroDorsal,
                                const char *fechaRegistro) {

            // verificamos primero que el archivo existe
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            // Leemos el header para obtener los datos
            ArchivoHeader header = leerHeader(nombreArchivo);

            // Validamos que el header no devuelva error
            if (header.cantidadRegistros == -1) {
                return false;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Si se produjo un error a la hora de abrir el archivo devolvemos false
            if (!archivo.is_open()) {
                return false;
            }

            // movemos el puntero de escritura al final
            archivo.seekp(0, std::ios::end);


            // usamos una variable de posicion
            size_t indice = MiSistema->numJugadoresActuales;

            // Agregamos al jugador
            std::strncpy(MiSistema->Jugadores[indice].nombre, nombre);
            std::strncpy(MiSistema->Jugadores[indice].cedula, cedula);
            std::strncpy(MiSistema->Jugadores[indice].posicion, posicion);
            MiSistema->Jugadores[indice].edad = edad;
            MiSistema->Jugadores[indice].idEquipo = idEquipo;
            MiSistema->Jugadores[indice].dorsal = numeroDorsal;
            std::strncpy(MiSistema->Jugadores[indice].fechaRegistro, fechaRegistro);
            MiSistema->Jugadores[indice].puntosAnotados = 0;
            MiSistema->Jugadores[indice].ID = MiSistema->siguienteIdJugador;

            // Aumentamos el numero de jugadores del equipo
            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                if (MiSistema->Equipos[e].ID == idEquipo) {
                    MiSistema->Equipos[e].numJugadores++; // Incrementamos el contador en el struct Equipo
                    break;
                }
            }

            // Aumentamos los contadores
            MiSistema->numJugadoresActuales++;
            MiSistema->siguienteIdJugador++;

            // Retornamos la dirección de memoria del jugador que está dentro del array dinámico
            return &(MiSistema->Jugadores[indice]);
        }

        Jugador *buscarJugadorPorID(, const int ID) {
            if (!sistemaJugadoresValido(MiSistema)) {
                return nullptr;
            }


            // Buscamos si el id esta relacionado con un jugador
            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                if (MiSistema->Jugadores[e].ID == ID) {
                    return &(MiSistema->Jugadores[e]);
                }
            }

            // si no encontró nada devuelve nullptr
            return nullptr;
        }

        Jugador **buscarJugadoresPorNombre(, const char *subcadena, int *cantJugadoresEncontrados) {
            if (!sistemaJugadoresValido(MiSistema)) {
                return nullptr;
            }


            // creamos dos variables para realizar copias y no dañar laa MiSistemaes
            char copiaJugador[100];
            char copiaSubcadena[100];

            // incializamos el contador en 0
            *cantJugadoresEncontrados = 0;

            // Reservamos espacio para una lista de jugadores
            Jugador **listaDeJugadoresEncontrados = new Jugador *[MiSistema->numJugadoresActuales];

            // hacemos la copia de la subcadena y la pasamos a minuscula
            std::strncpy(copiaSubcadena, subcadena);
            Auxiliares::toMinus(copiaSubcadena);

            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                // realizamos la copia del nombre del jugador para esta iteración
                std::strncpy(copiaJugador, MiSistema->Jugadores[e].nombre);
                Auxiliares::toMinus(copiaJugador);
                // Si conseguimos un jugador que coincida con la subcadena
                if (std::strstr(copiaJugador, copiaSubcadena) != nullptr) {
                    // añadimos el jugador a la lista
                    listaDeJugadoresEncontrados[*(cantJugadoresEncontrados)] = &(MiSistema->Jugadores[e]);
                    (*cantJugadoresEncontrados)++;
                }
            }
            return listaDeJugadoresEncontrados;
        }

        Jugador **listarJugadoresPorEquipo(, const int idEquipo, int *cantidadJugadores) {

            // inicializamos en 0
            *cantidadJugadores = 0;

            if (!sistemaJugadoresValido(MiSistema)) {
                return nullptr;
            }

            // Buscamos si el equipo solicitado realmente existe
            Equipo *EquipoAux = equipos::buscarEquipoPorID(MiSistema, idEquipo);

            // si devuelve nullptr el equipo no existe
            if (EquipoAux == nullptr) {
                return nullptr;
            }

            // Si el equipo no tiene jugadores
            if (EquipoAux->numJugadores == 0) {
                return nullptr;
            }

            // Creamos la lista de jugadores de ese equipo con el tamaño correcto
            Jugador **listaDeJugadores = new Jugador *[EquipoAux->numJugadores];

            // buscamos los jugadores
            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {

                // Si encontramos un jugador asociado a ese id
                if (MiSistema->Jugadores[e].idEquipo == idEquipo) {

                    // Nos aseguramos de no sobrepasarnos el limite
                    if (*cantidadJugadores < EquipoAux->numJugadores) {
                        listaDeJugadores[*cantidadJugadores] = &(MiSistema->Jugadores[e]);
                        (*cantidadJugadores)++;
                    }
                }
            }
            return listaDeJugadores;
        }

        Jugador **listarJugadores(, int *cantidadJugadores) {
            // inicializamos en 0
            *cantidadJugadores = 0;

            if (!sistemaJugadoresValido(MiSistema)) {
                return nullptr;
            }

            // Creamos y reservamos espacio para la lista
            Jugador **listaDeJugadores = new Jugador *[MiSistema->numJugadoresActuales];
            (*cantidadJugadores) = MiSistema->numJugadoresActuales;

            // Buscamos y Agregamos los jugadores
            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                // Añadimos la direccion de los jugadores a la lista
                listaDeJugadores[e] = &(MiSistema->Jugadores[e]);
            }

            return listaDeJugadores;
        }

        bool actualizarJugador(, int ID, Jugador jugadorActualizado) {
            if (!sistemaJugadoresValido(MiSistema)) {
                return false;
            }


            Jugador *ptrJugador = buscarJugadorPorID(MiSistema, ID);
            if (ptrJugador == nullptr) {
                return false;
            }

            // Si el nombre esta duplicado
            if (nombreDuplicadoParaActualizar(MiSistema, jugadorActualizado.nombre, ID)) {
                return false;
            }

            // Si el dorsal esta duplicado
            if (dorsalDuplicadoParaActualizar(MiSistema, jugadorActualizado.dorsal, ID, ptrJugador->idEquipo)) {
                return false;
            }

            // En caso de que sí, actualizamos el jugador
            // Solo actualizamos NOMBRE, EDAD, DORSAL O POSICION
            std::strncpy(ptrJugador->nombre, jugadorActualizado.nombre);
            std::strncpy(ptrJugador->posicion, jugadorActualizado.posicion);
            ptrJugador->edad = jugadorActualizado.edad;
            ptrJugador->dorsal = jugadorActualizado.dorsal;

            return true;
        }

        // Solo podemos eliminar un jugador si su equipo no ha disputado partidos
        bool eliminarJugador(, int ID) {
            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (!sistemaJugadoresValido(MiSistema)) {
                return false;
            }

            int posicion = -1; // -1 porque las posiciones no son negativas

            // Buscamos la posicion del jugador
            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                if (MiSistema->Jugadores[e].ID == ID) {
                    posicion = e;
                    break;
                }
            }

            // Si no encontró nada
            if (posicion == -1) {
                return false;
            }

            // si ya anotó puntos no lo podemos borrar
            if (MiSistema->Jugadores[posicion].puntosAnotados > 0) {
                return false;
            }

            // Guardamos el ID del equipo del jugador a eliminar antes de desplazar el array
            int equipoId = MiSistema->Jugadores[posicion].idEquipo;

            // Lo eliminamos rodando todos los demas jugadores
            for (size_t i = posicion; i < MiSistema->numJugadoresActuales - 1; i++) {
                MiSistema->Jugadores[i] = MiSistema->Jugadores[i + 1];
            }

            // Reducimos el número de jugadores actuales
            MiSistema->numJugadoresActuales--;

            // Disminuimos el contador de jugadores del equipo correspondiente (si existe)
            Equipo *EqAux = equipos::buscarEquipoPorID(MiSistema, equipoId);
            if (EqAux != nullptr && EqAux->numJugadores > 0) {
                EqAux->numJugadores--;
            }

            return true;
        }
    } // namespace jugadores

    namespace partidos {

        int minJugadoresPorDeporte() {
            for (size_t e = 0; e < Validadores::totalDeportes; e++) {
                if (std::strcmp(Validadores::Deportes[e], Validadores::deporteActual) == 0) {
                    return Validadores::MinimoJugadoresPorDeporte[e];
                }
            }
            return 1;
        }

        bool existeID(, const int ID) {

            // Verificamos que todo este inicializado por precaucion
            if (MiSistema == nullptr || MiSistema->Partidos == nullptr) {
                return false;
            }

            // Si no hay partidos devolver false
            if (MiSistema->numPartidosActuales == 0) {
                return false;
            }

            // Recorremos y verficamos que existe el ID
            for (size_t e = 0; e < MiSistema->numPartidosActuales; e++) {
                if (ID == MiSistema->Partidos[e].ID) {
                    return true;
                }
            }
            return false;
        }

        int contarPartidosDeEquipo(const, int idEquipo) {
            if (MiSistema == nullptr || MiSistema->Partidos == nullptr)
                return 0;

            int contador = 0;
            for (int i = 0; i < MiSistema->numPartidosActuales; i++) {
                if (MiSistema->Partidos[i].idEquipoLocal == idEquipo || MiSistema->Partidos[i].idEquipoVisitante == idEquipo) {
                    contador++;
                }
            }
            return contador;
        }

        const char *estadoPartidos[] = {"PROGRAMADO", "JUGADO", "CANCELADO"};

        // Programa un partido entre dos equipos
        // Valida: ambos equipos existen, no son el mismo, no tienen partido ya programado entre sí
        // Retorna puntero al partido creado, o nullptr si falla alguna validación
        Partido *programarPartido(, int IDLocal, int IDVisitante, const char *fecha, const char *descripcion) {
            // Si el sistema no está definido ni los equipos ni los jugadores
            if (MiSistema == nullptr || MiSistema->Jugadores == nullptr || MiSistema->Equipos == nullptr || fecha == nullptr || descripcion == nullptr) {
                return nullptr;
            }

            // Si no hay al menos 2 equipos no se puede programar un partido
            if (MiSistema->numEquiposActuales <= 1) {
                return nullptr;
            }

            // No se puede programar un partido entre el mismo equipo
            if (IDLocal == IDVisitante) {
                return nullptr;
            }

            // Si los equipos no existen
            Equipo *EquipoLocal = Logica::equipos::buscarEquipoPorID(MiSistema, IDLocal);
            Equipo *EquipoVisitante = Logica::equipos::buscarEquipoPorID(MiSistema, IDVisitante);

            // Si uno de los dos equipos no existe cancelamos
            if (EquipoLocal == nullptr || EquipoVisitante == nullptr) {
                return nullptr;
            }

            // Si tienen partidos programados entre sí
            for (size_t e = 0; e < MiSistema->numPartidosActuales; e++) {
                bool tienePartidoEntreSi = ((MiSistema->Partidos[e].idEquipoLocal == IDLocal) && (MiSistema->Partidos[e].idEquipoVisitante == IDVisitante) ||
                                            (MiSistema->Partidos[e].idEquipoLocal == IDVisitante) && (MiSistema->Partidos[e].idEquipoVisitante == IDLocal));

                // Si ya hay un partido programado entre ellos
                if (tienePartidoEntreSi && (std::strcmp(MiSistema->Partidos[e].estado, estadoPartidos[0]) == 0)) {
                    return nullptr;
                }
            }

            /*int minimoRequerido = minJugadoresPorDeporte();
            // Verificar que ambos equipos tengan suficientes jugadores
            if (EquipoLocal->numJugadores < minimoRequerido || EquipoVisitante->numJugadores < minimoRequerido) {
                return nullptr;
            }*/

            // Verificar si hay espacio, y redimensionar si es necesario
            if (MiSistema->numPartidosActuales == MiSistema->capacidadPartidos) {
                redimensionar::rPartidos(MiSistema);
            }

            // Alamacenamos en una variable la nueva posición del partido
            size_t nuevaPosicion = MiSistema->numPartidosActuales;

            // Procedemos a programar el partido
            MiSistema->Partidos[nuevaPosicion].ID = MiSistema->siguienteIdPartido;
            MiSistema->Partidos[nuevaPosicion].idEquipoLocal = IDLocal;
            MiSistema->Partidos[nuevaPosicion].idEquipoVisitante = IDVisitante;
            std::strncpy(MiSistema->Partidos[nuevaPosicion].fecha, fecha);
            std::strncpy(MiSistema->Partidos[nuevaPosicion].descripcion, descripcion);

            // Asignamos los valores iniciales
            std::strncpy(MiSistema->Partidos[nuevaPosicion].estado, estadoPartidos[0]); // PROGRAMADO
            MiSistema->Partidos[nuevaPosicion].puntosLocal = 0;
            MiSistema->Partidos[nuevaPosicion].puntosVisitante = 0;

            // Aumentamos el numero de partidos
            MiSistema->numPartidosActuales++;
            // Aumentamos el ID enseguida
            MiSistema->siguienteIdPartido++;

            return &(MiSistema->Partidos[nuevaPosicion]);
        }

        //   Victoria local  → local  +3 pts, +1 victoria  / visitante +1 derrota
        //   Empate          → ambos  +1 pt,  +1 empate
        //   Victoria visit. → visit. +3 pts, +1 victoria  / local     +1 derrota
        Partido *registrarResultado(, int IDPartido, int puntosLocal, int puntosVisitante) {
            if (!sistemaPartidosValido(MiSistema)) {
                return nullptr;
            }

            // Seguridad
            if (MiSistema->numEquiposActuales <= 1) {
                return nullptr;
            }

            // Por seguridad
            if (puntosLocal < 0 || puntosVisitante < 0) {
                return nullptr;
            }

            int posicion = -1;

            // Buscamos la posicion del partido en el array
            for (size_t e = 0; e < MiSistema->numPartidosActuales; e++) {
                // Si conseguimos una coincidencia con el IDPartido
                if (MiSistema->Partidos[e].ID == IDPartido) {
                    posicion = e;
                    break;
                }
            }

            // Si no se encontró nada o el partido no está en estado programado
            if (posicion == -1 || std::strcmp(MiSistema->Partidos[posicion].estado, Logica::partidos::estadoPartidos[0]) != 0) {
                return nullptr;
            }

            // Si ocurre un emparte en deportes donde no se permiten empates retornamos nullptr
            if (puntosLocal == puntosVisitante) {
                if (std::strcmp(Validadores::deporteActual, "BALONCESTO") == 0 || std::strcmp(Validadores::deporteActual, "TENIS") == 0 ||
                    std::strcmp(Validadores::deporteActual, "VOLEIBOL") == 0 || std::strcmp(Validadores::deporteActual, "BEISBOL") == 0 ||
                    std::strcmp(Validadores::deporteActual, "SOFTBOL") == 0) {
                    return nullptr;
                }
            }

            // Buscamos los equipos mediante sus IDs para validar existencia
            Equipo *EqLocal = equipos::buscarEquipoPorID(MiSistema, MiSistema->Partidos[posicion].idEquipoLocal);
            Equipo *EqVisitante = equipos::buscarEquipoPorID(MiSistema, MiSistema->Partidos[posicion].idEquipoVisitante);

            // Validamos que no apunten a nulo antes de cambiar el estado
            if (EqLocal == nullptr || EqVisitante == nullptr) {
                return nullptr;
            }

            // Con la validación realizada, actualizamos el estado y los puntos del partido
            MiSistema->Partidos[posicion].puntosLocal = puntosLocal;
            MiSistema->Partidos[posicion].puntosVisitante = puntosVisitante;
            std::strncpy(MiSistema->Partidos[posicion].estado, Logica::partidos::estadoPartidos[1]); // JUGADO

            // Si el equipo local ganó
            if (puntosLocal > puntosVisitante) {
                EqLocal->victorias++; // Aumentamos las victorias
                EqLocal->puntos += 3; // Aumentamos los puntos

                EqVisitante->derrotas++; // Aumentamos las derrotas

                // Si el equipo visitante ganó
            } else if (puntosVisitante > puntosLocal) {
                EqVisitante->victorias++; // Aumentamos las victorias
                EqVisitante->puntos += 3; // Aumentamos los puntos

                EqLocal->derrotas++; // Aumentamos las derrotas

                // Si empataron en un deporte permitido
            } else {
                EqLocal->empates++;
                EqLocal->puntos += 1;

                EqVisitante->empates++;
                EqVisitante->puntos += 1;
            }

            // Añadimos los puntos a favor y en contra
            EqLocal->puntosAFavor += puntosLocal;
            EqLocal->puntosEnContra += puntosVisitante;
            EqLocal->jugados++;
            EqVisitante->puntosAFavor += puntosVisitante;
            EqVisitante->puntosEnContra += puntosLocal;
            EqVisitante->jugados++;

            return &(MiSistema->Partidos[posicion]);
        }

        Partido *buscarPartidoPorID(, int IDPartido) {
            if (!sistemaPartidosValido(MiSistema)) {
                return nullptr;
            }

            for (size_t e = 0; e < MiSistema->numPartidosActuales; e++) {
                if (MiSistema->Partidos[e].ID == IDPartido) {
                    return &(MiSistema->Partidos[e]);
                }
            }

            return nullptr;
        }

        Partido **listarPartidos(, int *cantidad) {
            if (!sistemaPartidosValido(MiSistema)) {
                return nullptr;
            }

            // inicializamos el array y las variables
            *cantidad = MiSistema->numPartidosActuales;
            Partido **listaDePartidos = new Partido *[*cantidad];

            // Recorremos el array y almacenamos las direcciones de memoria de cada objet0
            for (size_t e = 0; e < *cantidad; e++) {
                listaDePartidos[e] = &(MiSistema->Partidos[e]);
            }

            return listaDePartidos;
        }

        Partido **listarPartidosPorEquipo(, int idEquipo, int *cantidad) {
            *cantidad = 0;
            if (!sistemaPartidosValido(MiSistema)) {
                return nullptr;
            }

            Partido **listaPartidosPorEquipo = new Partido *[MiSistema->numPartidosActuales];

            // Buscamos los partidos en donde haya participado dicho equipo
            for (size_t e = 0; e < MiSistema->numPartidosActuales; e++) {
                // Si encontramos coincidencias en alguno en local o vistante lo almacenamos
                if (MiSistema->Partidos[e].idEquipoLocal == idEquipo || MiSistema->Partidos[e].idEquipoVisitante == idEquipo) {
                    listaPartidosPorEquipo[*cantidad] = &(MiSistema->Partidos[e]);
                    (*cantidad)++;
                }
            }

            return listaPartidosPorEquipo;
        }

        // Retorna array de partidos con ese estado ("PROGRAMADO", "JUGADO", "CANCELADO")
        // El llamador libera el array con delete[]
        Partido **listarPartidosPorSuEstado(, const char *estado, int *cantidad) {
            *cantidad = 0;
            if (!sistemaPartidosValido(MiSistema)) {
                return nullptr;
            }

            Partido **listaPartidosPorEstado = new Partido *[MiSistema->numPartidosActuales];

            for (size_t e = 0; e < MiSistema->numPartidosActuales; e++) {
                if (std::strcmp(MiSistema->Partidos[e].estado, estado) == 0) {
                    listaPartidosPorEstado[*cantidad] = &(MiSistema->Partidos[e]);
                    (*cantidad)++;
                }
            }

            return listaPartidosPorEstado;
        }

        // Cancela un partido: cambia estado a "CANCELADO"
        // Si el partido ya fue JUGADO, revierte las estadísticas de ambos equipos
        // Retorna true si se canceló, false si no existe o ya estaba cancelado
        bool cancelarPartido(, int IDPartido) {
            if (!sistemaPartidosValido(MiSistema)) {
                return false;
            }

            // Primero buscamos la posicion del partido en memoria
            int posicion = -1;
            for (size_t e = 0; e < MiSistema->numPartidosActuales; e++) {
                // Si conseguimos una coincidencia
                if (MiSistema->Partidos[e].ID == IDPartido) {
                    posicion = e;
                    break;
                }
            }

            // Si no se encontró nada
            if (posicion == -1) {
                return false;
            }

            // Si el estado del partido es CANCELADO no lo procesamos
            if ((std::strcmp(MiSistema->Partidos[posicion].estado, estadoPartidos[2]) == 0)) {
                return false;
            }

            // Si está PROGRAMADO lo cambiamos a cancelado solamente
            if ((std::strcmp(MiSistema->Partidos[posicion].estado, estadoPartidos[0]) == 0)) {
                std::strncpy(MiSistema->Partidos[posicion].estado, estadoPartidos[2]);
                return true;
            }

            // Buscamos los equipos para revertir las estadisticas
            Equipo *EqLocal = equipos::buscarEquipoPorID(MiSistema, MiSistema->Partidos[posicion].idEquipoLocal);
            Equipo *EqVisitante = equipos::buscarEquipoPorID(MiSistema, MiSistema->Partidos[posicion].idEquipoVisitante);

            // Si no se encotraron los equipos
            if (EqLocal == nullptr || EqVisitante == nullptr) {
                return false;
            }

            // Verificamos quien gano el partido

            // Si ganó el local
            if (MiSistema->Partidos[posicion].puntosLocal > MiSistema->Partidos[posicion].puntosVisitante) {
                // Modificamos primero el equipo local
                EqLocal->victorias--;
                EqLocal->puntos -= 3;

                // Modificamos el visitante
                EqVisitante->derrotas--;

                // Si ganó el visitante
            } else if (MiSistema->Partidos[posicion].puntosLocal < MiSistema->Partidos[posicion].puntosVisitante) {
                // Modificamos primero el equipo visitante
                EqVisitante->victorias--;
                EqVisitante->puntos -= 3;

                // Modificamos el Local
                EqLocal->derrotas--;

                // Si fue empate
            } else {
                EqVisitante->empates--;
                EqVisitante->puntos -= 1;
                EqLocal->empates--;
                EqLocal->puntos -= 1;
            }

            // Cambiamos el estado a CANCELADO
            std::strncpy(MiSistema->Partidos[posicion].estado, estadoPartidos[2]); // CANCELADO

            // Revertimos los puntos a favor y en contra
            EqLocal->puntosAFavor -= MiSistema->Partidos[posicion].puntosLocal;
            EqLocal->puntosEnContra -= MiSistema->Partidos[posicion].puntosVisitante;
            EqVisitante->puntosAFavor -= MiSistema->Partidos[posicion].puntosVisitante;
            EqVisitante->puntosEnContra -= MiSistema->Partidos[posicion].puntosLocal;

            // Restamos los partidos jugados
            EqLocal->jugados--;
            EqVisitante->jugados--;

            return true;
        }
    } // namespace partidos

    void definirFormato(Torneo &torneoAux, int opcion) {
        if (opcion == 1) {
            std::strncpy(torneoAux.formato, "GRUPOS");
        } else if (opcion == 2) {
            std::strncpy(torneoAux.formato, "ELIMINATORIA");
        }
    }

} // namespace Logica

// ============================================//
//   6. PRESENTACION                           //
// ============================================//

namespace Presentacion {

    namespace equipos {
        // Recolectamos los datos para registrar el equipo
        void RegistrarEquipos(const char *nombreArchivo) {
            bool flagError = false;
            Equipo nuevo;
            char entrenadorAux[100];
            char ciudadAux[50];
            char fechaAux[11];
            Equipo *nuevoEquipo = nullptr;
            char confirmacion;

            // Recolectamos el nombre del Equipo
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(nuevo.nombre, sizeof(Equipo::nombre), "Ingrese el nombre del Equipo: ", Validadores::Nombres);

                // Validamos nombre duplicado
                if (Logica::nombreDuplicado<Equipo>(nombreArchivo, nuevo.nombre, &Equipo::nombre)) {
                    std::cout << "Error, el nombre '" << nuevo.nombre << "' ya está en uso\n";
                    flagError = true;
                    Auxiliares::waitfor(3000);
                    continue;
                }
                Auxiliares::waitfor(2000);
            } while (flagError);


            // Recolectamos el nombre del entrenador del nuevo Equipo
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(nuevo.entrenador, sizeof(Equipo::entrenador), "Ingrese el nombre del Entrenador: ", Validadores::Nombres);

                // Validamos nombre duplicado
                if (Logica::equipos::nombreEntrenadorDuplicado(MiSistema, entrenadorAux)) {
                    std::cout << "Error, el nombre '" << entrenadorAux << "' ya direge otro equipo\n";
                    flagError = true;
                    Auxiliares::waitfor(3000);
                    continue;
                }
                Auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la fecha de registro del equipo
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(fechaAux, sizeof(Equipo::fecha), "Ingrese la fecha de Registro del Equipo: ", Validadores::fechaValidaRegistroDeJugadorOEquipo);
            Auxiliares::waitfor(2000);
            Auxiliares::limpiarPantalla();

            // Recolectamos la ciudad del Equipo
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(ciudadAux, 50, "Ingrese el nombre de la Ciudad del Equipo: ", Validadores::Nombres);
            Auxiliares::waitfor(2000);
            Auxiliares::limpiarPantalla();

            // Pedimos la confirmacion al usuario
            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea registrar este equipo? (S/N): ");
            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                Auxiliares::limpiarPantalla();
                // Agregamos el equipo a nuestro array dinamico
                nuevoEquipo = Logica::equipos::registrarEquipo(MiSistema, nombreAux, entrenadorAux, ciudadAux, fechaAux);

                // Si el equipo no se creo
                if (nuevoEquipo == nullptr) {
                    std::cout << "Error: No se logró registrar el equipo\n";
                    return;
                }

                // Si el equipo se creo conn éxito
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        EQUIPO REGISTRADO CON ÉXITO        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Mostramos los datos ingresados
                std::cout << "Torneo: " << MiSistema->torneo.nombre << std::endl;
                std::cout << "Nombre del Equipo: " << nuevoEquipo->nombre << std::endl;
                std::cout << "Entrenador del Equipo: " << nuevoEquipo->entrenador << std::endl;
                std::cout << "Ciudad del Equipo: " << nuevoEquipo->ciudad << std::endl;
                std::cout << "Fecha de Registro del Equipo: " << nuevoEquipo->fechaRegistro << std::endl;
                std::cout << "Id del Equipo: " << nuevoEquipo->ID << std::endl;

            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                Auxiliares::limpiarPantalla();
                std::cout << "\nRegistro de Equipo Cancelado\n";
            } else {
                Auxiliares::limpiarPantalla();
                std::cout << "\nError: No se ingresó una opción correcta (S/N)\n";
                std::cout << "\nRegistro de Equipo Cancelado\n";
            }
            Auxiliares::pausarPrograma();
        }

        void buscarEquipoPorID() {
            int ID = 0;
            Equipo *EquipoBuscado = nullptr;
            Auxiliares::limpiarPantalla();

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║        BUSQUEDA DE EQUIPOS POR ID         ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarDatos(ID, "Ingrese el ID: ", Validadores::IDvalido);

            EquipoBuscado = Logica::equipos::buscarEquipoPorID(MiSistema, ID);

            if (EquipoBuscado == nullptr) {
                std::cout << "El equipo de ID " << ID << " no fue encontrado\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║             EQUIPO ENCONTRADO             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  ID del Equipo:       " << EquipoBuscado->ID << "\n";
                std::cout << "  Nombre:              " << EquipoBuscado->nombre << "\n";
                std::cout << "  Entrenador:          " << EquipoBuscado->entrenador << "\n";
                std::cout << "  Ciudad Origen:       " << EquipoBuscado->ciudad << "\n";
                std::cout << "  Fecha de Registro:   " << EquipoBuscado->fechaRegistro << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Estadísticas en el Torneo:\n";
                std::cout << "    Puntos Totales:    " << EquipoBuscado->puntos << "\n";
                std::cout << "    Victorias:         " << EquipoBuscado->victorias << "\n";
                std::cout << "    Empates:           " << EquipoBuscado->empates << "\n";
                std::cout << "    Derrotas:          " << EquipoBuscado->derrotas << "\n";
                std::cout << "    Puntos a Favor:    " << EquipoBuscado->puntosAFavor << "\n";
                std::cout << "    Puntos en Contra:  " << EquipoBuscado->puntosEnContra << "\n";
            }
            std::cout << "-------------------------------------------------------------\n\n";
            Auxiliares::pausarPrograma();
        }

        void buscarEquiposPorSubCadena() {
            Auxiliares::limpiarPantalla();
            int contEquiposEncontrados = 0;
            char subcadena[100];
            Equipo **arrayDePunterosAEquipos = nullptr;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║      BUSQUEDA DE EQUIPOS POR NOMBRE       ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(subcadena, 100, "Escribe el nombre (o parte del nombre) del equipo que buscas: ", Validadores::Nombres);
            Auxiliares::waitfor(1000);
            std::cout << "Buscando..." << std::endl;

            // Llamamos a la funcion de busqueda
            arrayDePunterosAEquipos = Logica::equipos::buscarEquipoPorSubCadena(MiSistema, subcadena, &contEquiposEncontrados);

            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            // Si no se enocontro ningun equipo
            if (arrayDePunterosAEquipos == nullptr || contEquiposEncontrados <= 0) {
                std::cout << "No se encontro ninguna coincidencia con: '" << subcadena << "'\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          RESULTADOS ENCONTRADOS           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "----------------------------------------------------------------------------\n";
                std::cout << "Se encontraron " << contEquiposEncontrados << " coincidencia(s):\n";
                std::cout << "----------------------------------------------------------------------------\n";

                for (size_t e = 0; e < contEquiposEncontrados; e++) {
                    std::cout << std::endl << e + 1 << ".\n";
                    std::cout << "   Nombre: " << (arrayDePunterosAEquipos[e])->nombre << std::endl;
                    std::cout << "   ID: " << (arrayDePunterosAEquipos[e])->ID << std::endl;
                }
                std::cout << "---------------------------------------------------------------------------\n";
            }

            // Liberamos el array
            if (arrayDePunterosAEquipos != nullptr) {
                delete[] arrayDePunterosAEquipos;
                arrayDePunterosAEquipos = nullptr;
            }

            Auxiliares::pausarPrograma();
        }

        void listarEquipos() {
            Auxiliares::limpiarPantalla();

            // Inicializamos las variables a utilizar
            int cantEquipos = 0;
            Equipo **listaDePtrAEquipos = nullptr;

            // llamamos a la funcion que nos devuelve la lista de punteros
            listaDePtrAEquipos = Logica::equipos::listarEquipos(MiSistema, &cantEquipos);

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            LISTA DE EQUIPOS               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Si no se consiguieron equipos
            if (listaDePtrAEquipos == nullptr || cantEquipos == 0) {
                std::cout << "No hay equipos registrados en el sistema actualmente.\n";
            } else {
                std::cout << "----------------------------------------------------------------------------------------- \n";
                std::cout << " " << std::left << std::setw(4) << "N°"
                          << " | " << std::setw(6) << "ID"
                          << " | " << std::setw(35) << "Nombre"
                          << " | " << std::setw(30) << "Ciudad"
                          << " | " << "Puntos" << "\n";

                std::cout << "----------------------------------------------------------------------------------------- \n";

                for (size_t e = 0; e < cantEquipos; e++) {
                    // Filas de datos con exactamente los mismos anchos modificados
                    std::cout << " " << std::left << std::setw(4) << (e + 1) << " | " << std::setw(6) << listaDePtrAEquipos[e]->ID << " | " << std::setw(35)
                              << listaDePtrAEquipos[e]->nombre << " | " << std::setw(30) << listaDePtrAEquipos[e]->ciudad << " | " << listaDePtrAEquipos[e]->puntos << "\n";
                }

                std::cout << "----------------------------------------------------------------------------------------- \n";
            }

            if (listaDePtrAEquipos != nullptr) {
                delete[] listaDePtrAEquipos;
                listaDePtrAEquipos = nullptr;
            }

            Auxiliares::pausarPrograma();
        }

        void mostrarTablaDePosiciones() {
            Auxiliares::limpiarPantalla();

            // Inicializamos las variables a utilizar
            int cantEquipos = 0;
            Equipo **TablaDePosiciones = nullptr;

            TablaDePosiciones = Logica::equipos::TablaDePosiciones(MiSistema, &cantEquipos);

            if (TablaDePosiciones == nullptr || cantEquipos == 0) {
                std::cout << "No hay Equipos Disponibles\n";
            } else {
                char nombreTorneo[100];
                strncpy(nombreTorneo, MiSistema->torneo.nombre);
                Auxiliares::toMayus(nombreTorneo);
                std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║                             TABLA DE POSICIONES                                     ║\n";
                std::cout << "║               " << std::left << std::setw(70) << nombreTorneo << "║\n";
                std::cout << "╠════╦═══════════════════════════════════════════════╦═════╦═══╦═══╦═══╦════╦════╦════╣\n";
                std::cout << "║ #  ║ Equipo                                        ║ PTS ║ J ║ G ║ E ║ D  ║ GF ║ GC ║\n";
                std::cout << "╠════╬═══════════════════════════════════════════════╬═════╬═══╬═══╬═══╬════╬════╬════╣\n";

                for (size_t e = 0; e < cantEquipos; e++) {
                    std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(45) << TablaDePosiciones[e]->nombre << " ║ " << std::right
                              << std::setw(3) << TablaDePosiciones[e]->puntos << " ║ " << std::right << std::setw(1) << TablaDePosiciones[e]->jugados << " ║ " << std::right
                              << std::setw(1) << TablaDePosiciones[e]->victorias << " ║ " << std::right << std::setw(1) << TablaDePosiciones[e]->empates << " ║ " << std::right
                              << std::setw(2) << TablaDePosiciones[e]->derrotas << " ║ " << std::right << std::setw(2) << TablaDePosiciones[e]->puntosAFavor << " ║ " << std::right
                              << std::setw(2) << TablaDePosiciones[e]->puntosEnContra << " ║\n";
                }
                std::cout << "╚════╩═══════════════════════════════════════════════╩═════╩═══╩═══╩═══╩════╩════╩════╝\n";
                std::cout << "\nReferencia: PTS=Puntos  J=Jugados  G=Ganados  E=Empatados\n";
                std::cout << "            D=Derrotas  GF=puntos a Favor  GC=puntos en Contra\n\n";
            }

            // liberamos
            if (TablaDePosiciones != nullptr) {
                delete[] TablaDePosiciones;
                TablaDePosiciones = nullptr;
            }
            Auxiliares::pausarPrograma();
        }

        void actualizarEquipo() {
            Auxiliares::limpiarPantalla();
            // Datos Actualizables:
            // Nombre del Equipo, Nombre del Entrenador, Nombre de la Ciudad

            // Variables
            char nombreAux[100];
            char entrenadorAux[100];
            char ciudadAux[100];
            bool actualizado = false;
            int ID = 0;
            bool flagError = false;
            char confirmacion;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            // Pedimos el ID del equipo que desean actualizar
            Auxiliares::ingresarDatos(ID, "Ingresa el ID del equipo que desea actualizar: ", Validadores::IDvalido);

            // si no Existe el ID
            if (!Logica::equipos::existeID(MiSistema, ID)) {
                std::cout << "Error el ID '" << ID << "' no pertenece a ningún equipo registrado\n";
                Auxiliares::pausarPrograma();
                return;
            }

            do {
                flagError = false;
                Auxiliares::waitfor(1500);
                Auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(nombreAux, 100, "Ingrese el nuevo nombre del Equipo: ", Validadores::Nombres);
                if (Logica::equipos::nombreDuplicadoParaActualizar(MiSistema, nombreAux, ID)) {
                    std::cout << "Error: ya hay otro equipo con el nombre '" << nombreAux << "'\n";
                    flagError = true;
                }
                Auxiliares::waitfor(1500);
                Auxiliares::limpiarPantalla();
            } while (flagError);

            do {
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(entrenadorAux, 100, "Ingrese el nuevo nombre del Entrenador del Equipo: ", Validadores::Nombres);
                if (Logica::equipos::nombreEntrenadorDuplicadoParaActualizar(MiSistema, entrenadorAux, ID)) {
                    std::cout << "Ya el entrenador '" << entrenadorAux << "' dirige otro equipo\n";
                    flagError = true;
                }
                Auxiliares::waitfor(1500);
                Auxiliares::limpiarPantalla();
            } while (flagError);

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(ciudadAux, 100, "Ingrese el nuevo de la Ciudad del Equipo: ", Validadores::Nombres);
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea aplicar estos cambios? (S/N): ");
            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                actualizado = Logica::equipos::actualizarEquipo(MiSistema, ID, nombreAux, entrenadorAux, ciudadAux);
                if (!actualizado) {
                    std::cout << "Se produjo un error a la hora de actualizar el equipo\n";
                } else {
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << "           Equipo Actualizado con Éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << "Nuevo Nombre del Equipo: " << nombreAux << std::endl;
                    std::cout << "Nuevo Nombre del Entrenador del Equipo: " << entrenadorAux << std::endl;
                    std::cout << "Nuevo Nombre de la Ciudad del Equipo: " << ciudadAux << std::endl;
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "Actualización de Datos Cancelada\n";
            } else {
                std::cout << "Error: No se ingresó una opción correcta (S/N)\n";
                std::cout << "Actualización de Datos Cancelada\n";
            }
            Auxiliares::pausarPrograma();
        }

        void eliminarEquipo() {
            Auxiliares::limpiarPantalla();

            bool eliminado = false;
            int ID = 0;
            char confirmacion;
            Equipo *EqAux = nullptr;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             ELIMINAR EQUIPOS              ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Pedimos el ID del equipo que desean actualizar
            Auxiliares::ingresarDatos(ID, "Ingresa el ID del equipo que desea eliminar: ", Validadores::IDvalido);

            // si no Existe el ID
            if (!Logica::equipos::existeID(MiSistema, ID)) {
                std::cout << "Error el ID '" << ID << "' no pertenece a ningún equipo registrado\n";
                Auxiliares::pausarPrograma();
                return;
            }
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            // buscamos el equipo mediante el ID ingresado
            EqAux = Logica::equipos::buscarEquipoPorID(MiSistema, ID);

            if (EqAux == nullptr) {
                std::cout << "Error, no se encontró el equipo que se desea eliminar";
                return;
            }

            int cantPartidosAsociados = Logica::partidos::contarPartidosDeEquipo(MiSistema, ID);
            if (cantPartidosAsociados > 0) {
                std::cout << " ADVERTENCIA: El equipo tiene " << cantPartidosAsociados << " partidos asociados.\n";
                std::cout << " No puede ser eliminado\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             ELIMINAR EQUIPOS              ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << "Equipo Seleccionado: \n\n";
            std::cout << "Nombre: " << EqAux->nombre << std::endl;
            std::cout << "Entrenador: " << EqAux->entrenador << std::endl;
            std::cout << "Ciudad: " << EqAux->ciudad << std::endl;

            Auxiliares::ingresarDatos(confirmacion, "¿Desea eliminar el equipo? (S/N): ");
            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                eliminado = Logica::equipos::eliminarEquipo(MiSistema, ID);
                if (!eliminado) {
                    std::cout << "Se produjo un error a la hora de eliminar el equipo\n";
                    std::cout << "Por favor revise que el equipo a eliminar no tenga partidos Asociados\n";
                } else {
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << "           Equipo Eliminado con Éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "Eliminacion de Equipo Cancelada\n";
            } else {
                std::cout << "Error: No se ingresó una opción correcta (S/N)\n";
                std::cout << "Eliminacion de Equipo Cancelada\n";
            }
            Auxiliares::pausarPrograma();
        }

    } // namespace equipos

    namespace jugadores {

        void RegistrarJugador() {
            Auxiliares::limpiarPantalla();
            bool flagError = false;
            char nombreAux[100];
            char cedulaAux[20];
            int edadAux = 0;
            char fechaAux[11];
            Jugador *nuevo = nullptr;
            char confirmacion;
            int dorsal = 0;
            int opcion = 0;
            char posicionAux[25];
            int idEquipoAux = 0;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Recolectamos el ID del equipo
            do {
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarDatos(idEquipoAux, "Ingrese el ID del equipo al que pertenece el jugador: ", Validadores::IDvalido);

                /// Si el ID no existe dentro de los equipos
                if (!Logica::equipos::existeID(MiSistema, idEquipoAux)) {
                    std::cout << "Error el ID '" << idEquipoAux << "' no pertenece a ningun equipo\n";
                    flagError = true;
                    Auxiliares::waitfor(2500);
                    continue;
                }
                Auxiliares::waitfor(1500);
            } while (flagError);

            // Recolectamos el nombre del Jugador
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(nombreAux, 100, "Ingrese el nombre del Jugador: ", Validadores::Nombres);

                // Validamos nombre duplicado
                if (Logica::jugadores::nombreDuplicado(MiSistema, nombreAux)) {
                    std::cout << "Error, el nombre '" << nombreAux << "' ya está en uso.\n";
                    flagError = true;
                    Auxiliares::waitfor(3000);
                    continue;
                }
                Auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la Edad
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarDatos(edadAux, "Ingrese la edad del Jugador: ", Validadores::Edad);
            Auxiliares::waitfor(1500);

            // Recolectamos la cedula
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(cedulaAux, 20, "Ingrese la cedula del jugador: ", Validadores::Cedulas);

                // Validamos nombre duplicado
                if (Logica::jugadores::CedulaRepetida(MiSistema, cedulaAux)) {
                    std::cout << " Error, la cedula '" << cedulaAux << "' ya le pertenece a otro jugador\n";
                    flagError = true;
                    Auxiliares::waitfor(3000);
                    continue;
                }
                Auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la Posicion
            do {
                flagError = false;
                Auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte actual del Torneo: " << MiSistema->torneo.deporte << "\n\n";

                const char **matrizDeporteActual = nullptr;

                for (size_t i = 0; i < Validadores::totalDeportes; i++) {
                    // Recorremos el array de matrices y verificamos lo que hay en la posicion 0
                    if (std::strcmp(Validadores::MapaDeportes[i][0], MiSistema->torneo.deporte) == 0) {
                        matrizDeporteActual = Validadores::MapaDeportes[i];
                        break;
                    }
                }

                // Mostramos las posiciones disponibles de esa fila
                std::cout << " Seleccione la posición del jugador:\n";

                // Para saber el numero de posiciones del deporte
                int contadorPosiciones = 0;

                for (size_t j = 1; matrizDeporteActual[j] != nullptr; j++) {
                    std::cout << " " << j << ". " << matrizDeporteActual[j] << "\n";
                    contadorPosiciones++;
                }
                std::cout << "\n";

                Auxiliares::ingresarDatos(opcion, "Seleccione una opción: ");

                // Verificamos que esté en el rango de opciones
                if (opcion >= 1 && opcion <= contadorPosiciones) {
                    // si es correcta guardamos la posicion
                    std::strncpy(posicionAux, matrizDeporteActual[opcion]);
                } else {
                    std::cout << "Error: Opción inválida. Por favor, intente de nuevo.\n";
                    flagError = true;
                    Auxiliares::waitfor(2000);
                }
            } while (flagError);


            // Recolectamos la fecha de registro del jugador
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(fechaAux, 11, "Ingrese la fecha de Registro del Jugador: ", Validadores::fechaValidaRegistroDeJugadorOEquipo);
            Auxiliares::waitfor(2000);
            Auxiliares::limpiarPantalla();

            // Recolectamos el dorsal del Jugador

            do {
                flagError = false;
                Equipo *eqAux = Logica::equipos::buscarEquipoPorID(MiSistema, idEquipoAux);
                Auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarDatos(dorsal, "Ingrese el Dorsal del jugador: ", Validadores::Dorsal);
                if (Logica::jugadores::DorsalDuplicado(MiSistema, dorsal, idEquipoAux)) {
                    std::cout << "Error el dorsal '" << dorsal << "' ya está ocupado en el equipo '" << eqAux->nombre << "'.\n";
                    Auxiliares::waitfor(2500);
                    flagError = true;
                }
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
            } while (flagError);

            // Pedimos la confirmacion al usuario
            Auxiliares::limpiarPantalla();
            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea registrar este jugador? (S/N): ");

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                // agregamos el jugador
                nuevo = Logica::jugadores::agregarJugador(MiSistema, idEquipoAux, nombreAux, cedulaAux, posicionAux, edadAux, dorsal, fechaAux);
                Auxiliares::waitfor(1200);
                Auxiliares::limpiarPantalla();
                // Si no se agregó
                if (nuevo == nullptr) {
                    std::cout << "ERROR al registrar al jugador.\n";
                    Auxiliares::pausarPrograma();
                    return;
                }

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║       JUGADOR REGISTRADO CON ÉXITO        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Torneo: " << MiSistema->torneo.nombre << std::endl;
                std::cout << " ID del Jugador: " << nuevo->ID << std::endl;
                std::cout << " Nombre del Jugador: " << nuevo->nombre << std::endl;
                std::cout << " Cédula: " << nuevo->cedula << std::endl;
                std::cout << " Edad: " << nuevo->edad << " años" << std::endl;
                std::cout << " Posición: " << nuevo->posicion << std::endl;
                std::cout << " Dorsal: " << nuevo->dorsal << std::endl;
                std::cout << " ID del Equipo asignado: " << nuevo->idEquipo << std::endl;
                std::cout << " Fecha de Registro: " << nuevo->fechaRegistro << std::endl;
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "Registro de Jugador Cancelado.\n";
            } else {
                std::cout << "ERROR: Opción incorrecta (S/N).\nRegistro de Jugador Cancelado.\n";
            }
            Auxiliares::pausarPrograma();
        }

        void buscarJugadorID() {
            Auxiliares::limpiarPantalla();
            int ID = 0;
            Jugador *jugadorBuscado = nullptr;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores registrados
            if (MiSistema->numJugadoresActuales == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║       BUSQUEDA DE JUGADORES POR ID        ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::ingresarDatos(ID, "Ingrese el ID: ", Validadores::IDvalido);

            jugadorBuscado = Logica::jugadores::buscarJugadorPorID(MiSistema, ID);

            // si no encontro un jugador
            if (jugadorBuscado == nullptr) {
                std::cout << "Error no hay ningun jugador registrado con el ID '" << ID << "'\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            JUGADOR ENCONTRADO             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  ID del Jugador:       " << jugadorBuscado->ID << "\n";
                std::cout << "  Nombre:               " << jugadorBuscado->nombre << "\n";
                std::cout << "  Cédula:               " << jugadorBuscado->cedula << "\n";
                std::cout << "  Edad:                 " << jugadorBuscado->edad << " años\n";
                std::cout << "  Posición:             " << jugadorBuscado->posicion << "\n";
                std::cout << "  Dorsal:               " << jugadorBuscado->dorsal << "\n";
                std::cout << "  ID Equipo Asignado:   " << jugadorBuscado->idEquipo << "\n";
                std::cout << "  Fecha de Registro:    " << jugadorBuscado->fechaRegistro << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Estadísticas en el Torneo:\n";
                std::cout << "    Puntos Anotados:    " << jugadorBuscado->puntosAnotados << "\n";
                std::cout << "-------------------------------------------------------------\n";
            }
            Auxiliares::pausarPrograma();
        }

        void buscarJugadorPorNombre() {
            Auxiliares::limpiarPantalla();
            char subcadena[100];
            int cantidadEncontrados = 0;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores registrados
            if (MiSistema->numJugadoresActuales == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║      BÚSQUEDA DE JUGADORES POR NOMBRE     ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::ingresarCadena(subcadena, 100, "Escribe el nombre (o parte del nombre) del jugador que buscas: ", Validadores::Nombres);
            Auxiliares::waitfor(1000);
            std::cout << "Buscando..." << std::endl;

            // llamamos a la funcion buscar por nombre y almacenamos el resultado
            Jugador **listaDePunterosAJugadores = Logica::jugadores::buscarJugadoresPorNombre(MiSistema, subcadena, &cantidadEncontrados);

            // Si no encontró nada o no devolvió nada
            if (listaDePunterosAJugadores == nullptr || cantidadEncontrados <= 0) {
                std::cout << "\nNo se encontraron jugadores que coincidan con '" << subcadena << "'.\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          COINCIDENCIAS ENCONTRADAS        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Se encontraron " << cantidadEncontrados << " jugador(es):\n";

                for (int e = 0; e < cantidadEncontrados; e++) {
                    std::cout << "-------------------------------------------------------------\n";
                    std::cout << "  ID: " << listaDePunterosAJugadores[e]->ID << " | Nombre: " << listaDePunterosAJugadores[e]->nombre << "\n";
                    std::cout << "  Cédula: " << listaDePunterosAJugadores[e]->cedula << " | Dorsal: [" << listaDePunterosAJugadores[e]->dorsal << "]\n";
                    std::cout << "  Edad: " << listaDePunterosAJugadores[e]->edad << " años | Posición: " << listaDePunterosAJugadores[e]->posicion << "\n";
                    std::cout << "  ID Equipo: " << listaDePunterosAJugadores[e]->idEquipo << " | Puntos Anotados: " << listaDePunterosAJugadores[e]->puntosAnotados << "\n";
                }
                std::cout << "-------------------------------------------------------------\n";
            }

            // Liberamos la memoria
            if (listaDePunterosAJugadores != nullptr) {
                delete[] listaDePunterosAJugadores;
                listaDePunterosAJugadores = nullptr;
            }

            std::cout << "\n";
            Auxiliares::pausarPrograma();
        }

        void mostrarJugadoresPorEquipo() {
            Auxiliares::limpiarPantalla();
            int idEquipo = 0;
            int cantidadEncontrados = 0;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores registrados
            if (MiSistema->numJugadoresActuales == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Pedimos el ID del equipo a consultar
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║      MOSTRAR JUGADORES POR EQUIPO         ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::ingresarDatos(idEquipo, "Ingrese el ID del Equipo: ", Validadores::IDvalido);

            // Buscamos el equipo primero
            Equipo *equipoBuscado = Logica::equipos::buscarEquipoPorID(MiSistema, idEquipo);

            // Si no encontramos un equipo con ese ID enviamos error
            if (equipoBuscado == nullptr) {
                std::cout << "\nError: El equipo con ID '" << idEquipo << "' no existe.\n";
                Auxiliares::pausarPrograma();
                return;
            }

            Auxiliares::limpiarPantalla();
            Auxiliares::waitfor(1000);
            std::cout << "\nBuscando jugadores...\n\n";

            // obtenemos la lista de punteros
            Jugador **listaJugadores = Logica::jugadores::listarJugadoresPorEquipo(MiSistema, idEquipo, &cantidadEncontrados);

            Auxiliares::limpiarPantalla();
            Auxiliares::waitfor(2000);

            // Si no obtenemos nada
            if (listaJugadores == nullptr || cantidadEncontrados == 0) {
                std::cout << "El equipo '" << equipoBuscado->nombre << "' actualmente no tiene jugadores registrados.\n";
            } else {
                std::cout << "╔═════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║ EQUIPO: " << std::left << std::setw(71) << equipoBuscado->nombre << " ║\n";
                std::cout << "║ ID DEL EQUIPO: " << std::left << std::setw(64) << equipoBuscado->ID << " ║\n";
                std::cout << "╠════╦══════════════════════════════════════════╦═══════════════╦═════╦═══════════╣\n";
                std::cout << "║ ID ║ Nombre                                   ║ Posición      ║ Edad║ Dorsal    ║\n";
                std::cout << "╠════╬══════════════════════════════════════════╬═══════════════╬═════╬═══════════╣\n";

                // Imprimimos los jugadores
                for (size_t e = 0; e < cantidadEncontrados; e++) {
                    std::cout << "║ " << std::right << std::setw(2) << listaJugadores[e]->ID << " ║ " << std::left << std::setw(40) << listaJugadores[e]->nombre << " ║ "
                              << std::left << std::setw(13) << listaJugadores[e]->posicion << " ║ " << std::right << std::setw(3) << listaJugadores[e]->edad << " ║ [" << std::right
                              << std::setw(2) << listaJugadores[e]->dorsal << "]      ║\n";
                }
                std::cout << "╚════╩══════════════════════════════════════════╩═══════════════╩═════╩═══════════╝\n";
                std::cout << " Total de jugadores en el equipo: " << cantidadEncontrados << "\n";
            }

            // liberamos
            if (listaJugadores != nullptr) {
                delete[] listaJugadores;
                listaJugadores = nullptr;
            }

            std::cout << "\n";
            Auxiliares::pausarPrograma();
        }

        void mostrarListaDeJugadores() {
            Auxiliares::limpiarPantalla();
            int cantidadEncontrados = 0;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            Auxiliares::limpiarPantalla();
            Auxiliares::waitfor(1500);
            std::cout << "Cargando todos los jugadores...\n\n";
            Auxiliares::limpiarPantalla();
            Auxiliares::waitfor(2500);

            // Llamamos a tu función lógica (asumiendo que sigue el mismo patrón de firmas)
            Jugador **listaJugadores = Logica::jugadores::listarJugadores(MiSistema, &cantidadEncontrados);

            // Validamos si el sistema tiene jugadores cargados
            if (listaJugadores == nullptr || cantidadEncontrados == 0) {
                std::cout << "No existen jugadores registrados en el sistema actualmente.\n";
            } else {
                std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║ SPORT G&C TOURNAMENTS                                                             ║\n";
                std::cout << "║ TORNEO: " << std::left << std::setw(73) << MiSistema->torneo.nombre << " ║\n";
                std::cout << "║ LISTA DE JUGADORES REGISTRADOS                                                    ║\n";
                std::cout << "╠════╦══════════════════════╦══════════════════════╦═══════════════╦═════╦══════════╣\n";
                std::cout << "║ ID ║ Nombre               ║ Equipo               ║ Posición      ║ Edad║ Dorsal   ║\n";
                std::cout << "╠════╬══════════════════════╬══════════════════════╬═══════════════╬═════╬══════════╣\n";

                // Imprimimos cada jugador en el sistema
                for (size_t e = 0; e < cantidadEncontrados; e++) {
                    // Buscamos el equipo en cada iteracion
                    Equipo *equipoAux = Logica::equipos::buscarEquipoPorID(MiSistema, listaJugadores[e]->idEquipo);
                    if (equipoAux == nullptr) {
                        Auxiliares::limpiarPantalla();
                        std::cout << "Error Inesperado en el Sistema\n";
                        Auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << "║ " << std::right << std::setw(2) << listaJugadores[e]->ID << " ║ " << std::left << std::setw(20) << listaJugadores[e]->nombre << " ║ "
                              << std::left << std::setw(20) << equipoAux->nombre << " ║ " << std::left << std::setw(13) << listaJugadores[e]->posicion << " ║ " << std::right
                              << std::setw(3) << listaJugadores[e]->edad << " ║ [" << std::right << std::setw(2) << listaJugadores[e]->dorsal << "]     ║\n";
                }
                std::cout << "╚════╩══════════════════════╩══════════════════════╩═══════════════╩═════╩══════════╝\n";
                std::cout << " Total de jugadores registrados en el sistema: " << cantidadEncontrados << "\n";
            }

            // Liberamos la memoria
            if (listaJugadores != nullptr) {
                delete[] listaJugadores;
                listaJugadores = nullptr;
            }

            std::cout << "\n";
            Auxiliares::pausarPrograma();
        }

        void ActualizarJugador() {
            Auxiliares::limpiarPantalla();
            // Datos Actualizables:
            // Nombre, Edad, Dorsal, Posicion

            // Variables
            char nombreAux[100];
            char posicionAux[25];
            int edadAux = 0;
            int dorsalAux = 0;

            int ID = 0;
            char confirmacion;
            bool actualizado = false;
            bool flagError = false;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores registrados
            if (MiSistema->numJugadoresActuales == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Recolectamos el ID del jugador
            Auxiliares::ingresarDatos(ID, "Ingresa el ID del jugador que desea actualizar: ", Validadores::IDvalido);

            // Si no existe el ID del jugador
            if (!Logica::jugadores::existeID(MiSistema, ID)) {
                std::cout << "Error: El ID '" << ID << "' no pertenece a ningún jugador registrado\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Recolectamos el nombre
            do {
                Auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(nombreAux, 100, "Ingrese el nuevo nombre del Jugador: ", Validadores::Nombres);

                if (Logica::jugadores::nombreDuplicadoParaActualizar(MiSistema, nombreAux, ID)) {
                    std::cout << "Error: ya hay otro jugador con el nombre '" << nombreAux << "'\n";
                    flagError = true;
                    Auxiliares::waitfor(2500);
                }

                Auxiliares::waitfor(1000);
                Auxiliares::limpiarPantalla();
            } while (flagError);

            // Recolectamos la Edad
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarDatos(edadAux, "Ingrese la nueva edad del Jugador: ", Validadores::Edad);
            Auxiliares::waitfor(1000);
            Auxiliares::limpiarPantalla();

            // Recolectamos la posición
            do {
                int opcion = 0;
                flagError = false;
                Auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte actual del Torneo: " << MiSistema->torneo.deporte << "\n\n";

                const char **matrizDeporteActual = nullptr;

                for (size_t i = 0; i < Validadores::totalDeportes; i++) {
                    // Recorremos el array de matrices y verificamos lo que hay en la posicion 0
                    if (std::strcmp(Validadores::MapaDeportes[i][0], MiSistema->torneo.deporte) == 0) {
                        matrizDeporteActual = Validadores::MapaDeportes[i];
                        break;
                    }
                }

                // Mostramos las posiciones disponibles de esa fila
                std::cout << " Seleccione la posición del jugador:\n";

                // Para saber el numero de posiciones del deporte
                int contadorPosiciones = 0;

                for (size_t j = 1; matrizDeporteActual[j] != nullptr; j++) {
                    std::cout << " " << j << ". " << matrizDeporteActual[j] << "\n";
                    contadorPosiciones++;
                }
                std::cout << "\n";

                Auxiliares::ingresarDatos(opcion, "Seleccione una opción: ");

                // Verificamos que esté en el rango de opciones
                if (opcion >= 1 && opcion <= contadorPosiciones) {
                    // si es correcta guardamos la posicion
                    std::strncpy(posicionAux, matrizDeporteActual[opcion]);
                } else {
                    std::cout << "Error: Opción inválida. Por favor, intente de nuevo.\n";
                    flagError = true;
                    Auxiliares::waitfor(2000);
                }
            } while (flagError);

            // Recolectamos el dorsal
            do {
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarDatos(dorsalAux, "Ingrese el nuevo dorsal del Jugador: ", Validadores::Dorsal);
                Auxiliares::waitfor(1000);

                // Buscamos al jugador para obtener el id de su equipo
                Jugador *jugadorAux = Logica::jugadores::buscarJugadorPorID(MiSistema, ID);
                Equipo *eqAux = Logica::equipos::buscarEquipoPorID(MiSistema, jugadorAux->idEquipo);

                if (jugadorAux == nullptr || eqAux == nullptr) {
                    std::cout << " Error Inesperado del Sistema\n";
                    Auxiliares::pausarPrograma();
                    return;
                }

                // Si el dorsal ingresado esta duplicado y es distinto del dorsal actual del jugador
                if (Logica::jugadores::dorsalDuplicadoParaActualizar(MiSistema, dorsalAux, ID, jugadorAux->idEquipo)) {
                    std::cout << " Error: El dorsal '" << dorsalAux << "' ya está en uso en el equipo '" << eqAux->nombre << "'.\n";
                    Auxiliares::waitfor(2000);
                    flagError = true;
                }
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
            } while (flagError);

            // Confirmación de los cambios
            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea actualizar el jugador? (S/N): ");

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

                Jugador jugadorActualizado;
                std::strncpy(jugadorActualizado.nombre, nombreAux);
                std::strncpy(jugadorActualizado.posicion, posicionAux);
                jugadorActualizado.edad = edadAux;
                jugadorActualizado.dorsal = dorsalAux;

                // Llamamos a la lógica correspondiente de Jugadores
                actualizado = Logica::jugadores::actualizarJugador(MiSistema, ID, jugadorActualizado);

                // Si no pudo ser actualizado
                if (!actualizado) {
                    std::cout << "Se produjo un error a la hora de actualizar el jugador\n";
                } else {
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << "           Jugador Actualizado con Éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << "Nuevo Nombre:   " << nombreAux << std::endl;
                    std::cout << "Nueva Posición: " << posicionAux << std::endl;
                    std::cout << "Nueva Edad:     " << edadAux << " años" << std::endl;
                    std::cout << "Nuevo Dorsal:   " << dorsalAux << std::endl;
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "Actualización de Datos Cancelada\n";
            } else {
                std::cout << "Error: No se ingresó una opción correcta (S/N)\n";
                std::cout << "Actualización de Datos Cancelada\n";
            }
            Auxiliares::pausarPrograma();
        }

        void EliminarJugador() {
            Auxiliares::limpiarPantalla();
            int ID = 0;
            char confirmacion;

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores registrados
            if (MiSistema->numJugadoresActuales == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            ELIMINAR JUGADOR               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Recolectamos el ID
            Auxiliares::ingresarDatos(ID, "Ingresa el ID del jugador que deseas eliminar: ", Validadores::IDvalido);

            // Buscamos al jugador para mostrar sus datos en pantalla antes de continuar
            Jugador *ptrJugador = Logica::jugadores::buscarJugadorPorID(MiSistema, ID);

            if (ptrJugador == nullptr) {
                std::cout << "\nError: El ID '" << ID << "' no pertenece a ningún jugador registrado.\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo al que pertenece
            Equipo *ptrEquipo = Logica::equipos::buscarEquipoPorID(MiSistema, ptrJugador->idEquipo);

            // Mostramos el jugador que se va a eliminar
            std::cout << "\n Se eliminará el siguiente jugador del sistema:\n";
            std::cout << " -----------------------------------------------\n";
            std::cout << " Nombre:     " << ptrJugador->nombre << "\n";
            std::cout << " Posición:   " << ptrJugador->posicion << "\n";
            std::cout << " Dorsal:     " << ptrJugador->dorsal << "\n";
            std::cout << " Puntos:     " << ptrJugador->puntosAnotados << "\n";

            // Validacion por si acaso ocurre un error imprevisto (no debería)
            std::cout << " Equipo:     " << (ptrEquipo == nullptr ? "ERROR" : ptrEquipo->nombre) << "\n";
            std::cout << " -----------------------------------------------\n\n";

            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de eliminar este jugador? (S/N): ");
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();
            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

                // Llamamos a la logica
                bool eliminado = Logica::jugadores::eliminarJugador(MiSistema, ID);

                if (eliminado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "           Jugador eliminado con éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cout << "\nError: No se pudo eliminar al jugador.\nVerifique que no tenga puntos anotados en el torneo.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\nElimnación de datos cancelada\n";
            } else {
                std::cout << "\nError: Opción inválida (S/N).\nEliminación de datos cancelada.\n";
            }
            Auxiliares::pausarPrograma();
        }

    } // namespace jugadores

    namespace partidos {
        void programarPartido() {
            Auxiliares::limpiarPantalla();
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay Equipos Disponibles para programar un partido\n";
                Auxiliares::pausarPrograma();
                return;
            }

            if (MiSistema->numEquiposActuales <= 1) {
                std::cout << "No se puede programar un partido con un solo equipo\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Calculamos el minimo de jugadores en el deporte actual para organizar un partido
            int minimoDeJugadores = Logica::partidos::minJugadoresPorDeporte();
            int IDLocal = 0, IDVisitante = 0;
            bool flagError = false;
            char fecha[11];
            char descripcion[200];
            char confirmacion;

            // Recolectamos el ID LOCAL
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarDatos(IDLocal, "Ingrese el ID del equipo local: ", Validadores::IDvalido);

                // Si el ID no corresponde a ningun equipo
                if (!Logica::equipos::existeID(MiSistema, IDLocal)) {
                    std::cout << "Error el ID '" << IDLocal << "' no está asociado a ningún equipo\n";
                    Auxiliares::waitfor(2000);
                    flagError = true;
                }

            } while (flagError);

            // Recolectamos el ID VISItante
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarDatos(IDVisitante, "Ingrese el ID del equipo visitante: ", Validadores::IDvalido);

                // Si el ID no corresponde a ningun equipo
                if (!Logica::equipos::existeID(MiSistema, IDVisitante)) {
                    std::cout << "Error el ID '" << IDVisitante << "' no está asociado a ningún equipo\n";
                    Auxiliares::waitfor(2000);
                    flagError = true;
                }

                // Si el ID VISITANTE es el mismo que el ID del local
                if (IDLocal == IDVisitante) {
                    std::cout << "Error no se puede programar un partido entre un mismo equipo\n";
                    Auxiliares::waitfor(2000);
                    flagError = true;
                }
            } while (flagError);

            Auxiliares::limpiarPantalla();
            Equipo *EqLocal = Logica::equipos::buscarEquipoPorID(MiSistema, IDLocal);
            Equipo *EqVisitante = Logica::equipos::buscarEquipoPorID(MiSistema, IDVisitante);

            // Validacion de nullptr de respaldo (no debería activarse)
            if (EqLocal == nullptr || EqVisitante == nullptr) {
                std::cout << "Error Inesperado \n";
                Auxiliares::pausarPrograma();
                return;
            }

            /*
            // Si no cumplen con el minimo de jugadores
            if ((EqLocal->numJugadores < minimoDeJugadores) || (EqVisitante->numJugadores < minimoDeJugadores)) {
                std::cout << "Error no se puede programar un partido.\n";
                std::cout << "Los equipos no cumplen con el minimo de jugadores establecido\n\n";
                std::cout << "Deporte: " << MiSistema->torneo.deporte << std::endl;
                std::cout << "Minimo de Jugadores por Equipo: " << minimoDeJugadores << std::endl;
                std::cout << "Numero de Jugadores de '" << EqLocal->nombre << "': " << EqLocal->numJugadores << std::endl;
                std::cout << "Numero de Jugadores de '" << EqVisitante->nombre << "': " << EqVisitante->numJugadores << std::endl;
                Auxiliares::pausarPrograma();
                return;
            }*/

            // Si ya tienen un partido programado
            for (size_t e = 0; e < MiSistema->numPartidosActuales; e++) {
                bool partidoEntreSi = (((MiSistema->Partidos[e].idEquipoLocal == IDLocal) && (MiSistema->Partidos[e].idEquipoVisitante == IDVisitante)) ||
                                       ((MiSistema->Partidos[e].idEquipoLocal == IDVisitante) && (MiSistema->Partidos[e].idEquipoVisitante == IDLocal)));
                if (partidoEntreSi && (std::strcmp(MiSistema->Partidos[e].estado, Logica::partidos::estadoPartidos[0]) == 0)) {
                    std::cout << "Error ya hay un partido programado entre el equipo " << EqLocal->nombre << "' y '" << EqVisitante->nombre << std::endl;
                    Auxiliares::pausarPrograma();
                    return;
                }
            }

            // Pedimos la fecha flagError = false;
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << "Encuentro: " << EqLocal->nombre << " VS " << EqVisitante->nombre << "\n\n";
            Auxiliares::ingresarCadena(fecha, 11, "Ingrese la fecha del partido (YYYY-MM-DD): ", Validadores::fechaValidaRegistroDePartidos);
            Auxiliares::waitfor(750);
            Auxiliares::limpiarPantalla();

            // Pedimos la descripcion
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n"; // Usamos nombre torneo por el alfanumerico
            Auxiliares::ingresarCadena(descripcion, 200, "Ingrese la descripción del partido: ", Validadores::nombreTorneo);
            Auxiliares::waitfor(750);
            Auxiliares::limpiarPantalla();

            std::cout << "\n";
            Auxiliares::ingresarDatos(confirmacion, "Confirme la programación del partido (S/N): ");
            Auxiliares::waitfor(750);
            Auxiliares::limpiarPantalla();

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                Partido *nuevoPartido = Logica::partidos::programarPartido(MiSistema, IDLocal, IDVisitante, fecha, descripcion);

                if (nuevoPartido != nullptr) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "                ¡Partido programado con éxito!\n";
                    std::cout << "                " << EqLocal->nombre << "  VS  " << EqVisitante->nombre << "\n";
                    std::cout << "                Fecha: " << nuevoPartido->fecha << std::endl;
                    std::cout << "                ID Asignado: " << nuevoPartido->ID << std::endl;
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cout << "\nSe produjo un error a la hora de programar el partido.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\nLa programación del partido ha sido cancelada.\n";
            } else {
                std::cout << "\nError: Opción inválida (S/N).\nLa programación del partido ha sido cancelada.";
            }

            Auxiliares::pausarPrograma();
        }

        void registrarResultado() {
            Auxiliares::limpiarPantalla();

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }
            // Si no hay partidos
            if (MiSistema->numPartidosActuales == 0) {
                std::cout << "Error: No hay partidos registrados en el sistema.\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Variables
            int IDPartido = 0;
            int puntosLocal = 0, puntosVisitante = 0;
            bool flagError = false;
            char confirmacion;
            Partido *ptrPartido = nullptr;

            // Recopilamos el ID del partido a registrar
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                Auxiliares::ingresarDatos(IDPartido, "Ingrese el ID del partido a registrar: ", Validadores::IDvalido);

                // Buscamos el partido
                ptrPartido = Logica::partidos::buscarPartidoPorID(MiSistema, IDPartido);
                if (ptrPartido == nullptr) {
                    std::cout << "Error: El ID de partido '" << IDPartido << "' no está asociado a ningún partido.\n";
                    Auxiliares::pausarPrograma();
                    return;
                } else if (std::strcmp(ptrPartido->estado, Logica::partidos::estadoPartidos[0]) != 0) {
                    std::cout << "Error: El partido ya fue JUGADO o no se encuentra en estado PROGRAMADO.\n";
                    Auxiliares::waitfor(2000);
                    flagError = true;
                }

            } while (flagError);

            Equipo *EqLocal = Logica::equipos::buscarEquipoPorID(MiSistema, ptrPartido->idEquipoLocal);
            Equipo *EqVisitante = Logica::equipos::buscarEquipoPorID(MiSistema, ptrPartido->idEquipoVisitante);

            if (EqLocal == nullptr || EqVisitante == nullptr) {
                std::cout << "Error Inesperado\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Recolectamos los puntos del partido
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "Deporte Actual del Torneo: " << Validadores::deporteActual << "\n";
                std::cout << "Partido: " << EqLocal->nombre << " VS " << EqVisitante->nombre << "\n\n";

                Auxiliares::ingresarDatos(puntosLocal, "Puntos del equipo LOCAL: ");
                Auxiliares::ingresarDatos(puntosVisitante, "Puntos del equipo VISITANTE: ");

                if (puntosLocal < 0 || puntosVisitante < 0) {
                    std::cout << "Error: Los puntajes no pueden ser valores negativos.\n";
                    Auxiliares::waitfor(2000);
                    flagError = true;

                    // Validamos el empate
                } else if (puntosLocal == puntosVisitante) {
                    if (std::strcmp(Validadores::deporteActual, "BALONCESTO") == 0 || std::strcmp(Validadores::deporteActual, "TENIS") == 0 ||
                        std::strcmp(Validadores::deporteActual, "VOLEIBOL") == 0 || std::strcmp(Validadores::deporteActual, "BEISBOL") == 0 ||
                        std::strcmp(Validadores::deporteActual, "SOFTBOL") == 0) {
                        std::cout << "Error: En el deporte " << Validadores::deporteActual << " no se permiten empates. Registre el marcador final con prórroga.\n";
                        Auxiliares::waitfor(3000);
                        flagError = true;
                    }
                }
            } while (flagError);

            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          RESUMEN DEL MARCADOR             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " " << EqLocal->nombre << " (" << puntosLocal << ")  VS  (" << puntosVisitante << ") " << EqVisitante->nombre << "\n\n";

            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de registrar este resultado definitivo? (S/N): ");
            Auxiliares::limpiarPantalla();

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                // Invocamos tu función lógica corregida
                Partido *partidoRegistrado = Logica::partidos::registrarResultado(MiSistema, IDPartido, puntosLocal, puntosVisitante);

                if (partidoRegistrado != nullptr) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "                  ¡Resultado registrado con éxito!\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << " Partido ID:  " << partidoRegistrado->ID << "\n";
                    std::cout << " Estado:      " << partidoRegistrado->estado << "\n";
                    std::cout << " Marcador:    " << EqLocal->nombre << " [" << partidoRegistrado->puntosLocal << "] vs [" << partidoRegistrado->puntosVisitante << "] "
                              << EqVisitante->nombre << "\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cout << "\nError: No se pudo registrar el partido.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\nRegistro de resultado cancelado.\n";
            } else {
                std::cout << "\nError: Opción inválida (S/N).\nRegistro de resultado cancelado.\n";
            }
            Auxiliares::pausarPrograma();
        }

        void buscarPartidoPorID() {
            Auxiliares::limpiarPantalla();

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos
            if (MiSistema->numPartidosActuales == 0) {
                std::cout << "Error: No hay partidos registrados en el sistema.\n";
                Auxiliares::pausarPrograma();
                return;
            }

            int IDPartido = 0;
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            BUSCAR PARTIDO POR ID          ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Recolectamos el ID
            Auxiliares::ingresarDatos(IDPartido, "Ingrese el ID del partido que desea consultar: ", Validadores::IDvalido);
            Auxiliares::limpiarPantalla();
            Auxiliares::waitfor(2000);

            // Buscamos el partido mediante el ID
            Partido *partido = Logica::partidos::buscarPartidoPorID(MiSistema, IDPartido);

            // Si no existe, avisamos y salimos
            if (partido == nullptr) {
                std::cout << "\nError: El ID de partido '" << IDPartido << "' no existe en el sistema.\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Buscamos los equipos para mostrar nombres reales en la presentacion
            Equipo *EqLocal = Logica::equipos::buscarEquipoPorID(MiSistema, partido->idEquipoLocal);
            Equipo *EqVisitante = Logica::equipos::buscarEquipoPorID(MiSistema, partido->idEquipoVisitante);

            if (EqLocal == nullptr || EqVisitante == nullptr) {
                std::cout << "\nError: No se logró recolectar los datos de los equipos.\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "Buscando..." << std::endl;
            Auxiliares::waitfor(1200);
            Auxiliares::limpiarPantalla();

            std::cout << "\n╔══════════════════════════════════════════════════╗\n";
            std::cout << "║              DETALLE DE PARTIDO                  ║\n";
            std::cout << "╠══════════════════════════════════════════════════╣\n";
            std::cout << "║ " << std::left << std::setw(14) << "ID Partido" << ": " << std::setw(32) << partido->ID << "║\n";
            std::cout << "║ " << std::left << std::setw(14) << "Estado" << ": " << std::setw(32) << partido->estado << "║\n";
            std::cout << "║ " << std::left << std::setw(14) << "Fecha" << ": " << std::setw(32) << partido->fecha << "║\n";
            std::cout << "║                                                  ║\n";
            std::cout << "║  " << std::left << std::setw(20) << EqLocal->nombre << " " << partido->puntosLocal << " - " << partido->puntosVisitante << "  " << std::setw(20)
                      << EqVisitante->nombre << " ║\n";
            std::cout << "║      (Local)                  (Visitante)        ║\n";
            std::cout << "║                                                  ║\n";
            std::cout << "║ Notas: " << std::left << std::setw(41) << partido->descripcion << "║\n";
            std::cout << "║                                                  ║\n";
            std::cout << "╚══════════════════════════════════════════════════╝\n\n";

            Auxiliares::pausarPrograma();
        }

        void listarTodosLosPartidos() {
            Auxiliares::limpiarPantalla();

            // Si no hay equipos registrados
            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos
            if (MiSistema->numPartidosActuales == 0) {
                std::cout << "Error: No hay partidos registrados en el sistema.\n";
                Auxiliares::pausarPrograma();
                return;
            }

            int cantPartidos = 0;
            Partido **listaDePartidos = Logica::partidos::listarPartidos(MiSistema, &cantPartidos);
            if (listaDePartidos == nullptr) {
                std::cout << "Error del Sistema\n";
            } else if (cantPartidos == 0) {
                std::cout << "No se encontró ningún partido\n";
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            LISTADO DE PARTIDOS            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << std::left << std::setw(6) << "ID" << std::left << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado"
                      << "Encuentro y Resultado\n";
            std::cout << "--------------------------------------------------------------------------------\n";

            // Recorremos para imprimir los mensajes
            for (size_t e = 0; e < cantPartidos; e++) {
                Partido *partido = listaDePartidos[e];
                Equipo *EqLocal = Logica::equipos::buscarEquipoPorID(MiSistema, partido->idEquipoLocal);
                Equipo *EqVisitante = Logica::equipos::buscarEquipoPorID(MiSistema, partido->idEquipoVisitante);
                if (EqLocal == nullptr || EqVisitante == nullptr) {
                    Auxiliares::limpiarPantalla();
                    std::cout << "Error Inesperado del Sistema\n";
                    Auxiliares::pausarPrograma();
                    return;
                }

                std::cout << std::left << std::setw(6) << partido->ID << std::left << std::setw(14) << partido->fecha << std::left << std::setw(13) << partido->estado;
                std::cout << EqLocal->nombre << " [" << partido->puntosLocal << "] vs [" << partido->puntosVisitante << "] " << EqVisitante->nombre << "\n";
            }
            std::cout << "--------------------------------------------------------------------------------\n\n";

            if (listaDePartidos != nullptr) {
                delete[] listaDePartidos;
                listaDePartidos = nullptr;
            }

            Auxiliares::pausarPrograma();
        }

        void buscarPartidosPorEquipo() {
            Auxiliares::limpiarPantalla();

            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "\n Error: No hay ningun equipo registrado actualmente.\n\n";
                Auxiliares::pausarPrograma();
                return;
            }

            if (MiSistema->numPartidosActuales == 0) {
                std::cout << "\n Error: No hay partidos registrados en el sistema actualmente.\n\n";
                Auxiliares::pausarPrograma();
                return;
            }

            int idEquipo = -1;
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║        HISTORIAL DE PARTIDOS POR EQUIPO   ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::ingresarDatos(idEquipo, "Ingrese el ID del equipo a consultar: ", Validadores::IDvalido);
            Auxiliares::limpiarPantalla();
            Auxiliares::waitfor(2500);

            // Validamos si el equipo existe para poder usarlo
            Equipo *EqBuscado = Logica::equipos::buscarEquipoPorID(MiSistema, idEquipo);
            if (EqBuscado == nullptr) {
                std::cout << "\n Error: El ID de equipo '" << idEquipo << "' no existe en el sistema.\n\n";
                Auxiliares::pausarPrograma();
                return;
            }

            // Inicializamos la cantidad de partidos en 0 y buscamos la lista de partidos
            int cantPartidos = 0;
            Partido **listaDePartidosPorEq = Logica::partidos::listarPartidosPorEquipo(MiSistema, idEquipo, &cantPartidos);

            if (listaDePartidosPorEq == nullptr) {
                std::cout << "\n Error: Ocurrió un error a la hora de mostrar la lista de partidos por equipo.\n\n";
            } else if (cantPartidos == 0) {
                // Si la lógica creó el arreglo pero el equipo no tiene partidos
                std::cout << "\n El equipo '" << EqBuscado->nombre << "' no tiene partidos registrados todavia.\n\n";
            } else {
                // Entra aquí si se encontraron partidos para el equipo
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PARTIDOS ENCONTRADOS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n";
                std::cout << " Historial para: " << EqBuscado->nombre << "\n\n";
                std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado"
                          << "Encuentro y Resultado\n";
                std::cout << "--------------------------------------------------------------------------------\n";

                // Recorremos e imprimimos todos los partidos del equipo
                for (int i = 0; i < cantPartidos; i++) {
                    Partido *partido = listaDePartidosPorEq[i];
                    Equipo *EqLocal = Logica::equipos::buscarEquipoPorID(MiSistema, partido->idEquipoLocal);
                    Equipo *EqVisitante = Logica::equipos::buscarEquipoPorID(MiSistema, partido->idEquipoVisitante);
                    if (EqLocal == nullptr || EqVisitante == nullptr) {
                        Auxiliares::limpiarPantalla();
                        std::cout << "Error Inesperado del Sistema\n";
                        Auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << std::left << std::setw(6) << partido->ID << std::left << std::setw(14) << partido->fecha << std::left << std::setw(13) << partido->estado;
                    std::cout << EqLocal->nombre << " [" << partido->puntosLocal << "] vs [" << partido->puntosVisitante << "] " << EqVisitante->nombre << "\n";
                }
                std::cout << "--------------------------------------------------------------------------------\n\n";
            }

            // Liberamos
            if (listaDePartidosPorEq != nullptr) {
                delete[] listaDePartidosPorEq;
                listaDePartidosPorEq = nullptr;
            }

            Auxiliares::pausarPrograma();
        }

        void listarPartidosPorEstado() {
            Auxiliares::limpiarPantalla();

            if (MiSistema->numEquiposActuales == 0) {
                std::cout << "\n Error: No hay ningun equipo registrado actualmente.\n\n";
                Auxiliares::pausarPrograma();
                return;
            }

            if (MiSistema->numPartidosActuales == 0) {
                std::cout << "\n Error: No hay partidos registrados en el sistema actualmente.\n\n";
                Auxiliares::pausarPrograma();
                return;
            }

            bool flagError = false;
            int opcion = -1;
            char estado[12];
            int cantidad = 0;

            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║         BUSCAR PARTIDOS POR ESTADO        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                std::cout << "Estados de partido consultables: \n\n";
                std::cout << "---------------------------------------------\n";
                std::cout << " 0. PROGRAMADO\n 1. JUGADO\n 2. CANCELADO\n\n";
                std::cout << "---------------------------------------------\n";
                Auxiliares::ingresarDatos(opcion, "Ingrese el tipo de estado de partido que desea consultar: ", Validadores::Positivo);

                switch (opcion) {
                    case 0: // Configuramos en PROGRAMADO
                        std::strncpy(estado, Logica::partidos::estadoPartidos[0]);
                        break;

                    case 1: // Configuramos en JUGADO
                        std::strncpy(estado, Logica::partidos::estadoPartidos[1]);
                        break;

                    case 2: // Configuramos en CANCELADO
                        std::strncpy(estado, Logica::partidos::estadoPartidos[2]);
                        break;

                    default:
                        std::cout << "Ingrese una opcion correcta\n";
                        flagError = true;
                        Auxiliares::pausarPrograma();
                }
            } while (flagError);

            Auxiliares::limpiarPantalla();
            std::cout << "Buscando..." << std::endl;
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            // Listamos todos los partidos
            Partido **listaDePartidos = Logica::partidos::listarPartidosPorSuEstado(MiSistema, estado, &cantidad);

            if (listaDePartidos == nullptr) {
                std::cout << "\n Error: Ocurrió un error en el Sistema.\n\n";
            } else if (cantidad == 0) {
                // Mensaje sutil si el arreglo se creó pero no hay registros con ese estado
                std::cout << "\n No se encontro ningun partido en estado '" << estado << "' actualmente.\n\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PARTIDOS ENCONTRADOS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n";
                std::cout << " Partidos en estado: " << estado << "\n\n";

                // Cabecera de columnas fijas usando setw
                std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado"
                          << "Encuentro y Resultado\n";
                std::cout << "--------------------------------------------------------------------------------\n";

                // Bucle de impresión de registros
                for (int i = 0; i < cantidad; i++) {
                    Partido *partido = listaDePartidos[i];
                    Equipo *EqLocal = Logica::equipos::buscarEquipoPorID(MiSistema, partido->idEquipoLocal);
                    Equipo *EqVisitante = Logica::equipos::buscarEquipoPorID(MiSistema, partido->idEquipoVisitante);

                    if (EqLocal == nullptr || EqVisitante == nullptr) {
                        Auxiliares::limpiarPantalla();
                        std::cout << "Error Inesperado del Sistema\n";
                        Auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << std::left << std::setw(6) << partido->ID << std::left << std::setw(14) << partido->fecha << std::left << std::setw(13) << partido->estado;
                    std::cout << EqLocal->nombre << " [" << partido->puntosLocal << "] vs [" << partido->puntosVisitante << "] " << EqVisitante->nombre << "\n";
                }
                std::cout << "--------------------------------------------------------------------------------\n\n";
            }

            // Liberamos
            if (listaDePartidos != nullptr) {
                delete[] listaDePartidos;
                listaDePartidos = nullptr;
            }

            Auxiliares::pausarPrograma();
        }

        void cancelarPartido() {
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             CANCELAR PARTIDOS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n";

            int IDPartido = -1;
            bool cancelado = false;
            char confirmacion;

            Auxiliares::ingresarDatos(IDPartido, "Ingrese el ID del Partido que desea cancelar: ", Validadores::IDvalido);
            Auxiliares::limpiarPantalla();
            std::cout << "\n Procesando solicitud...";
            Auxiliares::waitfor(1200);
            Auxiliares::limpiarPantalla();

            Partido *PartidoAux = Logica::partidos::buscarPartidoPorID(MiSistema, IDPartido);

            if (PartidoAux == nullptr) {
                std::cout << "\nError: El ID '" << IDPartido << "' no pertenece a ningún partido registrado.\n";
                Auxiliares::pausarPrograma();
                return;
            }

            Equipo *EqLocal = Logica::equipos::buscarEquipoPorID(MiSistema, PartidoAux->idEquipoLocal);
            Equipo *EqVisitante = Logica::equipos::buscarEquipoPorID(MiSistema, PartidoAux->idEquipoVisitante);
            if (EqLocal == nullptr || EqVisitante == nullptr) {
                Auxiliares::limpiarPantalla();
                std::cout << "Error Inesperado del Sistema\n";
                Auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             CANCELAR PARTIDOS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << "\n Se borrará el registro del partido: \n\n";
            std::cout << " Encuentro: " << EqLocal->nombre << " [ " << PartidoAux->puntosLocal << " ]  --  [ " << PartidoAux->puntosVisitante << "] " << EqVisitante->nombre
                      << std::endl;
            std::cout << " Fecha: " << PartidoAux->fecha << std::endl;
            std::cout << " ID: " << PartidoAux->ID << std::endl;
            std::cout << " Estado del Partido: " << PartidoAux->estado << std::endl << std::endl;

            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de eliminar el registro de este partido? (S/N): ");

            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                // Llamamos a la logica
                cancelado = Logica::partidos::cancelarPartido(MiSistema, IDPartido);

                if (cancelado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "           Partido Cancelado con éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cout << "\nError: No se pudo cancelar el Partido.\nError de Sistema.\n";
                }

            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "Operación Cancelada\n";
            } else {
                std::cout << "Error: No se ingresó una opción válida.\n Operación Cancelada\n";
            }
            Auxiliares::pausarPrograma();
        }

    } // namespace partidos

    namespace menu {

        void datosInicialesTorneo() {
            // variables auxiliares
            Torneo torneoAux;
            int opcionFormato = 0;
            bool opcionValida = false;

            //* Aqui se recopilan los datos iniciales del torneo

            // Ingresar Nombre
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(torneoAux.nombre, 100, "Nombre del Torneo: ", Validadores::nombreTorneo);
            Auxiliares::waitfor(1500);

            // Ingresar Deporte
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Mostrar deportes disponibles
            std::cout << "Deportes disponibles:\n";
            for (size_t e = 0; e < Validadores::totalDeportes; e++) {
                std::cout << " - " << Validadores::Deportes[e] << std::std::endl;
            }
            std::cout << std::endl;

            // Validación externa
            char mensajeError[TAMANO_MENSAJE_ERROR];
            bool deporteValido = false;
            do {
                Auxiliares::ingresarCadena(torneoAux.deporte, 50, "Deporte del Torneo: ");
                // Normalizamos a mayusculas para facilitar la comparación
                Auxiliares::toMayus(torneoAux.deporte);

                if (!Validadores::existeDeporte(torneoAux.deporte, mensajeError)) {
                    std::cout << "Error: " << mensajeError << std::std::endl;
                    Auxiliares::waitfor(2000);
                    deporteValido = false;
                } else {
                    deporteValido = true;
                }
            } while (!deporteValido);

            // Definimos el deporte actual en Validadores
            Validadores::definirDeporteActual(torneoAux.deporte);
            Auxiliares::waitfor(1500);

            // Ingresar Formato
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << "1. Formato de Grupos (Todos contra todos)\n";
            std::cout << "2. Formato de Eliminatoria Directa\n";
            std::cout << "--------------------------------------------------\n";
            do {
                opcionValida = true;
                Auxiliares::ingresarDatos(opcionFormato, "Seleccione el formato (1 o 2): ");
                if (opcionFormato != 1 && opcionFormato != 2) {
                    std::cout << "Opcion invalida. Intente de nuevo.\n";
                    opcionValida = false;
                }
            } while (!opcionValida);

            // desde la logica definimos el tipo de torneo en base a la opcion ingresada
            Logica::definirFormato(torneoAux, opcionFormato);
            Auxiliares::waitfor(1500);

            // Ingresar Fecha de Inicio del torneo
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(torneoAux.fechaInicio, 11, "Fecha De Inicio del Torneo: ", Validadores::FechaValida);
            Validadores::definirFechaInicio(torneoAux.fechaInicio);
            Auxiliares::waitfor(1500);

            // Ingresar Fecha de Finalizacion de Torneo
            Auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::ingresarCadena(torneoAux.fechaFin, 11, "Fecha de Finalización del Torneo: ", Validadores::ValidarFechaFin);
            Validadores::definirFechaFin(torneoAux.fechaFin);
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            // enviamos los datos
            Logica::inicializarSistemaDeportivo(MiSistema, torneoAux);

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║  NUEVO TORNEO CREADO CON ÉXITO            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << "Nombre: " << MiSistema->torneo.nombre << std::endl;
            std::cout << "Deporte: " << MiSistema->torneo.deporte << std::endl;
            std::cout << "Formato: " << MiSistema->torneo.formato << std::endl;
            std::cout << "Fecha de inicio del torneo: " << MiSistema->torneo.fechaInicio << std::endl;
            std::cout << "Fecha de Finalización del torneo: " << MiSistema->torneo.fechaFin;
            Auxiliares::pausarPrograma();
        }

        void Principal() {
            Auxiliares::limpiarPantalla();

            std::cout << "\n   ╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "   ║                    Sport G&C Tournaments                     ║\n";
            std::cout << "   ║  Torneo:   " << std::left << std::setw(50) << MiSistema->torneo.nombre << "║\n";
            std::cout << "   ║  Deporte: " << std::left << std::setw(18) << MiSistema->torneo.deporte << " | Formato: " << std::left << std::setw(21) << MiSistema->torneo.formato
                      << "║\n";
            std::cout << "   ╠══════════════════════════════════════════════════════════════╣\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "1. Gestión de Equipos" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "2. Gestión de Jugadores" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "3. Gestión de Partidos" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "4. Tabla de Posiciones" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "0. Salir" << "║\n";
            std::cout << "   ╚══════════════════════════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void GestionDeEquipos() {
            Auxiliares::limpiarPantalla();
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
            Auxiliares::limpiarPantalla();
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
            Auxiliares::limpiarPantalla();
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
            Auxiliares::limpiarPantalla();
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
            Auxiliares::limpiarPantalla();
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
            Auxiliares::limpiarPantalla();
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
            Auxiliares::limpiarPantalla();
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

    } // namespace menu

    void mensajeSalida() {
        Auxiliares::limpiarPantalla();
        std::cout << "Saliendo...";
        Auxiliares::waitfor(3500);
    }

    void mensajeMenuPrincipal() {
        Auxiliares::limpiarPantalla();
        std::cout << "Volviendo al Menú Principal...";
        Auxiliares::waitfor(3000);
    }

    void mensajeMenuAnterior() {
        Auxiliares::limpiarPantalla();
        std::cout << "Volviendo al menu anterior...";
        Auxiliares::waitfor(3000);
    }

    void mensajeDefault() {
        Auxiliares::limpiarPantalla();
        std::cout << "ERROR has ingresado una opcion inválida. Intentalo nuevamente.\n";
        Auxiliares::waitfor(3500);
    }
} // namespace Presentacion

// ============================================//
//   7. MAIN                                   //
// ============================================//

int main() {
    // Llamamos a la función de configuración de Idioma al inicio
    Auxiliares::configurarIdioma();

    // ? ----------------------------------------------//
    // ? DECLARACION DE VARIABLES                      //
    // ? ----------------------------------------------//

    // Estructuras
    SistemaDeportivo MiSistema;
    Torneo MiTorneo;

    // Punteros
    SistemaDeportivo *ptrMiSistema = &MiSistema;

    // Variables Estaticas
    int opcionMenu = -1;     // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionSubMenu = -1;  // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionMenuBusq = -1; // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionMenuListar = -1;
    char confirmacion;

    // Los arrays de deportes y posiciones se definen en el namespace Validadores
    // (no duplicar aquí para evitar inconsistencias)

    // Inicio del Programa
    Presentacion::menu::datosInicialesTorneo(ptrMiSistema);

    // Estructura del switch
    do {
        // inicializamos las variables para evitar conflictos
        opcionMenu = -1;
        opcionSubMenu = -1;
        opcionMenuListar = -1;

        // Presentamos el menu principal
        Presentacion::menu::Principal(ptrMiSistema);
        Auxiliares::ingresarDatos(opcionMenu, "Seleccione una opcion: ", Validadores::Positivo);
        Auxiliares::limpiarPantalla();
        Auxiliares::waitfor(1500);
        switch (opcionMenu) {

            // Salida del Programa
            case 0:
                Auxiliares::limpiarPantalla();
                Auxiliares::waitfor(2500);
                Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea salir del programa? (S/N): ");
                if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                    Presentacion::mensajeSalida();
                } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                    std::cout << "Salida del Programa Cancelada\n"; // Forzamos la vuelta del bucle
                    opcionMenu = -1;
                    Auxiliares::waitfor(3000);
                    Auxiliares::limpiarPantalla();
                } else {
                    std::cout << "Error: No se ingresó una opción correcta (S/N)\n";
                    std::cout << "Salida del Programa Cancelada\n";
                    Auxiliares::waitfor(3000);
                    // Forzamos la vuelta del bucle
                    opcionMenu = -1;
                }
                break;

            // Gestión de Equipos
            case 1:
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
                std::cout << "Ingresando al apartado de Gestión de Equipos..." << std::endl;
                Auxiliares::waitfor(3000);

                do {
                    Auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    opcionMenuBusq = -1;
                    Presentacion::menu::GestionDeEquipos();
                    Auxiliares::ingresarDatos(opcionSubMenu, "Seleccione una opcion: ", Validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0: // Volver al menu Principal
                            Presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Registrar nuevos equipos
                            Presentacion::equipos::RegistrarEquipos(ptrMiSistema);
                            break;

                        case 2: // Menu Buscar equipos
                            do {
                                opcionMenuBusq = -1;
                                Presentacion::menu::menuBuscarEquipo();
                                Auxiliares::ingresarDatos(opcionMenuBusq, "Seleccione una opcion: ", Validadores::Positivo);

                                switch (opcionMenuBusq) {
                                    case 0: // Volver al menu anterior
                                        Presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Busqueda por ID
                                        Presentacion::equipos::buscarEquipoPorID(ptrMiSistema);
                                        break;

                                    case 2: // Busqueda por nombre
                                        Presentacion::equipos::buscarEquiposPorSubCadena(ptrMiSistema);
                                        break;

                                    default:
                                        Presentacion::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);

                            break;

                        case 3: // Actualizar Equipos
                            Presentacion::equipos::actualizarEquipo(ptrMiSistema);
                            break;

                        case 4: // Listar Equipos
                            Presentacion::equipos::listarEquipos(ptrMiSistema);
                            break;

                        case 5: // Eliminar Equipos
                            Presentacion::equipos::eliminarEquipo(ptrMiSistema);
                            break;

                        default:
                            Presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);

                break;

            // Gestión de Jugadores
            case 2:
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
                std::cout << "Ingresando al apartado de Gestión de Jugadores..." << std::endl;
                Auxiliares::waitfor(3000);

                do {
                    Auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    Presentacion::menu::GestionDeJugadores();
                    Auxiliares::ingresarDatos(opcionSubMenu, "Seleccione un opcion: ", Validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0: // Volver al menu Principal
                            Presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Registrar nuevos jugadores
                            Presentacion::jugadores::RegistrarJugador(ptrMiSistema);
                            break;

                        case 2: // Menu Buscar jugadores
                            do {
                                opcionMenuBusq = -1;
                                Presentacion::menu::menuBuscarJugador();
                                Auxiliares::ingresarDatos(opcionMenuBusq, "Seleccione una opcion: ", Validadores::Positivo);

                                switch (opcionMenuBusq) {
                                    case 0: // Volver al menu anterior
                                        Presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Busqueda por ID
                                        Presentacion::jugadores::buscarJugadorID(ptrMiSistema);
                                        break;

                                    case 2: // Busqueda por nombre
                                        Presentacion::jugadores::buscarJugadorPorNombre(ptrMiSistema);
                                        break;

                                    default:
                                        Presentacion::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);
                            break;

                        case 3: // Actualizar Jugador
                            Presentacion::jugadores::ActualizarJugador(ptrMiSistema);
                            break;

                        case 4: // Listar todos los jugadores
                            Presentacion::jugadores::mostrarListaDeJugadores(ptrMiSistema);
                            break;

                        case 5: // Listar Jugadores por Equipos
                            Presentacion::jugadores::mostrarJugadoresPorEquipo(ptrMiSistema);
                            break;

                        case 6: // Eliminar jugador
                            Presentacion::jugadores::EliminarJugador(ptrMiSistema);
                            break;

                        default:
                            Presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);
                break;

            // Gestión de Partidos
            case 3:
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
                std::cout << "Ingresando al apartado de Gestión de Partidos..." << std::endl;
                Auxiliares::waitfor(3000);
                do {
                    Auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    Presentacion::menu::GestionDePartidos();
                    Auxiliares::ingresarDatos(opcionSubMenu, "Seleccione un opcion: ", Validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0: // Volver al menú principal
                            Presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Programar Partidos
                            Presentacion::partidos::programarPartido(ptrMiSistema);
                            break;

                        case 2: // Registrar el Resultado de un partido
                            Presentacion::partidos::registrarResultado(ptrMiSistema);
                            break;

                        case 3: // Buscar partidos
                            do {
                                opcionMenuBusq = -1;
                                Presentacion::menu::buscarPartidos();
                                Auxiliares::ingresarDatos(opcionMenuBusq, "Seleccione una opcion: ", Validadores::Positivo);

                                switch (opcionMenuBusq) {
                                    case 0: // Volver al menu anterior
                                        Presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Busqueda por Equipo
                                        Presentacion::partidos::buscarPartidosPorEquipo(ptrMiSistema);
                                        break;

                                    case 2: // Busqueda por ID
                                        Presentacion::partidos::buscarPartidoPorID(ptrMiSistema);
                                        break;

                                    default:
                                        Presentacion::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);
                            break;

                        case 4: // Listar Partidos
                            do {
                                opcionMenuListar = -1;
                                Presentacion::menu::listarPartidos();
                                Auxiliares::ingresarDatos(opcionMenuListar, "Seleccione una opcion: ", Validadores::Positivo);

                                switch (opcionMenuListar) {
                                    case 0: // Volver al menu anterior
                                        Presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Listar todos los partidos
                                        Presentacion::partidos::listarTodosLosPartidos(ptrMiSistema);
                                        break;

                                    case 2: // Listar partidos por estado
                                        Presentacion::partidos::listarPartidosPorEstado(ptrMiSistema);
                                        break;

                                    default:
                                        Presentacion::mensajeDefault();
                                }
                            } while (opcionMenuListar != 0);
                            break;

                        case 5: // Cancelar Partidos
                            Presentacion::partidos::cancelarPartido(ptrMiSistema);
                            break;

                        default:
                            Presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);

                break;

            // Tabla de Posiciones
            case 4:
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
                std::cout << "Ingresando al apartado de Tabla de Posiciones..." << std::endl;
                Auxiliares::waitfor(3000);
                Auxiliares::limpiarPantalla();
                Presentacion::equipos::mostrarTablaDePosiciones(ptrMiSistema);
                break;

            // Si no se selecciona una opcion correcta enviamos un mensaje de aviso
            default:
                Presentacion::mensajeDefault();
        }
        // El bucle se repite si el usuario no eligió la opcion de salir en el menu Principal
    } while (opcionMenu != 0);

    // liberar memoria y cierre del programa
    Logica::liberarSistema(ptrMiSistema);
    return 0;
}