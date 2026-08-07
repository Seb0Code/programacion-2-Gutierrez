#ifndef FUNCIONES_IN_OUT_HPP
#define FUNCIONES_IN_OUT_HPP

#include "../utils/formatos.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

namespace presentacion {
    namespace funcionesInOut {

        // Funciones auxiliares de cancelación
        std::string normalizarEntradaAMayus(const std::string &valor);

        bool esCancelacion(const std::string &valor);

        /**
         * @brief Ingresa cualquier tipo de dato de forma segura y maneja la cancelación.
         */
        template <typename var1> bool ingresarDatos(var1 &variable, const char *mensaje, bool *cancelado = nullptr) {
            std::string entrada;

            while (true) {
                if (cancelado != nullptr) {
                    *cancelado = false;
                }

                std::cout << mensaje << std::flush;
                std::cin.clear();
                std::getline(std::cin, entrada);

                if (esCancelacion(entrada)) {
                    if (cancelado != nullptr) {
                        *cancelado = true;
                    }
                    return false;
                }

                std::stringstream ss(entrada);
                if (!(ss >> variable) || (ss >> std::ws && !ss.eof())) {
                    std::cout << "Error: Tipo de dato incorrecto. Intente de nuevo.\n\n";
                    Formatos::esperarTiempo(3000);
                    continue;
                }

                return true;
            }
        }

        template <typename var1, typename Validador> bool ingresarDatos(var1 &variable, const char *mensaje, bool *cancelado, Validador validador) {
            std::string entrada;

            while (true) {
                if (cancelado != nullptr) {
                    *cancelado = false;
                }

                std::cout << mensaje << std::flush;
                std::cin.clear();
                std::getline(std::cin, entrada);

                if (esCancelacion(entrada)) {
                    if (cancelado != nullptr) {
                        *cancelado = true;
                    }
                    return false;
                }

                std::stringstream ss(entrada);
                if (!(ss >> variable) || (ss >> std::ws && !ss.eof())) {
                    std::cout << "Error: Tipo de dato incorrecto. Intente de nuevo.\n\n";
                    Formatos::esperarTiempo(3000);
                    continue;
                }

                if (!validador(variable)) {
                    std::cout << "Error: El valor ingresado no cumple con la validación solicitada. Intente de nuevo.\n\n";
                    Formatos::esperarTiempo(3000);
                    continue;
                }

                return true;
            }
        }

        /**
         * @brief Ingresa cadenas de texto (char array) con límite de tamaño de forma segura.
         */
        inline bool ingresarCadena(char *texto, size_t tamañoMaximo, const char *mensaje, bool *cancelado = nullptr) {
            std::string entrada;

            while (true) {
                if (cancelado != nullptr) {
                    *cancelado = false;
                }

                std::cout << mensaje << std::flush;
                std::cin.clear();
                std::getline(std::cin, entrada);

                if (esCancelacion(entrada)) {
                    if (cancelado != nullptr) {
                        *cancelado = true;
                    }
                    return false;
                }

                if (entrada.size() >= tamañoMaximo) {
                    std::cout << "Error: El texto ingresado supera el tamaño permitido. Intente de nuevo.\n\n";
                    Formatos::esperarTiempo(3000);
                    continue;
                }

                Formatos::copiarCadena(texto, entrada.c_str(), tamañoMaximo);
                return true;
            }
        }

        template <typename Validador> bool ingresarCadena(char *texto, size_t tamañoMaximo, const char *mensaje, bool *cancelado, Validador validador) {
            std::string entrada;

            while (true) {
                if (cancelado != nullptr) {
                    *cancelado = false;
                }

                std::cout << mensaje << std::flush;
                std::cin.clear();
                std::getline(std::cin, entrada);

                if (esCancelacion(entrada)) {
                    if (cancelado != nullptr) {
                        *cancelado = true;
                    }
                    return false;
                }

                if (entrada.size() >= tamañoMaximo) {
                    std::cout << "Error: El texto ingresado supera el tamaño permitido. Intente de nuevo.\n\n";
                    Formatos::esperarTiempo(3000);
                    continue;
                }

                if (!validador(entrada.c_str())) {
                    std::cout << "Error: El texto ingresado no cumple con la validación solicitada. Intente de nuevo.\n\n";
                    Formatos::esperarTiempo(3000);
                    continue;
                }

                Formatos::copiarCadena(texto, entrada.c_str(), tamañoMaximo);
                return true;
            }
        }
    } // namespace funcionesInOut
} // namespace presentacion

#endif