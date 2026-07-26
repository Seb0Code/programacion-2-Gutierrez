#include "../../include/utils/formatos.hpp"
#include "../../include/utils/validaciones.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
#include <iostream>
#include <locale>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

// funcion que pausa el programa por un tiempo determinado
void Formatos::esperarTiempo(int tiempo) { std::this_thread::sleep_for(std::chrono::milliseconds(tiempo)); }

// funcion que limpia la consola
void Formatos::limpiarPantalla() {
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

// funcion que se encarga de pausar el programa hasta que el usuario ingrese enter por la consola
void Formatos::pausarPrograma() {
    std::cout << std::endl << std::endl;
    // Ignora cualquier carácter sobrante en el búfer hasta encontrar el salto de línea
    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Mostramos el mensaje
    std::cout << "\n Presione Enter para continuar...";

    // Espera a que el usuario presione la tecla Enter
    std::cin.get();
}

// funcion que se encarga de configurar el idioma para aceptar caracteres del lenguaje español
void Formatos::configurarIdioma() {
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

// esta funcion transforma el texto a mayuscula
char *Formatos::convertirCadenaAMayus_pchar(char *texto) {
    if (GestorDeValidaciones::validarCadenaVacia(texto)) {
        return nullptr;
    }

    int longitud = std::strlen(texto);
    std::transform(texto, texto + longitud, texto, ::toupper);
    return texto;
}

std::string Formatos::convertirAMayus_string(std::string &texto) {
    // Validamos texto vacío
    if (texto.empty()) {
        return texto;
    }

    std::transform(texto.begin(), texto.end(), texto.begin(), ::toupper);
    return texto;
}

// esta funcion transforma el texto a minuscula
char *Formatos::convertirCadenaAMinus_pchar(char *texto) {
    if (GestorDeValidaciones::validarCadenaVacia(texto)) {
        return nullptr;
    }

    int longitud = std::strlen(texto);
    std::transform(texto, texto + longitud, texto, ::tolower);
    return texto;
}

std::string Formatos::convertirAMinus_string(std::string &texto) {
    // Validamos texto vacío
    if (texto.empty()) {
        return texto;
    }

    std::transform(texto.begin(), texto.end(), texto.begin(), ::tolower);
    return texto;
}