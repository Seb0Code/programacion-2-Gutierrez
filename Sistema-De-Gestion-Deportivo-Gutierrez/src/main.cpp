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

struct Jugador {
    //
};

struct Arbitro {
    //
};

struct Equipo {
    //
};

struct Partido {
    //
};

struct Torneo {
    //
};

struct SistemaDeportivo {
    //
};

int main() {
    // Llamamos a la función de configuración de Idioma al inicio
    configurarIdioma();
}