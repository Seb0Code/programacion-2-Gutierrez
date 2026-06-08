// Sport G&C Tournaments (SISTEMA DE GESTION DE TORNEOS DEPORTIVOS)

// ============================================//
//   1. INCLUDES, DIRECTIVAS Y LOS USING       //
// ============================================//
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring> //para el uso de strcpy
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

using std::cin;
using std::cout;
using std::endl;

// ============================================//
//   2. STRUCTS                                //
// ============================================//

struct Jugador {
    unsigned int ID;                 // Id del jugador
    char nombre[100];                // nombre completo del jugador
    unsigned int edad;               // edad del jugador
    char cedula[20];                 // cedula del jugador
    unsigned int IDequipo;           // Id del equipo al que pertenece
    unsigned short dorsal;           // dorsal del jugador
    char posicion[25];               // posicion del jugador segun el deporte
    char fechaRegistro[11];          // fecha de registro del jugador en formato YYYY-MM-DD
    unsigned int puntosAnotados = 0; // puntos que anotó el jugador
};

struct ArbitroCentral {
    char nombre[100];       // nombre completo del arbitro
    unsigned int edad;      // edad del arbitro
    char cedula[20];        // cedula del arbitro
    unsigned int ID;        // Id del arbitro
    char fechaRegistro[11]; // fecha de registro del arbitro en formato YYYY-MM-DD
};

struct Equipo {
    unsigned int ID;                 // Id del equipo
    char nombre[100];                // nombre completo del equipo
    char ciudad[100];                // ciudad de origen del equipo
    char entrenador[100];            // nombre completo del entrenador
    char fechaRegistro[11];          // fecha de registro del equipo en Formato: YYYY-MM-DD
    unsigned int jugados = 0;        // Partidos jugados
    unsigned int puntos = 0;         // puntos del equipo
    unsigned int victorias = 0;      // victorias conseguidas
    unsigned int derrotas = 0;       // derrotas conseguidas
    unsigned int empates = 0;        // empates conseguidos
    unsigned int puntosAFavor = 0;   // capacidad de puntos a favor
    unsigned int puntosEnContra = 0; // capacidad de puntos en contra
    unsigned int numJugadores = 0;   // Numero de jugadores del equipo
};

struct Partido {
    int id;                  // Id del partido
    int idEquipoLocal;       // ID del equipo local
    int idEquipoVisitante;   // ID del equipo visitante
    int puntosLocal = 0;     // puntos del equipo local
    int puntosVisitante = 0; // puntos del equipo visitante
    char fecha[11];          // fecha en la que se jugó el partido en Formato YYYY-MM-DD
    char estado[12];         // "PROGRAMADO", "JUGADO" o "CANCELADO"
    char descripcion[200];   // Notas adicionales (opcional)
};

struct Torneo {
    char nombre[100];     // Nombre del torneo
    char deporte[50];     // Deporte
    char formato[25];     // "GRUPOS" o "ELIMINATORIA"
    char fechaInicio[11]; // fecha de inicio en Formato YYYY-MM-DD
    char fechaFin[11];    // fecha de finalizacion en Formato: YYYY-MM-DD
};

struct SistemaDeportivo {
    Torneo torneo; // Un objeto torneo

    Partido *Partidos;
    size_t numPartidosActuales;
    size_t capacidadPartidos;

    Equipo *Equipos;
    size_t numEquiposActuales;
    size_t capacidadEquipos;

    Jugador *Jugadores;
    size_t numJugadoresActuales;
    size_t capacidadJugadores;

    int siguienteIdEquipo;
    int siguienteIdJugador;
    int siguienteIdPartido;
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
        // INTENTO 1: Usar códigos de escape ASCII/ANSI (El método más rápido y moderno)
        // Enviamos el comando de borrado. Si la terminal lo soporta, se limpiará al instante.
        std::cout << "\x1B[2J\x1B[H" << std::flush;

        // INTENTO 2: Fallback tradicional mediante comandos del Sistema Operativo
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
    template <typename Tipo1> //
    void ingresarDatos(Tipo1 &variable, const char *mensaje, bool (*ptrValidador)(Tipo1, char *) = nullptr) {
        // bandera que se activa si el usuario ingresa un tipo de dato incorrecto
        bool flag = false;
        const int tamConst = 150;
        char mensajeError[tamConst];
        do {
            mensajeError[0] = '\0'; // Limpieza preventiva del error anterior
            flag = false;
            cout << mensaje << std::flush;
            cin >> variable;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                flag = true; // activamos la bandera
                cout << "Error Tipo de Dato Incorrecto\n\n";
                waitfor(3000);
            } else {
                // si el puntero no contiene la direccion de ninguna direccion se omite este bloque
                if (ptrValidador != nullptr) {
                    flag = !ptrValidador(variable, mensajeError); // si no es valido se activa la bandera
                    cout << mensajeError << endl << endl;
                }
                // Si la lectura fue exitosa, limpiamos el enter residual
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } while (flag);
    }

    // funcion para ingresar cadenas de texto
    void ingresarCadena(char *texto, int tamañoMaximo, const char *mensaje, bool (*ptrValidador)(const char *, char *) = nullptr) {
        const int tamconst = 150;
        bool flag = false;
        char mensajeError[tamconst];

        do {
            mensajeError[0] = '\0'; // Limpieza preventiva del error
            flag = false;

            // usamos std::flush para obligar a la pantalla a mostrar el mensaje
            cout << mensaje << std::flush;

            // Se lee toda la linea
            cin.getline(texto, tamañoMaximo);

            // Si falla
            if (cin.fail()) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                flag = true;
                cout << "ERROR: Excediste el limite de caracteres permitido (" << tamañoMaximo - 1 << "). Intente de nuevo.\n\n";
                Auxiliares::waitfor(3000);
                continue; // Saltamos directo a la siguiente iteración ya que no es necesario el validador
            }

            // Si la lectura no tuvo errores, pasamos el texto por el validador
            if (ptrValidador != nullptr) {
                if (!ptrValidador(texto, mensajeError)) {
                    flag = true; // Si el validador retorna false, la bandera se activa para repetir
                    cout << mensajeError << endl << endl;
                    waitfor(3500);
                }
            }
        } while (flag);
    }

    // funcion que se encarga de pausar el programa hasta que el usuario ingrese enter por la consola
    void pausarPrograma() {
        // Ignora cualquier carácter sobrante en el búfer hasta encontrar el salto de línea
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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

    char deporteActual[50] = "";
    char fechaDeIni[11];
    char fechaDeFin[11];

    // Función que se llamará una sola vez al crear el torneo
    void definirDeporteActual(const char *deporte) {
        std::strcpy(deporteActual, deporte);
        // La aseguramos en mayúsculas de una vez
        Auxiliares::toMayus(deporteActual);
    }

    // definimos las fechas para las futuras validaciones de datos
    void definirFechaInicio(const char *fechaInicio) { std::strcpy(fechaDeIni, fechaInicio); }

    void definirFechaFin(const char *fechaFin) { std::strcpy(fechaDeFin, fechaFin); }

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
                esNum = std::isdigit(*(texto + e));
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
                esLetra = std::isalpha(*(texto + e));
                esEspacio = std::isspace(*(texto + e));
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
            std::strcpy(mensajeError, "El dato ingresado no puede ser negativo");
            return false;
        }
        return true;
    }

    bool IDvalido(const unsigned int id, char *mensajeError) {
        if (id <= 0) {
            std::strcpy(mensajeError, "Error: El ID debe ser mayor a 0");
            return false;
        }
        return true;
    }

    bool Edad(const unsigned int edad, char *mensajeError) {
        // la edad no puede ser negativa ni igual a 0, tampoco puede ser mayor a 120
        if (edad < 14 || edad > 50) {
            // asignamos la siguiente cadena de texto a el array de char
            std::strcpy(mensajeError, "La edad debe esta entre un rango de 14-50");
            return false;
        }
        return true;
    }

    bool Dorsal(const unsigned short dorsal, char *mensajeError) {
        if (dorsal < 1 || dorsal > 99) {
            // std:strcpy copia el mensaje del segundo parametro dentro de un const char*
            std::strcpy(mensajeError, "El dorsal debe esta entre un rango de 1-99");
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
            std::strcpy(mensajeError, "La fecha no debe estar vacía");
            return false; // si esta vacio devolvemos que la fecha no es válida
        }

        // Validación de tamaño y guiones (YYYY-MM-DD)
        if (!TamañoValido(fecha, 10) || fecha[4] != '-' || fecha[7] != '-') {
            std::strcpy(mensajeError, "Formato incorrecto. Debe usar YYYY-MM-DD (Ej: 2026-05-31).");
            return false;
        }

        // Verificamos que los demás caracteres sean numéricos
        for (size_t e = 0; e < 10; e++) {
            if (e == 4 || e == 7)
                continue;
            if (!std::isdigit(fecha[e])) {
                std::strcpy(mensajeError, "La fecha no debe contener caracteres no numericos.");
                return false;
            }
        }

        // Extracción numérica basada en el formato YYYY-MM-DD
        /*año = (fecha[0] - '0') * 1000 + (fecha[1] - '0') * 100 + (fecha[2] - '0') * 10 + (fecha[3] - '0');
        mes = (fecha[5] - '0') * 10 + (fecha[6] - '0');
        dia = (fecha[8] - '0') * 10 + (fecha[9] - '0');*/
        FechaToNum(fecha, año, mes, dia);
        // si es año bisiesto febrero pasa a tener 29 dias
        if (mes == 2 && esBisiesto(año)) {
            diasPorMes[1] = 29;
        }

        // validar que los meses esten en el rango y que los años no sean negativos
        if (mes < 1 || mes > 12 || año < 1) {
            std::strcpy(mensajeError, "Los meses deben estar entre el rango de 1-12 o año inválido");
            return false;
        }

        // validar que el dia
        if (dia < 1 || dia > diasPorMes[mes - 1]) {
            std::strcpy(mensajeError, "El dia ingresado no existe para ese mes.");
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
            std::strcpy(mensajeError, "La fecha de Finalizacion no puede ser antes que la fecha de Inicio");
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
            std::strcpy(mensajeError, "La fecha del Partido esta fuera del rango del torneo");
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
            std::strcpy(mensajeError, "Error, el registro debe hacerse antes del dia de inicio del torneo");
            return false;
        }

        // Calculamos la diferencia de tiempo en meses entre ambas fechas
        int difAños = añoIni - añoRgt;
        int difMeses = (mesIni - mesRgt);
        int totalDeMesesDiferencia = (difAños * 12) + difMeses;

        // Verificamos que el registro no sea mas de 6 meses antes
        if (std::abs(totalDeMesesDiferencia) > 6) {
            std::strcpy(mensajeError, "Error, el registro solo puede hacerse hasta maximo 6 meses antes del incio del torneo");
            return false;
        }
        return true;
    }

    bool Cedulas(const char *cedula, char *mensajeError) {
        const size_t tamañoMin = 7, tamañomax = 10;

        // validamos que no esté vacío
        if (charVacio(cedula)) {
            std::strcpy(mensajeError, "La cedula no puede estar vacía.");
            return false; // si esta vacio devolvemos que la fecha no es válida
        }

        // verificamos que solo tenga numeros
        if (!soloNumeros(cedula)) {
            std::strcpy(mensajeError, "La cedula no puede contener caracteres no númericos.");
            return false;
        }

        // medimos la longitud
        size_t longitud = strlen(cedula);

        // si la longitud esta fuera del rando
        if (longitud < tamañoMin || longitud > tamañomax) {
            std::strcpy(mensajeError, "Longitud de cedula invalida (Debe tener entre 7 y 10 digitos).");
            return false;
        }

        return true;
    }

    bool Nombres(const char *nombre, char *mensajeError) {
        // validamos que no esté vacío
        if (charVacio(nombre)) {
            std::strcpy(mensajeError, "El nombre no puede estar vacío");
            return false; // si esta vacio devolvemos que la fecha no es válida
        }

        // validamos que solo contenga letras
        if (!soloLetras(nombre)) {
            std::strcpy(mensajeError, "El nombre solo debe contener letras y espacios");
            return false;
        }
        // ! Nota: No se válida el tamaño ya que la funcion ingresarCadena ya valida que no supere el tamaño max
        return true;
    }

    bool nombreTorneo(const char *nombreTorneo, char *mensajeError) {
        // validamos que no esté vacío
        if (charVacio(nombreTorneo)) {
            std::strcpy(mensajeError, "El nombre no puede estar vacío");
            return false; // si esta vacio devolvemos que la fecha no es válida
        }

        //
        if (!esAlfanumericoConEspacios(nombreTorneo)) {
            std::strcpy(mensajeError, "El nombre solo debe contener caracteres alfanumericos o espacios");
            return false;
        }

        return true;
    }

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

    // Función para validar si un deporte está en la lista (usa mensaje de error estilo original)
    bool existeDeporte(const char *deporte, char *mensajeError) {
        if (charVacio(deporte)) {
            std::strcpy(mensajeError, "El Deporte ingresado no puede estar Vacio");
            return false;
        }

        // Creamos una copia porque no se puede modificar la original y la convertimos a mayuscula
        char copiaDeporte[50];
        std::strcpy(copiaDeporte, deporte);
        Auxiliares::toMayus(copiaDeporte);

        for (size_t e = 0; e < sizeof(Deportes) / sizeof(Deportes[0]); e++) {
            if (std::strcmp(copiaDeporte, Deportes[e]) == 0) {
                return true;
            }
        }
        std::strcpy(mensajeError, "El deporte ingresado no esta en la lista de deportes validos.");
        return false;
    }

    // Función para validar la posición del jugador según el deporte del torneo
    bool Posicion(const char *posicion, char *mensajeError) {
        if (charVacio(posicion)) {
            std::strcpy(mensajeError, "La posición no puede estar vacía.");
            return false;
        }

        if (charVacio(deporteActual)) {
            std::strcpy(mensajeError, "Error: No se ha definido el deporte del torneo todavía.");
            return false;
        }

        char copiaPosicion[50];
        std::strcpy(copiaPosicion, posicion);
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
                break;
            }
        }
        std::strcpy(mensajeError, "Error del sistema: El deporte actual no coincide con los registros.");
        return false;
    }

} // namespace Validadores

// ============================================//
//   5. CAPA DE LOGICA                         //
// ============================================//

namespace Logica {

    void inicializarSistemaDeportivo(SistemaDeportivo *MiSistema, Torneo torneo) {

        // Si no ha sido inicializado
        if (MiSistema == nullptr) {
            return;
        }

        // Inicializamos el Torneo
        MiSistema->torneo = torneo;

        // inicializar la capacidad total de las variables
        MiSistema->capacidadEquipos = 4;
        MiSistema->capacidadJugadores = 4;
        MiSistema->capacidadPartidos = 4;

        // inicializar los arrays
        MiSistema->Equipos = new Equipo[4];    // se deben inicializar con tamaño de 4
        MiSistema->Jugadores = new Jugador[4]; // se deben inicializar con tamaño de 4
        MiSistema->Partidos = new Partido[4];  // se deben inicializar con tamaño de 4

        // inicializar los contadores
        MiSistema->numEquiposActuales = 0;
        MiSistema->numJugadoresActuales = 0;
        MiSistema->numPartidosActuales = 0;

        // inicializar los IDs
        MiSistema->siguienteIdEquipo = 1;
        MiSistema->siguienteIdJugador = 1;
        MiSistema->siguienteIdPartido = 1;
    }

    void liberarSistema(SistemaDeportivo *MiSistema) {

        // Si no ha sido inicializado
        if (MiSistema == nullptr) {
            return;
        }

        // Inicializamos el Torneo
        MiSistema->torneo = {};

        // inicializar la capacidad total de las variables
        MiSistema->capacidadEquipos = 0;
        MiSistema->capacidadJugadores = 0;
        MiSistema->capacidadPartidos = 0;

        // se liberan los arrays y los apuntamos a nullptr
        delete[] MiSistema->Equipos;
        MiSistema->Equipos = nullptr;
        delete[] MiSistema->Jugadores;
        MiSistema->Jugadores = nullptr;
        delete[] MiSistema->Partidos;
        MiSistema->Partidos = nullptr;

        // contadores en 0
        MiSistema->numEquiposActuales = 0;
        MiSistema->numJugadoresActuales = 0;
        MiSistema->numPartidosActuales = 0;

        // IDs a 0
        MiSistema->siguienteIdEquipo = 0;
        MiSistema->siguienteIdJugador = 0;
        MiSistema->siguienteIdPartido = 0;
    }

    void definirFormato(Torneo &torneoAux, int opcion) {
        if (opcion == 1) {
            std::strcpy(torneoAux.formato, "GRUPOS");
        } else if (opcion == 2) {
            std::strcpy(torneoAux.formato, "ELIMINATORIA");
        }
    }

    namespace redimensionar {
        void rEquipos(SistemaDeportivo *original) {

            // Pequeña validacion en caso de que el puntero no apunte a nada
            if (original == nullptr) {
                return;
            }

            // Almacenamos la capacidad antigua para el bucle for
            size_t capacidadAntigua = original->capacidadEquipos;

            // Protección contra capacidad inicial en 0. Si es cero arranca en 4, si no se duplica
            original->capacidadEquipos = (capacidadAntigua == 0) ? 4 : capacidadAntigua * 2;

            // Creamos array con nueva capacidad
            Equipo *nuevoArray = new Equipo[original->capacidadEquipos];

            // Copiamos los elementos en el nuevo array uno por uno
            for (size_t e = 0; e < capacidadAntigua; e++) {
                nuevoArray[e] = original->Equipos[e];
            }

            // liberamos la memoria del array antiguo
            if (original->Equipos != nullptr) {
                delete[] original->Equipos;
            }

            // cambiamos el lugar al que apunta el puntero original para que apunte
            original->Equipos = nuevoArray; // al nuevo bloque de memoria redimensionado
        }

        void rJugadores(SistemaDeportivo *original) {

            // Pequeña validacion en caso de que el puntero no apunte a nada
            if (original == nullptr) {
                return;
            }

            // Almacenamos la capacidad antigua para el bucle for
            size_t capacidadAntigua = original->capacidadJugadores;

            // Protección contra capacidad inicial en 0. Si es cero arranca en 4, si no se duplica
            original->capacidadJugadores = (capacidadAntigua == 0) ? 4 : capacidadAntigua * 2;

            // Creamos array con nueva capacidad
            Jugador *nuevoArray = new Jugador[original->capacidadJugadores];

            // Copiamos los elementos en el nuevo array uno por uno
            for (size_t e = 0; e < capacidadAntigua; e++) {
                nuevoArray[e] = original->Jugadores[e];
            }

            // liberamos la memoria del array antiguo
            if (original->Jugadores != nullptr) {
                delete[] original->Jugadores;
            }

            // cambiamos el lugar al que apunta el puntero original para que apunte
            original->Jugadores = nuevoArray; // al nuevo bloque de memoria redimensionado
        }

        void rPartidos(SistemaDeportivo *original) {

            // Pequeña validacion en caso de que el puntero no apunte a nada
            if (original == nullptr) {
                return;
            }

            // Almacenamos la capacidad antigua para el bucle for
            size_t capacidadAntigua = original->capacidadPartidos;

            // Protección contra capacidad inicial en 0. Si es cero arranca en 4, si no se duplica
            original->capacidadPartidos = (capacidadAntigua == 0) ? 4 : capacidadAntigua * 2;

            // Creamos array con nueva capacidad
            Partido *nuevoArray = new Partido[original->capacidadPartidos];

            // Copiamos los elementos en el nuevo array uno por uno
            for (size_t e = 0; e < capacidadAntigua; e++) {
                nuevoArray[e] = original->Partidos[e];
            }

            // liberamos la memoria del array antiguo
            if (original->Partidos != nullptr) {
                delete[] original->Partidos;
            }

            // cambiamos el lugar al que apunta el puntero original para que apunte
            original->Partidos = nuevoArray; // al nuevo bloque de memoria redimensionado
        }
    } // namespace redimensionar

    namespace equipos {

        bool existeID(SistemaDeportivo *MiSistema, const unsigned int ID) {
            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr) {
                return false;
            }

            // Verificar que si haya equipos
            if (MiSistema->numEquiposActuales == 0) {
                return false;
            }

            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                if (ID == MiSistema->Equipos[e].ID) {
                    return true;
                }
            }
            return false;
        }

        bool nombreDuplicado(SistemaDeportivo *MiSistema, const char *nombre) {

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr) {
                return false;
            }

            if (MiSistema->numEquiposActuales == 0) {
                return false;
            }

            char nombreBusquedaAux[100];
            std::strcpy(nombreBusquedaAux, nombre);
            Auxiliares::toMinus(nombreBusquedaAux);

            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                char nombreEquipoAux[100];
                std::strcpy(nombreEquipoAux, MiSistema->Equipos[e].nombre);
                Auxiliares::toMinus(nombreEquipoAux);
                if (std::strcmp(nombreBusquedaAux, nombreEquipoAux) == 0) {
                    return true;
                }
            }
            return false;
        }

        bool nombreEntrenadorDuplicado(SistemaDeportivo *MiSistema, const char *entrenador) {

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr) {
                return false;
            }

            if (MiSistema->numEquiposActuales == 0) {
                return false;
            }

            char entrenadorBusquedaAux[100];
            std::strcpy(entrenadorBusquedaAux, entrenador);
            Auxiliares::toMinus(entrenadorBusquedaAux);

            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                char entrenadorEquipoAux[100];
                std::strcpy(entrenadorEquipoAux, MiSistema->Equipos[e].entrenador);
                Auxiliares::toMinus(entrenadorEquipoAux);
                if (std::strcmp(entrenadorBusquedaAux, entrenadorEquipoAux) == 0) {
                    return true;
                }
            }
            return false;
        }

        Equipo *agregarEquipo(SistemaDeportivo *MiSistema, const char *nombre, const char *entrenador, const char *ciudad, const char *fecha) {

            // Verificar si hay espacio, y redimensionar si es necesario
            if (MiSistema->numEquiposActuales == MiSistema->capacidadEquipos) {
                redimensionar::rEquipos(MiSistema);
            }

            // usamos una variable de posicion
            size_t indice = MiSistema->numEquiposActuales;

            std::strcpy(MiSistema->Equipos[indice].nombre, nombre);
            std::strcpy(MiSistema->Equipos[indice].entrenador, entrenador);
            std::strcpy(MiSistema->Equipos[indice].ciudad, ciudad);

            // Inicializamos las estadísticas
            MiSistema->Equipos[indice].jugados = 0;
            MiSistema->Equipos[indice].victorias = 0;
            MiSistema->Equipos[indice].empates = 0;
            MiSistema->Equipos[indice].derrotas = 0;

            // Asignamos la fecha (Usar la fecha inicio del torneo es un buen placeholder,
            // aunque el manual pide "fecha actual", esto te sirve por ahora)
            std::strcpy(MiSistema->Equipos[indice].fechaRegistro, fecha);

            MiSistema->Equipos[indice].puntos = 0;
            MiSistema->Equipos[indice].puntosAFavor = 0;
            MiSistema->Equipos[indice].puntosEnContra = 0;

            // Asignamos el ID
            MiSistema->Equipos[indice].ID = MiSistema->siguienteIdEquipo;

            // Aumentamos los contadores
            MiSistema->numEquiposActuales++;
            MiSistema->siguienteIdEquipo++;

            // Retornamos la dirección de memoria del equipo que está dentro del array dinámico
            return &(MiSistema->Equipos[indice]);
        }

        Equipo *buscarEquipoPorID(SistemaDeportivo *MiSistema, const unsigned int id) {

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr) {
                return nullptr;
            }

            // Verificar que si haya equipos
            if (MiSistema->numEquiposActuales == 0) {
                return nullptr;
            }

            // Si pasa las validaciones recorremos el array dinamico con un for
            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                // si encontramos un id que coincida con el de algun equipo
                if (id == MiSistema->Equipos[e].ID) {
                    // devolvemos la direccion de memoria de ese equipo
                    return &(MiSistema->Equipos[e]);
                }
            }

            // Si no conseguimos nada devolvemos nullptr
            return nullptr;
        }

        Equipo **buscarEquipoPorSubCadena(SistemaDeportivo *MiSistema, const char *subcadena, int *contEquiposEncontrados) {
            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr) {
                return nullptr;
            }

            // Verificar que si haya equipos
            if (MiSistema->numEquiposActuales == 0) {
                return nullptr;
            }

            char copiaEquipo[100];
            char copiaSubcadena[100];
            std::strcpy(copiaSubcadena, subcadena);

            // incializamos el contador en 0
            *contEquiposEncontrados = 0;

            // creamos un array de punteros dinamico con tamaño maximo el numero de equipos que hay
            Equipo **arrayEquiposEncontrados = new Equipo *[MiSistema->numEquiposActuales];

            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                // hacemos una copia del nombre del equipo
                std::strcpy(copiaEquipo, MiSistema->Equipos[e].nombre);

                // lo pasamos a minus para comparar mejor
                // Buscamos si la subcadena coincide con la copia usando std::strstr
                if (std::strstr(Auxiliares::toMinus(copiaEquipo), Auxiliares::toMinus(copiaSubcadena)) != nullptr) {
                    arrayEquiposEncontrados[*(contEquiposEncontrados)] = &(MiSistema->Equipos[e]);
                    (*contEquiposEncontrados)++;
                }
            }
            return arrayEquiposEncontrados;
        }

        Equipo **listarEquipos(SistemaDeportivo *MiSistema, unsigned int *cantEquipos) {

            // inicializamos en 0 por si no pasa las validaciones
            *cantEquipos = 0;

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr) {
                return nullptr;
            }

            // Verificar que si haya equipos
            if (MiSistema->numEquiposActuales == 0) {
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
        }

        Equipo **TablaDePosiciones(SistemaDeportivo *MiSistema, unsigned int *cantEquipos) {
            // inicializamos en 0 por si no pasa las validaciones
            *cantEquipos = 0;
            int difPtsEq1 = 0, difPtsEq2 = 0;
            bool intercambiar = false;

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr) {
                return nullptr;
            }

            // Verificar que si haya equipos
            if (MiSistema->numEquiposActuales == 0) {
                return nullptr;
            }

            // inicializamos nuestras variables
            *cantEquipos = MiSistema->numEquiposActuales;
            Equipo *ptrAux = nullptr;

            // Declaramos un array de punteros a equipos
            // Y generamos una lista con las direcciones de memoria de cada equipo
            Equipo **listaDePtrAEquipos = Logica::equipos::listarEquipos(MiSistema, cantEquipos);

            // Ordenamos por cantidad de puntos de mayor a menor
            // Restamos 1 para no acceder a memoria indebida
            for (size_t e = 0; e < (*cantEquipos) - 1; e++) {
                // restamos 1 por la misma razon y 'e' para no recorrer los elemento ya ordenados del final
                for (size_t r = 0; r < (*cantEquipos) - e - 1; r++) {

                    // incializamos esta bandera en false, se activa si los equipos
                    // requieren que se interambien por los criterios de cada condicion
                    intercambiar = false;

                    //* Condicion 1
                    // Si el equipo 1 tiene menos puntos que el equipo 2;
                    if ((listaDePtrAEquipos[r]->puntos) < (listaDePtrAEquipos[r + 1]->puntos)) {
                        intercambiar = true;

                        // Si poseen igual cantidad de puntos
                    } else if ((listaDePtrAEquipos[r]->puntos) == (listaDePtrAEquipos[r + 1]->puntos)) {

                        // Calculamos diferencia de puntos
                        difPtsEq1 = (listaDePtrAEquipos[r]->puntosAFavor) - (listaDePtrAEquipos[r]->puntosEnContra);
                        difPtsEq2 = (listaDePtrAEquipos[r + 1]->puntosAFavor) - (listaDePtrAEquipos[r + 1]->puntosEnContra);

                        // *Condicion 2
                        // Si el equipo de la izquierda tiene menor diferencia de puntos
                        // lo ubicamos a la deracha es decir lo bajamos una posicion
                        if (difPtsEq1 < difPtsEq2) {
                            intercambiar = true;

                            // Si la diferencia de goles es igual tambien
                        } else if (difPtsEq1 == difPtsEq2) {

                            //* Condicion 3
                            // comparamos los puntos a favor
                            if (listaDePtrAEquipos[r]->puntosAFavor < listaDePtrAEquipos[r + 1]->puntosAFavor) {
                                intercambiar = true;

                                // Si los puntos a favor son iguales desempatamos por victorias
                            } else if (listaDePtrAEquipos[r]->puntosAFavor == listaDePtrAEquipos[r + 1]->puntosAFavor) {

                                // * Condicion 4
                                // comparamos las victorias

                                if (listaDePtrAEquipos[r]->victorias < listaDePtrAEquipos[r + 1]->victorias) {
                                    intercambiar = true;
                                }
                            }
                        }
                    }

                    // Si se cumple alguna condicion hacemos el intercambio
                    if (intercambiar) {
                        // Guardamos el equipo con menos puntos en un ptr auxiliar
                        ptrAux = listaDePtrAEquipos[r];

                        // Luego movemos la direccion del mayor a la direccion donde estaba el menor
                        listaDePtrAEquipos[r] = listaDePtrAEquipos[r + 1];

                        // colocamos en la nueva posicion al equipo con menos puntos
                        listaDePtrAEquipos[r + 1] = ptrAux;
                    }
                }
            }
            return listaDePtrAEquipos;
        }

        bool actualizarEquipo(SistemaDeportivo *MiSistema, const unsigned int ID, const char *nombre, const char *entrenador, const char *ciudad) {

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr) {
                return false;
            }

            // Verificar que si haya equipos
            if (MiSistema->numEquiposActuales == 0) {
                return false;
            }

            // Buscamos el equipo mediante el ID
            Equipo *ptrEquipo = nullptr;
            ptrEquipo = buscarEquipoPorID(MiSistema, ID);


            if (ptrEquipo == nullptr) {
                return false;
            }

            // Actualizamos los datos
            std::strcpy(ptrEquipo->nombre, nombre);
            std::strcpy(ptrEquipo->entrenador, entrenador);
            std::strcpy(ptrEquipo->ciudad, ciudad);

            return true;
        }

        bool eliminarEquipo(SistemaDeportivo *MiSistema, const unsigned int ID) {

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr) {
                return false;
            }

            // Verificar que si haya equipos
            if (MiSistema->numEquiposActuales == 0) {
                return false;
            }

            int posicion = -1;

            // Buscamos la posición del equipo
            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                // Si encontramos una coincidencia almacenamos la posicion en memoria
                if (MiSistema->Equipos[e].ID == ID) {
                    posicion = e;
                    break;
                }
            }

            // Verificamos si no se encontró ningun equipo con ese ID
            if (posicion == -1) {
                return false;
            }

            // Verificamos que no tenga partidos asociados
            for (size_t e = 0; e < MiSistema->numPartidosActuales; e++) {
                if ((MiSistema->Partidos[e].idEquipoLocal == ID) || (MiSistema->Partidos[e].idEquipoVisitante == ID)) {
                    return false;
                }
            }

            // Se ejecuta siempre y cuando exista al menos 1 jugador regisrado
            if (MiSistema->numJugadoresActuales > 0) {

                // Este primer bucle for actua como un buscador de los jugadores con ID asociado
                // Se ejecuta de atrás hacia adelante porque es la forma mas segura
                for (int r = (int)MiSistema->numJugadoresActuales - 1; r >= 0; r--) {

                    // Si el jugador pertenece al equipo que estamos borrando
                    if ((int)MiSistema->Jugadores[r].IDequipo == (int)ID) {

                        // Desplazamos los jugadores encontrados al final para que no sean tomados en cuenta
                        for (size_t k = r; k < MiSistema->numJugadoresActuales - 1; k++) {
                            MiSistema->Jugadores[k] = MiSistema->Jugadores[k + 1];
                        }

                        // Reducimos el contador de jugadores totales del sistema
                        MiSistema->numJugadoresActuales--;
                    }
                }
            }


            // Desplazamos y reescribimos a los equipos hacia la izquierda
            for (size_t e = posicion; e < MiSistema->numEquiposActuales - 1; e++) {
                MiSistema->Equipos[e] = MiSistema->Equipos[e + 1];
            }

            // Disminuimos el numero de equipos y el ID autoincremental
            MiSistema->numEquiposActuales--;
            return true;
        }

    } // namespace equipos

    namespace jugadores {

        bool existeID(SistemaDeportivo *MiSistema, const unsigned int ID) {

            // Verificamos que todo este inicializado por precaucion
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr || MiSistema->Jugadores == nullptr) {
                return false;
            }

            // Si no hay equipos ni jugadores devolver false
            if (MiSistema->numEquiposActuales == 0 || MiSistema->numJugadoresActuales == 0) {
                return false;
            }

            // Recorremos y verficamos que existe el ID
            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                if (ID == MiSistema->Jugadores[e].ID) {
                    return true;
                }
            }
            return false;
        }

        bool nombreDuplicado(SistemaDeportivo *MiSistema, const char *nombre) {

            // Verificamos que todo este inicializado por precaucion
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr || MiSistema->Jugadores == nullptr) {
                return false;
            }

            // Si no hay equipos ni jugadores devolver false
            if (MiSistema->numEquiposActuales == 0 || MiSistema->numJugadoresActuales == 0) {
                return false;
            }

            char nombreAux[100];
            std::strcpy(nombreAux, nombre);
            Auxiliares::toMinus(nombreAux);

            // Recorremos en busca de un nombre duplicado
            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                char nombreJugadorAux[100];
                std::strcpy(nombreJugadorAux, MiSistema->Jugadores[e].nombre);
                Auxiliares::toMinus(nombreJugadorAux);
                if (std::strcmp(nombreAux, nombreJugadorAux) == 0) {
                    return true;
                }
            }
            return false;
        }

        bool DorsalDuplicado(SistemaDeportivo *MiSistema, unsigned short Dorsal, const unsigned int IDequipo) {

            // Verificamos que todo este inicializado por precaucion
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr || MiSistema->Jugadores == nullptr) {
                return false;
            }

            // Si no hay equipos ni jugadores devolver false
            if (MiSistema->numEquiposActuales == 0 || MiSistema->numJugadoresActuales == 0) {
                return false;
            }

            // Recorremos en busca de dorsal duplicado
            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {

                // si hay alguien con el mismo dorsal y esta en el mismo equipo
                if ((Dorsal == MiSistema->Jugadores[e].dorsal) && (MiSistema->Jugadores[e].IDequipo == IDequipo)) {
                    return true;
                }
            }
            return false;
        }

        bool CedulaRepetida(SistemaDeportivo *MiSistema, const char *Cedula) {
            // Verificamos que todo este inicializado por precaucion
            if (MiSistema == nullptr || MiSistema->Equipos == nullptr || MiSistema->Jugadores == nullptr) {
                return false;
            }

            // Si no hay equipos ni jugadores devolver false
            if (MiSistema->numEquiposActuales == 0 || MiSistema->numJugadoresActuales == 0) {
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

        Jugador *agregarJugador(SistemaDeportivo *MiSistema, const unsigned int IDEquipo, const char *nombre, const char *cedula, const char *posicion, const int unsigned edad,
                                const unsigned short numeroDorsal, const char *fechaRegistro) {

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Jugadores == nullptr || MiSistema->Equipos == nullptr) {
                return nullptr;
            }

            if (MiSistema->numEquiposActuales == 0) {
                return nullptr;
            }

            // Verificar si hay espacio, y redimensionar si es necesario
            if (MiSistema->numJugadoresActuales == MiSistema->capacidadJugadores) {
                redimensionar::rJugadores(MiSistema);
            }

            // usamos una variable de posicion
            size_t indice = MiSistema->numJugadoresActuales;

            std::strcpy(MiSistema->Jugadores[indice].nombre, nombre);
            std::strcpy(MiSistema->Jugadores[indice].cedula, cedula);
            std::strcpy(MiSistema->Jugadores[indice].posicion, posicion);
            MiSistema->Jugadores[indice].edad = edad;
            MiSistema->Jugadores[indice].IDequipo = IDEquipo;
            MiSistema->Jugadores[indice].dorsal = numeroDorsal;
            std::strcpy(MiSistema->Jugadores[indice].fechaRegistro, fechaRegistro);
            MiSistema->Jugadores[indice].puntosAnotados = 0;
            MiSistema->Jugadores[indice].ID = MiSistema->siguienteIdJugador;

            // Aumentamos el numero de jugadores del equipo
            for (size_t e = 0; e < MiSistema->numEquiposActuales; e++) {
                if (MiSistema->Equipos[e].ID == IDEquipo) {
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

        Jugador *buscarJugadorPorID(SistemaDeportivo *MiSistema, const unsigned int ID) {
            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Jugadores == nullptr || MiSistema->Equipos == nullptr) {
                return nullptr;
            }

            // si no hay equipos cancelamos la busqueda
            if (MiSistema->numEquiposActuales == 0) {
                return nullptr;
            }

            // si no hay jugadores cancelamos la busqueda
            if (MiSistema->numJugadoresActuales == 0) {
                return nullptr;
            }

            // Buscamos si el id esta relacionado con un jugador
            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                if (MiSistema->Jugadores[e].ID == ID) {
                    return &(MiSistema->Jugadores[e]);
                }
            }

            // si no encontró nada decuelve nullptr
            return nullptr;
        }

        Jugador **buscarJugadoresPorNombre(SistemaDeportivo *MiSistema, const char *subcadena, int *cantJugadoresEncontrados) {
            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Jugadores == nullptr || MiSistema->Equipos == nullptr) {
                return nullptr;
            }

            // si no hay equipos cancelamos la busqueda
            if (MiSistema->numEquiposActuales == 0) {
                return nullptr;
            }

            // si no hay jugadores cancelamos la busqueda
            if (MiSistema->numJugadoresActuales == 0) {
                return nullptr;
            }

            // creamos dos variables para realizar copias y no dañar laa originales
            char copiaJugador[100];
            char copiaSubcadena[100];

            // incializamos el contador en 0
            *cantJugadoresEncontrados = 0;

            // Reservamos espacio para una lista de jugadores
            Jugador **listaDeJugadoresEncontrados = new Jugador *[MiSistema->numJugadoresActuales];

            // hacemos la copia de la subcadena y la pasamos a minuscula
            std::strcpy(copiaSubcadena, subcadena);
            Auxiliares::toMinus(copiaSubcadena);

            for (size_t e = 0; e < MiSistema->numJugadoresActuales; e++) {
                // realizamos la copia del nombre del jugador para esta iteración
                std::strcpy(copiaJugador, MiSistema->Jugadores[e].nombre);
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

        Jugador **listarJugadoresPorEquipo(SistemaDeportivo *MiSistema, const unsigned int IDEquipo, unsigned int *cantidadJugadores) {

            // inicializamos en 0
            *cantidadJugadores = 0;

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Jugadores == nullptr || MiSistema->Equipos == nullptr) {
                return nullptr;
            }

            // si no hay equipos cancelamos la busqueda
            if (MiSistema->numEquiposActuales == 0) {
                return nullptr;
            }

            // si no hay jugadores cancelamos la busqueda
            if (MiSistema->numJugadoresActuales == 0) {
                return nullptr;
            }

            // Buscamos si el equipo solicitado realmente existe
            Equipo *EquipoAux = equipos::buscarEquipoPorID(MiSistema, IDEquipo);

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
                if (MiSistema->Jugadores[e].IDequipo == IDEquipo) {

                    // Nos aseguramos de no sobrepasarnos el limite
                    if (*cantidadJugadores < EquipoAux->numJugadores) {
                        listaDeJugadores[*cantidadJugadores] = &(MiSistema->Jugadores[e]);
                        (*cantidadJugadores)++;
                    }
                }
            }
            return listaDeJugadores;
        }

        Jugador **listarJugadores(SistemaDeportivo *MiSistema, unsigned int *cantidadJugadores) {
            // inicializamos en 0
            *cantidadJugadores = 0;

            // verificamos que ni el sistema ni el array de equipos apunte a nullptr
            if (MiSistema == nullptr || MiSistema->Jugadores == nullptr || MiSistema->Equipos == nullptr) {
                return nullptr;
            }

            // si no hay equipos cancelamos la busqueda
            if (MiSistema->numEquiposActuales == 0) {
                return nullptr;
            }

            // si no hay jugadores cancelamos la busqueda
            if (MiSistema->numJugadoresActuales == 0) {
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

        //
        bool actualizarJugador(SistemaDeportivo *MiSistema, int ID, Jugador jugadorActualizado) {
            //
        }

        // Elimina un jugador del sistema
        // Retorna true si se eliminó, false si no existe
        bool eliminarJugador(SistemaDeportivo *MiSistema, int ID);
    } // namespace jugadores

    namespace partidos {
        //
    }

} // namespace Logica

// ============================================//
//   6. PRESENTACION                           //
// ============================================//

namespace Presentacion {

    namespace menu {

        void datosInicialesTorneo(SistemaDeportivo *MiSistema) {
            // variables auxiliares
            Torneo torneoAux;
            int opcionFormato = 0;
            bool opcionValida = false;

            //* Aqui se recopilan los datos iniciales del torneo

            // Ingresar Nombre
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(torneoAux.nombre, 100, "Nombre del Torneo: ", Validadores::Nombres);
            Auxiliares::waitfor(1500);

            // Ingresar Deporte
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Mostrar deportes disponibles
            cout << "Deportes disponibles:\n";
            for (size_t e = 0; e < Validadores::totalDeportes; e++) {
                cout << " - " << Validadores::Deportes[e] << std::endl;
            }

            // Validación externa
            char mensajeError[150];
            bool deporteValido = false;
            do {
                Auxiliares::ingresarCadena(torneoAux.deporte, 50, "Deporte del Torneo: ");
                // Normalizamos a mayusculas para facilitar la comparación
                Auxiliares::toMayus(torneoAux.deporte);

                if (!Validadores::existeDeporte(torneoAux.deporte, mensajeError)) {
                    cout << "Error: " << mensajeError << std::endl;
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
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            cout << "1. Formato de Grupos (Todos contra todos)\n";
            cout << "2. Formato de Eliminatoria Directa\n";
            cout << "--------------------------------------------------\n";
            do {
                opcionValida = true;
                Auxiliares::ingresarDatos(opcionFormato, "Seleccione el formato (1 o 2): ");
                if (opcionFormato != 1 && opcionFormato != 2) {
                    cout << "Opcion invalida. Intente de nuevo.\n";
                    opcionValida = false;
                }
            } while (!opcionValida);

            // desde la logica definimos el tipo de torneo en base a la opcion ingresada
            Logica::definirFormato(torneoAux, opcionFormato);
            Auxiliares::waitfor(1500);

            // Ingresar Fecha de Inicio del torneo
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(torneoAux.fechaInicio, 11, "Fecha De Inicio del Torneo: ", Validadores::FechaValida);
            Validadores::definirFechaInicio(torneoAux.fechaInicio);
            Auxiliares::waitfor(1500);

            // Ingresar Fecha de Finalizacion de Torneo
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::ingresarCadena(torneoAux.fechaFin, 11, "Fecha de Finalización del Torneo: ", Validadores::ValidarFechaFin);
            Validadores::definirFechaFin(torneoAux.fechaFin);
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            // enviamos los datos
            Logica::inicializarSistemaDeportivo(MiSistema, torneoAux);

            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║  NUEVO TORNEO CREADO CON ÉXITO            ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            cout << "Nombre: " << torneoAux.nombre << endl;
            cout << "Deporte: " << torneoAux.deporte << endl;
            cout << "Formato: " << torneoAux.formato << endl;
            cout << "Fecha de inicio del torneo: " << torneoAux.fechaInicio << endl;
            cout << "Fecha de Finalización del torneo: " << torneoAux.fechaFin;
            Auxiliares::pausarPrograma();
        }

        void Principal(SistemaDeportivo *MiSistema) {
            Auxiliares::limpiarPantalla();

            cout << "\n   ╔══════════════════════════════════════════════════════════════╗\n";
            cout << "   ║                    Sport G&C Tournaments                     ║\n";
            cout << "   ║  Torneo:   " << std::left << std::setw(50) << MiSistema->torneo.nombre << "║\n";
            cout << "   ║  Deporte: " << std::left << std::setw(18) << MiSistema->torneo.deporte << " | Formato: " << std::left << std::setw(21) << MiSistema->torneo.formato
                 << "║\n";
            cout << "   ╠══════════════════════════════════════════════════════════════╣\n";
            cout << "   ║  " << std::left << std::setw(61) << "1. Gestión de Equipos" << "║\n";
            cout << "   ║  " << std::left << std::setw(61) << "2. Gestión de Jugadores" << "║\n";
            cout << "   ║  " << std::left << std::setw(61) << "3. Gestión de Partidos" << "║\n";
            cout << "   ║  " << std::left << std::setw(60) << "4. Tabla de Posiciones" << "║\n";
            cout << "   ║  " << std::left << std::setw(60) << "0. Salir" << "║\n";
            cout << "   ╚══════════════════════════════════════════════════════════════╝\n";
            cout << endl;
        }

        void GestionDeEquipos() {
            Auxiliares::limpiarPantalla();
            cout << "\n   ╔═══════════════════════════════════════════╗\n";
            cout << "   ║          GESTIÓN DE EQUIPOS               ║\n";
            cout << "   ╠═══════════════════════════════════════════╣\n";
            cout << "   ║  1. Registrar equipo                      ║\n";
            cout << "   ║  2. Buscar equipo                         ║\n";
            cout << "   ║  3. Actualizar equipo                     ║\n";
            cout << "   ║  4. Listar equipos                        ║\n";
            cout << "   ║  5. Eliminar equipo                       ║\n";
            cout << "   ║  0. Volver al menú principal              ║\n";
            cout << "   ╚═══════════════════════════════════════════╝\n";
            cout << endl;
        }

        void GestionDePartidos() {
            Auxiliares::limpiarPantalla();
            cout << "\n   ╔═══════════════════════════════════════════╗\n";
            cout << "   ║        GESTIÓN DE PARTIDOS                ║\n";
            cout << "   ╠═══════════════════════════════════════════╣\n";
            cout << "   ║  1. Programar partido                     ║\n";
            cout << "   ║  2. Registrar resultado                   ║\n";
            cout << "   ║  3. Buscar partido                        ║\n";
            cout << "   ║  4. Listar partidos                       ║\n";
            cout << "   ║  5. Cancelar partido                      ║\n";
            cout << "   ║  0. Volver al menú principal              ║\n";
            cout << "   ╚═══════════════════════════════════════════╝\n";
            cout << endl;
        }

        void GestionDeJugadores() {
            Auxiliares::limpiarPantalla();
            cout << "\n   ╔═══════════════════════════════════════════╗\n";
            cout << "   ║        GESTIÓN DE JUGADORES               ║\n";
            cout << "   ╠═══════════════════════════════════════════╣\n";
            cout << "   ║  1. Registrar jugador                     ║\n";
            cout << "   ║  2. Buscar jugador                        ║\n";
            cout << "   ║  3. Actualizar jugador                    ║\n";
            cout << "   ║  4. Listar jugadores (todos)              ║\n";
            cout << "   ║  5. Listar jugadores por equipo           ║\n";
            cout << "   ║  6. Eliminar jugador                      ║\n";
            cout << "   ║  0. Volver al menú principal              ║\n";
            cout << "   ╚═══════════════════════════════════════════╝\n";
            cout << endl;
        }
    } // namespace menu

    namespace equipos {
        // Recolectamos los datos para registrar el equipo
        void RegistrarEquipos(SistemaDeportivo *MiSistema) {
            bool flagError = false;
            char nombreAux[100];
            char entrenadorAux[100];
            char ciudadAux[50];
            char fechaAux[11];
            Equipo *nuevo = nullptr;
            char confirmacion;

            // Recolectamos el nombre del Equipo
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║          REGISTRAR NUVEVO EQUIPO          ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(nombreAux, 100, "Ingrese el nombre del Equipo: ", Validadores::Nombres);

                // Validamos nombre duplicado
                if (Logica::equipos::nombreDuplicado(MiSistema, nombreAux)) {
                    cout << "Error, el nombre '" << nombreAux << "' ya está en uso\n";
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
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║          REGISTRAR NUVEVO EQUIPO          ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(entrenadorAux, 100, "Ingrese el nombre del Entrenador: ", Validadores::Nombres);

                // Validamos nombre duplicado
                if (Logica::equipos::nombreEntrenadorDuplicado(MiSistema, entrenadorAux)) {
                    cout << "Error, el nombre '" << entrenadorAux << "' ya direge otro equipo\n";
                    flagError = true;
                    Auxiliares::waitfor(3000);
                    continue;
                }
                Auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la fecha de registro del equipo
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║          REGISTRAR NUVEVO EQUIPO          ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(fechaAux, 11, "Ingrese la fecha de Registro del Equipo: ", Validadores::fechaValidaRegistroDeJugadorOEquipo);
            Auxiliares::waitfor(2000);
            Auxiliares::limpiarPantalla();

            // Recolectamos la ciudad del Equipo
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║          REGISTRAR NUVEVO EQUIPO          ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(ciudadAux, 50, "Ingrese el nombre de la Ciudad del Equipo: ", Validadores::Nombres);
            Auxiliares::waitfor(2000);
            Auxiliares::limpiarPantalla();

            // Pedimos la confirmacion al usuario
            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea registrar este equipo? (S/N): ");
            if (toupper(confirmacion) == 'S') {
                // Agregamos el equipo a nuestro array dinamico
                nuevo = Logica::equipos::agregarEquipo(MiSistema, nombreAux, entrenadorAux, ciudadAux, fechaAux);

                // Si el equipo no se creo
                if (nuevo == nullptr) {
                    cout << "Error: No se logró registrar el equipo\n";
                    return;
                }

                // Si el equipo se creo conn éxito
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║        EQUIPO REGISTRADO CON ÉXITO        ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Mostramos los datos ingresados
                cout << "Torneo: " << MiSistema->torneo.nombre << endl;
                cout << "Nombre del Equipo: " << nuevo->nombre << endl;
                cout << "Entrenador del Equipo: " << nuevo->entrenador << endl;
                cout << "Ciudad del Equipo: " << nuevo->ciudad << endl;
                cout << "Fecha de Registro del Equipo: " << nuevo->fechaRegistro << endl;
                cout << "Id del Equipo: " << nuevo->ID << endl;
            } else if (toupper(confirmacion) == 'N') {
                cout << "Registro de Equipo Cancelad\n";
            } else {
                cout << "Error: No se ingresó una opción correcta (S/N)\n";
                cout << "Registro de Equipo Cancelado\n";
            }
            Auxiliares::pausarPrograma();
        }

        void buscarEquipoPorID(SistemaDeportivo *MiSistema) {
            unsigned int ID = 0;
            Equipo *EquipoBuscado = nullptr;
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║        BUSQUEDA DE EQUIPOS POR ID         ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarDatos(ID, "Ingrese el ID: ", Validadores::IDvalido);

            EquipoBuscado = Logica::equipos::buscarEquipoPorID(MiSistema, ID);

            if (EquipoBuscado == nullptr) {
                cout << "El equipo de ID " << ID << " no fue encontrado\n";
            } else {
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║             EQUIPO ENCONTRADO             ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";
                cout << "-------------------------------------------------------------\n";
                cout << "  ID del Equipo:       " << EquipoBuscado->ID << "\n";
                cout << "  Nombre:              " << EquipoBuscado->nombre << "\n";
                cout << "  Entrenador:          " << EquipoBuscado->entrenador << "\n";
                cout << "  Ciudad Origen:       " << EquipoBuscado->ciudad << "\n";
                cout << "  Fecha de Registro:   " << EquipoBuscado->fechaRegistro << "\n";
                cout << "-------------------------------------------------------------\n";
                cout << "  Estadísticas en el Torneo:\n";
                cout << "    Puntos Totales:    " << EquipoBuscado->puntos << "\n";
                cout << "    Victorias:         " << EquipoBuscado->victorias << "\n";
                cout << "    Empates:           " << EquipoBuscado->empates << "\n";
                cout << "    Derrotas:          " << EquipoBuscado->derrotas << "\n";
                cout << "    Puntos a Favor:    " << EquipoBuscado->puntosAFavor << "\n";
                cout << "    Puntos en Contra:  " << EquipoBuscado->puntosEnContra << "\n";
            }
            cout << "-------------------------------------------------------------\n\n";
            Auxiliares::pausarPrograma();
        }

        void buscarEquiposPorSubCadena(SistemaDeportivo *Misistema) {
            Auxiliares::limpiarPantalla();
            int contEquiposEncotrados = 0;
            char subcadena[100];
            Equipo **arrayDePunterosAEquipos = nullptr;
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║      BUSQUEDA DE EQUIPOS POR NOMBRE       ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(subcadena, 100, "Escribe el nombre (o parte del nombre) del equipo que buscas: ", Validadores::Nombres);
            Auxiliares::waitfor(1000);
            cout << "Buscando..." << endl;

            // Llamamos a la funcion de busqueda
            arrayDePunterosAEquipos = Logica::equipos::buscarEquipoPorSubCadena(Misistema, subcadena, &contEquiposEncotrados);

            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            // Si no se enocontro ningun equipo
            if (arrayDePunterosAEquipos == nullptr || contEquiposEncotrados <= 0) {
                cout << "No se encontro ninguna coincidencia con: '" << subcadena << "'\n";
            } else {
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║          RESULTADOS ENCONTRADOS           ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";
                cout << "----------------------------------------------------------------------------\n";
                cout << "Se encontraron " << contEquiposEncotrados << " coincidencia(s):\n";
                cout << "----------------------------------------------------------------------------\n";

                for (size_t e = 0; e < contEquiposEncotrados; e++) {
                    cout << endl << e + 1 << ".\n";
                    cout << "   Nombre: " << (arrayDePunterosAEquipos[e])->nombre << endl;
                    cout << "   ID: " << (arrayDePunterosAEquipos[e])->ID << endl;
                }
                cout << "---------------------------------------------------------------------------\n";
            }

            // Liberamos el array

            if (arrayDePunterosAEquipos != nullptr) {
                delete[] arrayDePunterosAEquipos;
                arrayDePunterosAEquipos = nullptr;
            }

            Auxiliares::pausarPrograma();
        }

        void listarEquipos(SistemaDeportivo *MiSistema) {
            Auxiliares::limpiarPantalla();

            // Inicializamos las variables a utilizar
            unsigned int cantEquipos = 0;
            Equipo **listaDePtrAEquipos = nullptr;

            // llamamos a la funcion que nos devuelve la lista de punteros
            listaDePtrAEquipos = Logica::equipos::listarEquipos(MiSistema, &cantEquipos);

            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║            LISTA DE EQUIPOS               ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            if (listaDePtrAEquipos == nullptr || cantEquipos == 0) {
                cout << "No hay equipos registrados en el sistema actualmente.\n";
            } else {
                cout << "----------------------------------------------------------------------------\n";
                cout << " N° | ID   | Nombre               | Ciudad               | Puntos \n";
                cout << "----------------------------------------------------------------------------\n";

                for (size_t e = 0; e < cantEquipos; e++) {
                    // Accedemos a los datos usando -> ya que cada elemento es un Equipo*
                    cout << " " << e + 1 << "  | " << listaDePtrAEquipos[e]->ID << "    | " << listaDePtrAEquipos[e]->nombre << " | " << listaDePtrAEquipos[e]->ciudad << " | "
                         << listaDePtrAEquipos[e]->puntos << "\n";
                }
                cout << "----------------------------------------------------------------------------\n";
            }

            if (listaDePtrAEquipos != nullptr) {
                delete[] listaDePtrAEquipos;
                listaDePtrAEquipos = nullptr;
            }

            Auxiliares::pausarPrograma();
        }

        void mostrarTablaDePosiciones(SistemaDeportivo *MiSistema) {
            Auxiliares::limpiarPantalla();

            // Inicializamos las variables a utilizar
            unsigned int cantEquipos = 0;
            Equipo **TablaDePosiciones = nullptr;

            TablaDePosiciones = Logica::equipos::TablaDePosiciones(MiSistema, &cantEquipos);

            if (TablaDePosiciones == nullptr || cantEquipos == 0) {
                cout << "No hay Equipos Disponibles\n";
            } else {
                char nombreTorneo[100];
                strcpy(nombreTorneo, MiSistema->torneo.nombre);
                Auxiliares::toMayus(nombreTorneo);
                cout << "╔═════════════════════════════════════════════════════════════════════════════════════════════╗\n";
                cout << "║                             TABLA DE POSICIONES                                             ║\n";
                cout << "║               " << std::left << std::setw(83) << nombreTorneo << "║\n";
                cout << "╠════╦═══════════════════════════════════════════════╦═════╦═══╦═══╦═══╦════╦════╦════╣\n";
                cout << "║ #  ║ Equipo                                        ║ PTS ║ J ║ G ║ E ║ D  ║ GF ║ GC ║\n";
                cout << "╠════╬═══════════════════════════════════════════════╬═════╬═══╬═══╬═══╬════╬════╬════╣\n";

                for (size_t e = 0; e < cantEquipos; e++) {
                    cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(45) << TablaDePosiciones[e]->nombre
                         << " ║ " // <-- ¡Subió a 45 espacios fijos!
                         << std::right << std::setw(3) << TablaDePosiciones[e]->puntos << " ║ " << std::right << std::setw(1) << TablaDePosiciones[e]->jugados << " ║ "
                         << std::right << std::setw(1) << TablaDePosiciones[e]->victorias << " ║ " << std::right << std::setw(1) << TablaDePosiciones[e]->empates << " ║ "
                         << std::right << std::setw(2) << TablaDePosiciones[e]->derrotas << " ║ " << std::right << std::setw(2) << TablaDePosiciones[e]->puntosAFavor << " ║ "
                         << std::right << std::setw(2) << TablaDePosiciones[e]->puntosEnContra << " ║\n";
                }
                cout << "╚════╩═══════════════════════════════════════════════╩═════╩═══╩═══╩═══╩════╩════╩════╝\n";
                cout << "\nReferencia: PTS=Puntos  J=Jugados  G=Ganados  E=Empatados\n";
                cout << "            D=Derrotas  GF=puntos a Favor  GC=puntos en Contra\n\n";
            }

            // liberamos
            if (TablaDePosiciones != nullptr) {
                delete[] TablaDePosiciones;
                TablaDePosiciones = nullptr;
            }
            Auxiliares::pausarPrograma();
        }

        void actualizarEquipo(SistemaDeportivo *MiSistema) {
            Auxiliares::limpiarPantalla();

            // Datos Actualizables:
            // Nombre del Equipo
            // Nombre del Entrenador
            // Nombre de la Ciudad

            // Variables
            char nombreAux[100];
            char entrenadorAux[100];
            char ciudadAux[100];
            bool actualizado = false;
            unsigned int ID = 0;
            char confirmacion;

            // Pedimos el ID del equipo que desean actualizar
            Auxiliares::ingresarDatos(ID, "Ingresa el ID del equipo que desea actualizar: ", Validadores::IDvalido);

            // si no Existe el ID
            if (!Logica::equipos::existeID(MiSistema, ID)) {
                cout << "Error el ID '" << ID << "' no pertenece a ningún equipo registrado\n";
                return;
            }

            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(nombreAux, 100, "Ingrese el nuevo nombre del Equipo: ", Validadores::Nombres);
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(entrenadorAux, 100, "Ingrese el nuevo nombre del Entrenador del Equipo: ", Validadores::Nombres);
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(ciudadAux, 100, "Ingrese el nuevo de la Ciudad del Equipo: ", Validadores::Nombres);
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea aplicar estos cambios? (S/N): ");
            if (toupper(confirmacion) == 'S') {
                actualizado = Logica::equipos::actualizarEquipo(MiSistema, ID, nombreAux, entrenadorAux, ciudadAux);
                if (!actualizado) {
                    cout << "Se produjo un error a la hora de actualizar el equipo\n";
                } else {
                    cout << "------------------------------------------------------------------------------\n";
                    cout << "           Equipo Actualizado con Éxito\n";
                    cout << "------------------------------------------------------------------------------\n";
                    cout << "Nuevo Nombre del Equipo: " << nombreAux << endl;
                    cout << "Nuevo Nombre del Entrenador del Equipo: " << entrenadorAux << endl;
                    cout << "Nuevo Nombre de la Ciudad del Equipo: " << ciudadAux << endl;
                }
            } else if (toupper(confirmacion) == 'N') {
                cout << "Actualización de Datos Cancelada\n";
            } else {
                cout << "Error: No se ingresó una opción correcta (S/N)\n";
                cout << "Actualización de Datos Cancelada\n";
            }

            Auxiliares::pausarPrograma();
        }

        void eliminarEquipo(SistemaDeportivo *MiSistema) {
            Auxiliares::limpiarPantalla();

            bool eliminado = false;
            unsigned int ID = 0;
            char confirmacion;
            Equipo *EqAux = nullptr;

            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║             ELIMINAR EQUIPOS              ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Pedimos el ID del equipo que desean actualizar
            Auxiliares::ingresarDatos(ID, "Ingresa el ID del equipo que desea eliminar: ", Validadores::IDvalido);

            // si no Existe el ID
            if (!Logica::equipos::existeID(MiSistema, ID)) {
                cout << "Error el ID '" << ID << "' no pertenece a ningún equipo registrado\n";
                return;
            }
            Auxiliares::waitfor(1500);
            Auxiliares::limpiarPantalla();

            // buscamos el equipo mediante el ID ingresado
            EqAux = Logica::equipos::buscarEquipoPorID(MiSistema, ID);

            if (EqAux == nullptr) {
                cout << "Error, no se encontró el equipo que se desea eliminar";
                return;
            }

            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║             ELIMINAR EQUIPOS              ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            cout << "Equipo Seleccionado: \n\n";
            cout << "Nombre: " << EqAux->nombre << endl;
            cout << "Entrenador: " << EqAux->entrenador << endl;
            cout << "Ciudad: " << EqAux->ciudad << endl;

            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea eliminar este equipo? (S/N): ");
            if (toupper(confirmacion) == 'S') {
                eliminado = Logica::equipos::eliminarEquipo(MiSistema, ID);
                if (!eliminado) {
                    cout << "Se produjo un error a la hora de eliminar el equipo\n";
                    cout << "Por favor revise que el equipo a eliminar no tenga partidos Asociados\n";
                } else {
                    cout << "------------------------------------------------------------------------------\n";
                    cout << "           Equipo Eliminado con Éxito\n";
                    cout << "------------------------------------------------------------------------------\n";
                }
            } else if (toupper(confirmacion) == 'N') {
                cout << "Eliminacion de Equipo Cancelada\n";
            } else {
                cout << "Error: No se ingresó una opción correcta (S/N)\n";
                cout << "Eliminacion de Equipo Cancelada\n";
            }

            Auxiliares::pausarPrograma();
        }

    } // namespace equipos

    namespace Jugadores {

        void RegistrarJugador(SistemaDeportivo *MiSistema) {
            bool flagError = false;
            char nombreAux[100];
            char cedulaAux[20];
            unsigned int edadAux = 0;
            char fechaAux[11];
            Jugador *nuevo = nullptr;
            char confirmacion;
            unsigned short dorsal = 0;
            int opcion = 0;
            char posicionAux[25];
            unsigned int IDEquipoAux = 0;

            // Recolectamos el ID del equipo
            do {
                flagError = false;
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║          REGISTRAR NUVEVO JUGADOR         ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarDatos(IDEquipoAux, "Ingrese el ID del equipo al que pertenece el jugador: ", Validadores::IDvalido);

                /// Si el id no existe dentro de los equipos
                if (!Logica::equipos::existeID(MiSistema, IDEquipoAux)) {
                    cout << "Error el ID '" << IDEquipoAux << "' no pertenece a ningun equipo\n";
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
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║          REGISTRAR NUVEVO JUGADOR          ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(nombreAux, 100, "Ingrese el nombre del Jugador: ", Validadores::Nombres);

                // Validamos nombre duplicado
                if (Logica::jugadores::nombreDuplicado(MiSistema, nombreAux)) {
                    cout << "Error, el nombre '" << nombreAux << "' ya está en uso\n";
                    flagError = true;
                    Auxiliares::waitfor(3000);
                    continue;
                }
                Auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la Edad
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║          REGISTRAR NUVEVO JUGADOR         ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarDatos(edadAux, "Ingrese la edad del Jugador", Validadores::Edad);
            Auxiliares::waitfor(1500);

            // Recolectamos la cedula
            do {
                Auxiliares::limpiarPantalla();
                flagError = false;
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║          REGISTRAR NUVEVO JUGADOR         ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";
                Auxiliares::ingresarCadena(cedulaAux, 20, "Ingrese la cedula del jugador: ", Validadores::Cedulas);

                // Validamos nombre duplicado
                if (Logica::jugadores::CedulaRepetida(MiSistema, cedulaAux)) {
                    cout << "Error, la cedula '" << cedulaAux << "' ya le pertenece a otro jugador\n";
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
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";
                cout << " Deporte actual del Torneo: " << MiSistema->torneo.deporte << "\n\n";

                const char **matrizDeporteActual = nullptr;

                for (size_t i = 0; i < Validadores::totalDeportes; i++) {
                    // Recorremos el array de matrices y verificamos lo que hay en la posicion 0
                    if (std::strcmp(Validadores::MapaDeportes[i][0], MiSistema->torneo.deporte) == 0) {
                        matrizDeporteActual = Validadores::MapaDeportes[i];
                        break;
                    }
                }

                // Mostramos las posiciones disponibles de esa fila
                cout << " Seleccione la posición del jugador:\n";

                // Para saber el numero de posiciones del deporte
                int contadorPosiciones = 0;

                for (size_t j = 1; matrizDeporteActual[j] != nullptr; j++) {
                    cout << " " << j << ". " << matrizDeporteActual[j] << "\n";
                    contadorPosiciones++;
                }
                cout << "\n";

                Auxiliares::ingresarDatos(opcion, "Seleccione una opción: ");

                // Verificamos que esté en el rango de opciones
                if (opcion >= 1 && opcion <= contadorPosiciones) {
                    // si es correcta guardamos la posicion
                    std::strcpy(posicionAux, matrizDeporteActual[opcion]);
                } else {
                    cout << "Error: Opción inválida. Por favor, intente de nuevo.\n";
                    flagError = true;
                    Auxiliares::waitfor(2000);
                }
            } while (flagError);


            // Recolectamos la fecha de registro del jugador
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║          REGISTRAR NUVEVO JUGADOR         ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(fechaAux, 11, "Ingrese la fecha de Registro del Jugador: ", Validadores::fechaValidaRegistroDeJugadorOEquipo);
            Auxiliares::waitfor(2000);
            Auxiliares::limpiarPantalla();

            // Recolectamos el dorsal del Jugador
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║          REGISTRAR NUVEVO JUGADOR         ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarDatos(dorsal, "Ingrese el Dorsal del jugador: ", Validadores::Dorsal);
            Auxiliares::waitfor(2000);
            Auxiliares::limpiarPantalla();


            // Pedimos la confirmacion al usuario
            Auxiliares::limpiarPantalla();
            Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea registrar este jugador? (S/N): ");

            if (toupper(confirmacion) == 'S') {
                // agregamos el jugador
                nuevo = Logica::jugadores::agregarJugador(MiSistema, IDEquipoAux, nombreAux, cedulaAux, posicionAux, edadAux, dorsal, fechaAux);

                // Si no se agregó
                if (nuevo == nullptr) {
                    cout << "ERROR al registrar al jugador.\n";
                    Auxiliares::pausarPrograma();
                    return;
                }

                //
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║       JUGADOR REGISTRADO CON ÉXITO        ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";

                cout << " Torneo: " << MiSistema->torneo.nombre << endl;
                cout << " ID del Jugador: " << nuevo->ID << endl;
                cout << " Nombre del Jugador: " << nuevo->nombre << endl;
                cout << " Cédula: " << nuevo->cedula << endl;
                cout << " Edad: " << nuevo->edad << " años" << endl;
                cout << " Posición: " << nuevo->posicion << endl;
                cout << " Dorsal: " << nuevo->dorsal << endl;
                cout << " ID del Equipo asignado: " << nuevo->IDequipo << endl;
                cout << " Fecha de Registro: " << nuevo->fechaRegistro << endl;

            } else if (toupper(confirmacion) == 'N') {
                cout << "Registro de Jugador Cancelado.\n";
            } else {
                cout << "ERROR: Opción incorrecta (S/N).\nRegistro de Jugador Cancelado.\n";
            }

            Auxiliares::pausarPrograma();
        }

        void buscarJugadorID(SistemaDeportivo *MiSistema) {
            Auxiliares::limpiarPantalla();
            unsigned int ID = 0;
            Jugador *jugadorBuscado = nullptr;
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║       BUSQUEDA DE JUGADORES POR ID        ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::ingresarDatos(ID, "Ingrese el ID: ", Validadores::IDvalido);

            jugadorBuscado = Logica::jugadores::buscarJugadorPorID(MiSistema, ID);

            // si no encontro un jugador
            if (jugadorBuscado == nullptr) {
                cout << "Error no hay ningun jugador registrado con el ID '" << ID << "'\n";
            } else {
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║            JUGADOR ENCONTRADO             ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";

                cout << "-------------------------------------------------------------\n";
                cout << "  ID del Jugador:       " << jugadorBuscado->ID << "\n";
                cout << "  Nombre:               " << jugadorBuscado->nombre << "\n";
                cout << "  Cédula:               " << jugadorBuscado->cedula << "\n";
                cout << "  Edad:                 " << jugadorBuscado->edad << " años\n";
                cout << "  Posición:             " << jugadorBuscado->posicion << "\n";
                cout << "  Dorsal:               " << jugadorBuscado->dorsal << "\n";
                cout << "  ID Equipo Asignado:   " << jugadorBuscado->IDequipo << "\n";
                cout << "  Fecha de Registro:    " << jugadorBuscado->fechaRegistro << "\n";
                cout << "-------------------------------------------------------------\n";
                cout << "  Estadísticas en el Torneo:\n";
                cout << "    Puntos Anotados:    " << jugadorBuscado->puntosAnotados << "\n";
                cout << "-------------------------------------------------------------\n";
            }
            Auxiliares::pausarPrograma();
        }

        void buscarJugadorPorNombre(SistemaDeportivo *MiSistema) {
            Auxiliares::limpiarPantalla();
            char subcadena[100];
            int cantidadEncontrados = 0;

            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║      BÚSQUEDA DE JUGADORES POR NOMBRE     ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::ingresarCadena(subcadena, 100, "Escribe el nombre (o parte del nombre) del jugador que buscas: ", Validadores::Nombres);
            Auxiliares::waitfor(1000);
            cout << "Buscando..." << endl;

            // llamamos a la funcion buscar por nombre y almacenamos el resultado
            Jugador **listaDePunterosAJugadores = Logica::jugadores::buscarJugadoresPorNombre(MiSistema, subcadena, &cantidadEncontrados);

            // Si no encontró nada o no devolvió nada
            if (listaDePunterosAJugadores == nullptr || cantidadEncontrados <= 0) {
                cout << "\nNo se encontraron jugadores que coincidan con '" << subcadena << "'.\n";
            } else {
                cout << "\n       ╔═══════════════════════════════════════════╗\n";
                cout << "       ║          COINCIDENCIAS ENCONTRADAS        ║\n";
                cout << "       ╚═══════════════════════════════════════════╝\n\n";
                cout << " Se encontraron " << cantidadEncontrados << " jugador(es):\n";

                for (int e = 0; e < cantidadEncontrados; e++) {
                    cout << "-------------------------------------------------------------\n";
                    cout << "  ID: " << listaDePunterosAJugadores[e]->ID << " | Nombre: " << listaDePunterosAJugadores[e]->nombre << "\n";
                    cout << "  Cédula: " << listaDePunterosAJugadores[e]->cedula << " | Dorsal: [" << listaDePunterosAJugadores[e]->dorsal << "]\n";
                    cout << "  Edad: " << listaDePunterosAJugadores[e]->edad << " años | Posición: " << listaDePunterosAJugadores[e]->posicion << "\n";
                    cout << "  ID Equipo: " << listaDePunterosAJugadores[e]->IDequipo << " | Puntos Anotados: " << listaDePunterosAJugadores[e]->puntosAnotados << "\n";
                }
                cout << "-------------------------------------------------------------\n";
            }

            // Liberamos la memoria
            if (listaDePunterosAJugadores != nullptr) {
                delete[] listaDePunterosAJugadores;
                listaDePunterosAJugadores = nullptr;
            }

            cout << "\n";
            Auxiliares::pausarPrograma();
        }

        void mostrarJugadoresPorEquipo(SistemaDeportivo *MiSistema) {
            Auxiliares::limpiarPantalla();
            unsigned int IDEquipo = 0;
            unsigned int cantidadEncontrados = 0;

            // Pedimos el ID del equipo a consultar
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║      MOSTRAR JUGADORES POR EQUIPO         ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::ingresarDatos(IDEquipo, "Ingrese el ID del Equipo: ", Validadores::IDvalido);

            // Buscamos el equipo primero
            Equipo *equipoBuscado = Logica::equipos::buscarEquipoPorID(MiSistema, IDEquipo);

            // Si no encontramos un equipo con ese ID enviamos error
            if (equipoBuscado == nullptr) {
                cout << "\nError: El equipo con ID '" << IDEquipo << "' no existe.\n";
                Auxiliares::pausarPrograma();
                return;
            }

            Auxiliares::waitfor(1000);
            cout << "\nBuscando jugadores...\n\n";

            // obtenemos la lista de punteros
            Jugador **listaJugadores = Logica::jugadores::listarJugadoresPorEquipo(MiSistema, IDEquipo, &cantidadEncontrados);

            // Si no obtenemos nada
            if (listaJugadores == nullptr || cantidadEncontrados == 0) {
                cout << "El equipo '" << equipoBuscado->nombre << "' actualmente no tiene jugadores registrados.\n";
            } else {
                cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗\n";
                cout << "║ EQUIPO: " << std::left << std::setw(73) << equipoBuscado->nombre << " ║\n";
                cout << "║ ID DEL EQUIPO: " << std::left << std::setw(66) << equipoBuscado->ID << " ║\n";
                cout << "╠════╦══════════════════════════════════════════╦═══════════════╦═════╦═══════════╣\n";
                cout << "║ ID ║ Nombre                                   ║ Posición      ║ Edad║ Dorsal    ║\n";
                cout << "╠════╬══════════════════════════════════════════╬═══════════════╬═════╬═══════════╣\n";

                // Imprimimos los jugadores
                for (size_t e = 0; e < cantidadEncontrados; e++) {
                    cout << "║ " << std::right << std::setw(2) << listaJugadores[e]->ID << " ║ " << std::left << std::setw(40) << listaJugadores[e]->nombre << " ║ " << std::left
                         << std::setw(13) << listaJugadores[e]->posicion << " ║ " << std::right << std::setw(3) << listaJugadores[e]->edad << " ║ [" << std::right << std::setw(2)
                         << listaJugadores[e]->dorsal << "]       ║\n";
                }
                cout << "╚════╩══════════════════════════════════════════╩═══════════════╩═════╩═══════════╝\n";
                cout << " Total de jugadores en el equipo: " << cantidadEncontrados << "\n";
            }

            // liberamos
            if (listaJugadores != nullptr) {
                delete[] listaJugadores;
                listaJugadores = nullptr;
            }

            cout << "\n";
            Auxiliares::pausarPrograma();
        }

        void mostrarListaDeJugadores(SistemaDeportivo *MiSistema) {
            Auxiliares::limpiarPantalla();
            unsigned int cantidadEncontrados = 0;

            // Encabezado de la sección
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║        LISTADO GENERAL DE JUGADORES       ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";

            Auxiliares::waitfor(1000);
            cout << "Cargando todos los jugadores...\n\n";

            // Llamamos a tu función lógica (asumiendo que sigue el mismo patrón de firmas)
            Jugador **listaJugadores = Logica::jugadores::listarJugadores(MiSistema, &cantidadEncontrados);

            // Validamos si el sistema tiene jugadores cargados
            if (listaJugadores == nullptr || cantidadEncontrados == 0) {
                cout << "No existen jugadores registrados en el sistema actualmente.\n";
            } else {
                cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗\n";
                cout << "║ SPORT G&C TOURNAMENTS                                                             ║\n";
                cout << "║ TORNEO: " << std::left << std::setw(73) << MiSistema->torneo.nombre << " ║\n";
                cout << "║ LISTA DE JUGADORES REGISTRADOS                                                    ║\n";
                cout << "╠════╦══════════════════════╦══════════════════════╦═══════════════╦═════╦═══════════╣\n";
                cout << "║ ID ║ Nombre               ║ Equipo               ║ Posición      ║ Edad║ Dorsal    ║\n";
                cout << "╠════╬══════════════════════╬══════════════════════╬═══════════════╬═════╬═══════════╣\n";

                // Imprimimos cada jugador en el sistema
                for (size_t e = 0; e < cantidadEncontrados; e++) {
                    // Buscamos el equipo en cada iteracion
                    Equipo *equipoAux = Logica::equipos::buscarEquipoPorID(MiSistema, listaJugadores[e]->IDequipo);

                    cout << "║ " << std::right << std::setw(2) << listaJugadores[e]->ID << " ║ " << std::left << std::setw(20) << listaJugadores[e]->nombre << " ║ " << std::left
                         << std::setw(20) << equipoAux->nombre << " ║ " << std::left << std::setw(13) << listaJugadores[e]->posicion << " ║ " << std::right << std::setw(3)
                         << listaJugadores[e]->edad << " ║ [" << std::right << std::setw(2) << listaJugadores[e]->dorsal << "]       ║\n";
                }
                cout << "╚════╩══════════════════════╩══════════════════════╩═══════════════╩═════╩═══════════╝\n";
                cout << " Total de jugadores registrados en el sistema: " << cantidadEncontrados << "\n";
            }

            // Liberamos la memoria
            if (listaJugadores != nullptr) {
                delete[] listaJugadores;
                listaJugadores = nullptr;
            }

            cout << "\n";
            Auxiliares::pausarPrograma();
        }

        void menuBuscarJugador(SistemaDeportivo *MiSistema) {
            //
        }

        void menuListarJugadores(SistemaDeportivo *MiSistema) {
            //
        }

        void menuActualizarJugador(SistemaDeportivo *MiSistema);
        void menuEliminarJugador(SistemaDeportivo *MiSistema);


        // Muestra jugador con el nombre del equipo (no solo el ID)
        void mostrarJugador(Jugador *jugador, SistemaDeportivo *s);
        void mostrarListaJugadores(Jugador **jugadores, int cantidad, SistemaDeportivo *s);
    } // namespace Jugadores

    void mensajeSalida() {
        cout << "Saliendo...";
        Auxiliares::waitfor(3500);
    }

    void mensajeDefault() {
        Auxiliares::limpiarPantalla();
        cout << "ERROR has ingresado una opcion inválida. Intentalo nuevamnete.\n";
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
    SistemaDeportivo *PtrMiSistema = &MiSistema;
    Torneo *PtrMiTorneo = &MiTorneo;

    // Variables Estaticas
    int opcionMenu = -1;    // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionSubMenu = -1; // declaramos en -1 para evitar que coincida con una de las opciones
    bool loginMenu = false;
    char confirmacion;

    // Los arrays de deportes y posiciones se definen en el namespace Validadores
    // (no duplicar aquí para evitar inconsistencias)

    // Inicio del Programa
    Presentacion::menu::datosInicialesTorneo(PtrMiSistema);

    // Estructura del switch
    do {
        // inicializamos las variables para evitar conflictos
        loginMenu = false;
        opcionMenu = -1;
        opcionSubMenu = -1;
        Auxiliares::limpiarPantalla();

        // Presentamos el menu principal
        Presentacion::menu::Principal(PtrMiSistema);
        Auxiliares::ingresarDatos(opcionMenu, "Seleccione una opcion: ", Validadores::Positivo);

        switch (opcionMenu) {

            // Salida del Programa
            case 0:
                Auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea aplicar estos cambios? (S/N): ");
                if (toupper(confirmacion) == 'S') {
                    Presentacion::mensajeSalida();
                } else if (toupper(confirmacion) == 'N') {
                    cout << "Salida del Programa Cancelada\n"; // Forzamos la vuelta del bucle
                    opcionMenu = -1;
                    Auxiliares::waitfor(3000);
                    Auxiliares::limpiarPantalla();
                } else {
                    cout << "Error: No se ingresó una opción correcta (S/N)\n";
                    cout << "Salida del Programa Cancelada\n";
                    Auxiliares::waitfor(3000);
                    Auxiliares::limpiarPantalla();
                    // Forzamos la vuelta del bucle
                    opcionMenu = -1;
                }
                break;

            // Gestión de Equipos
            case 1:
                cout << "Opcion ingresada: " << opcionMenu << endl;
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
                cout << "Ingresando al apartado de Gestión de Equipos..." << endl;
                Auxiliares::waitfor(3000);
                Auxiliares::limpiarPantalla();
                Presentacion::menu::GestionDeEquipos();
                Auxiliares::ingresarDatos(opcionSubMenu, "Seleccione un opcion: ", Validadores::Positivo);
                break;

            // Gestión de Jugadores
            case 2:
                cout << "Opcion ingresada: " << opcionMenu << endl;
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
                cout << "Ingresando al apartado de Gestión de Jugadores..." << endl;
                Auxiliares::waitfor(3000);
                Auxiliares::limpiarPantalla();
                Presentacion::menu::GestionDeJugadores();
                Auxiliares::ingresarDatos(opcionSubMenu, "Seleccione un opcion: ", Validadores::Positivo);
                break;

            // Gestión de Partidos
            case 3:
                cout << "Opcion ingresada: " << opcionMenu << endl;
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
                cout << "Ingresando al apartado de Gestión de Partidos..." << endl;
                Auxiliares::waitfor(3000);
                Auxiliares::limpiarPantalla();
                Presentacion::menu::GestionDePartidos();
                Auxiliares::ingresarDatos(opcionSubMenu, "Seleccione un opcion: ", Validadores::Positivo);
                break;

            // Tabla de Posiciones
            case 4:
                cout << "Opcion ingresada: " << opcionMenu << endl;
                Auxiliares::waitfor(2000);
                Auxiliares::limpiarPantalla();
                cout << "Ingresando al apartado de Tabla de Posiciones..." << endl;
                Auxiliares::waitfor(3000);
                Auxiliares::limpiarPantalla();
                break;

            // Si no se selecciona una opcion correcta enviamos un mensaje de aviso
            default:
                Presentacion::mensajeDefault();
        }
        // El bucle se repite si el usuario no eligió la opcion de salir en el menu Principal
    } while (opcionMenu != 0);

    // liberar memoria y cierre del programa
    Logica::liberarSistema(PtrMiSistema);
}