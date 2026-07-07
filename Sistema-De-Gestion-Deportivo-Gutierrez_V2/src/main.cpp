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
#include <sstream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

// Definicion de Variables globales
const int TAMANO_MENSAJE_ERROR = 150;
const int TAMANO_NOMBRE = 100;
const int TAMANO_FECHA = 11;
const int TAMANO_DEPORTE = 50;
const int TAMANO_CEDULA = 20;
const int TAMANO_POSICION = 25;
const int TAMANO_ESTADO = 12;
const int TAMANO_FORMATO = 20;
const int TAMANO_DESCRIPCION = 200;
const int TAMANO_LOCAL_O_VISITANTE = 12;
const char *NOMBRE_ARCHIVO_TORNEO = "../bin/datos/torneo.bin";
const char *NOMBRE_ARCHIVO_JUGADORES = "../bin/datos/jugadores.bin";
const char *NOMBRE_ARCHIVO_EQUIPOS = "../bin/datos/equipos.bin";
const char *NOMBRE_ARCHIVO_PARTIDOS = "../bin/datos/partidos.bin";
const int MAX_RESULTADOS = 100;
const int MAX_ANOTACIONES = 22;
const int MAX_TARJETAS_AMARILLAS = 30;
const int MAX_TARJETAS_ROJAS = 8;
const int MINUTO_MINIMO = 1;
const int MINUTO_MAXIMO = 120;

namespace fs = std::filesystem;

// ============================================//
//   2. STRUCTS                                //
// ============================================//

struct ArchivoHeader {
    int cantidadRegistros; // Total histórico (incluyendo eliminados lógicamente) // 0
    int proximoID;         // Siguiente ID a asignar (autoincremental)  // 1
    int registrosActivos;  // Registros con eliminado == false  // 0
    int version;           // Control de versión del archivo (iniciar en 1) // 1
};

struct Jugador {
    // Datos básicos
    int ID;
    int idEquipo;
    char nombre[TAMANO_NOMBRE];
    char cedula[TAMANO_CEDULA];
    char posicion[TAMANO_POSICION];
    int edad;
    int numeroDorsal;
    char fechaRegistro[TAMANO_FECHA]; // fecha de registro del jugador en formato YYYY-MM-DD
    // bool suspendido;

    // Estadísticas individuales (se actualizan al registrar goles en partidos)
    int anotaciones;
    int tarjetaAmarillas;
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
    int idJugador;                         // ID del jugador que anotó (0 = desconocido / gol en contra)
    int minuto;                            // Minuto del partido en que se anotó (1 - 120)
    char equipo[TAMANO_LOCAL_O_VISITANTE]; // "LOCAL" o "VISITANTE"
};

struct tarjetaAmarilla {
    int idJugador;                         // ID del jugador que anotó (0 = desconocido / gol en contra)
    int minuto;                            // Minuto del partido en que se anotó (1 - 120)
    char equipo[TAMANO_LOCAL_O_VISITANTE]; // "LOCAL" o "VISITANTE"
};

struct tarjetaRoja {
    int idJugador;                         // ID del jugador que anotó (0 = desconocido / gol en contra)
    int minuto;                            // Minuto del partido en que se anotó (1 - 120)
    char equipo[TAMANO_LOCAL_O_VISITANTE]; // "LOCAL" o "VISITANTE"
};

struct Partido {
    // Datos básicos
    int ID;
    int idEquipoLocal;
    int idEquipoVisitante;
    char fecha[TAMANO_FECHA];
    char estado[TAMANO_ESTADO]; // "PROGRAMADO", "JUGADO", "CANCELADO"
    char descripcion[TAMANO_DESCRIPCION];

    // Marcador global (para acceso rápido sin recorrer el array de goles)
    int anotacionesLocal;
    int anotacionesVisitante;

    // Tarjetas por equipo
    int tarjetasAmaLocal;
    int tarjetasAmaVisitante;
    int tarjetasRojasLocal;
    int tarjetasRojasVisitante;

    // Detalle de goles (NUEVO en Proyecto 2)
    Anotacion anotaciones[MAX_ANOTACIONES];           // Máximo 22 goles por partido
    tarjetaAmarilla tarjetaA[MAX_TARJETAS_AMARILLAS]; // Máximo 30 tarjetas A por partido
    tarjetaRoja tarjetaR[MAX_TARJETAS_ROJAS];         // Máximo 8 tarjetas R por partido
    int numAnotaciones;
    int numtarjetaAma;
    int numTarjetasRojas;

    // Metadata de control
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Torneo {
    char nombre[TAMANO_NOMBRE];     // Nombre del torneo
    char deporte[TAMANO_DEPORTE];   // Deporte del torneo
    char formato[TAMANO_FORMATO];   // "GRUPOS" o "ELIMINATORIA"
    char fechaInicio[TAMANO_FECHA]; // Formato YYYY-MM-DD
    char fechaFin[TAMANO_FECHA];    // Formato YYYY-MM-DD

    // Metadata de control
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

    // Variable para ver si los datos ya están inicializados
    bool inicializado;
};

// ============================================//
//   3. FUNCIONES aUXILIARES                   //
// ============================================//

// grupo de funciones que no inciden como tal en el sistema pero que mejoran su funcionamiento
namespace auxiliares {

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

    namespace {
        std::string normalizarEntradaAMayus(const std::string &valor) {
            std::string copia = valor;
            std::transform(copia.begin(), copia.end(), copia.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
            return copia;
        }

        bool esCancelacion(const std::string &valor) {
            std::string copia = normalizarEntradaAMayus(valor);
            return copia == "CANCELAR" || copia == "SALIR";
        }
    } // namespace

    // funcion para ingresar cualquier tipo de dato
    template <typename var1> //
    bool ingresarDatos(var1 &variable, const char *mensaje, bool *cancelado = nullptr, bool (*ptrValidador)(var1, char *) = nullptr) {
        bool flag = false;
        char mensajeError[TAMANO_MENSAJE_ERROR];
        std::string entrada;

        do {
            mensajeError[0] = '\0';
            flag = false;

            if (cancelado != nullptr) {
                *cancelado = false;
            }

            std::cout << mensaje << " (escriba 'cancelar' para cancelar): " << std::flush;
            std::cin.clear();
            std::getline(std::cin, entrada);

            // Verificamos si quiere cancelar
            if (esCancelacion(entrada)) {
                if (cancelado != nullptr) {
                    *cancelado = true;
                }
                return false;
            }

            // Intenamos convertir al ripo de dato
            std::stringstream ss(entrada);
            if (!(ss >> variable)) {
                std::cout << "Error Tipo de Dato Incorrecto\n\n";
                waitfor(3000);
                continue;
            }

            if (ss >> std::ws && !ss.eof()) {
                std::cout << "Error Tipo de Dato Incorrecto\n\n";
                waitfor(3000);
                continue;
            }

            // Verificamos la validacion enviada
            if (ptrValidador != nullptr) {
                flag = !ptrValidador(variable, mensajeError);
                if (flag) {
                    std::cout << mensajeError << std::endl << std::endl;
                    waitfor(3500);
                }
            }
        } while (flag);

        return true;
    }

    // funcion para ingresar cadenas de texto
    bool ingresarCadena(char *texto, size_t tamañoMaximo, const char *mensaje, bool *cancelado = nullptr, bool (*ptrValidador)(const char *, char *) = nullptr) {
        const int TAMANO_MENSAJE_ERROR = 150;
        bool flag = false;
        char mensajeError[TAMANO_MENSAJE_ERROR];
        std::string entrada;

        do {
            mensajeError[0] = '\0';
            flag = false;

            if (cancelado != nullptr) {
                *cancelado = false;
            }

            std::cout << mensaje << " (escriba 'cancelar' para cancelar): " << std::flush;
            std::cin.clear();
            std::getline(std::cin, entrada);

            if (esCancelacion(entrada)) {
                if (cancelado != nullptr) {
                    *cancelado = true;
                }
                return false;
            }

            if (entrada.size() >= tamañoMaximo) {
                std::cout << "ERROR: Excediste el limite de caracteres permitido (" << tamañoMaximo - 1 << "). Intente de nuevo.\n\n";
                auxiliares::waitfor(3000);
                continue;
            }

            std::strncpy(texto, entrada.c_str(), tamañoMaximo - 1);
            texto[tamañoMaximo - 1] = '\0';

            if (ptrValidador != nullptr) {
                if (!ptrValidador(texto, mensajeError)) {
                    flag = true;
                    std::cout << mensajeError << std::endl << std::endl;
                    waitfor(3500);
                }
            }
        } while (flag);

        return true;
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

} // namespace auxiliares

// ============================================//
//   4. vALIDADORES                            //
// ============================================//

namespace validadores {

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
        auxiliares::toMayus(deporteActual);
    }

    // definimos las fechas para las futuras validaciones de datos
    void definirFechaInicio(const char *fechaInicio) { std::strncpy(fechaDeIni, fechaInicio, 11); }

    void definirFechaFin(const char *fechaFin) { std::strncpy(fechaDeFin, fechaFin, 11); }

    // =======================================================================================//
    //  Validaciones auxiliares (no se debe poder acceder a ellas desde fuera del namespace)  //
    // =======================================================================================//

    // encapsula todas las funciones que solo funcionan como auxiliares a otras funciones validadores principales
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

    bool minuto(const int variable, char *mensajeError) {
        if (variable < MINUTO_MINIMO || variable > MINUTO_MAXIMO) {
            std::strncpy(mensajeError, "El minuto debe estar entre ", TAMANO_MENSAJE_ERROR);
            return false;
        }
        return true;
    }

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
        auxiliares::toMayus(copiaDeporte);

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
        auxiliares::toMayus(copiaPosicion);

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

} // namespace validadores

// ============================================//
//   5. CAPA DE LOGICA                         //
// ============================================//

namespace Logica {

    // Abre el archivo y retorna el header si el archivo existe y si tiene header
    ArchivoHeader leerHeader(const char *nombreArchivo) {

        // si no existe el archivo devolvemos el header lleno de -1
        ArchivoHeader header, headerError = {-1, -1, -1, -1};

        if (!existeArchivo(nombreArchivo)) {

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
            if (archivo.fail()) {
                archivo.close();
                return headerError;
            }

            archivo.close();
            return header;
        }
    }

    namespace {

        void crearCarpeta() {
            // Sube un nivel, entra a bin y define la carpeta datos
            fs::path rutaDatos = "../bin/datos";

            try {
                // create_directories crea toda la ruta y devuelve false si ya existe
                if (fs::create_directories(rutaDatos)) {
                    std::cout << "Carpeta 'datos' creada en: " << rutaDatos << std::endl;
                } else {
                    // Entra aqui si la carpeta ya existia (no hace nada ni da error)
                    std::cout << "La carpeta 'datos' ya existe. No se realizaron cambios." << std::endl;
                }
            } catch (const fs::filesystem_error &e) {
                std::cerr << "Error de permisos o sistema: " << e.what() << std::endl;
            }
        }

        void definirFormato(Torneo &torneo, int opcion) {
            if (opcion == 1) {
                std::strncpy(torneo.formato, "GRUPOS", TAMANO_FORMATO);
            } else if (opcion == 2) {
                std::strncpy(torneo.formato, "ELIMINATORIA", TAMANO_FORMATO);
            }
        }

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
            comprobar.close();
            return true;
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

                // Si el archivo es de torneo solo cerramos el archivo // no añadimos header
                if (std::strcmp(nombreArchivo, NOMBRE_ARCHIVO_TORNEO) == 0) {
                    archivo.close();
                    return true;
                }

                // inicializamos un archivo Header
                ArchivoHeader nuevo = {0, 1, 0, 1};

                // Movemos el puntero de escritura al inicio por seguridad
                archivo.seekp(0, std::ios::beg);

                // escribimos el header en el archivo binario
                archivo.write(reinterpret_cast<const char *>(&nuevo), sizeof(ArchivoHeader));

                // Verificamos que se haya escrito bien
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
                if (archivo.fail()) {
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

                // Verficamos que la lectura haya sido correcta
                if (archivo.fail()) {
                    archivo.close();
                    return indice;
                }

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
        bool cadenaDuplicada(const char *nombreArchivo, const char *nombre, var2 struct3::*miembro) {
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
            auxiliares::toMinus(nombreBusquedaAux);
            struct3 registroTemporal;

            // movemos el puntero de indice despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // buscamos en el binario
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct3))) {

                // verificamos que no hayan errores en la lectura
                if (archivo.fail()) {
                    archivo.close();
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
        bool cadenaDuplicadaParaActualizar(const char *nombreArchivo, const char *nombre, const int idEquipo, var3 struct4::*miembro) {
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
            auxiliares::toMinus(copiaNuevoNombre);
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
                auxiliares::toMinus(nombreAux);

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
        int buscarRegistrosPorSucadena(const char *nombreArchivo, struct6 resultados[], const char *subcadena, const int maxResultados) {
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
            auxiliares::toMinus(copiaSubcadena);

            // Ubicamos el puntero de posicion despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Realizamos la lectura del archivo
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct6))) {

                // Verificamos que no haya error al hacer la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return error;
                }

                // Si encontramos un registro eliminado lo saltamos
                if (registroTemporal.eliminado) {
                    return error;
                }

                // Si aún no llegamos a la cantidad maxima de registros hacemos la comparacion
                if (cantidadDeRegistrosEncontrados < maxResultados) {

                    // hacemos una copia del nombre del registro
                    std::strncpy(copiaRegistro, registroTemporal.nombre, TAMANO_NOMBRE);
                    auxiliares::toMinus(copiaRegistro);

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
                    archivo.close();
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
    } // namespace

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
            auxiliares::toMinus(copiaSubcadena);


            // incializamos el contador en 0
            *contEquiposEncontrados = 0;

            // creamos un array de punteros dinamico con tamaño maximo el numero de equipos que hay
            Equipo **arrayEquiposEncontrados = new Equipo *[MiSistema->numEquiposActuales];

            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                // hacemos una copia del nombre del equipo
                std::strncpy(copiaRegistro, MiSistema->Equipos[e].nombre, sizeof(copiaRegistro));
                auxiliares::toMinus(copiaRegistro);

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
            Equipo **listaDeEquipos = new Equipo *[*cantEquipos];

            // recorremos el bucle para listar cada direccion de memoria de los equipos
            for (size_t e = 0; e < (*cantEquipos); e++) {
                listaDeEquipos[e] = &(MiSistema->Equipos[e]);
            }

            return listaDeEquipos;
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

        /*bool actualizarEquipo(const char *nombreArchivo, const int ID, const char *nombre, const char *entrenador, const char *ciudad) {

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
            if (cadenaDuplicadaParaActualizar(MiSistema, nombre, ID)) {
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
        }*/

        bool eliminarEquipo(const char *nombreArchivo, const int ID) {

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
                archivo.close();
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

                // verificamos que no se haya producido un error
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                if ((partidoTemporal.idEquipoLocal == registroTemporal.ID) || (partidoTemporal.idEquipoVisitante == registroTemporal.ID)) {
                    return false;
                }
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

                // Verificamos que la lectura haya sido correcta
                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    return false;
                }

                if (jugadorTemporal.idEquipo == registroTemporal.ID) {
                    return false;
                }
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

        bool DorsalDuplicado(const char *nombreArchivo, const int dorsal, const int idEquipo) {

            // Verificamos que exista el archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que se haya abierto
            if (!archivo.is_open()) {
                return false;
            }

            // Movemos el puntero de lectura despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            Jugador jugadorTemporal;

            // Leemos el archivo
            while (archivo.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {

                // Verificamos si no hubo error en la lectura
                if (archivo.fail()) {
                    archivo.close();
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

        bool dorsalDuplicadoParaActualizar(const char *nombreArchivo, const int dorsal, const int IDJugador, const int idEquipo, bool &error) {

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
                    archivo.close();
                    error = true;
                    return false;
                }

                // Saltamos al jugador que está siendo actualizado
                if (jugadorTemporal.ID == IDJugador) {
                    continue;
                }

                // Si encontramos coincidencia y pertence al mismo equipo
                if ((jugadorTemporal.numeroDorsal == dorsal) && (jugadorTemporal.idEquipo == idEquipo)) {
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        bool CedulaRepetida(const char *nombreArchivo, const char *cedula, bool &error) {

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
                    archivo.close();
                    error = true;
                    return false;
                }

                if (std::strcmp(jugadorTemporal.cedula, cedula) == 0) {
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        bool registrarJugador(const char *nombreArchivo, Jugador &nuevoJugador) {

            int error = -1;

            // verificamos primero que el archivo existe
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            // Leemos el header para obtener los datos
            ArchivoHeader header = leerHeader(nombreArchivo);

            // Validamos que el header no devuelva error
            if (header.cantidadRegistros == error) {
                return error;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Si se produjo un error a la hora de abrir el archivo devolvemos error
            if (!archivo.is_open()) {
                return error;
            }

            // * Inicializamos las estadisticas
            nuevoJugador.anotaciones = 0;
            nuevoJugador.tarjetaAmarillas = 0;
            nuevoJugador.tarjetasRojas = 0;

            // * Inicializamos las fechas de creacion y ultima modificacion
            nuevoJugador.fechaCreacion = std::time(nullptr);
            nuevoJugador.fechaUltimaModificacion = std::time(nullptr);

            // * Colocamos el jugador como activo y le colocamos el ID
            nuevoJugador.ID = header.proximoID;
            nuevoJugador.eliminado = false;

            // * Actualizamos el header
            header.cantidadRegistros++;
            header.registrosActivos++;
            header.proximoID++;

            // * Movemos el puntero de escritura al final
            archivo.seekp(0, std::ios::end);

            // * Registramos el nuevo jugador
            archivo.write(reinterpret_cast<const char *>(&nuevoJugador), sizeof(Jugador));

            // Verficamos que no haya errores con la escritura
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo de jugadores
            archivo.close();

            // * Actualziamos el header
            actualizarHeader(nombreArchivo, header);


            // * Aumentamos el numero de jugadores del equipo

            // buscamos el indice fisico del equipo al que pertence el jugador
            size_t indice = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, nuevoJugador.idEquipo);

            // Verificamos que indice no arroje error
            if (indice == error) {
                return false;
            }

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Equipo);

            std::fstream archivoEquipo;
            archivoEquipo.open(NOMBRE_ARCHIVO_EQUIPOS, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que se haya abierto correctamente
            if (!archivoEquipo.is_open()) {
                return false;
            }

            // Nos movemos a la posicion del registro (puntero de lectura)
            archivoEquipo.seekg(posicion, std::ios::beg);

            Equipo equipoTemporal;

            // Leemos el equipo
            archivoEquipo.read(reinterpret_cast<char *>(&equipoTemporal), sizeof(Equipo));

            // Verificamos que no haya errores de lectura
            if (archivoEquipo.fail()) {
                archivo.close();
                return false;
            }

            // aumentamos en 1 el numero de jugadores
            equipoTemporal.numJugadores++;

            // Ahora nos movemos a la posicion del registro (puntero de escritura)
            archivoEquipo.seekp(posicion, std::ios::beg);

            // Sobreescribmos el registro
            archivoEquipo.write(reinterpret_cast<const char *>(&equipoTemporal), sizeof(Equipo));

            // Verificamos que no hubo errores
            if (archivoEquipo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo de equipo
            archivoEquipo.close();
            return true;
        }

        int listarJugadoresPorEquipo(const char *nombreArchivo, const int idEquipo, Jugador resultados[], int maxResultados) {

            int error = -1;
            int cantidadDeEquiposEncontrados = 0;

            // verificamos primero que el archivo existe
            if (!existeArchivo(nombreArchivo)) {
                return error;
            }

            // Leemos el header para obtener los datos
            ArchivoHeader header = leerHeader(nombreArchivo);

            // Validamos que el header no devuelva error
            if (header.cantidadRegistros == error) {
                return false;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Si se produjo un error a la hora de abrir el archivo devolvemos false
            if (!archivo.is_open()) {
                return false;
            }

            Equipo equipoTemporal;
            Jugador jugadorTemporal;

            // Buscamos si el equipo solicitado realmente existe
            bool existe = buscarRegistrosPorId(nombreArchivo, equipoTemporal, idEquipo);

            // Verificamos si el equipo existe
            if (!existe) {
                return error;
            }

            // Si el equipo no tiene jugadores
            if (equipoTemporal.numJugadores == 0) {
                return error;
            }

            // buscamos los jugadores
            while (archivo.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {

                // Verificamos que la lectura no arroje error
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // Si aun nos superamos el maximo de resultados permitido
                if (cantidadDeEquiposEncontrados < maxResultados) {

                    // Si el jugador no pertence al equipo lo ignoramos
                    if (jugadorTemporal.idEquipo != idEquipo) {
                        continue;
                    }

                    // Guardamos los jugadores
                    resultados[cantidadDeEquiposEncontrados] = jugadorTemporal;

                    // aumentamos el contador de equipos encontrados
                    cantidadDeEquiposEncontrados++;

                } else { // si no rompemos el bucle
                    break;
                }
            }

            return cantidadDeEquiposEncontrados;
        }

        /*bool actualizarJugador(, int ID, Jugador jugadorActualizado) {
            if (!sistemaJugadoresValido(MiSistema)) {
                return false;
            }


            Jugador *jugadorBuscado.= buscarJugadorPorID(MiSistema, ID);
            if (jugadorBuscado.== nullptr) {
                return false;
            }

            // Si el nombre esta duplicado
            if (cadenaDuplicadaParaActualizar(MiSistema, jugadorActualizado.nombre, ID)) {
                return false;
            }

            // Si el dorsal esta duplicado
            if (dorsalDuplicadoParaActualizar(MiSistema, jugadorActualizado.dorsal, ID, jugadorBuscado.idEquipo)) {
                return false;
            }

            // En caso de que sí, actualizamos el jugador
            // Solo actualizamos NOMBRE, EDAD, DORSAL O POSICION
            std::strncpy(jugadorBuscado.nombre, jugadorActualizado.nombre);
            std::strncpy(jugadorBuscado.posicion, jugadorActualizado.posicion);
            jugadorBuscado.edad = jugadorActualizado.edad;
            jugadorBuscado.dorsal = jugadorActualizado.dorsal;

            return true;
        }*/

        bool eliminarJugador(const char *nombreArchivo, int ID) {

            int error = -1;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que se haya abierto correctamente
            if (!archivo.is_open()) {
                return false;
            }

            ArchivoHeader header = leerHeader(nombreArchivo);

            // Verificamos que el header se haya leido correctamente
            if (header.cantidadRegistros == -1) {
                return false;
            }

            // * Borramos el Jugador

            // Buscamos el indice fisico del jugador
            size_t indice = buscarIndicePorID<Jugador>(nombreArchivo, ID);

            // Verificamos si no se encontró ningun jugador con ese ID
            if (indice == error) {
                return false;
            }

            Jugador registroTemporal;

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Jugador);

            // Movemos el puntero de lectura al indice de posicion
            archivo.seekg(posicion, std::ios::beg);

            // * Leemos el registro
            archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Jugador));

            // Verificamos que no haya fallado
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Lo colocamos como eliminado
            registroTemporal.eliminado = true;

            // Movemos el puntero de escritura al indice de posicion
            archivo.seekp(posicion, std::ios::beg);

            // * Sobreescribimos el registro
            archivo.write(reinterpret_cast<const char *>(&registroTemporal), sizeof(Jugador));

            // Verificamos que no haya fallado
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo
            archivo.close();

            // Reducimos el numero de jugadores activos
            header.registrosActivos--;

            //  *Actualizamos el header
            actualizarHeader(nombreArchivo, header);


            // * Disminuimos el contador de jugadores del equipo correspondiente

            Equipo equipoTemporal;
            bool existeEquipo = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoTemporal, registroTemporal.idEquipo);

            // si tiene equipo disminuimos el numero de jugadoress del equipo, sino no hacemos nada
            if (existeEquipo && equipoTemporal.numJugadores > 0) {
                equipoTemporal.numJugadores--;
            }

            return true;
        }
    } // namespace jugadores

    namespace partidos {

        int minJugadoresPorDeporte() {
            for (size_t e = 0; e < validadores::totalDeportes; e++) {
                if (std::strcmp(validadores::Deportes[e], validadores::deporteActual) == 0) {
                    return validadores::MinimoJugadoresPorDeporte[e];
                }
            }
            return 1;
        }

        const char *estadoPartidos[] = {"PROGRAMADO", "JUGADO", "CANCELADO"};

        bool hayPartidoProgramadoEntre2(std::ifstream archivo, Partido &partido) {

            // Verificamos que esté aiberto antes de empezar a leer
            if (!archivo.is_open()) {
                return false;
            }

            // Movemos el puntero despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            Partido pAux;

            // Realizamos la lectura
            while (archivo.read(reinterpret_cast<char *>(&pAux), sizeof(Partido))) {

                // Verificamos que no se haya producido errores en la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                bool tienePartidoEntreSi = ((pAux.idEquipoLocal == partido.idEquipoLocal) && (pAux.idEquipoVisitante == partido.idEquipoVisitante)) ||
                                           ((pAux.idEquipoLocal == partido.idEquipoVisitante) && (pAux.idEquipoVisitante == partido.idEquipoLocal));

                // Si ya hay un partido programado entre ellos
                if (tienePartidoEntreSi && (std::strcmp(pAux.estado, estadoPartidos[0]) == 0)) {
                    return true;
                }
            }

            return false;
        }

        // Retorna array de partidos con ese estado ("PROGRAMADO", "JUGADO", "CANCELADO")
        // El llamador libera el array con delete[]
        int listarPartidosPorSuEstado(const char *nombreArchivo, Partido resultados[], const char *estado, const int maxResultados) {
            int cantidadDeRegistrosEncontrados = 0;
            int error = -1;
            Partido registroTemporal;
            ArchivoHeader header;

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

            if (header.cantidadRegistros == error) {
                archivo.close();
                return error;
            }

            // Ubicamos el puntero de posicion despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Realizamos la lectura del archivo
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Partido))) {

                // Verificamos si no hubo un falló en la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return error;
                }

                // Si encontramos un archivo que esta eliminado lo saltamos
                if (registroTemporal.eliminado) {
                    continue;
                }

                // Si aún no llegamos a la cantidad maxima de registros hacemos la comparacion
                if (cantidadDeRegistrosEncontrados < maxResultados) {

                    // comparamos con los estados
                    if (std::strcmp(resultados[cantidadDeRegistrosEncontrados].estado, estado) == 0) {
                        resultados[cantidadDeRegistrosEncontrados] = registroTemporal;
                        cantidadDeRegistrosEncontrados++;
                    }

                } else {
                    // sino salimos del bucle
                    break;
                }
            }

            // Verificamos que se hayan leido todos los registros
            if (header.registrosActivos < cantidadDeRegistrosEncontrados) {
                archivo.close();
                return error;
            }

            archivo.close();
            return cantidadDeRegistrosEncontrados;
        }

        bool programarPartido(const char *nombreArchivo, Partido &nuevoPartido) {
            int error = -1;

            // * Validaciones

            // Verificamos que exista el archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            // Leemos el header del archivo de Equipos
            ArchivoHeader headerEquipos = leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que el header se haya leido correctamente
            if (headerEquipos.cantidadRegistros == error) {
                return false;
            }

            // Si no hay al menos 2 equipos no se puede programar un partido
            if (headerEquipos.registrosActivos <= 1) {
                return false;
            }

            // No se puede programar un partido entre el mismo equipo
            if (nuevoPartido.idEquipoLocal == nuevoPartido.idEquipoVisitante) {
                return false;
            }

            Equipo eqLocal, eqVisitante;

            // Si los equipos no existen
            bool existeEquipoLocal = buscarRegistrosPorId<Equipo>(nombreArchivo, eqLocal, nuevoPartido.idEquipoLocal);
            bool existeEquipoVisitante = buscarRegistrosPorId<Equipo>(nombreArchivo, eqVisitante, nuevoPartido.idEquipoVisitante);

            // Si uno de los dos equipos no existe cancelamos
            if ((!existeEquipoLocal) || (!existeEquipoVisitante)) {
                return false;
            }

            // * Leemos el header del archivo de partidos
            ArchivoHeader header = leerHeader(nombreArchivo);

            // * Creamos y abrimos el archivo

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que abrió correctamente
            if (!archivo.is_open()) {
                return false;
            }

            // * Realizamos una lectura de todos los archivos para verificar que no tengan partidos programados entre sí

            if (hayPartidoProgramadoEntre2) {
                return false;
            }

            /*int minimoRequerido = minJugadoresPorDeporte();
            // Verificar que ambos equipos tengan suficientes jugadores
            if (EquipoLocal->numJugadores < minimoRequerido || EquipoVisitante->numJugadores < minimoRequerido) {
                return nullptr;
            }*/

            // * Procedemos a programar el partido
            nuevoPartido.ID = header.proximoID;

            // Asignamos los valores iniciales
            std::strncpy(nuevoPartido.estado, estadoPartidos[0], TAMANO_FECHA); // PROGRAMADO
            nuevoPartido.anotacionesLocal = 0;
            nuevoPartido.anotacionesVisitante = 0;
            nuevoPartido.numAnotaciones = 0;
            nuevoPartido.eliminado = false;

            // Asignamos los valores de tiempo
            nuevoPartido.fechaCreacion = std::time(nullptr);
            nuevoPartido.fechaUltimaModificacion = std::time(nullptr);

            // Movemos el puntero de escritura al final para programar el nuevo partido
            archivo.seekp(0, std::ios::end);

            // * Guardamos el registro en el archivo
            archivo.write(reinterpret_cast<const char *>(&nuevoPartido), sizeof(Partido));

            // Verificamos que la escritura no arroje errores
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo
            archivo.close();

            // Modificamos los datos del header
            header.proximoID++;
            header.cantidadRegistros++;
            header.registrosActivos++;

            // Actualizamos el header
            actualizarHeader(nombreArchivo, header);

            return true;
        }

        // TODO:   Victoria local  → local  +3 pts, +1 victoria  / visitante +1 derrota
        // TODO:   Empate          → ambos  +1 pt,  +1 empate
        // TODO:   Victoria visit. → visit. +3 pts, +1 victoria  / local     +1 derrota
        bool registrarResultado(const char *nombreArchivo, Partido registroPartido) {
            int error = -1;
            Partido resultados[MAX_RESULTADOS];
            // * Validaciones

            // Verificamos que exista el archivo
            if (!existeArchivo(nombreArchivo) && !existeArchivo(NOMBRE_ARCHIVO_EQUIPOS) && !existeArchivo(NOMBRE_ARCHIVO_JUGADORES)) {
                return false;
            }

            // Leemos el header del archivo de Equipos
            ArchivoHeader headerEquipos = leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que el header se haya leido correctamente
            if (headerEquipos.cantidadRegistros == error) {
                return false;
            }

            // Si no hay al menos 2 equipos no se puede programar un partido
            if (headerEquipos.registrosActivos <= 1) {
                return false;
            }

            //  Ahora leemos el header de partidos para verificar que hayan partidos
            ArchivoHeader headerPartidos = leerHeader(nombreArchivo);

            // Verificamos que el header se haya leido correctamente
            if (headerPartidos.cantidadRegistros == error) {
                return false;
            }

            // Si no hay partidos activos no hacemos nada
            if (headerPartidos.registrosActivos == 0) {
                return false;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que esté abierto
            if (!archivo.is_open()) {
                return false;
            }

            Partido registroTemporal;
            int contador;

            // Verificamos que haya partidos en estado programado
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Partido))) {

                // Verificamos que la lectura haya sido correcta
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // Verificamos si el estado es programado
                if (std::strcmp(registroTemporal.estado, estadoPartidos[0]) == 0) {
                    contador++;
                }
            }

            // Verificamos que hay partidos
            if (contador <= 0) {
                archivo.close();
                return false;
            }

            // Por seguridad
            if (registroPartido.anotacionesLocal < 0 || registroPartido.anotacionesVisitante < 0) {
                archivo.close();
                return false;
                ;
            }

            // Ademas verificamos que el numAnotaciones no sea mayor que el maximo
            if (registroPartido.numAnotaciones > MAX_ANOTACIONES) {
                archivo.close();
                return false;
            }


            // * 1. Leemos el partido y verificamos que exsite

            // verificamos que el partido si exista (en estado programado);
            Partido nuevoPartido;
            bool existePartido = buscarRegistrosPorId<Partido>(nombreArchivo, nuevoPartido, registroPartido.ID);

            // Verificamos si encontro el partido
            if (!existePartido) {
                archivo.close();
                return false;
            }

            // Si encontró el partido verificamos que esté en estado programado
            if (std::strcmp(nuevoPartido.estado, estadoPartidos[0]) != 0) {
                archivo.close();
                return false;
            }

            // * 2. Leemos cada Equipo de los binarios

            Equipo eqLocal, eqVisitante;
            bool existe;

            existe = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, nuevoPartido.idEquipoLocal);

            // Verificamos por seguridad
            if (!existe) {
                archivo.close();
                return false;
            }

            existe = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, nuevoPartido.idEquipoVisitante);

            // Verificamos por seguridad
            if (!existe) {
                archivo.close();
                return false;
            }

            // * 3. Determinamos el resultado del partido y actualizamos las estadisticas en memoria de cada equipo

            // Si ocurre un empate en deportes donde no se permiten empates retornamos false
            if (registroPartido.anotacionesLocal == registroPartido.anotacionesVisitante) {
                if (std::strcmp(validadores::deporteActual, "BALONCESTO") == 0 || std::strcmp(validadores::deporteActual, "TENIS") == 0 ||
                    std::strcmp(validadores::deporteActual, "VOLEIBOL") == 0 || std::strcmp(validadores::deporteActual, "BEISBOL") == 0 ||
                    std::strcmp(validadores::deporteActual, "SOFTBOL") == 0) {
                    archivo.close();
                    return false;
                }
            }

            // Actualizamos las anotaciones/tarjetas del partido
            nuevoPartido.anotacionesLocal = registroPartido.anotacionesLocal;
            nuevoPartido.anotacionesVisitante = registroPartido.anotacionesVisitante;

            nuevoPartido.tarjetasAmaLocal = registroPartido.tarjetasAmaLocal;
            nuevoPartido.tarjetasAmaVisitante = registroPartido.tarjetasAmaVisitante;

            nuevoPartido.tarjetasRojasLocal = registroPartido.tarjetasRojasLocal;
            nuevoPartido.tarjetasAmaVisitante = registroPartido.tarjetasRojasVisitante;

            // Si el equipo local ganó
            if (registroPartido.anotacionesLocal > registroPartido.anotacionesVisitante) {
                eqLocal.victorias++; // Aumentamos las victorias del local
                eqLocal.puntos += 3; // Aumentamos los puntos del local

                eqVisitante.derrotas++; // Aumentamos las derrotas del visitante

                // Si el equipo visitante ganó
            } else if (registroPartido.anotacionesVisitante > registroPartido.anotacionesLocal) {
                eqVisitante.victorias++; // Aumentamos las victorias
                eqVisitante.puntos += 3; // Aumentamos los puntos

                eqLocal.derrotas++; // Aumentamos las derrotas

                // Si empataron en un deporte permitido
            } else {
                eqLocal.empates++;
                eqLocal.puntos += 1;

                eqVisitante.empates++;
                eqVisitante.puntos += 1;
            }

            // Añadimos los puntos a favor y en contra
            eqLocal.anotacionAFavor += registroPartido.anotacionesLocal;
            eqLocal.anotacionEnContra += registroPartido.anotacionesVisitante;
            eqLocal.jugados++;

            eqVisitante.anotacionAFavor += registroPartido.anotacionesVisitante;
            eqVisitante.anotacionEnContra += registroPartido.anotacionesLocal;
            eqVisitante.jugados++;

            // * 4. Agregamos el id del partido al array de cada equipo y aumentamos el numero de partidos
            eqLocal.partidosIDs[eqLocal.cantidadPartidos] = registroPartido.ID;
            eqVisitante.partidosIDs[eqVisitante.cantidadPartidos] = registroPartido.ID;
            eqLocal.cantidadPartidos++;
            eqVisitante.cantidadPartidos++;

            // Agregamos la fecha de modificacion
            eqLocal.fechaUltimaModificacion = std::time(nullptr);
            eqVisitante.fechaUltimaModificacion = std::time(nullptr);

            // * 5. Registramos goles/tarjetas y detalle de goles/tarjetas
            nuevoPartido.numAnotaciones = registroPartido.numAnotaciones;
            nuevoPartido.numtarjetaAma = registroPartido.numtarjetaAma;
            nuevoPartido.numTarjetasRojas = registroPartido.numTarjetasRojas;

            // Puedo usar std::copy pero solo usaré for por el momento
            for (size_t e = 0; e < nuevoPartido.numAnotaciones; e++) {
                nuevoPartido.anotaciones[e] = registroPartido.anotaciones[e];
            }

            for (size_t e = 0; e < nuevoPartido.numtarjetaAma; e++) {
                nuevoPartido.tarjetaA[e] = registroPartido.tarjetaA[e];
            }

            for (size_t e = 0; e < nuevoPartido.numTarjetasRojas; e++) {
                nuevoPartido.tarjetaR[e] = registroPartido.tarjetaR[e];
            }

            // * 6. Actualizamos las estadisticas de los jugadores por cada gol / tarjeta

            std::fstream archivoJugadores;
            archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que esté abierto
            if (!archivoJugadores.is_open()) {
                archivo.close();
                return false;
            }

            Jugador jugadorAux;

            // Modificamos los goles
            for (size_t e = 0; e < registroPartido.numAnotaciones; e++) {

                // Si fue un autogol, saltamos esta iteracion
                if (registroPartido.anotaciones[e].idJugador == 0) {
                    continue;
                }

                // Buscamos el indice
                size_t indiceBuscado = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, nuevoPartido.anotaciones[e].idJugador);

                // Si ocurrió un error detenemos el proceso
                if (indiceBuscado == error) {
                    return false;
                }

                // Calculamos la posicion a la que nos vamos  a mover
                std::streampos posicion = sizeof(ArchivoHeader) + indiceBuscado * sizeof(Jugador);

                // Nos movemos a esa posicion
                archivoJugadores.seekg(posicion, std::ios::beg);

                // Leemos ese registro
                archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                // modificamos los valores
                jugadorAux.anotaciones++;

                // Nos movemos a la posicion nuevamente
                archivoJugadores.seekp(posicion, std::ios::beg);

                // Escribimos el jugador
                archivoJugadores.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));
            }

            // Modificamos las tarjetas amarillas
            for (size_t e = 0; e < registroPartido.numtarjetaAma; e++) {
                // Buscamos el indice
                size_t indiceBuscado = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, nuevoPartido.tarjetaA[e].idJugador);

                // Si ocurrió un error detenemos el proceso
                if (indiceBuscado == error) {
                    return false;
                }

                // Calculamos la posicion a la que nos vamos  a mover
                std::streampos posicion = sizeof(ArchivoHeader) + indiceBuscado * sizeof(Jugador);

                // Nos movemos a esa posicion
                archivoJugadores.seekg(posicion, std::ios::beg);

                // Leemos ese registro
                archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                // modificamos los valores
                jugadorAux.tarjetaAmarillas++;

                // Nos movemos a la posicion nuevamente
                archivoJugadores.seekp(posicion, std::ios::beg);

                // Escribimos el jugador
                archivoJugadores.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));
            }

            // Modificamos las tarjetas rojas
            for (size_t e = 0; e < registroPartido.numTarjetasRojas; e++) {
                // Buscamos el indice
                size_t indiceBuscado = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, nuevoPartido.tarjetaR[e].idJugador);

                // Si ocurrió un error detenemos el proceso
                if (indiceBuscado == error) {
                    return false;
                }

                // Calculamos la posicion a la que nos vamos  a mover
                std::streampos posicion = sizeof(ArchivoHeader) + indiceBuscado * sizeof(Jugador);

                // Nos movemos a esa posicion
                archivoJugadores.seekg(posicion, std::ios::beg);

                // Leemos ese registro
                archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                // modificamos los valores
                jugadorAux.tarjetasRojas++;

                // Nos movemos a la posicion nuevamente
                archivoJugadores.seekp(posicion, std::ios::beg);

                // Escribimos el jugador
                archivoJugadores.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));
            }

            // Cerramos el archivo
            archivoJugadores.close();

            // * 7. Cambiamos el estado del partido a jugado
            std::strncpy(nuevoPartido.estado, estadoPartidos[1], TAMANO_ESTADO); // JUGADO
            nuevoPartido.fechaUltimaModificacion = std::time(nullptr);

            // * 8. Escribimos cada equipo en su repectivo archivo

            // Abrimos el archivo de equipos
            std::fstream archivoEquipos;
            archivoEquipos.open(NOMBRE_ARCHIVO_EQUIPOS, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que el archivo abrió
            if (!archivo.is_open()) {
                archivo.close();
                return false;
            }

            // Buscamos el indice de cada equipo
            size_t indiceLocal, indiceVisitante;
            indiceLocal = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal.ID);
            indiceVisitante = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante.ID);

            // Verificamos que no devuelvan error
            if (indiceLocal == error || indiceVisitante == error) {
                archivo.close();
                return false;
            }

            // Calculamos las posiciones
            std::streampos posicionLocal = sizeof(ArchivoHeader) + indiceLocal * sizeof(Equipo);
            std::streampos posicionVisitante = sizeof(ArchivoHeader) + indiceVisitante * sizeof(Equipo);

            // * Añadimos al Equipo Local

            // Nos movemos a la posicion
            archivoEquipos.seekp(posicionLocal, std::ios::beg);

            // Escribimos el equipo
            archivoEquipos.write(reinterpret_cast<const char *>(&eqLocal), sizeof(Equipo));

            // Verificamos si no dió error la escritura
            if (archivoEquipos.fail()) {
                archivoEquipos.close();
                archivo.close();
                return false;
            }

            // * Añadimos al Equipo Visitante

            // Nos movemos a la posicion
            archivoEquipos.seekp(posicionVisitante, std::ios::beg);

            // Escribimos el equipo
            archivoEquipos.write(reinterpret_cast<const char *>(&eqVisitante), sizeof(Equipo));

            // Verificamos si no dió error
            if (archivoEquipos.fail()) {
                archivoEquipos.close();
                archivo.close();
                return false;
            }

            // cerramos el archivo
            archivoEquipos.close();

            // * 9. Guardamos el partido
            size_t indice = buscarIndicePorID<Partido>(nombreArchivo, nuevoPartido.ID);

            // Verificamos que no devuelva error
            if (indice == error) {
                archivo.close();
                return false;
            }

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Partido);

            // Nos movemos a esa posicion
            archivo.seekp(posicion, std::ios::beg);

            // Sobreescribimos el partido
            archivo.write(reinterpret_cast<const char *>(&nuevoPartido), sizeof(Partido));

            // Verificamos que no devuelva error
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo
            archivo.close();

            return true;
        }

        int listarPartidosPorEquipo(const char *nombreArchivo, const int idEquipo, Partido resultados[], int maxResultados) {
            int cantidadDeRegistrosEncontrados = 0;
            int error = -1;
            Partido registroTemporal;
            ArchivoHeader header;

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

            // Verificamos que la lectura del header no sea incorrecta
            if (header.cantidadRegistros == error) {
                archivo.close();
                return error;
            }

            // Ubicamos el puntero de posicion despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Realizamos la lectura del archivo
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Partido))) {

                // Verificamos si no hubo un fallo en la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return error;
                }

                // Si encontramos un archivo que esta eliminado lo saltamos
                if (registroTemporal.eliminado) {
                    continue;
                }

                // Si aún no llegamos a la cantidad maxima de registros hacemos la comparacion
                if (cantidadDeRegistrosEncontrados < maxResultados) {

                    // comparamos con los estados
                    if (registroTemporal.ID == idEquipo) {
                        resultados[cantidadDeRegistrosEncontrados] = registroTemporal;
                        cantidadDeRegistrosEncontrados++;
                    }

                } else {
                    // sino salimos del bucle
                    break;
                }
            }

            // Verificamos que se hayan leido todos los registros
            if (header.registrosActivos < cantidadDeRegistrosEncontrados) {
                return error;
            }

            archivo.close();
            return cantidadDeRegistrosEncontrados;
        }

        // Cancela un partido: cambia estado a "CANCELADO"
        // Si el partido ya fue JUGADO, revierte las estadísticas de ambos equipos
        // Retorna true si se canceló, false si no existe o ya estaba cancelado
        bool cancelarPartido(const char *nombreArchivo, const int idPartido) {
            int error = -1;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que el archivo este abierto
            if (!archivo.is_open()) {
                return false;
            }

            // * Buscar y leer el Partido

            Partido pAux;
            size_t indice = buscarIndicePorID<Partido>(nombreArchivo, idPartido);

            // Verificamos que el indice no sea paramtro de error (que sea -1)
            if (indice == error) {
                archivo.close();
                return false;
            }

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Partido);

            // Movemos el puntero de lectura a esa posicion
            archivo.seekg(posicion, std::ios::beg);

            archivo.read(reinterpret_cast<char *>(&pAux), sizeof(Partido));

            // Verificamos que no se produjo un error
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Por seguridad (No sea el caso de que haya un bug o un error no se)
            if (pAux.eliminado) {
                return false;
            }

            // Si está jugado debemos revertir todo
            if (std::strcmp(pAux.estado, estadoPartidos[1]) == 0) {

                // * 1. Buscamos los equipos para revertir las estadisticas

                std::fstream archivoEquipos;
                archivoEquipos.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

                // Verificamos que esté abierto
                if (!archivoEquipos.is_open()) {
                    return false;
                }

                Equipo eqLocal, eqVisitante;

                // Buscamos el indice fisico
                size_t indiceLocal = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, pAux.idEquipoLocal);
                size_t indiceVisitante = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, pAux.idEquipoVisitante);

                // Verificamos que la lectura de los indices fue correcta
                if (indiceLocal == error || indiceVisitante == error) {
                    return false;
                }

                // Calculamos la posicion
                std::streampos posicionLocal = sizeof(ArchivoHeader) + indice * sizeof(Equipo);
                std::streampos posicionVisitante = sizeof(ArchivoHeader) + indice * sizeof(Equipo);

                // Obtenemos el equipo local primero
                archivoEquipos.seekg(posicionLocal, std::ios::beg);
                archivo.read(reinterpret_cast<char *>(&eqLocal), sizeof(Equipo));

                // Verificamos si falló
                if (archivoEquipos.fail()) {
                    archivo.close();
                    archivoEquipos.close();
                    return false;
                }

                // Luego el equipo visitante
                archivoEquipos.seekg(posicionVisitante, std::ios::beg);
                archivoEquipos.read(reinterpret_cast<char *>(&eqVisitante), sizeof(Equipo));

                // Verificamos si falló
                if (archivoEquipos.fail()) {
                    archivo.close();
                    archivoEquipos.close();
                    return false;
                }

                // Si el equipo local ganó
                if (pAux.anotacionesLocal > pAux.anotacionesVisitante) {
                    eqLocal.victorias--; // Revertimos las victorias del local
                    eqLocal.puntos -= 3; // Revertimos los puntos del local

                    eqVisitante.derrotas--; // Revertimos las derrotas del visitante

                    // Si el equipo visitante ganó
                } else if (pAux.anotacionesVisitante > pAux.anotacionesLocal) {
                    eqVisitante.victorias--; // Revertimos las victorias
                    eqVisitante.puntos -= 3; // Revertimos los puntos

                    eqLocal.derrotas--; // Revertimos las derrotas

                    // Si empataron en un deporte permitido
                } else {
                    eqLocal.empates--;   // Revertimos los empates
                    eqLocal.puntos -= 1; // Revertimos los puntos

                    eqVisitante.empates--;   // Revertimos los empates
                    eqVisitante.puntos -= 1; // Revertimos los puntos
                }

                // Quitamos los puntos a favor y en contra
                eqLocal.anotacionAFavor -= pAux.anotacionesLocal;
                eqLocal.anotacionEnContra -= pAux.anotacionesVisitante;
                eqLocal.jugados--;

                eqVisitante.anotacionAFavor -= pAux.anotacionesVisitante;
                eqVisitante.anotacionEnContra -= pAux.anotacionesLocal;
                eqVisitante.jugados--;

                // Eliminamos el id del partido al array de cada equipo y revertimos el numero de partidos
                eqLocal.partidosIDs[eqLocal.cantidadPartidos] = 0;
                eqVisitante.partidosIDs[eqVisitante.cantidadPartidos] = 0;
                eqLocal.cantidadPartidos--;
                eqVisitante.cantidadPartidos--;

                // Agregamos la fecha de modificacion
                eqLocal.fechaUltimaModificacion = std::time(nullptr);
                eqVisitante.fechaUltimaModificacion = std::time(nullptr);

                // * 2. Buscamos los jugadores para revertir estadisticas

                // Abrimos el archivo de jugadores
                std::fstream archivoJugadores;
                archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary | std::ios::out | std::ios::out);

                // Verificamos si falló
                if (archivoJugadores.fail()) {
                    archivoEquipos.close();
                    archivo.close();
                    return false;
                }

                // Limpiamos los goles y los detalles de cada gol
                for (size_t e = 0; e < pAux.numAnotaciones; e++) {

                    Jugador jugadorAux;

                    // Buscamos el índice jugador que realió la anotacion
                    size_t indiceJugador = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, pAux.anotaciones[e].idJugador);

                    // Calculamos la posicion
                    std::streampos posicionJugador = sizeof(ArchivoHeader) + indiceJugador * sizeof(Jugador);

                    // Movemos el puntero de lectura
                    archivoJugadores.seekg(posicionJugador, std::ios::beg);

                    // Realizamos la lectura
                    archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        archivo.close();
                    }

                    // Modificamos el valor
                    jugadorAux.anotaciones--;

                    // Movemos el puntero de escritura
                    archivo.seekp(posicionJugador, std::ios::beg);

                    // Sobreescribimos el archivo
                    archivo.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        archivo.close();
                    }

                    pAux.anotaciones[e] = {0, 0, 0};
                }

                pAux.anotacionesLocal = 0;
                pAux.anotacionesVisitante = 0;
                pAux.numAnotaciones = 0;

                // Limpiamos las tarjetas amarillas y los detalles de cada tarjeta Amarilla
                for (size_t e = 0; e < pAux.numtarjetaAma; e++) {

                    Jugador jugadorAux;

                    // Buscamos el índice jugador que realió la anotacion
                    size_t indiceJugador = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, pAux.tarjetaA[e].idJugador);

                    // Calculamos la posicion
                    std::streampos posicionJugador = sizeof(ArchivoHeader) + indiceJugador * sizeof(Jugador);

                    // Movemos el puntero de lectura
                    archivoJugadores.seekg(posicionJugador, std::ios::beg);

                    // Realizamos la lectura
                    archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        archivo.close();
                    }

                    // Modificamos el valor
                    jugadorAux.tarjetaAmarillas--;

                    // Movemos el puntero de escritura
                    archivo.seekp(posicionJugador, std::ios::beg);

                    // Sobreescribimos el archivo
                    archivo.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        archivo.close();
                    }

                    pAux.tarjetaA[e] = {0, 0, 0};
                }

                pAux.tarjetasAmaLocal = 0;
                pAux.tarjetasAmaVisitante = 0;
                pAux.numtarjetaAma = 0;

                // Limpiamos las tarjetas rojas y los detalles de cada tarjeta roja
                for (size_t e = 0; e < pAux.numTarjetasRojas; e++) {

                    Jugador jugadorAux;

                    // Buscamos el índice jugador que realió la anotacion
                    size_t indiceJugador = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, pAux.tarjetaR[e].idJugador);

                    // Calculamos la posicion
                    std::streampos posicionJugador = sizeof(ArchivoHeader) + indiceJugador * sizeof(Jugador);

                    // Movemos el puntero de lectura
                    archivoJugadores.seekg(posicionJugador, std::ios::beg);

                    // Realizamos la lectura
                    archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        archivo.close();
                    }

                    // Modificamos el valor
                    jugadorAux.tarjetasRojas--;

                    // Movemos el puntero de escritura
                    archivo.seekp(posicionJugador, std::ios::beg);

                    // Sobreescribimos el archivo
                    archivo.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        archivo.close();
                    }

                    pAux.tarjetaR[e] = {0, 0, 0};
                }

                pAux.tarjetasRojasLocal = 0;
                pAux.tarjetasRojasVisitante = 0;
                pAux.numTarjetasRojas = 0;

                archivoJugadores.close();

                // * Sobreescribimos los equipos

                // Primero el equipo local
                archivoEquipos.seekp(posicionLocal, std::ios::beg);
                archivo.write(reinterpret_cast<const char *>(&eqLocal), sizeof(Equipo));

                // Verificamos si falló
                if (archivoEquipos.fail()) {
                    archivo.close();
                    archivoEquipos.close();
                    return false;
                }

                // Luego el equipo visitante
                archivoEquipos.seekp(posicionVisitante, std::ios::beg);
                archivoEquipos.write(reinterpret_cast<const char *>(&eqVisitante), sizeof(Equipo));

                // Verificamos si falló
                if (archivoEquipos.fail()) {
                    archivo.close();
                    archivoEquipos.close();
                    return false;
                }

                // Cerramos el archivo
                archivoEquipos.close();

                archivo.close();
                return true;

                // Si está programado solo lo colocamos como eliminado
            } else if (std::strcmp(pAux.estado, estadoPartidos[0]) == 0) {

                // Colocamos el partido como cancelado
                std::strncpy(pAux.estado, estadoPartidos[2], TAMANO_ESTADO);

                // No se si deba eliminarlo
                // pAux.eliminado =true;

                // Movemos el puntero de escritura a la posicion
                archivo.seekp(posicion, std::ios::beg);

                // Sobreescribimos el archivo
                archivo.write(reinterpret_cast<const char *>(&pAux), sizeof(Partido));

                // Verificamos que no se produjo un error
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                archivo.close();
                return true;

            } else {

                archivo.close();
                return false; // Si está cancelado devolvemos error
            }
        }

        /*bool eliminarPartido() {
            //
        }*/
    } // namespace partidos

} // namespace Logica

// ============================================//
//   6. pRESENTACION                           //
// ============================================//

namespace presentacion {

    namespace equipos {

        // Recolectamos los datos para registrar el equipo
        void RegistrarEquipos(const char *nombreArchivo) {
            bool flagError = false;
            bool cancelado = false;
            Equipo nuevo;
            char confirmacion;

            // Recolectamos el nombre del Equipo
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarCadena(nuevo.nombre, sizeof(Equipo::nombre), "Ingrese el nombre del Equipo: ", &cancelado, validadores::Nombres)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    return;
                }

                // Validamos nombre duplicado
                if (Logica::cadenaDuplicada<Equipo>(nombreArchivo, nuevo.nombre, &Equipo::nombre)) {
                    std::cerr << "Error, el nombre '" << nuevo.nombre << "' ya está en uso\n";
                    flagError = true;
                    auxiliares::waitfor(3000);
                    continue;
                }
                auxiliares::waitfor(2000);
            } while (flagError);


            // Recolectamos el nombre del entrenador del nuevo Equipo
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarCadena(nuevo.entrenador, sizeof(Equipo::entrenador), "Ingrese el nombre del Entrenador: ", &cancelado, validadores::Nombres)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Validamos nombre duplicado
                if (Logica::cadenaDuplicada(NOMBRE_ARCHIVO_EQUIPOS, nuevo.entrenador, &Equipo::entrenador)) {
                    std::cerr << "Error, el nombre '" << nuevo.entrenador << "' ya direge otro equipo\n";
                    flagError = true;
                    auxiliares::waitfor(3000);
                    continue;
                }
                auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la fecha de registro del equipo
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(nuevo.fechaRegistro, sizeof(Equipo::fechaRegistro), "Ingrese la fecha de Registro del Equipo: ", &cancelado,
                                            validadores::fechaValidaRegistroDeJugadorOEquipo)) {
                std::cout << "\nRegistro cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(2000);
            auxiliares::limpiarPantalla();

            // Recolectamos la ciudad del Equipo
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(nuevo.ciudad, 50, "Ingrese el nombre de la Ciudad del Equipo: ", &cancelado, validadores::Nombres)) {
                std::cout << "\nRegistro cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(2000);
            auxiliares::limpiarPantalla();

            // Pedimos la confirmacion al usuario
            if (!auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea registrar este equipo? (S/N)", &cancelado)) {
                std::cout << "\nRegistro cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                auxiliares::limpiarPantalla();

                // Abrimos el archivo de equipos
                std::fstream archivoEquipos;
                archivoEquipos.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

                // Verificamos que abrió correctamente
                if (!archivoEquipos.is_open()) {
                    std::cerr << "\nError del Sistema!\n";
                    std::cout << "Registro Cancelado\n";
                    auxiliares::pausarPrograma;
                    return;
                }

                // Buscamos y leemos el nombre del torneo
                Torneo torneo;

                std::ifstream archivoTorneo;
                archivoTorneo.open(NOMBRE_ARCHIVO_TORNEO, std::ios::binary);

                if (!archivoTorneo.is_open()) {
                    std::cerr << "\nError del Sistema\n";
                    std::cout << "Registro Cancelado\n";
                    archivoEquipos.close();
                    auxiliares::pausarPrograma;
                    return;
                }

                // Movemos el puntero de lectura al principio
                archivoTorneo.seekg(0, std::ios::beg);

                // Leemos el torneo
                archivoTorneo.read(reinterpret_cast<char *>(&torneo), sizeof(Torneo));

                // Verificamos si la lectura fue exitosa
                if (archivoTorneo.fail()) {
                    std::cerr << "\nError del Sistema\n";
                    std::cout << "Registro Cancelado\n";
                    archivoTorneo.close();
                    archivoEquipos.close();
                    auxiliares::pausarPrograma;
                    return;
                }

                // Cerramos el archivo
                archivoTorneo.close();

                // Movemos el puntero de posicion al final
                archivoEquipos.seekp(0, std::ios::end);

                // Escribimos el nuevo archivo
                archivoEquipos.write(reinterpret_cast<const char *>(&nuevo), sizeof(Equipo));

                // Verificamos que no se produjo error en la escritura
                if (archivoEquipos.fail()) {
                    std::cerr << "\nError del Ssitema\n";
                    std::cout << "Registro Cancelado\n";
                    archivoEquipos.close();
                    auxiliares::pausarPrograma;
                    return;
                }

                // Cerramos el fichero
                archivoEquipos.close();

                // Si el equipo se creo conn éxito
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        EQUIPO REGISTRADO CON ÉXITO        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Mostramos los datos ingresados
                std::cout << "Torneo: " << torneo.nombre << std::endl;
                std::cout << "Nombre del Equipo: " << nuevo.nombre << std::endl;
                std::cout << "Entrenador del Equipo: " << nuevo.entrenador << std::endl;
                std::cout << "Ciudad del Equipo: " << nuevo.ciudad << std::endl;
                std::cout << "Fecha de Registro del Equipo: " << nuevo.fechaRegistro << std::endl;
                std::cout << "Id del Equipo: " << nuevo.ID << std::endl;

            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                auxiliares::limpiarPantalla();
                std::cout << "\nRegistro de Equipo Cancelado\n";
            } else {
                auxiliares::limpiarPantalla();
                std::cerr << "\nError: No se ingresó una opción correcta (S/N)\n";
                std::cout << "\nRegistro de Equipo Cancelado\n";
            }
            auxiliares::pausarPrograma();
        }

        void buscarEquipoPorID(const char *nombreArchivo) {
            int ID = 0;
            bool cancelado = false;
            int error = -1;
            bool equipoEncontrado = false;
            Equipo equipoBuscado;
            auxiliares::limpiarPantalla();

            // Leemos el header del archivo de equipo para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = Logica::leerHeader(nombreArchivo);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Pedimos los datos de busqueda
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║        BUSQUEDA DE EQUIPOS POR ID         ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarDatos(ID, "Ingrese el ID (escriba 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "\nBusqueda cancelada por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo
            equipoEncontrado = Logica::buscarRegistrosPorId<Equipo>(nombreArchivo, equipoBuscado, ID);

            // Limpiamos la pantalla
            auxiliares::limpiarPantalla();
            std::cout << "\nBuscando...\n";
            auxiliares::waitfor(1500);

            // Si no fue encontrado enviamos mensaje de error de busqueda, si se encontro mostramos los datos
            if (!equipoEncontrado) {
                std::cout << "\nEl equipo de ID " << ID << " no fue encontrado\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║             EQUIPO ENCONTRADO             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  ID del Equipo:       " << equipoBuscado.ID << "\n";
                std::cout << "  Nombre:              " << equipoBuscado.nombre << "\n";
                std::cout << "  Entrenador:          " << equipoBuscado.entrenador << "\n";
                std::cout << "  Ciudad Origen:       " << equipoBuscado.ciudad << "\n";
                std::cout << "  Fecha de Registro:   " << equipoBuscado.fechaRegistro << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Estadísticas en el Torneo:\n";
                std::cout << "    Puntos Totales:    " << equipoBuscado.puntos << "\n";
                std::cout << "    Victorias:         " << equipoBuscado.victorias << "\n";
                std::cout << "    Empates:           " << equipoBuscado.empates << "\n";
                std::cout << "    Derrotas:          " << equipoBuscado.derrotas << "\n";
                std::cout << "    Puntos a Favor:    " << equipoBuscado.anotacionAFavor << "\n";
                std::cout << "    Puntos en Contra:  " << equipoBuscado.anotacionEnContra << "\n";
            }
            std::cout << "-------------------------------------------------------------\n\n";
            auxiliares::pausarPrograma();
        }

        void buscarEquiposPorSubCadena(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            bool cancelado = false;
            int cantEquiposEncontrados = 0;
            int error = -1;
            char subcadena[TAMANO_NOMBRE];
            const int maxResultados = MAX_RESULTADOS;
            Equipo resultados[maxResultados];

            // Leemos el header del archivo de equipo para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = Logica::leerHeader(nombreArchivo);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Solicitamos los datos de busqueda
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║      BUSQUEDA DE EQUIPOS POR NOMBRE       ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(subcadena, TAMANO_NOMBRE, "Escribe el nombre (o parte del nombre) del equipo que buscas (ingresa 'cancelar' para cancelar): ",
                                            &cancelado, validadores::Nombres)) {
                std::cout << "\nBusqueda cancelada por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Limpiamos la pantalla
            auxiliares::limpiarPantalla();

            std::cout << "\nBuscando...\n";

            // Llamamos a la funcion de busqueda
            cantEquiposEncontrados = Logica::buscarRegistrosPorSucadena<Equipo>(nombreArchivo, resultados, subcadena, maxResultados);

            auxiliares::waitfor(1500);
            auxiliares::limpiarPantalla();

            // Si no se enocontro ningun equipo
            if (cantEquiposEncontrados <= 0) {
                std::cout << "\nNo se encontro ninguna coincidencia con: '" << subcadena << "'\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          RESULTADOS ENCONTRADOS           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "----------------------------------------------------------------------------\n";
                std::cout << "Se encontraron " << cantEquiposEncontrados << " coincidencia(s):\n";
                std::cout << "----------------------------------------------------------------------------\n";

                for (size_t e = 0; e < cantEquiposEncontrados; e++) {
                    std::cout << std::endl << e + 1 << ".\n";
                    std::cout << "   Nombre:                   " << resultados[e].nombre << "\n";
                    ;
                    std::cout << "   ID:                       " << resultados[e].ID << "\n";
                    std::cout << "   Entrenador:               " << resultados[e].entrenador << "\n";
                    std::cout << "   Ciudad:                   " << resultados[e].ciudad << "\n";
                    std::cout << "   Cantidad de Partidos:     " << resultados[e].cantidadPartidos << "\n";
                    std::cout << "   Fecha de Registro:        " << resultados[e].fechaRegistro << "\n";
                }
                std::cout << "---------------------------------------------------------------------------\n";
            }

            auxiliares::pausarPrograma();
        }

        void listarEquipos(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            // Inicializamos las variables a utilizar
            int cantEquipos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Equipo listaDeEquipos[maxResultados];

            // llamamos a la funcion que nos devuelve la lista de punteros
            cantEquipos = Logica::listarRegistros<Equipo>(nombreArchivo, listaDeEquipos, maxResultados);

            // Si no se consiguieron equipos
            if (cantEquipos == 0) {
                std::cout << "No hay equipos registrados en el sistema actualmente.\n";
            } else {

                // Listamos todos los equipos
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            LISTA DE EQUIPOS               ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "----------------------------------------------------------------------------------------- \n";
                std::cout << " " << std::left << std::setw(4) << "N°"
                          << " | " << std::setw(6) << "ID"
                          << " | " << std::setw(35) << "Nombre"
                          << " | " << std::setw(30) << "Ciudad"
                          << " | " << "Puntos" << "\n";

                std::cout << "----------------------------------------------------------------------------------------- \n";

                for (size_t e = 0; e < cantEquipos; e++) {
                    // Filas de datos con exactamente los mismos anchos modificados
                    std::cout << " " << std::left << std::setw(4) << (e + 1) << " | " << std::setw(6) << listaDeEquipos[e].ID << " | " << std::setw(35) << listaDeEquipos[e].nombre
                              << " | " << std::setw(30) << listaDeEquipos[e].ciudad << " | " << listaDeEquipos[e].puntos << "\n";
                }

                std::cout << "----------------------------------------------------------------------------------------- \n";
            }

            auxiliares::pausarPrograma();
        }

        void mostrarTablaDePosiciones(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1200);

            // Inicializamos las variables a utilizar
            int cantEquipos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Equipo tablaDePosiciones[maxResultados];

            // Buscamos y leemos el nombre del torneo
            Torneo torneo;
            std::ifstream archivoTorneo;
            archivoTorneo.open(nombreArchivo, std::ios::binary);

            // si se produjo un error
            if (!archivoTorneo.is_open()) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Movemos el puntero de letura al principio por seguridad
            archivoTorneo.seekg(0, std::ios::beg);

            // Realizamos la lectura
            archivoTorneo.read(reinterpret_cast<char *>(&torneo), sizeof(Torneo));

            // Si la lectura tuvo problemas
            if (archivoTorneo.fail()) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Cerramos el archivo de torneos
            archivoTorneo.close();

            // Armamos la tabla de posiciones
            cantEquipos = Logica::equipos::tablaDePosiciones(nombreArchivo, tablaDePosiciones, maxResultados);

            auxiliares::waitfor(7500);

            // CArgando la tabla de posiciones
            std::cout << "\nCargando la tabla de posiciones...\n";

            auxiliares::waitfor(1200);

            // Si no hay resultados no mostramos nada, sino mostramos la tabla con los equipos
            if (cantEquipos == 0) {
                std::cout << "\nNo hay Equipos Disponibles\n";
            } else {

                // Mostramos la tabla de posiciones
                char nombreTorneo[TAMANO_NOMBRE];
                auxiliares::toMayus(torneo.nombre);
                std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║                             TABLA DE POSICIONES                                     ║\n";
                std::cout << "║               " << std::left << std::setw(70) << torneo.nombre << "║\n";
                std::cout << "╠════╦═══════════════════════════════════════════════╦═════╦═══╦═══╦═══╦════╦════╦════╣\n";
                std::cout << "║ #  ║ Equipo                                        ║ PTS ║ J ║ G ║ E ║ D  ║ GF ║ GC ║\n";
                std::cout << "╠════╬═══════════════════════════════════════════════╬═════╬═══╬═══╬═══╬════╬════╬════╣\n";

                for (size_t e = 0; e < cantEquipos; e++) {
                    std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(45) << tablaDePosiciones[e].nombre << " ║ " << std::right
                              << std::setw(3) << tablaDePosiciones[e].puntos << " ║ " << std::right << std::setw(1) << tablaDePosiciones[e].jugados << " ║ " << std::right
                              << std::setw(1) << tablaDePosiciones[e].victorias << " ║ " << std::right << std::setw(1) << tablaDePosiciones[e].empates << " ║ " << std::right
                              << std::setw(2) << tablaDePosiciones[e].derrotas << " ║ " << std::right << std::setw(2) << tablaDePosiciones[e].anotacionAFavor << " ║ " << std::right
                              << std::setw(2) << tablaDePosiciones[e].anotacionEnContra << " ║\n";
                }
                std::cout << "╚════╩═══════════════════════════════════════════════╩═════╩═══╩═══╩═══╩════╩════╩════╝\n";
                std::cout << "\nReferencia: PTS=Puntos  J=Jugados  G=Ganados  E=Empatados\n";
                std::cout << "            D=Derrotas  GF=puntos a Favor  GC=puntos en Contra\n\n";
            }

            auxiliares::pausarPrograma();
        }

        void eliminatorias() {
            // En proceso..
        }

        void actualizarEquipo() {
            auxiliares::limpiarPantalla();
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
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            // Pedimos el ID del equipo que desean actualizar
            auxiliares::ingresarDatos(ID, "Ingresa el ID del equipo que desea actualizar: ", nullptr, validadores::IDvalido);

            // si no Existe el ID
            if (!Logica::equipos::existeID(MiSistema, ID)) {
                std::cout << "Error el ID '" << ID << "' no pertenece a ningún equipo registrado\n";
                auxiliares::pausarPrograma();
                return;
            }

            do {
                flagError = false;
                auxiliares::waitfor(1500);
                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                auxiliares::ingresarCadena(nombreAux, 100, "Ingrese el nuevo nombre del Equipo: ", nullptr, validadores::Nombres);
                if (Logica::equipos::cadenaDuplicadaParaActualizar(MiSistema, nombreAux, ID)) {
                    std::cout << "Error: ya hay otro equipo con el nombre '" << nombreAux << "'\n";
                    flagError = true;
                }
                auxiliares::waitfor(1500);
                auxiliares::limpiarPantalla();
            } while (flagError);

            do {
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                auxiliares::ingresarCadena(entrenadorAux, 100, "Ingrese el nuevo nombre del Entrenador del Equipo: ", nullptr, validadores::Nombres);
                if (Logica::equipos::nombreEntrenadorDuplicadoParaActualizar(MiSistema, entrenadorAux, ID)) {
                    std::cout << "Ya el entrenador '" << entrenadorAux << "' dirige otro equipo\n";
                    flagError = true;
                }
                auxiliares::waitfor(1500);
                auxiliares::limpiarPantalla();
            } while (flagError);

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            auxiliares::ingresarCadena(ciudadAux, 100, "Ingrese el nuevo de la Ciudad del Equipo: ", nullptr, validadores::Nombres);
            auxiliares::waitfor(1500);
            auxiliares::limpiarPantalla();

            auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea aplicar estos cambios? (S/N): ");
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
            auxiliares::pausarPrograma();
        }

        void eliminarEquipo(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            int error = -1;
            bool eliminado = false;
            bool encontrado = false;
            bool cancelado = false;
            int ID = 0;
            char confirmacion;
            Equipo equipoAux;
            ArchivoHeader headerEquipos;

            // Leemos el header del archivo de equipo para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = Logica::leerHeader(nombreArchivo);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Eliminación Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             ELIMINAR EQUIPOS              ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Pedimos el ID del equipo que desean actualizar
            if (!auxiliares::ingresarDatos(ID, "Ingresa el ID del equipo que desea eliminar (ingresa 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "\nRegistro cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // si no Existe el ID
            if (!Logica::existeID<Equipo>(nombreArchivo, ID)) {
                std::cerr << "Error el ID '" << ID << "' no pertenece a ningún equipo registrado\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(1500);
            auxiliares::limpiarPantalla();

            // buscamos el equipo mediante el ID ingresado
            encontrado = Logica::buscarRegistrosPorId<Equipo>(nombreArchivo, equipoAux, ID);

            // si no se encontró ningun equipo
            if (!encontrado) {
                std::cerr << "Error, no se encontró el equipo que se desea eliminar";
                auxiliares::pausarPrograma();
                return;
            }

            // Si tiene partidos jugados
            if (equipoAux.cantidadPartidos > 0) {
                std::cout << " ADVERTENCIA: El equipo tiene " << equipoAux.cantidadPartidos << " partidos asociados.\n";
                std::cout << " No puede ser eliminado hasta que cancele los partidos del equipo\n";
                auxiliares::pausarPrograma();
                return;
            }

            // ? ofrecer opcion de cancelar todos los partidos

            // Si tiene jugadores
            if (equipoAux.numJugadores > 0) {
                std::cout << " ADVERTENCIA: El equipo tiene " << equipoAux.numJugadores << " jugadores.\n";
                std::cout << " No puede ser eliminado\n";
                auxiliares::pausarPrograma();
                return;
            }

            // ? ofrecer opcion de eliminar todos los jugadores?

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             ELIMINAR EQUIPOS              ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << "Equipo Seleccionado: \n\n";
            std::cout << "Nombre: " << equipoAux.nombre << std::endl;
            std::cout << "Entrenador: " << equipoAux.entrenador << std::endl;
            std::cout << "Ciudad: " << equipoAux.ciudad << std::endl;

            auxiliares::ingresarDatos(confirmacion, "¿Desea eliminar el equipo? (S/N): ");
            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                eliminado = Logica::equipos::eliminarEquipo(nombreArchivo, ID);
                if (!eliminado) {
                    std::cout << "Se produjo un error a la hora de eliminar el equipo\n";
                    std::cout << "Por favor revise que el equipo a eliminar no tenga partidos asociados ni jugadores registrados\n";
                } else {
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << "           Equipo Eliminado con Éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "Eliminacion de Equipo Cancelada\n";
            } else {
                std::cerr << "Error: No se ingresó una opción correcta (S/N)\n";
                std::cout << "Eliminacion de Equipo Cancelada\n";
            }
            auxiliares::pausarPrograma();
        }

    } // namespace equipos

    namespace jugadores {

        void registrarJugador(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int error = -1;
            Jugador nuevo;
            bool flagError = false;
            char confirmacion;
            bool cancelado = false;
            int opcion = 0;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Registro Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Leemos el torneo
            Torneo torneo;
            std::ifstream archivoTorneo;
            archivoTorneo.open(NOMBRE_ARCHIVO_TORNEO, std::ios::binary);

            // Verificamos que abrió correctamente
            if (!archivoTorneo.is_open()) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Registro Cancelado\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Ubicamos el puntero de lectura al inicio
            archivoTorneo.seekg(0, std::ios::beg);

            // Leemos el el torneo
            archivoTorneo.read(reinterpret_cast<char *>(&torneo), sizeof(Torneo));

            // Verificamos si la lectura fue exitosa
            if (archivoTorneo.fail()) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Registro Cancelado\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Cerrmaos el archivo
            archivoTorneo.close();

            // Recolectamos el ID del equipo
            do {
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarDatos(nuevo.idEquipo, "Ingrese el ID del equipo al que pertenece el jugador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                /// Si el ID no existe dentro de los equipos
                if (!Logica::existeID<Equipo>(nombreArchivo, nuevo.idEquipo)) {
                    std::cout << "Error el ID '" << nuevo.idEquipo << "' no pertenece a ningun equipo\n";
                    flagError = true;
                    auxiliares::waitfor(2500);
                    continue;
                }
                auxiliares::waitfor(1500);
            } while (flagError);

            // Recolectamos el nombre del Jugador
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarCadena(nuevo.nombre, TAMANO_NOMBRE, "Ingrese el nombre del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                validadores::Nombres)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Validamos nombre duplicado
                if (Logica::cadenaDuplicada<Jugador>(nombreArchivo, nuevo.nombre, &Jugador::nombre)) {
                    std::cout << "Error, el nombre '" << nuevo.nombre << "' ya está en uso.\n";
                    flagError = true;
                    auxiliares::waitfor(3000);
                    continue;
                }
                auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la Edad
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarDatos(nuevo.edad, "Ingrese la edad del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::Edad)) {
                std::cout << "\nRegistro cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(1500);

            // Recolectamos la cedula
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarCadena(nuevo.cedula, TAMANO_CEDULA, "Ingrese la cedula del jugador (ingrese 'cancelar' para cancelar): ", nullptr, validadores::Cedulas)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Validamos nombre duplicado
                if (Logica::cadenaDuplicada(nombreArchivo, nuevo.cedula, &Jugador::cedula)) {
                    std::cout << " Error, la cedula '" << nuevo.cedula << "' ya le pertenece a otro jugador\n";
                    flagError = true;
                    auxiliares::waitfor(3000);
                    continue;
                }
                auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la Posicion
            do {
                flagError = false;
                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte actual del Torneo: " << torneo.deporte << "\n\n";

                const char **matrizDeporteActual = nullptr;

                for (size_t i = 0; i < validadores::totalDeportes; i++) {
                    // Recorremos el array de matrices y verificamos lo que hay en la posicion 0
                    if (std::strcmp(validadores::MapaDeportes[i][0], torneo.deporte) == 0) {
                        matrizDeporteActual = validadores::MapaDeportes[i];
                        break;
                    }
                }

                // Mostramos las posiciones disponibles de esa fila
                std::cout << "\n Seleccione la posición del jugador:\n";

                // Para saber el numero de posiciones del deporte
                int contadorPosiciones = 0;

                for (size_t j = 1; matrizDeporteActual[j] != nullptr; j++) {
                    std::cout << " " << j << ". " << matrizDeporteActual[j] << "\n";
                    contadorPosiciones++;
                }
                std::cout << "\n";

                if (!auxiliares::ingresarDatos(opcion, "Seleccione una opción (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos que esté en el rango de opciones
                if (opcion >= 1 && opcion <= contadorPosiciones) {
                    // si es correcta guardamos la posicion
                    std::strncpy(nuevo.posicion, matrizDeporteActual[opcion], TAMANO_POSICION);
                } else {
                    std::cerr << "Error: Opción inválida. Por favor, intente de nuevo.\n";
                    flagError = true;
                    auxiliares::waitfor(2000);
                }
            } while (flagError);


            // Recolectamos la fecha de registro del jugador
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(nuevo.fechaRegistro, 11, "Ingrese la fecha de Registro del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                            validadores::fechaValidaRegistroDeJugadorOEquipo)) {
                std::cout << "\nRegistro cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(2000);
            auxiliares::limpiarPantalla();

            // Recolectamos el dorsal del Jugador
            do {
                flagError = false;
                Equipo equipoBuscado;
                bool existe = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, nuevo.idEquipo);
                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                if (!auxiliares::ingresarDatos(nuevo.numeroDorsal, "Ingrese el Dorsal del jugador (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::Dorsal)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                if (Logica::jugadores::DorsalDuplicado(nombreArchivo, nuevo.numeroDorsal, nuevo.idEquipo)) {
                    std::cout << "Error el dorsal '" << nuevo.numeroDorsal << "' ya está ocupado en el equipo '" << equipoBuscado.nombre << "'.\n";
                    flagError = true;
                }
                auxiliares::waitfor(2000);
                auxiliares::limpiarPantalla();
            } while (flagError);

            // Pedimos la confirmacion al usuario
            auxiliares::limpiarPantalla();
            auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea registrar este jugador? (S/N): ");

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                // agregamos el jugador
                bool registrado = Logica::jugadores::registrarJugador(nombreArchivo, nuevo);
                auxiliares::waitfor(1200);
                auxiliares::limpiarPantalla();
                // Si no se agregó
                if (!registrado) {
                    std::cerr << "Error al registrar al jugador.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║       JUGADOR REGISTRADO CON ÉXITO        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Torneo: " << torneo.nombre;
                std::cout << "\n ID del Jugador: " << nuevo.ID;
                std::cout << "\n Nombre del Jugador: " << nuevo.nombre;
                std::cout << "\n Cédula: " << nuevo.cedula;
                std::cout << "\n Edad: " << nuevo.edad << " años";
                std::cout << "\n Posición: " << nuevo.posicion;
                std::cout << "\n Dorsal: " << nuevo.numeroDorsal;
                std::cout << "\n ID del Equipo asignado: " << nuevo.idEquipo;
                std::cout << "\n Fecha de Registro: " << nuevo.fechaRegistro;
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << " Registro de Jugador Cancelado.\n";
            } else {
                std::cerr << " ERROR: Opción incorrecta (S/N).\nRegistro de Jugador Cancelado.\n";
            }
            auxiliares::pausarPrograma();
        }

        void buscarJugadorID(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int ID = 0;
            int error = -1;
            bool encontrado = false;
            bool cancelado = false;
            Jugador jugadorBuscado;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = Logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Leemos el torneo
            Torneo torneo;
            std::ifstream archivoTorneo;
            archivoTorneo.open(NOMBRE_ARCHIVO_TORNEO, std::ios::binary);

            // Verificamos que abrió correctamente
            if (!archivoTorneo.is_open()) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Registro Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Ubicamos el puntero de lectura al inicio
            archivoTorneo.seekg(0, std::ios::beg);

            // Leemos el el torneo
            archivoTorneo.read(reinterpret_cast<char *>(&torneo), sizeof(Torneo));

            // Verificamos si la lectura fue exitosa
            if (archivoTorneo.fail()) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Registro Cancelado\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Cerramos el archivo
            archivoTorneo.close();

            // Si no hay jugadores registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║       BUSQUEDA DE JUGADORES POR ID        ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            if (!auxiliares::ingresarDatos(ID, "Ingrese el ID (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "\nRegistro Cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            encontrado = Logica::buscarRegistrosPorId<Jugador>(nombreArchivo, jugadorBuscado, ID);

            auxiliares::waitfor(800);
            auxiliares::limpiarPantalla();
            std::cout << "\nBuscando...\n";
            auxiliares::waitfor(1500);


            // si no encontro un jugador
            if (!encontrado) {
                std::cerr << "Error no hay ningun jugador registrado con el ID '" << ID << "'\n";
            } else {


                // Buscamos el equipo del jugador
                Equipo equipoBuscado;
                encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, jugadorBuscado.idEquipo);

                // Verificamos que el equipo fue enoncontrado
                if (!encontrado) {
                    std::cout << "\nError del Sistema!\n";
                    std::cout << "Busqueda Cancelada\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Listamos los datos
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            JUGADOR ENCONTRADO             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Torneo:                 " << auxiliares::toMayus(torneo.nombre) << "\n";
                std::cout << "  Deporte:                " << torneo.deporte << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Informacion del Jugador: \n";
                std::cout << "    ID del Jugador:       " << jugadorBuscado.ID << "\n";
                std::cout << "    Nombre:               " << jugadorBuscado.nombre << "\n";
                std::cout << "    Cédula:               " << jugadorBuscado.cedula << "\n";
                std::cout << "    Edad:                 " << jugadorBuscado.edad << " años \n";
                std::cout << "    Posición:             " << jugadorBuscado.posicion << "\n";
                std::cout << "    Dorsal:               " << jugadorBuscado.numeroDorsal << "\n";
                std::cout << "    Fecha de Registro:    " << jugadorBuscado.fechaRegistro << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Informacion del Equipo: \n";
                std::cout << "    ID Equipo:            " << jugadorBuscado.idEquipo << "\n";
                std::cout << "    Equipo:               " << equipoBuscado.nombre << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Estadísticas en el Torneo:\n";
                std::cout << "    Anotaciones:          " << jugadorBuscado.anotaciones << "\n";
                std::cout << "    Tarjetas Amarillas:   " << jugadorBuscado.tarjetaAmarillas << "\n";
                std::cout << "    Tarjetas Rojas:       " << jugadorBuscado.tarjetasRojas << "\n";
                std::cout << "-------------------------------------------------------------\n";
            }
            auxiliares::pausarPrograma();
        }

        void buscarJugadorPorNombre(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            char subcadena[TAMANO_NOMBRE];
            int cantJugadoresEncontrados = 0;
            int error = -1;
            bool cancelado = false;
            bool encontrado = false;
            const int maxResultados = MAX_RESULTADOS;
            Jugador resultados[maxResultados];

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = Logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║      BÚSQUEDA DE JUGADORES POR NOMBRE     ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            if (!auxiliares::ingresarCadena(subcadena, 100, "Escribe el nombre (o parte del nombre) del jugador que buscas (escribe 'cancelar' para cancelar): ", &cancelado,
                                            validadores::Nombres)) {
                std::cout << "\nRegistro Cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1200);
            std::cout << "Buscando..." << std::endl;
            auxiliares::limpiarPantalla();
            auxiliares::waitfor(500);

            // llamamos a la funcion buscar por nombre y almacenamos el resultado
            cantJugadoresEncontrados = Logica::buscarRegistrosPorSucadena<Jugador>(nombreArchivo, resultados, subcadena, maxResultados);

            // Si no encontró nada
            if (cantJugadoresEncontrados <= 0) {
                std::cout << "\nNo se encontraron jugadores que coincidan con '" << subcadena << "'.\n";
            } else {

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          COINCIDENCIAS ENCONTRADAS        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Se encontraron " << cantJugadoresEncontrados << " jugador(es):\n";

                for (int e = 0; e < cantJugadoresEncontrados; e++) {

                    // Buscamos el nombre del Equipo
                    Equipo equipoBuscado;
                    encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, resultados[e].idEquipo);

                    // Si no se encontró el equipo
                    if (!encontrado) {
                        std::cerr << "Error del Sistema!";
                        std::cout << "Busqueda Cancelada";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << "-------------------------------------------------------------\n";
                    std::cout << "  ID:                    " << resultados[e].ID << "\n";
                    std::cout << "  Nombre:                " << resultados[e].nombre << "\n";
                    std::cout << "  Cédula:                " << resultados[e].cedula << "\n";
                    std::cout << "  Dorsal:                [" << resultados[e].numeroDorsal << "]\n";
                    std::cout << "  Edad:                  " << resultados[e].edad << " años \n";
                    std::cout << "  Posición:              " << resultados[e].posicion << "\n";
                    std::cout << "  Nombre del Equipo:     " << equipoBuscado.nombre << "\n";
                    std::cout << "  ID Equipo:             " << resultados[e].idEquipo << "\n";
                    std::cout << "  Anotaciones:           " << resultados[e].anotaciones << "\n";
                    std::cout << "  Tarjetas Amarillas:    " << resultados[e].tarjetaAmarillas << "\n";
                    std::cout << "  Tarjetas Rojas:    " << resultados[e].tarjetasRojas << "\n";
                }
                std::cout << "-------------------------------------------------------------\n";
            }

            std::cout << "\n";
            auxiliares::pausarPrograma();
        }

        void mostrarJugadoresPorEquipo(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int idEquipo = 0;
            int cantJugadoresEncontrados = 0;
            const int maxResultados = MAX_RESULTADOS;
            Jugador listaDeJugadores[maxResultados];
            int error = -1;
            bool cancelado = false;
            bool encontrado = false;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = Logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Registro Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }


            // Pedimos el ID del equipo a consultar
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║      MOSTRAR JUGADORES POR EQUIPO         ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            if (!auxiliares::ingresarDatos(idEquipo, "Ingrese el ID del Equipo: ", &cancelado, validadores::IDvalido)) {
                std::cout << "\nOperación Cancelada por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo primero
            Equipo equipoBuscado;
            encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, idEquipo);

            // Si no encontramos un equipo con ese ID enviamos error
            if (!encontrado) {
                std::cerr << "\nError: El equipo con ID '" << idEquipo << "' no existe.\n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1000);
            std::cout << "\nBuscando jugadores...\n\n";

            // obtenemos la lista de punteros
            cantJugadoresEncontrados = Logica::jugadores::listarJugadoresPorEquipo(nombreArchivo, idEquipo, listaDeJugadores, maxResultados);

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1000);

            // Si no obtenemos nada
            if (cantJugadoresEncontrados == 0) {
                std::cout << "El equipo '" << equipoBuscado.nombre << "' actualmente no tiene jugadores registrados.\n";
            } else {
                std::cout << "╔═════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║ EQUIPO: " << std::left << std::setw(71) << equipoBuscado.nombre << " ║\n";
                std::cout << "║ ID DEL EQUIPO: " << std::left << std::setw(64) << equipoBuscado.ID << " ║\n";
                std::cout << "╠════╦══════════════════════════════════════════╦═══════════════╦═════╦═══════════╣\n";
                std::cout << "║ ID ║ Nombre                                   ║ Posición      ║ Edad║ Dorsal    ║\n";
                std::cout << "╠════╬══════════════════════════════════════════╬═══════════════╬═════╬═══════════╣\n";

                // Imprimimos los jugadores
                for (size_t e = 0; e < cantJugadoresEncontrados; e++) {
                    std::cout << "║ " << std::right << std::setw(2) << listaDeJugadores[e].ID << " ║ " << std::left << std::setw(40) << listaDeJugadores[e].nombre << " ║ "
                              << std::left << std::setw(13) << listaDeJugadores[e].posicion << " ║ " << std::right << std::setw(3) << listaDeJugadores[e].edad << " ║ ["
                              << std::right << std::setw(2) << listaDeJugadores[e].numeroDorsal << "]      ║\n";
                }
                std::cout << "╚════╩══════════════════════════════════════════╩═══════════════╩═════╩═══════════╝\n";
                std::cout << " Total de jugadores en el equipo: " << cantJugadoresEncontrados << "\n";
            }

            std::cout << "\n";
            auxiliares::pausarPrograma();
        }

        void mostrarListaDeJugadores(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int cantJugadoresEncontrados = 0;
            int error = -1;
            const int maxResultados = MAX_RESULTADOS;
            Jugador listaDeJugadores[maxResultados];
            bool encontrado = false;

            // Leemos el header del archivo de Equipos para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operación Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Leemos el torneo
            Torneo torneo;
            std::ifstream archivoTorneo;
            archivoTorneo.open(NOMBRE_ARCHIVO_TORNEO, std::ios::binary);

            // Verificamos que abrió correctamente
            if (!archivoTorneo.is_open()) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operación Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Ubicamos el puntero de lectura al inicio
            archivoTorneo.seekg(0, std::ios::beg);

            // Leemos el el torneo
            archivoTorneo.read(reinterpret_cast<char *>(&torneo), sizeof(Torneo));

            // Verificamos si la lectura fue exitosa
            if (archivoTorneo.fail()) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operación Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Cerrmaos el archivo
            archivoTorneo.close();
            auxiliares::waitfor(1000);

            // Llamamos a tu función lógica
            cantJugadoresEncontrados = Logica::listarRegistros<Jugador>(nombreArchivo, listaDeJugadores, maxResultados);

            // Validamos si el sistema tiene jugadores cargados
            if (cantJugadoresEncontrados == 0) {
                std::cout << "No existen jugadores registrados en el sistema actualmente.\n";
            } else {

                std::cout << "Cargando todos los jugadores...\n\n";
                auxiliares::limpiarPantalla();
                auxiliares::waitfor(1000);

                std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║ SPORT G&C TOURNAMENTS                                                             ║\n";
                std::cout << "║ TORNEO: " << std::left << std::setw(73) << torneo.nombre << " ║\n";
                std::cout << "║ LISTA DE JUGADORES REGISTRADOS                                                    ║\n";
                std::cout << "╠════╦══════════════════════╦══════════════════════╦═══════════════╦═════╦══════════╣\n";
                std::cout << "║ ID ║ Nombre               ║ Equipo               ║ Posición      ║ Edad║ Dorsal   ║\n";
                std::cout << "╠════╬══════════════════════╬══════════════════════╬═══════════════╬═════╬══════════╣\n";

                // Imprimimos cada jugador en el sistema
                for (size_t e = 0; e < cantJugadoresEncontrados; e++) {

                    // Buscamos el equipo en cada iteracion
                    Equipo equipoBuscado;
                    encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, listaDeJugadores[e].idEquipo);

                    if (!encontrado) {
                        auxiliares::limpiarPantalla();
                        std::cout << "Error del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << "║ " << std::right << std::setw(2) << listaDeJugadores[e].ID << " ║ " << std::left << std::setw(20) << listaDeJugadores[e].nombre << " ║ "
                              << std::left << std::setw(20) << equipoBuscado.nombre << " ║ " << std::left << std::setw(13) << listaDeJugadores[e].posicion << " ║ " << std::right
                              << std::setw(3) << listaDeJugadores[e].edad << " ║ [" << std::right << std::setw(2) << listaDeJugadores[e].numeroDorsal << "]     ║\n";
                }
                std::cout << "╚════╩══════════════════════╩══════════════════════╩═══════════════╩═════╩══════════╝\n";
                std::cout << " Total de jugadores registrados en el sistema: " << cantJugadoresEncontrados << "\n";
            }

            std::cout << "\n";
            auxiliares::pausarPrograma();
        }

        void actualizarJugador() {
            auxiliares::limpiarPantalla();
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
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores registrados
            if (MiSistema->numJugadoresActuales == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Recolectamos el ID del jugador
            auxiliares::ingresarDatos(ID, "Ingresa el ID del jugador que desea actualizar: ", nullptr, validadores::IDvalido);

            // Si no existe el ID del jugador
            if (!Logica::jugadores::existeID(MiSistema, ID)) {
                std::cout << "Error: El ID '" << ID << "' no pertenece a ningún jugador registrado\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Recolectamos el nombre
            do {
                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                auxiliares::ingresarCadena(nombreAux, 100, "Ingrese el nuevo nombre del Jugador: ", nullptr, validadores::Nombres);

                if (Logica::jugadores::cadenaDuplicadaParaActualizar(MiSistema, nombreAux, ID)) {
                    std::cout << "Error: ya hay otro jugador con el nombre '" << nombreAux << "'\n";
                    flagError = true;
                    auxiliares::waitfor(2500);
                }

                auxiliares::waitfor(1000);
                auxiliares::limpiarPantalla();
            } while (flagError);

            // Recolectamos la Edad
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            auxiliares::ingresarDatos(edadAux, "Ingrese la nueva edad del Jugador: ", validadores::Edad);
            auxiliares::waitfor(1000);
            auxiliares::limpiarPantalla();

            // Recolectamos la posición
            do {
                int opcion = 0;
                flagError = false;
                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte actual del Torneo: " << MiSistema->torneo.deporte << "\n\n";

                const char **matrizDeporteActual = nullptr;

                for (size_t i = 0; i < validadores::totalDeportes; i++) {
                    // Recorremos el array de matrices y verificamos lo que hay en la posicion 0
                    if (std::strcmp(validadores::MapaDeportes[i][0], MiSistema->torneo.deporte) == 0) {
                        matrizDeporteActual = validadores::MapaDeportes[i];
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

                auxiliares::ingresarDatos(opcion, "Seleccione una opción: ");

                // Verificamos que esté en el rango de opciones
                if (opcion >= 1 && opcion <= contadorPosiciones) {
                    // si es correcta guardamos la posicion
                    std::strncpy(posicionAux, matrizDeporteActual[opcion]);
                } else {
                    std::cout << "Error: Opción inválida. Por favor, intente de nuevo.\n";
                    flagError = true;
                    auxiliares::waitfor(2000);
                }
            } while (flagError);

            // Recolectamos el dorsal
            do {
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                auxiliares::ingresarDatos(dorsalAux, "Ingrese el nuevo dorsal del Jugador: ", validadores::Dorsal);
                auxiliares::waitfor(1000);

                // Buscamos al jugador para obtener el id de su equipo
                Jugador *jugadorAux = Logica::jugadores::buscarJugadorPorID(MiSistema, ID);
                Equipo *eqAux = Logica::equipos::buscarEquipoPorID(MiSistema, jugadorAux->idEquipo);

                if (jugadorAux == nullptr || eqAux == nullptr) {
                    std::cout << " Error Inesperado del Sistema\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Si el dorsal ingresado esta duplicado y es distinto del dorsal actual del jugador
                if (Logica::jugadores::dorsalDuplicadoParaActualizar(MiSistema, dorsalAux, ID, jugadorAux->idEquipo)) {
                    std::cout << " Error: El dorsal '" << dorsalAux << "' ya está en uso en el equipo '" << eqAux->nombre << "'.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                }
                auxiliares::waitfor(2000);
                auxiliares::limpiarPantalla();
            } while (flagError);

            // Confirmación de los cambios
            auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea actualizar el jugador? (S/N): ");

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
            auxiliares::pausarPrograma();
        }

        void eliminarJugador(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int ID = 0;
            char confirmacion;
            int error = -1;
            bool cancelado = false;
            Jugador jugadorBuscado;
            bool encontrado = false;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = Logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operacion Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            ELIMINAR JUGADOR               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Recolectamos el ID
            if (!auxiliares::ingresarDatos(ID, "Ingresa el ID del jugador que deseas eliminar (ingresa 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "\nOperacion Cancelada por el usuario\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos al jugador para mostrar sus datos en pantalla antes de continuar
            encontrado = Logica::buscarRegistrosPorId<Jugador>(nombreArchivo, jugadorBuscado, ID);

            // Si no encontro el jugador con ese ID
            if (!encontrado) {
                std::cerr << "\nError: El ID '" << ID << "' no pertenece a ningún jugador registrado.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Verificamos que no tenga puntos anotados
            if (jugadorBuscado.anotaciones > 0) {
                std::cout << "\nError el jugador " << jugadorBuscado.nombre << " no puede tener anotaciones en el torneo\n";
            }

            // Buscamos el equipo al que pertenece
            Equipo equipoBuscado;
            encontrado = Logica::buscarRegistrosPorId<Equipo>(nombreArchivo, equipoBuscado, jugadorBuscado.idEquipo);

            // Si no ecnontro el equipo del jugador
            if (!encontrado) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Mostramos el jugador que se va a eliminar
            std::cout << "\n Se eliminará el siguiente jugador del sistema:\n";
            std::cout << " -----------------------------------------------\n";
            std::cout << " Nombre:     " << jugadorBuscado.nombre << "\n";
            std::cout << " Posición:   " << jugadorBuscado.posicion << "\n";
            std::cout << " Dorsal:     " << jugadorBuscado.numeroDorsal << "\n";
            std::cout << " Anotaciones:     " << jugadorBuscado.anotaciones << "\n";
            std::cout << " Equipo:     " << equipoBuscado.nombre << "\n";
            std::cout << " -----------------------------------------------\n\n";

            auxiliares::ingresarDatos(confirmacion, "¿Está seguro de eliminar este jugador? (S/N): ");
            auxiliares::waitfor(1500);
            auxiliares::limpiarPantalla();
            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

                // Llamamos a la logica
                bool eliminado = Logica::jugadores::eliminarJugador(nombreArchivo, ID);

                if (eliminado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "           Jugador eliminado con éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cerr << "\nError: No se pudo eliminar al jugador.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\nElimnación de datos cancelada\n";
            } else {
                std::cerr << "\nError: Opción inválida (S/N).\nEliminación de datos cancelada.\n";
            }
            auxiliares::pausarPrograma();
        }

    } // namespace jugadores

    namespace partidos {
        void programarPartido(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int error = -1;
            bool cancelado = false;
            bool encontrado = false;
            Equipo eqLocal, eqVisitante;

            // Leemos el header del archivo de equipo para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = Logica::leerHeader(nombreArchivo);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay mas equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "\nNo hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            if (headerEquipos.registrosActivos <= 1) {
                std::cout << "\nNo es posible programar un partido con solo un equipo\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Calculamos el minimo de jugadores en el deporte actual para organizar un partido
            int minimoDeJugadores = Logica::partidos::minJugadoresPorDeporte();
            Partido nuevoPartido;
            bool flagError = false;
            char confirmacion;
            bool programado = false;

            // Recolectamos el ID LOCAL
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarDatos(nuevoPartido.idEquipoLocal, "Ingrese el ID del equipo local (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\nOperacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Si el ID no corresponde a ningun equipo
                if (!Logica::existeID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, nuevoPartido.idEquipoLocal)) {
                    std::cout << "Error el ID '" << nuevoPartido.idEquipoLocal << "' no está asociado a ningún equipo\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                }

            } while (flagError);

            // Recolectamos el ID VISItante
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarDatos(nuevoPartido.idEquipoVisitante, "Ingrese el ID del equipo visitante (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\nOperacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Si el ID no corresponde a ningun equipo
                if (!Logica::existeID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, nuevoPartido.idEquipoVisitante)) {
                    std::cerr << "Error el ID '" << nuevoPartido.idEquipoVisitante << "' no está asociado a ningún equipo\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                }

                // Si el ID VISITANTE es el mismo que el ID del local
                if (nuevoPartido.idEquipoLocal == nuevoPartido.idEquipoVisitante) {
                    std::cerr << "Error no se puede programar un partido entre un mismo equipo\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                }
            } while (flagError);

            auxiliares::limpiarPantalla();

            // Buscamos el equipo local
            encontrado = Logica::buscarRegistrosPorId<Equipo>(nombreArchivo, eqLocal, nuevoPartido.idEquipoLocal);

            // Verificamos si fue encontrado
            if (!encontrado) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operación Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo visitante
            encontrado = Logica::buscarRegistrosPorId<Equipo>(nombreArchivo, eqVisitante, nuevoPartido.idEquipoVisitante);

            // Verificamos si fue encontrado
            if (!encontrado) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operación Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            /*
            // Si no cumplen con el minimo de jugadores
            if ((eqLocal.numJugadores < minimoDeJugadores) || (eqVisitante.numJugadores < minimoDeJugadores)) {
                std::cout << "Error no se puede programar un partido.\n";
                std::cout << "Los equipos no cumplen con el minimo de jugadores establecido\n\n";
                std::cout << "Deporte: " << MiSistema->torneo.deporte << std::endl;
                std::cout << "Minimo de Jugadores por Equipo: " << minimoDeJugadores << std::endl;
                std::cout << "Numero de Jugadores de '" << EqLocal->nombre << "': " << EqLocal->numJugadores << std::endl;
                std::cout << "Numero de Jugadores de '" << EqVisitante->nombre << "': " << EqVisitante->numJugadores << std::endl;
                auxiliares::pausarPrograma();
                return;
            }*/

            // Abrimos el archivo de Partidos
            Partido partidoAux;
            std::fstream archivoPartidos;
            archivoPartidos.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos si abrió el archivo
            if (archivoPartidos.is_open()) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operación Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Ubicamos el puntero de lectura despues del header
            archivoPartidos.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Si ya tienen un partido programado
            while (archivoPartidos.read(reinterpret_cast<char *>(&partidoAux), sizeof(Partido))) {

                // Verificamos que la lectura sea correcta
                if (archivoPartidos.fail()) {
                    std::cerr << "\nError del Sistema!\n";
                    std::cout << "Operación Cancelada\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                bool partidoEntreSi = (((partidoAux.idEquipoLocal == nuevoPartido.idEquipoLocal) && (partidoAux.idEquipoVisitante == nuevoPartido.idEquipoVisitante)) ||
                                       ((partidoAux.idEquipoLocal == nuevoPartido.idEquipoVisitante) && (partidoAux.idEquipoVisitante == nuevoPartido.idEquipoLocal)));
                if (partidoEntreSi && (std::strcmp(partidoAux.estado, Logica::partidos::estadoPartidos[0]) == 0)) {
                    std::cerr << "Error ya hay un partido programado entre el equipo " << eqLocal.nombre << "' y '" << eqVisitante.nombre << std::endl;
                    auxiliares::pausarPrograma();
                    return;
                }
            }

            // Cerramos el archivo de Partidos
            archivoPartidos.close();

            // Pedimos la fecha
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << "Encuentro: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";
            if (!auxiliares::ingresarCadena(nuevoPartido.fecha, TAMANO_FECHA, "Ingrese la fecha del partido (YYYY-MM-DD) (ingrese 'cancelar' para cancelar): ", &cancelado,
                                            validadores::fechaValidaRegistroDePartidos)) {
                std::cout << "\nOperacion Cancelada por el Usuario\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(750);
            auxiliares::limpiarPantalla();

            // Pedimos la descripcion
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(nuevoPartido.descripcion, 200, "Ingrese la descripción del partido (ingrese 'cancelar' para cancelar): ", &cancelado,
                                            validadores::nombreTorneo)) {
                std::cout << "\nOperacion Cancelada por el Usuario\n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::waitfor(750);
            auxiliares::limpiarPantalla();

            std::cout << "\n";
            auxiliares::ingresarDatos(confirmacion, "Confirme la programación del partido (S/N): ");
            auxiliares::waitfor(750);
            auxiliares::limpiarPantalla();

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                programado = Logica::partidos::programarPartido(nombreArchivo, nuevoPartido);

                if (programado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "                ¡Partido programado con éxito!\n";
                    std::cout << "                " << eqLocal.nombre << "  VS  " << eqVisitante.nombre << "\n";
                    std::cout << "                Fecha: " << nuevoPartido.fecha << std::endl;
                    std::cout << "                ID Asignado: " << nuevoPartido.ID << std::endl;
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cerr << "\nSe produjo un error a la hora de programar el partido.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\nLa programación del partido ha sido cancelada.\n";
            } else {
                std::cerr << "\nError: Opción inválida (S/N).\nLa programación del partido ha sido cancelada.";
            }

            auxiliares::pausarPrograma();
        }

        void registrarResultado(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int error = -1;

            // Buscamos y leemos el nombre del torneo
            Torneo torneo;
            std::ifstream archivoTorneo;
            archivoTorneo.open(nombreArchivo, std::ios::binary);

            // si se produjo un error
            if (!archivoTorneo.is_open()) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Movemos el puntero de letura al principio por seguridad
            archivoTorneo.seekg(0, std::ios::beg);

            // Realizamos la lectura
            archivoTorneo.read(reinterpret_cast<char *>(&torneo), sizeof(Torneo));

            // Si la lectura tuvo problemas
            if (archivoTorneo.fail()) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Cerramos el archivo de torneos
            archivoTorneo.close();

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = Logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << "No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Variables
            bool flagError = false;
            char confirmacion;
            bool cancelado = false;
            bool encontrado = false;
            Partido registroPartido, partidoAux;

            // Recopilamos el ID del partido a registrar
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                if (!auxiliares::ingresarDatos(registroPartido.ID, "Ingrese el ID del partido a registrar (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\nOperacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Buscamos el partido
                encontrado = Logica::buscarRegistrosPorId<Partido>(nombreArchivo, partidoAux, registroPartido.ID);
                if (!encontrado) {
                    std::cerr << "Error: El ID de partido '" << registroPartido.ID << "' no está asociado a ningún partido.\n";
                    auxiliares::pausarPrograma();
                    return;
                } else if (std::strcmp(partidoAux.estado, Logica::partidos::estadoPartidos[0]) != 0) {
                    std::cerr << "Error: El partido ya fue JUGADO o no se encuentra en estado PROGRAMADO.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                }

            } while (flagError);

            // Buscamos los equipos
            Equipo eqLocal, eqVisitante;

            // Buscamos el equipo local
            encontrado = Logica::buscarRegistrosPorId<Equipo>(nombreArchivo, eqLocal, partidoAux.idEquipoLocal);

            // Verificamos que fue encontrado
            if (!encontrado) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operacion Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo visitante
            encontrado = Logica::buscarRegistrosPorId<Equipo>(nombreArchivo, eqVisitante, partidoAux.idEquipoVisitante);

            // Verificamos que fue encontrado
            if (!encontrado) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operacion Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Recolectamos las anotaciones del partido
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           REGISTRAR ANOTACIONES           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << "Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo local
                if (!auxiliares::ingresarDatos(registroPartido.anotacionesLocal, "Número de Anotaciones del Equipo Local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << "Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();

                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << "Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo Visitante
                if (!auxiliares::ingresarDatos(registroPartido.anotacionesVisitante,
                                               "Número de Anotaciones del Equipo Visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << "Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos que sean positivos
                if (registroPartido.anotacionesLocal < 0 || registroPartido.anotacionesVisitante < 0) {
                    std::cerr << "Error: El número de anotaciones no puede ser un valor negativo.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                    // Validamos el empate
                } else if (registroPartido.anotacionesLocal == registroPartido.anotacionesVisitante) {

                    if (std::strcmp(validadores::deporteActual, "BALONCESTO") == 0 || std::strcmp(validadores::deporteActual, "TENIS") == 0 ||
                        std::strcmp(validadores::deporteActual, "VOLEIBOL") == 0 || std::strcmp(validadores::deporteActual, "BEISBOL") == 0 ||
                        std::strcmp(validadores::deporteActual, "SOFTBOL") == 0) {
                        std::cout << "Error: En el deporte " << validadores::deporteActual << " no se permiten empates. Registre el marcador final con prórroga.\n";
                        auxiliares::waitfor(3000);
                        flagError = true;
                        continue;
                    }
                }

                // Obtenemos el numero de anotaciones totales
                registroPartido.numAnotaciones = registroPartido.anotacionesLocal + registroPartido.anotacionesVisitante;

                // Verificamos que el numero de anotaciones no supere el máximo permitido
                if (registroPartido.numAnotaciones > MAX_ANOTACIONES) {
                    std::cout << "Error: El número de anotaciones no puede ser mayor al maximo permitido (" << MAX_ANOTACIONES << ").\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }

                // * Registramos el detalle de cada gol

                // Registramos las anotaciones del local
                for (size_t e = 0; e < registroPartido.anotacionesLocal; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Anotaciones del Equipo Local ---------- \n";
                    std::cout << " Anotacion " << e + 1 << "/" << registroPartido.anotacionesLocal;

                    // Pedimos el minuto en el que anotó el gol
                    if (!auxiliares::ingresarDatos(registroPartido.anotaciones[e].minuto, "Ingrese el minuto en el que se anotó (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                   validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    do {
                        flagError = false;
                        // Pedimos el ID del jugador que anotó el gol
                        if (!auxiliares::ingresarDatos(registroPartido.anotaciones[e].idJugador,
                                                       "Ingrese el ID del jugador que anotó (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        if (!Logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, registroPartido.anotaciones[e].idJugador)) {
                            std::cerr << "\nError el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);

                    std::strncpy(registroPartido.anotaciones[e].equipo, "LOCAL", TAMANO_LOCAL_O_VISITANTE);
                }

                // Registramos las anotaciones del visitante
                for (size_t e = registroPartido.anotacionesLocal; e < registroPartido.numAnotaciones; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Anotaciones del Equipo Visitante ---------- \n";
                    std::cout << " Anotacion " << (e - registroPartido.anotacionesLocal) + 1 << "/" << registroPartido.anotacionesVisitante;

                    // Pedimos el minuto en el que anotó el gol
                    if (!auxiliares::ingresarDatos(registroPartido.anotaciones[e].minuto, "Ingrese el minuto en el que se anotó (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                   validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    do {
                        flagError = false;
                        // Pedimos el ID del jugador que anotó el gol
                        if (!auxiliares::ingresarDatos(registroPartido.anotaciones[e].idJugador,
                                                       "Ingrese el ID del jugador que anotó (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        if (!Logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, registroPartido.anotaciones[e].idJugador)) {
                            std::cerr << "\nError el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);

                    std::strncpy(registroPartido.anotaciones[e].equipo, "VISITANTE", TAMANO_LOCAL_O_VISITANTE);
                }

            } while (flagError);

            // Recoletamos las tarjetas amarillas
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║             REGISTRAR TARJETAS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << "Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo local
                if (!auxiliares::ingresarDatos(registroPartido.tarjetasAmaLocal,
                                               "Número de Tarjetas Amarillas del Equipo Local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << "Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();

                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << "Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo Visitante
                if (!auxiliares::ingresarDatos(registroPartido.tarjetasAmaVisitante,
                                               "Número de Tarjetas Amarillas del Equipo Visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << "Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos que sean positivos
                if (registroPartido.tarjetasAmaLocal < 0 || registroPartido.tarjetasAmaVisitante < 0) {
                    std::cout << "Error: El número de tarjetas amarillas no puede ser un valor negativo.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                    // Validamos el empate
                }

                // Obtenemos el numero de tarjetas Amarillas totales
                registroPartido.numtarjetaAma = registroPartido.tarjetasAmaLocal + registroPartido.tarjetasAmaVisitante;

                // Verificamos que el numero de TARJETAS amarillas no supere el máximo permitido
                if (registroPartido.numtarjetaAma > MAX_TARJETAS_AMARILLAS) {
                    std::cout << "Error: El número de tarjetas Amarillas no puede ser mayor al maximo permitido (" << MAX_TARJETAS_AMARILLAS << ").\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }

                // * Registramos el detalle de cada tarjeta amarillas

                // Registramos las tarjetas amarillas del local
                for (size_t e = 0; e < registroPartido.tarjetasAmaLocal; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Tarjetas Amarillas del Equipo Local ---------- \n";
                    std::cout << " Tarjeta A " << e + 1 << "/" << registroPartido.tarjetasAmaLocal;

                    // Pedimos el minuto en el que anotó el gol
                    if (!auxiliares::ingresarDatos(registroPartido.tarjetaA[e].minuto,
                                                   "Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    do {
                        flagError = false;
                        // Pedimos el ID del jugador que le sacaron la tarjeta amarilla
                        if (!auxiliares::ingresarDatos(registroPartido.tarjetaA[e].idJugador,
                                                       "Ingrese el ID del jugador que tiene tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        if (!Logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, registroPartido.tarjetaA[e].idJugador)) {
                            std::cerr << "\nError el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);

                    std::strncpy(registroPartido.tarjetaA[e].equipo, "LOCAL", TAMANO_LOCAL_O_VISITANTE);
                }

                // Registramos las tarjetas amarillas del visitante
                for (size_t e = registroPartido.tarjetasAmaLocal; e < registroPartido.numtarjetaAma; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Tarjetas Amarillas del Equipo Visitante ---------- \n";
                    std::cout << " Tarjeta A " << (e - registroPartido.tarjetasAmaLocal) + 1 << "/" << registroPartido.tarjetasAmaVisitante;

                    // Pedimos el minuto en el que anotó el gol
                    if (!auxiliares::ingresarDatos(registroPartido.tarjetaA[e].minuto,
                                                   "Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    do {
                        flagError = false;
                        // Pedimos el ID del jugador que tiene tarjeta amarilla
                        if (!auxiliares::ingresarDatos(registroPartido.tarjetaA[e].idJugador,
                                                       "Ingrese el ID del jugador que tiene la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        if (!Logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, registroPartido.tarjetaA[e].idJugador)) {
                            std::cerr << "\nError el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);

                    std::strncpy(registroPartido.tarjetaA[e].equipo, "VISITANTE", TAMANO_LOCAL_O_VISITANTE);
                }

            } while (flagError);

            // Recolectamos las tarjetas rojas
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║             REGISTRAR TARJETAS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << "Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo local
                if (!auxiliares::ingresarDatos(registroPartido.tarjetasRojasLocal, "Número de Tarjetas Rojas del Equipo Local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << "Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();

                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << "Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo Visitante
                if (!auxiliares::ingresarDatos(registroPartido.tarjetasRojasVisitante,
                                               "Número de Tarjetas Rojas del Equipo Visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << "Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos que sean positivos
                if (registroPartido.tarjetasRojasLocal < 0 || registroPartido.tarjetasRojasVisitante < 0) {
                    std::cout << "Error: El número de tarjetas amarillas no puede ser un valor negativo.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }

                // Obtenemos el numero de tarjetas Rojas totales
                registroPartido.numTarjetasRojas = registroPartido.tarjetasRojasLocal + registroPartido.tarjetasRojasVisitante;

                // Verificamos que el numero de tarjetas Rojas no supere el máximo permitido
                if (registroPartido.numTarjetasRojas > MAX_TARJETAS_ROJAS) {
                    std::cout << "Error: El número de tarjetas Rojas no puede ser mayor al maximo permitido (" << MAX_TARJETAS_ROJAS << ").\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }

                // * Registramos el detalle de cada tarjeta roja

                // Registramos las tarjetas rojas del local
                for (size_t e = 0; e < registroPartido.tarjetasRojasLocal; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Tarjetas Rojas del Equipo Local ---------- \n";
                    std::cout << " Tarjeta R " << e + 1 << "/" << registroPartido.tarjetasRojasLocal;

                    // Pedimos el minuto en el que se produjo la tarjeta roja
                    if (!auxiliares::ingresarDatos(registroPartido.tarjetaR[e].minuto,
                                                   "Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    do {
                        flagError = false;
                        // Pedimos el ID del jugador que le sacaron la tarjeta roja
                        if (!auxiliares::ingresarDatos(registroPartido.tarjetaR[e].idJugador,
                                                       "Ingrese el ID del jugador que tiene tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        // Si no existe un jugador con ese ID
                        if (!Logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, registroPartido.tarjetaR[e].idJugador)) {
                            std::cerr << "\nError el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);

                    std::strncpy(registroPartido.tarjetaR[e].equipo, "LOCAL", TAMANO_LOCAL_O_VISITANTE);
                }

                // Registramos las tarjetas amarillas del visitante
                for (size_t e = registroPartido.tarjetasRojasLocal; e < registroPartido.numTarjetasRojas; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();

                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Tarjetas Amarillas del Equipo Visitante ---------- \n";
                    std::cout << " Tarjeta A " << (e - registroPartido.tarjetasRojasLocal) + 1 << "/" << registroPartido.tarjetasRojasVisitante;

                    // Pedimos el minuto el que le sacaron la tarjeta roja
                    if (!auxiliares::ingresarDatos(registroPartido.tarjetaR[e].minuto,
                                                   "Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    do {
                        flagError = false;
                        // Pedimos el ID del jugador que le sacaron la tarjeta Roja
                        if (!auxiliares::ingresarDatos(registroPartido.tarjetaR[e].idJugador,
                                                       "Ingrese el ID del jugador que tiene la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        if (!Logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, registroPartido.tarjetaR[e].idJugador)) {
                            std::cerr << "\nError el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);

                    std::strncpy(registroPartido.tarjetaR[e].equipo, "VISITANTE", TAMANO_LOCAL_O_VISITANTE);
                }

            } while (flagError);

            // Mostramos el marcador
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          RESUMEN DEL MARCADOR             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " " << eqLocal.nombre << " " << registroPartido.anotacionesLocal << "  -  " << registroPartido.anotacionesVisitante << " " << eqVisitante.nombre << "\n\n";

            auxiliares::ingresarDatos(confirmacion, "¿Está seguro de registrar este resultado definitivo? (S/N): ");
            auxiliares::limpiarPantalla();

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                // Invocamos tu función lógica corregida
                bool registrado = Logica::partidos::registrarResultado(nombreArchivo, registroPartido);

                if (registrado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "                  ¡Resultado registrado con éxito!\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << " Partido ID:  " << registroPartido.ID << "\n";
                    std::cout << " Estado:      " << registroPartido.estado << "\n";
                    std::cout << " Marcador:    " << eqLocal.nombre << " " << registroPartido.anotacionesLocal << "  -  " << registroPartido.anotacionesVisitante << " "
                              << eqVisitante.nombre << "\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cerr << "\nError: No se pudo registrar el partido.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\nRegistro de resultado cancelado.\n";
            } else {
                std::cerr << "\nError: Opción inválida (S/N).\nRegistro de resultado cancelado.\n";
            }
            auxiliares::pausarPrograma();
        }

        void buscarPartidoPorID(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            bool cancelado = false;
            bool encontrado = false;
            int error = -1;
            Partido partidoBuscado;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = Logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << "No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            int idPartido = 0;
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            BUSCAR PARTIDO POR ID          ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Recolectamos el ID
            if (!auxiliares::ingresarDatos(idPartido, "Ingrese el ID del partido que desea consultar (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "Operación Cancelada por el Usuario\n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(500);

            // Buscamos el partido mediante el ID
            encontrado = Logica::buscarRegistrosPorId<Partido>(nombreArchivo, partidoBuscado, idPartido);

            // Si no existe, avisamos y salimos
            if (!encontrado) {
                std::cerr << "\nError: El ID de partido '" << idPartido << "' no existe en el sistema.\n";
                auxiliares::pausarPrograma();
                return;
            }

            Equipo eqLocal, eqVisitante;

            // Buscamos los equipos para mostrar nombres reales en la presentacion

            // Buscamos el Local
            encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, partidoBuscado.idEquipoLocal);

            // Si no existe, avisamos y salimos
            if (!encontrado) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el visitante
            encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, partidoBuscado.idEquipoVisitante);

            // Si no existe, avisamos y salimos
            if (!encontrado) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "Buscando..." << std::endl;
            auxiliares::waitfor(1200);
            auxiliares::limpiarPantalla();

            std::cout << "\n╔══════════════════════════════════════════════════╗\n";
            std::cout << "║              DETALLE DE PARTIDO                  ║\n";
            std::cout << "╠══════════════════════════════════════════════════╣\n";
            std::cout << "║ " << std::left << std::setw(14) << "ID Partido" << ": " << std::setw(32) << partido->ID << "║\n";
            std::cout << "║ " << std::left << std::setw(14) << "Estado" << ": " << std::setw(32) << partido->estado << "║\n";
            std::cout << "║ " << std::left << std::setw(14) << "Fecha" << ": " << std::setw(32) << partido->fecha << "║\n";
            std::cout << "║                                                  ║\n";
            std::cout << "║  " << std::left << std::setw(20) << EqLocal->nombre << " " << partido->anotacionesLocal << " - " << partido->puntosVisitante << "  " << std::setw(20)
                      << EqVisitante->nombre << " ║\n";
            std::cout << "║      (Local)                  (Visitante)        ║\n";
            std::cout << "║                                                  ║\n";
            std::cout << "║ Notas: " << std::left << std::setw(41) << partido->descripcion << "║\n";
            std::cout << "║                                                  ║\n";
            std::cout << "╚══════════════════════════════════════════════════╝\n\n";

            auxiliares::pausarPrograma();
        }

        void listarTodosLosPartidos(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int error = -1;
            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = Logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << "No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            int cantPartidos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Partido listaDePartidos[maxResultados];

            // Obtenemos la lista de partidos
            cantPartidos = Logica::listarRegistros<Partido>(nombreArchivo, listaDePartidos, maxResultados);

            if (cantPartidos == 0) {
                std::cout << "No se encontró ningún partido\n";
                auxiliares::pausarPrograma();
                return;
            } else if (cantPartidos == -1) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            LISTADO DE PARTIDOS            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado"
                      << "Encuentro y Resultado\n";
            std::cout << "--------------------------------------------------------------------------------\n";

            // Recorremos para imprimir los mensajes
            for (size_t e = 0; e < cantPartidos; e++) {

                // Buscamos los equipos
                bool encontrado = false;

                Equipo eqLocal, eqVisitante;

                // Buscamos el equipo local
                encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, listaDePartidos[e].idEquipoLocal);

                // Verificamos que si lo encontró
                if (!encontrado) {
                    std::cerr << "\nError del Sistema!\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Buscamos el equipo visitante
                encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, listaDePartidos[e].idEquipoVisitante);

                // Verificamos que si lo encontró
                if (!encontrado) {
                    std::cerr << "\nError del Sistema!\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                std::cout << std::left << std::setw(6) << listaDePartidos[e].ID << std::left << std::setw(14) << listaDePartidos[e].fecha << std::left << std::setw(13)
                          << listaDePartidos[e].estado;
                std::cout << eqLocal.nombre << " " << listaDePartidos[e].anotacionesLocal << "  -  " << listaDePartidos[e].anotacionesVisitante << " " << eqVisitante.nombre
                          << "\n";
            }
            std::cout << "--------------------------------------------------------------------------------\n\n";


            auxiliares::pausarPrograma();
        }

        void buscarPartidosPorEquipo(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            int idEquipo = -1;
            bool cancelado = false;
            int cantPartidos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Partido listaDePartidos[maxResultados];
            int error = -1;
            bool flagError = false;
            bool encontrado = false;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = Logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << "No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            do {
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        HISTORIAL DE PARTIDOS POR EQUIPO   ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Pedimos al usuario el id
                if (!auxiliares::ingresarDatos(idEquipo, "Ingrese el ID del equipo a consultar (ingresa 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                    std::cout << "\nOperación Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos si ese id existe
                if (!Logica::existeID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, idEquipo)) {
                    std::cout << "El id " << idEquipo << " no le pertenece a ningún equipo\n";
                    flagError = true;
                    auxiliares::pausarPrograma();
                    continue;
                }

            } while (flagError);


            auxiliares::limpiarPantalla();
            auxiliares::waitfor(500);
            std::cout << "\nBuscando...\n";
            auxiliares::waitfor(1200);
            auxiliares::limpiarPantalla();

            // Validamos si el equipo existe para poder usarlo
            Equipo equipoBuscado;
            encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, idEquipo);

            if (!encontrado) {
                std::cout << "\n Error del sistema.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Inicializamos la cantidad de partidos en 0 y buscamos la lista de partidos
            cantPartidos = Logica::partidos::listarPartidosPorEquipo(nombreArchivo, idEquipo, listaDePartidos, maxResultados);

            if (cantPartidos == error) {
                std::cerr << "\n Error: Ocurrió un error a la hora de mostrar la lista de partidos por equipo.\n\n";
            } else if (cantPartidos == 0) {
                // Si la lógica creó el arreglo pero el equipo no tiene partidos
                std::cout << "\n El equipo '" << equipoBuscado.nombre << "' no tiene partidos registrados todavia.\n\n";
            } else {
                // Entra aquí si se encontraron partidos para el equipo
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PARTIDOS ENCONTRADOS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n";
                std::cout << " Historial para: " << equipoBuscado.nombre << "\n\n";
                std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado"
                          << "Encuentro y Resultado\n";
                std::cout << "--------------------------------------------------------------------------------\n";

                // Recorremos e imprimimos todos los partidos del equipo
                for (size_t e = 0; e < cantPartidos; e++) {

                    // Buscamos los equipos
                    Equipo eqLocal, eqVisitante;

                    // Buscamos el local
                    encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, listaDePartidos[e].idEquipoLocal);

                    // Verificamos que fue encontrado
                    if (!encontrado) {
                        std::cerr << "\nError del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    // buscamos el Visitante
                    encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, listaDePartidos[e].idEquipoVisitante);

                    // Verificamos que fue encontrado
                    if (!encontrado) {
                        std::cerr << "\nError del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << std::left << std::setw(6) << listaDePartidos[e].ID << std::left << std::setw(14) << listaDePartidos[e].fecha << std::left << std::setw(13)
                              << listaDePartidos[e].estado;
                    std::cout << eqLocal.nombre << " " << listaDePartidos[e].anotacionesLocal << "  -  " << listaDePartidos[e].anotacionesVisitante << " " << eqVisitante.nombre
                              << "\n";
                }
                std::cout << "--------------------------------------------------------------------------------\n\n";
            }

            auxiliares::pausarPrograma();
        }

        void listarPartidosPorEstado(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            int idEquipo = -1;
            bool cancelado = false;
            int cantPartidos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Partido listaDePartidos[maxResultados];
            int error = -1;
            bool flagError = false;
            bool encontrado = false;
            char estado[TAMANO_ESTADO];
            int opcion = -1;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = Logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = Logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma;
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << "No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║         BUSCAR PARTIDOS POR ESTADO        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                std::cout << "Estados de partido consultables: \n\n";
                std::cout << "---------------------------------------------\n";
                std::cout << " 0. PROGRAMADO\n 1. JUGADO\n 2. CANCELADO\n\n";
                std::cout << "---------------------------------------------\n";
                auxiliares::ingresarDatos(opcion, "Ingrese el tipo de estado de partido que desea consultar (ingrese 'canelar' para cancelar): ", &cancelado,
                                          validadores::Positivo);

                switch (opcion) {
                    case 0: // Configuramos en PROGRAMADO
                        std::strncpy(estado, Logica::partidos::estadoPartidos[0], TAMANO_ESTADO);
                        break;

                    case 1: // Configuramos en JUGADO
                        std::strncpy(estado, Logica::partidos::estadoPartidos[1], TAMANO_ESTADO);
                        break;

                    case 2: // Configuramos en CANCELADO
                        std::strncpy(estado, Logica::partidos::estadoPartidos[2], TAMANO_ESTADO);
                        break;

                    default:
                        std::cout << "Ingrese una opcion correcta\n";
                        flagError = true;
                        auxiliares::pausarPrograma();
                }
            } while (flagError);

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(500);
            std::cout << "Buscando..." << std::endl;
            auxiliares::waitfor(1200);
            auxiliares::limpiarPantalla();

            // Listamos todos los partidos
            cantPartidos = Logica::partidos::listarPartidosPorSuEstado(nombreArchivo, listaDePartidos, estado, maxResultados);

            if (cantPartidos == -1) {
                std::cerr << "\n Error del Sistema.\n";
            } else if (cantPartidos == 0) {
                std::cout << "\n No se encontro ningun partido en estado '" << estado << "' actualmente.\n\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PARTIDOS ENCONTRADOS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n";
                std::cout << " Partidos en estado: " << estado << "\n\n";

                std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado"
                          << "Encuentro y Resultado\n";
                std::cout << "--------------------------------------------------------------------------------\n";

                // Bucle de impresión de registros
                for (size_t e = 0; e < cantPartidos; e++) {
                    // Buscamos los equipos
                    Equipo eqLocal, eqVisitante;

                    // Buscamos el local
                    encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, listaDePartidos[e].idEquipoLocal);

                    // Verificamos que fue encontrado
                    if (!encontrado) {
                        std::cerr << "\nError del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    // buscamos el Visitante
                    encontrado = Logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, listaDePartidos[e].idEquipoVisitante);

                    // Verificamos que fue encontrado
                    if (!encontrado) {
                        std::cerr << "\nError del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << std::left << std::setw(6) << listaDePartidos[e].ID << std::left << std::setw(14) << listaDePartidos[e].fecha << std::left << std::setw(13)
                              << listaDePartidos[e].estado;
                    std::cout << eqLocal.nombre << " " << listaDePartidos[e].anotacionesLocal << "  -  " << listaDePartidos[e].anotacionesVisitante << " " << eqVisitante.nombre
                              << "\n";
                }
                std::cout << "--------------------------------------------------------------------------------\n\n";
            }

            // Liberamos
            if (listaDePartidos != nullptr) {
                delete[] listaDePartidos;
                listaDePartidos = nullptr;
            }

            auxiliares::pausarPrograma();
        }

        void cancelarPartido(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();


            int idPartido = -1;
            bool cancelado = false;
            char confirmacion;
            Partido partidoAuxiliar;
            Equipo eqLocal, eqVisitante;
            bool existe = false;
            bool flagError = false;


            do {
                flagError = false;
                auxiliares::limpiarPantalla();

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║             CANCELAR PARTIDOS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n";
                if (!auxiliares::ingresarDatos(idPartido, " Ingrese el ID del Partido que desea cancelar (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\n Operación cancelada por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::limpiarPantalla();
                auxiliares::waitfor(250);
                std::cout << "\n Procesando solicitud...";
                auxiliares::waitfor(1200);
                auxiliares::limpiarPantalla();


                // Buscamos el Partido para mostrar los datos de ese partido antes de eliminarlo
                existe = Logica::buscarRegistrosPorId<Partido>(nombreArchivo, partidoAuxiliar, idPartido);

                // Verificamos si existe
                if (!existe) {
                    std::cerr << "\n Error: El ID '" << idPartido << "' no pertenece a ningún partido registrado.\n";
                    flagError = true;
                    auxiliares::pausarPrograma();
                }

            } while (flagError);


            // Buscamos el equipo local
            existe = Logica::buscarRegistrosPorId(nombreArchivo, eqLocal, partidoAuxiliar.idEquipoLocal);

            // Verificamos si existe
            if (!existe) {
                std::cerr << "\n Error del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo visitante
            existe = Logica::buscarRegistrosPorId(nombreArchivo, eqLocal, partidoAuxiliar.idEquipoLocal);

            // Verificamos si existe
            if (!existe) {
                std::cerr << "\n Error del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             CANCELAR PARTIDOS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << "\n Se borrará el registro del partido: \n\n";
            std::cout << " Encuentro: " << eqLocal.nombre << "  " << partidoAuxiliar.anotacionesLocal << "   -   " << partidoAuxiliar.puntosVisitante << "  " << eqVisitante.nombre
                      << std::endl;
            std::cout << " Fecha: " << partidoAuxiliar.fecha << std::endl;
            std::cout << " ID: " << partidoAuxiliar.ID << std::endl;
            std::cout << " Estado del Partido: " << partidoAuxiliar.estado << "\n\n";

            auxiliares::ingresarDatos(confirmacion, " ¿Está seguro de eliminar el registro de este partido? (S/N): ");

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1000);


            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                // Llamamos a la logica
                bool partidoCancelado = Logica::partidos::cancelarPartido(nombreArchivo, idPartido);

                if (partidoCancelado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "           Partido Cancelado con Éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cerr << "\n Error: No se pudo cancelar el Partido debido a un Error del Sistema.\n";
                }

            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << " Operación Cancelada!\n";
            } else {
                std::cerr << " Error: No se ingresó una opción válida.\n Operación Cancelada!\n";
            }
            auxiliares::pausarPrograma();
        }

    } // namespace partidos

    namespace reportes {
        //
    }

    namespace mantenimiento {
        //
    }

    namespace menu {

        void datosInicialesTorneo() {

            // Leemos el torneo para ver si ya ha sido inicializado
            std::fstream archivoTorneo;
            archivoTorneo.open(NOMBRE_ARCHIVO_TORNEO, std::ios::binary | std::ios::in | std::ios::out);

            Torneo torneoAux; // Para lectura

            // Verificamos que abrió correctamente
            if (!archivoTorneo.is_open()) {
                std::cerr << "Error del sistema!\n";
                auxiliares::pausarPrograma();
                std::exit(1);
            }

            // Movemos el puntero de lectura al inicio
            archivoTorneo.seekg(0, std::ios::beg);

            // leemos el ell torneo
            archivoTorneo.read(reinterpret_cast<char *>(&torneoAux), sizeof(Torneo));

            // Verificamos que el archivo haya leido correctamente
            if (archivoTorneo.fail()) {
                std::cerr << "Error del Sistema!\n";
                std::exit(1);
            }

            // Si ya esta inicializado no hacemos nada
            if (torneoAux.inicializado) {
                std::cout << "Iniciando el torneo " << torneoAux.nombre << std::endl;
                auxiliares::waitfor(2500);
                return;
            }

            // variables auxiliares
            Torneo torneo;
            int opcionFormato = 0;
            bool opcionValida = false;

            // * Aqui se recopilan los datos iniciales del torneo

            // Ingresar Nombre
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            auxiliares::ingresarCadena(torneo.nombre, TAMANO_NOMBRE, "Nombre del Torneo: ", nullptr, validadores::nombreTorneo);
            auxiliares::waitfor(1500);

            // Ingresar Deporte
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Mostrar deportes disponibles
            std::cout << "Deportes disponibles:\n";
            for (size_t e = 0; e < validadores::totalDeportes; e++) {
                std::cout << " - " << validadores::Deportes[e] << std::endl;
            }
            std::cout << std::endl;

            // Validación externa
            char mensajeError[TAMANO_MENSAJE_ERROR];
            bool deporteValido = false;

            do {
                auxiliares::ingresarCadena(torneo.deporte, TAMANO_DEPORTE, "Deporte del Torneo: ", nullptr, validadores::nombreTorneo);

                // Convertimos a mayus
                auxiliares::toMayus(torneo.deporte);

                if (!validadores::existeDeporte(torneo.deporte, mensajeError)) {
                    std::cerr << "Error: " << mensajeError << std::endl;
                    auxiliares::waitfor(2000);
                    deporteValido = false;
                } else {
                    deporteValido = true;
                }
            } while (!deporteValido);

            // Definimos el deporte actual en validadores
            validadores::definirDeporteActual(torneo.deporte);
            auxiliares::waitfor(1500);

            // Ingresar Formato
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << "1. Formato de Grupos (Todos contra todos)\n";
            std::cout << "2. Formato de Eliminatoria Directa\n";
            std::cout << "--------------------------------------------------\n";

            do {
                opcionValida = true;
                auxiliares::ingresarDatos(opcionFormato, "Seleccione el formato (1 o 2): ");
                if (opcionFormato != 1 && opcionFormato != 2) {
                    std::cerr << "Opcion invalida. Intente de nuevo.\n";
                    opcionValida = false;
                }
            } while (!opcionValida);

            // desde la logica definimos el tipo de torneo en base a la opcion ingresada
            Logica::definirFormato(torneo, opcionFormato);
            auxiliares::waitfor(1500);

            // Ingresar Fecha de Inicio del torneo
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            auxiliares::ingresarCadena(torneo.fechaInicio, TAMANO_FECHA, "Fecha De Inicio del Torneo: ", nullptr, validadores::FechaValida);
            validadores::definirFechaInicio(torneo.fechaInicio);
            auxiliares::waitfor(1500);

            // Ingresar Fecha de Finalizacion de Torneo
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            auxiliares::ingresarCadena(torneo.fechaFin, TAMANO_FECHA, "Fecha de Finalización del Torneo: ", nullptr, validadores::ValidarFechaFin);
            validadores::definirFechaFin(torneo.fechaFin);
            auxiliares::waitfor(1500);
            auxiliares::limpiarPantalla();

            // Movemos el puntero de escritura al inicio
            archivoTorneo.seekp(0, std::ios::beg);

            // guardamos los datos en el fichero binario
            archivoTorneo.write(reinterpret_cast<const char *>(&torneo), sizeof(Torneo));

            // Verificamos que la escritura fue correcta
            if (archivoTorneo.fail()) {
                std::cerr << "Error del Sistema!\n";
                std::exit(1);
            }


            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║  NUEVO TORNEO CREADO CON ÉXITO            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << "Nombre: " << torneo.nombre << std::endl;
            std::cout << "Deporte: " << torneo.deporte << std::endl;
            std::cout << "Formato: " << torneo.formato << std::endl;
            std::cout << "Fecha de inicio del torneo: " << torneo.fechaInicio << std::endl;
            std::cout << "Fecha de Finalización del torneo: " << torneo.fechaFin;

            archivoTorneo.close();
            auxiliares::pausarPrograma();
        }

        void Principal() {
            auxiliares::limpiarPantalla();

            // Abrimos el archivo de torneos para acceder a los datos que necesitamos
            std::ifstream archivoTorneo;
            archivoTorneo.open(NOMBRE_ARCHIVO_TORNEO, std::ios::binary);

            // Verificamos que se abrio el archivo
            if (!archivoTorneo.is_open()) {
                std::cerr << "Error del Sistema!\n";
                std::exit(1);
            }

            // Movemos el puntero de lectura al principio
            archivoTorneo.seekg(0, std::ios::beg);

            Torneo torneo;
            archivoTorneo.read(reinterpret_cast<char *>(&torneo), sizeof(Torneo));

            // Verificamos si no hubo un fallo en la lectura
            if (archivoTorneo.fail()) {
                std::cerr << "Error del Sistema!\n";
                std::exit(1);
            }

            // Cerramos el archivo
            archivoTorneo.close();

            std::cout << "\n   ╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "   ║                    Sport G&C Tournaments                     ║\n";
            std::cout << "   ║  Torneo:   " << std::left << std::setw(50) << torneo.nombre << "║\n";
            std::cout << "   ║  Deporte: " << std::left << std::setw(18) << torneo.deporte << " | Formato: " << std::left << std::setw(21) << torneo.formato << "║\n";
            std::cout << "   ╠══════════════════════════════════════════════════════════════╣\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "1. Gestión de Equipos" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "2. Gestión de Jugadores" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "3. Gestión de Partidos" << "║\n";
            // std::cout << "   ║  " << std::left << std::setw(60) << "4. Tabla de Posiciones" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "4. Reportes" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "5. Mantenimiento" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "0. Salir" << "║\n";
            std::cout << "   ╚══════════════════════════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void GestionDeEquipos() {
            auxiliares::limpiarPantalla();
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
            auxiliares::limpiarPantalla();
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
            auxiliares::limpiarPantalla();
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
            auxiliares::limpiarPantalla();
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
            auxiliares::limpiarPantalla();
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
            auxiliares::limpiarPantalla();
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
            auxiliares::limpiarPantalla();
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
            std::cout << "╔═══════════════════════════════════════════╗\n";
            std::cout << "║              REPORTES                     ║\n";
            std::cout << "╠═══════════════════════════════════════════╣\n";
            std::cout << "║  1. Tabla de posiciones                   ║\n";
            std::cout << "║  2. Tabla de goleadores (Top 10)          ║\n";
            std::cout << "║  3. Ficha técnica de partido              ║\n";
            std::cout << "║  0. Volver                                ║\n";
            std::cout << "╚═══════════════════════════════════════════╝\n";
        }

        void Mantenimiento() {
            std::cout << "╔═══════════════════════════════════════════╗\n";
            std::cout << "║           MANTENIMIENTO                   ║\n";
            std::cout << "╠═══════════════════════════════════════════╣\n";
            std::cout << "║  1. Verificar integridad referencial      ║\n";
            std::cout << "║  2. Crear backup de datos                 ║\n";
            std::cout << "║  0. Volver                                ║\n";
            std::cout << "╚═══════════════════════════════════════════╝\n";
        }

    } // namespace menu

    void mensajeSalida() {
        auxiliares::limpiarPantalla();
        std::cout << "\n Saliendo... \n";
        auxiliares::waitfor(2500);
    }

    void mensajeMenuPrincipal() {
        auxiliares::limpiarPantalla();
        std::cout << "\n Volviendo al Menú Principal... \n";
        auxiliares::waitfor(2500);
    }

    void mensajeMenuAnterior() {
        auxiliares::limpiarPantalla();
        std::cout << "\n Volviendo al menu anterior... \n";
        auxiliares::waitfor(2500);
    }

    void mensajeDefault() {
        auxiliares::limpiarPantalla();
        std::cout << "\n Error: Has ingresado una opcion inválida. Intentalo nuevamente.\n";
        auxiliares::waitfor(2500);
    }
} // namespace presentacion

// ============================================//
//   7. MAIN                                   //
// ============================================//

int main() {

    // Llamamos a la función de configuración de Idioma al inicio
    auxiliares::configurarIdioma();

    // Creamos una carpeta de datos;
    Logica::crearCarpeta();

    // Inicializamos los ficheros
    Logica::inicializarSistemaArchivos();

    // Variables Estaticas
    int opcionMenu = -1;     // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionSubMenu = -1;  // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionMenuBusq = -1; // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionMenuListar = -1;
    char confirmacion;

    // Inicio del Programa
    presentacion::menu::datosInicialesTorneo();

    // Estructura del switch
    do {

        // inicializamos las variables para evitar conflictos
        opcionMenu = -1;
        opcionSubMenu = -1;
        opcionMenuListar = -1;

        // Presentamos el menu principal
        presentacion::menu::Principal();
        auxiliares::ingresarDatos(opcionMenu, "Seleccione una opcion: ", nullptr, validadores::Positivo);
        auxiliares::limpiarPantalla();
        auxiliares::waitfor(1500);
        switch (opcionMenu) {

            // Salida del Programa
            case 0:
                auxiliares::limpiarPantalla();
                auxiliares::waitfor(2500);
                auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea salir del programa? (S/N): ");
                if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                    presentacion::mensajeSalida();
                } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                    std::cout << "Salida del Programa Cancelada\n"; // Forzamos la vuelta del bucle
                    opcionMenu = -1;
                    auxiliares::waitfor(3000);
                    auxiliares::limpiarPantalla();
                } else {
                    std::cout << "Error: No se ingresó una opción correcta (S/N)\n";
                    std::cout << "Salida del Programa Cancelada\n";
                    auxiliares::waitfor(3000);
                    // Forzamos la vuelta del bucle
                    opcionMenu = -1;
                }
                break;

            // Gestión de Equipos
            case 1:
                auxiliares::waitfor(2000);
                auxiliares::limpiarPantalla();
                std::cout << "Ingresando al apartado de Gestión de Equipos..." << std::endl;
                auxiliares::waitfor(3000);

                do {
                    auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    opcionMenuBusq = -1;
                    presentacion::menu::GestionDeEquipos();
                    auxiliares::ingresarDatos(opcionSubMenu, "Seleccione una opcion: ", nullptr, validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0: // Volver al menu Principal
                            presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Registrar nuevos equipos
                            presentacion::equipos::RegistrarEquipos(NOMBRE_ARCHIVO_EQUIPOS);
                            break;

                        case 2: // Menu Buscar equipos
                            do {
                                opcionMenuBusq = -1;
                                presentacion::menu::menuBuscarEquipo();
                                auxiliares::ingresarDatos(opcionMenuBusq, "Seleccione una opcion: ", nullptr, validadores::Positivo);

                                switch (opcionMenuBusq) {
                                    case 0: // Volver al menu anterior
                                        presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Busqueda por ID
                                        presentacion::equipos::buscarEquipoPorID(NOMBRE_ARCHIVO_EQUIPOS);
                                        break;

                                    case 2: // Busqueda por nombre
                                        presentacion::equipos::buscarEquiposPorSubCadena(NOMBRE_ARCHIVO_EQUIPOS);
                                        break;

                                    default:
                                        presentacion::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);

                            break;

                        case 3: // Actualizar Equipos
                            presentacion::equipos::actualizarEquipo(NOMBRE_ARCHIVO_EQUIPOS);
                            break;

                        case 4: // Listar Equipos
                            presentacion::equipos::listarEquipos(NOMBRE_ARCHIVO_EQUIPOS);
                            break;

                        case 5: // Eliminar Equipos
                            presentacion::equipos::eliminarEquipo(NOMBRE_ARCHIVO_EQUIPOS);
                            break;

                        default:
                            presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);

                break;

            // Gestión de Jugadores
            case 2:
                auxiliares::waitfor(2000);
                auxiliares::limpiarPantalla();
                std::cout << "Ingresando al apartado de Gestión de Jugadores..." << std::endl;
                auxiliares::waitfor(3000);

                do {
                    auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::GestionDeJugadores();
                    auxiliares::ingresarDatos(opcionSubMenu, "Seleccione un opcion: ", nullptr, validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0: // Volver al menu Principal
                            presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Registrar nuevos jugadores
                            presentacion::jugadores::registrarJugador(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 2: // Menu Buscar jugadores
                            do {
                                opcionMenuBusq = -1;
                                presentacion::menu::menuBuscarJugador();
                                auxiliares::ingresarDatos(opcionMenuBusq, "Seleccione una opcion: ", nullptr, validadores::Positivo);

                                switch (opcionMenuBusq) {
                                    case 0: // Volver al menu anterior
                                        presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Busqueda por ID
                                        presentacion::jugadores::buscarJugadorID(NOMBRE_ARCHIVO_JUGADORES);
                                        break;

                                    case 2: // Busqueda por nombre
                                        presentacion::jugadores::buscarJugadorPorNombre(NOMBRE_ARCHIVO_JUGADORES);
                                        break;

                                    default:
                                        presentacion::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);
                            break;

                        case 3: // Actualizar Jugador
                            presentacion::jugadores::actualizarJugador(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 4: // Listar todos los jugadores
                            presentacion::jugadores::mostrarListaDeJugadores(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 5: // Listar Jugadores por Equipos
                            presentacion::jugadores::mostrarJugadoresPorEquipo(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 6: // Eliminar jugador
                            presentacion::jugadores::eliminarJugador(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        default:
                            presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);
                break;

            // Gestión de Partidos
            case 3:
                auxiliares::waitfor(2000);
                auxiliares::limpiarPantalla();
                std::cout << "Ingresando al apartado de Gestión de Partidos..." << std::endl;
                auxiliares::waitfor(3000);
                do {
                    auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::GestionDePartidos();
                    auxiliares::ingresarDatos(opcionSubMenu, "Seleccione un opcion: ", nullptr, validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0: // Volver al menú principal
                            presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Programar Partidos
                            presentacion::partidos::programarPartido(NOMBRE_ARCHIVO_PARTIDOS);
                            break;

                        case 2: // Registrar el Resultado de un partido
                            presentacion::partidos::registrarResultado(NOMBRE_ARCHIVO_PARTIDOS);
                            break;

                        case 3: // Buscar partidos
                            do {
                                opcionMenuBusq = -1;
                                presentacion::menu::buscarPartidos();
                                auxiliares::ingresarDatos(opcionMenuBusq, "Seleccione una opcion: ", nullptr, validadores::Positivo);

                                switch (opcionMenuBusq) {
                                    case 0: // Volver al menu anterior
                                        presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Busqueda por Equipo
                                        presentacion::partidos::buscarPartidosPorEquipo(NOMBRE_ARCHIVO_PARTIDOS);
                                        break;

                                    case 2: // Busqueda por ID
                                        presentacion::partidos::buscarPartidoPorID(NOMBRE_ARCHIVO_PARTIDOS);
                                        break;

                                    default:
                                        presentacion::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);
                            break;

                        case 4: // Listar Partidos
                            do {
                                opcionMenuListar = -1;
                                presentacion::menu::listarPartidos();
                                auxiliares::ingresarDatos(opcionMenuListar, "Seleccione una opcion: ", nullptr, validadores::Positivo);

                                switch (opcionMenuListar) {
                                    case 0: // Volver al menu anterior
                                        presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Listar todos los partidos
                                        presentacion::partidos::listarTodosLosPartidos(NOMBRE_ARCHIVO_PARTIDOS);
                                        break;

                                    case 2: // Listar partidos por estado
                                        presentacion::partidos::listarPartidosPorEstado(NOMBRE_ARCHIVO_PARTIDOS);
                                        break;

                                    default:
                                        presentacion::mensajeDefault();
                                }
                            } while (opcionMenuListar != 0);
                            break;

                        case 5: // Cancelar Partidos
                            presentacion::partidos::cancelarPartido(NOMBRE_ARCHIVO_PARTIDOS);
                            break;

                        default:
                            presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);

                break;

            // Tabla de Posiciones
            case 4:
                auxiliares::waitfor(2000);
                auxiliares::limpiarPantalla();
                std::cout << "Ingresando al apartado de Tabla de Posiciones..." << std::endl;
                auxiliares::waitfor(3000);
                auxiliares::limpiarPantalla();
                presentacion::equipos::mostrarTablaDePosiciones(NOMBRE_ARCHIVO_EQUIPOS);
                break;

            // Reportes
            case 5:

                break;

            // Mantenimiento
            case 6:

                break;

            // Si no se selecciona una opcion correcta enviamos un mensaje de aviso
            default:
                presentacion::mensajeDefault();
        }
        // El bucle se repite si el usuario no eligió la opcion de salir en el menu Principal
    } while (opcionMenu != 0);

    return 0;
}