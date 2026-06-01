// Sport G&C Tournaments (SISTEMA DE GESTION DE TORNEOS DEPORTIVOS)

// ============================================//
//   1. INCLUDES, DIRECTIVAS Y LOS USING       //
// ============================================//
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstring> //para el uso de strcpy
#include <iostream>
#include <limits>
#include <locale>
// #include <sstream> // para el validador de fechas dinamico //! no lo implementé
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

using std::cin;
using std::cout;
using std::endl;
using std::string;

// ============================================//
//   2. STRUCTS                                //
// ============================================//

struct Jugador {
    unsigned int ID;                 // Id del jugador
    char nombre[100];                // nombre completo del jugador
    unsigned int edad;               // edad del jugador
    char cedula[15];                 // cedula del jugador
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
    unsigned int puntos = 0;         // puntos del equipo
    unsigned int victorias = 0;      // victorias conseguidas
    unsigned int derrotas = 0;       // derrotas conseguidas
    unsigned int empates = 0;        // empates conseguidos
    unsigned int puntosAFavor = 0;   // capacidad de puntos a favor
    unsigned int puntosEnContra = 0; // capacidad de puntos en contra
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
    int numPartidosActuales;
    int capacidadPartidos;

    Equipo *Equipos;
    int numEquiposActuales;
    int capacidadEquipos;

    Jugador *Jugadores;
    int numJugadoresActuales;
    int capacidadJugadores;

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
        int longitud = std::strlen(texto);
        std::transform(texto, texto + longitud, texto, ::toupper);
        return texto;
    }

    // esta funcion transforma el texto a minuscula
    char *toMinus(char *texto) {
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
                cout << "Error Tipo de Dato Incorrecto\n";
                waitfor(3000);
            } else {
                // si el puntero no contiene la direccion de ninguna direccion se omite este bloque
                if (ptrValidador != nullptr) {
                    flag = !ptrValidador(variable, mensajeError); // si no es valido se activa la bandera
                    cout << mensajeError << endl;
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

            // 3. Verificamos si la lectura falló
            if (cin.fail()) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                flag = true;
                cout << "ERROR: Excediste el limite de caracteres permitido (" << tamañoMaximo - 1 << "). Intente de nuevo.\n";
                Auxiliares::waitfor(3000);
                continue; // Saltamos directo a la siguiente iteración ya que no es necesario el validador
            }

            // Si la lectura no tuvo errores, pasamos el texto por el validador
            if (ptrValidador != nullptr) {
                if (!ptrValidador(texto, mensajeError)) {
                    flag = true; // Si el validador retorna false, la bandera se activa para repetir
                    cout << mensajeError << endl;
                    waitfor(3500);
                }
            }
        } while (flag);
    }

    // funcion que se encarga de pausar el programa hasta que el usuario ingrese enter por la consola
    void pausarPrograma() {
        // Limpia el búfer de entrada por si  quedaron caracteres (como '\n')
        std::cin.clear(); // Restablece los flags de error por si std::cin estaba en estado de fallo

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
    // =======================================================================================//
    // Declaracion de arrays que serán usados para algunas validaciones                       //
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

    // ahora mediante ptrs dobles creamos puntero doble que apunta a un array de punteros
    const char **MapaDeportes[] = {MatrizFutbol, MatrizBasket, MatrizVoleibol, MatrizBeisbol, MatrizSoftbol, MatrizHandball, MatrizHockey, MatrizRugby, MatrizTenis};

    const size_t totalDeportes = sizeof(MapaDeportes) / sizeof(MapaDeportes[0]);

    char deporteActual[50] = "";

    // Función que se llamará una sola vez al crear el torneo
    void setDeporteActual(const char *deporte) {
        std::strcpy(deporteActual, deporte);
        // La aseguramos en mayúsculas de una vez
        Auxiliares::toMayus(deporteActual);
    }

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

        bool esBisiesto(int anio) {
            // 1. Un año no puede ser bisiesto si es 0 o negativo
            if (anio <= 0) {
                return false;
            }
            // 2. Aplicar la regla de divisibilidad del año bisiesto
            return (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
        }
    } // namespace

    // ====================================================================================//
    //  Validaciones principales                                                           //
    // ====================================================================================//

    bool Edad(const short edad, char *mensajeError) {
        // la edad no puede ser negativa ni igual a 0, tampoco puede ser mayor a 120
        if (edad < 14 || edad > 50) {
            // asignamos la siguiente cadena de texto a el array de char
            std::strcpy(mensajeError, "La edad debe esta entre un rango de 14-50");
            return false;
        }
        return true;
    }

    bool Dorsal(const short dorsal, char *mensajeError) {
        if (dorsal < 1 || dorsal > 99) {
            // std:strcpy copia el mensaje del segundo parametro dentro de un const char*
            std::strcpy(mensajeError, "El dorsal debe esta entre un rango de 1-99");
            return false;
        }
        return true;
    }

    bool Fechas(const char *fecha, char *mensajeError) {
        short dia = 0, mes = 0, año = 0;
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
        año = (fecha[0] - '0') * 1000 + (fecha[1] - '0') * 100 + (fecha[2] - '0') * 10 + (fecha[3] - '0');
        mes = (fecha[5] - '0') * 10 + (fecha[6] - '0');
        dia = (fecha[8] - '0') * 10 + (fecha[9] - '0');

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

    bool Cedulas(const char *cedula, char *mensajeError) {
        int tamañoMin = 7, tamañomax = 10;

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

    // nos permite dividir el tamaño de un array de punteros basicamente, entre el tamaño de un puntero y hallar el tamaño de datos de esta forma
    const size_t numDeportes = sizeof(Deportes) / sizeof(Deportes[0]);

    // Función para validar si un deporte está en la lista
    bool existeDeporte(const char *deporte, char *mensajeError) {
        if (charVacio(deporte)) {
            strcpy(mensajeError, "El Deporte ingresado no puede estar Vacio");
            return false;
        }

        // Creamos una copia porque no se puede modificar la original y la convertimos a mayuscula
        char copiaDeporte[50];
        strcpy(copiaDeporte, deporte);
        Auxiliares::toMayus(copiaDeporte);

        for (size_t e = 0; e < numDeportes; e++) {
            // comparamos letra a letra cada literal con el deporte ingresado
            if (strcmp(copiaDeporte, Deportes[e]) == 0) {
                return true;
            }
        }
        strcpy(mensajeError, "El deporte ingresado no esta en la lista de deportes validos.");
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

        // creamos una copia de la posicion ingresada ya que no podemos modificar la original (y la colocamos en mayus)
        char copiaPosicion[50];
        std::strcpy(copiaPosicion, posicion);
        Auxiliares::toMayus(copiaPosicion);

        // Buscamos la fila de nuestro deporte actual en la matriz
        for (size_t i = 0; i < totalDeportes; i++) {
            // El elemento [0] de cada fila es el nombre del deporte
            if (std::strcmp(deporteActual, MapaDeportes[i][0]) == 0) {

                // Si encontramos el deporte recorremos lo recorremos
                size_t j = 1; // Empezamos en 1 para saltarnos el nombre del deporte
                while (MapaDeportes[i][j] != nullptr) {
                    if (std::strcmp(copiaPosicion, MapaDeportes[i][j]) == 0) {
                        return true;
                    }
                    j++;
                }
                // Si no lo encontró
                std::strcpy(mensajeError, "Posición inválida para el deporte de este torneo.");
                return false;
            }
        }
        return false;
    }

    /*bool tipoTorneo (const char* tipotorneo, char* mensajeError) {
        if (charVacio(tipotorneo)) {
            std::strcpy(mensajeError, "El tipo de Torneo no puede estar vacio, ")
            return false;
        }
    }*/

} // namespace Validadores

// ============================================//
//   5. CAPA DE LOGICA                         //
// ============================================//

namespace Logica {

    void inicializarSistemaDeportivo(SistemaDeportivo *MiSistema, Torneo torneo) {
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
            std::strcpy(torneoAux.formato, "ELIIMINATORIA");
        }
    }

    namespace equipos {
        //
    }

    namespace partidos {
        //
    }

    namespace jugadores {
        //
    }

    namespace redimensionar {
        //
    }

    namespace agregar {
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
            bool error = 0;

            // Aqui se recopilan los datos iniciales del torneo
            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(torneoAux.nombre, 100, "Nombre del Torneo: ", Validadores::Nombres);

            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(torneoAux.deporte, 50, "Deporte del Torneo: ", Validadores::existeDeporte);
            // Le indicamos cual va a ser el deporte a nuestro namespace de validadores
            Validadores::setDeporteActual(torneoAux.deporte);

            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            cout << "1. Formato de Grupos (Todos contra todos)\n";
            cout << "2. Formato de Eliminatoria Directa\n";
            cout << "--------------------------------------------------\n";
            do {
                error = false;
                Auxiliares::ingresarDatos(opcionFormato, "Seleccione el formato (1 o 2): ");
                if (opcionFormato != 1 && opcionFormato != 2) {
                    cout << "Opcion invalida. Intente de nuevo.\n";
                    error = true;
                }
            } while (error);
            // desde la logica definimos el tipo de torneo en base a la opcion ingresada
            Logica::definirFormato(torneoAux, opcionFormato);
            cout << "\nFormato seleccionado: " << torneoAux.formato << endl;
            Auxiliares::waitfor(1000);

            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(torneoAux.fechaInicio, 11, "Fecha De Inicio del Torneo: ", Validadores::Fechas);

            Auxiliares::limpiarPantalla();
            cout << "\n       ╔═══════════════════════════════════════════╗\n";
            cout << "       ║ DATOS INICIALES DEL TORNEO                ║\n";
            cout << "       ╚═══════════════════════════════════════════╝\n\n";
            Auxiliares::ingresarCadena(torneoAux.fechaFin, 11, "Fecha de Finalización del Torneo: ", Validadores::Fechas);

            // enviamos los datos
            Logica::inicializarSistemaDeportivo(MiSistema, torneoAux);
        }


    } // namespace menu
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


    // Inicio del Programa
    Presentacion::menu::datosInicialesTorneo(PtrMiSistema);

    cout << "\nDatos del torneo cargados correctamente.\n";
    Auxiliares::pausarPrograma();

    // liberar memoria y cierre del programa
    Logica::liberarSistema(PtrMiSistema);
}