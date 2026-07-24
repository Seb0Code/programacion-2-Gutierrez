#include "../../include/utils/auxiliares.hpp"
#include "../../include/utils/validaciones.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <locale>

#ifdef _WIN32
#include <windows.h>
#endif

// funcion que se encarga de configurar el idioma para aceptar caracteres del lenguaje español
void Auxiliares::configurarIdioma() {
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
char *Auxiliares::convertirCadenaAMayus(char *texto) {
    if (GestorDeValidaciones::validarCadenaVacia(texto)) {
        return nullptr;
    }

    int longitud = std::strlen(texto);
    std::transform(texto, texto + longitud, texto, ::toupper);
    return texto;
}

// esta funcion transforma el texto a minuscula
char *Auxiliares::convertirCadenaAMinus(char *texto) {
    if (GestorDeValidaciones::validarCadenaVacia(texto)) {
        return nullptr;
    }

    int longitud = std::strlen(texto);
    std::transform(texto, texto + longitud, texto, ::tolower);
    return texto;
}