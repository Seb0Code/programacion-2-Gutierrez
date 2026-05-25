// Sport G&C Tournaments (SISTEMA DE GESTION DE TORNEOS DEPORTIVOS)
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <locale>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

using std::cin;
using std::cout;
using std::endl;
using std::string;

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
    string toMayus(string texto) {
        std::transform(texto.begin(), texto.end(), texto.begin(), ::toupper);
        return texto;
    }

    // esta funcion transforma el texto a minuscula
    string toMinus(string texto) {
        std::transform(texto.begin(), texto.end(), texto.begin(), ::tolower);
        return texto;
    }

    // funcion para ingresar cualquier tipo de dato
    template <typename Tipo1> //
    void ingresarDatos(Tipo1 &texto) {
        // bandera que se activa si el usuario ingresa un tipo de dato incorrecto
        bool flag = false;
        do {
            limpiarPantalla();
            flag = false;
            cout << "Ingresa aqui: ";
            cin >> texto;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                flag = true; // activamos la bandera
                cout << "Error Tipo de Dato Incorrecto\n";
                waitfor(3000);
            } else {
                // Si la lectura fue exitosa, limpiamos el enter residual
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        } while (flag);
    }

    // funcion para ingresar cadenas de texto
    void ingresarCadena(string &cadena) {
        // bandera que se activa si el usuario ingresa un tipo de dato incorrecto
        bool flag = false;
        do {
            limpiarPantalla();
            flag = false;
            cout << "Ingresa aqui: ";
            // cin >> std::ws extrae cualquier ENTER basura que haya quedado en el búfer antes de leer la cadena.
            getline(cin >> std::ws, cadena);
            if (cin.fail()) {
                cin.clear();
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                flag = true; // activamos la bandera
                cout << "Error Tipo de Dato Incorrecto\n";
                waitfor(3000);
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

struct Jugador {
    unsigned int ID;                 // Id del jugador
    char nombre[100];                // nombre completo del jugador
    unsigned int edad;               // edad del jugador
    char cedula[15];                 // cedula del jugador
    unsigned int IDequipo;           // Id del equipo al que pertenece
    unsigned int dorsal;             // dorsal del jugador
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
    unsigned int puntosAFavor = 0;   // Total de puntos a favor
    unsigned int puntosEnContra = 0; // Total de puntos en contra
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

    Partido *arrayPartidos;
    int numPartidosActuales;
    int totalPartidos;

    Equipo *arrayEquipos;
    int numEquiposActuales;
    int totalEquipos;

    Jugador *arrayJugadores;
    int numJugadoresActuales;
    int totalJugadores;

    int siguienteIdEquipo;
    int siguienteIdJugador;
    int siguienteIdPartido;
};

namespace logica {
    template <typename T> //
    T crearArray(int tamanio) {
        T *nuevoArray = new T[tamanio];
        return nuevoArray;
    }
} // namespace logica

int main() {
    // Llamamos a la función de configuración de Idioma al inicio
    Auxiliares::configurarIdioma();
}