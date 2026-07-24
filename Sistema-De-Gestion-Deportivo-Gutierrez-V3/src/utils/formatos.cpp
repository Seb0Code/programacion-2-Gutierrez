#include "../../include/utils/formatos.hpp"
#include <chrono>
#include <iostream>
#include <thread>

// funcion que pausa el programa por un tiempo determinado
void Formatos::esperarTiempo(int tiempo) { std::this_thread::sleep_for(std::chrono::milliseconds(tiempo)); }

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

// funcion que se encarga de pausar el programa hasta que el usuario ingrese enter por la consola
void pausarPrograma() {
    std::cout << std::endl << std::endl;
    // Ignora cualquier carácter sobrante en el búfer hasta encontrar el salto de línea
    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // Mostramos el mensaje
    std::cout << "\nPresione Enter para continuar...";

    // Espera a que el usuario presione la tecla Enter
    std::cin.get();
}