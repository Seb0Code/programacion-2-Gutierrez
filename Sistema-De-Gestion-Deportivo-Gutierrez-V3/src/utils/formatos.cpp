#include "../../include/utils/formatos.hpp"
#include "../../include/utils/validaciones.hpp"
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
#include <iostream>
#include <limits>
#include <locale>
#include <sstream>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif

// Funcion que pausa el programa por un tiempo determinado
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
char *Formatos::convertirTextoAMayus(char *texto) {

    // Verificamos que la cadena no esté vacía
    if (GestorDeValidaciones::validarCadenaVacia(texto)) {
        return nullptr;
    }

    // Calculamos la longitud del archivo y transformamos a mayus
    int longitud = std::strlen(texto);
    std::transform(texto, texto + longitud, texto, ::toupper);
    return texto;
}

std::string Formatos::convertirTextoAMayus(std::string &texto) {
    // Validamos texto vacío
    if (texto.empty()) {
        return texto;
    }

    // Calculamos la longitud del archivo y transformamos a mayus
    std::transform(texto.begin(), texto.end(), texto.begin(), ::toupper);
    return texto;
}

// esta funcion transforma el texto a minuscula
char *Formatos::convertirTextoAMinus(char *texto) {
    if (GestorDeValidaciones::validarCadenaVacia(texto)) {
        return nullptr;
    }

    // Calculamos la longitud del archivo y transformamos a minus
    int longitud = std::strlen(texto);
    std::transform(texto, texto + longitud, texto, ::tolower);
    return texto;
}

std::string Formatos::convertirTextoAMinus(std::string &texto) {
    // Validamos texto vacío
    if (texto.empty()) {
        return texto;
    }

    // Calculamos la longitud del archivo y transformamos a minus
    std::transform(texto.begin(), texto.end(), texto.begin(), ::tolower);
    return texto;
}

Fecha Formatos::convertirTextoAFecha(const char *fecha) {

    // Verificamos que la cadena no esté vacía
    if (GestorDeValidaciones::validarCadenaVacia(fecha)) {
        return {0, 0, 0};
    }

    Fecha f;

    // Se declaran vacios
    char guion1 = '\0';
    char guion2 = '\0';

    std::stringstream ss(fecha);

    // Vamos separando cada cosa
    if (ss >> f.anio >> guion1 >> f.mes >> guion2 >> f.dia) {
        if (guion1 != '-' || guion2 != '-') {
            return {0, 0, 0}; // Error
        }
    } else {
        return {0, 0, 0}; // si no se pudo separar las fechas
    }

    return f;
}