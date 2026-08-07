#include "../include/models/fecha.hpp"
#include "../include/utils/formatos.hpp"
#include "../include/utils/validaciones.hpp"
#include <cassert>
#include <iostream>

// Colores ANSI para que la terminal nos muestre los resultados de forma visual
#define COLOR_VERDE "\033[32m"
#define COLOR_ROJO "\033[31m"
#define COLOR_RESET "\033[0m"

// Macro personalizada para evaluar las pruebas
#define PROBAR(expresion, mensaje)                                                                                                                                                 \
    if (expresion) {                                                                                                                                                               \
        std::cout << COLOR_VERDE << "  [PASÓ] " << COLOR_RESET << mensaje << "\n";                                                                                                 \
    } else {                                                                                                                                                                       \
        std::cout << COLOR_ROJO << "  [FALLÓ] " << COLOR_RESET << mensaje << "\n";                                                                                                 \
        fallos++;                                                                                                                                                                  \
    }

int main() {
    int fallos = 0;
    std::cout << "==================================================\n";
    std::cout << "EJECUTANDO BATERÍA DE PRUEBAS UNITARIAS (TESTS)\n";
    std::cout << "==================================================\n\n";

    // -------------------------------------------------------------------------
    // GRUPO 1: Pruebas de Sobrecarga de Textos
    // -------------------------------------------------------------------------
    std::cout << "[1] Evaluando GestorDeValidaciones (Textos):\n";

    PROBAR(GestorDeValidaciones::validarCadenaVacia(""), "Detectar const char* vacio");
    PROBAR(GestorDeValidaciones::validarCadenaVacia(std::string("")), "Detectar std::string vacio");
    PROBAR(GestorDeValidaciones::validarEsCadenaVaciaOSoloEspacios("   "), "Detectar cadena con solo espacios");
    PROBAR(GestorDeValidaciones::validarSoloNumeros("1234567"), "Validar cadena numerica correcta");
    PROBAR(!GestorDeValidaciones::validarSoloNumeros("123a45"), "Rechazar numeros con letras intercaladas");
    PROBAR(GestorDeValidaciones::validarSoloLetras("Juan Perez"), "Validar letras con espacios permitidos");

    std::cout << "\n";

    // -------------------------------------------------------------------------
    // GRUPO 2: Pruebas de Parseo de Fechas
    // -------------------------------------------------------------------------
    std::cout << "[2] Evaluando Formatos (Parseo de Fechas YYYY-MM-DD):\n";

    Fecha f1 = Formatos::convertirTextoAFecha("2026-07-26");
    PROBAR(f1.anio == 2026 && f1.mes == 7 && f1.dia == 26, "Parsear fecha estricta correcta");

    Fecha fErr = Formatos::convertirTextoAFecha("2026/07/26");
    PROBAR(fErr.anio == 0, "Rechazar delimitadores incorrectos (barras)");

    std::cout << "\n";

    // -------------------------------------------------------------------------
    // GRUPO 3: Pruebas de Lógica de Calendario y Operadores
    // -------------------------------------------------------------------------
    std::cout << "[3] Evaluando Validaciones Logicas y Operadores de Fecha:\n";

    PROBAR(GestorDeValidaciones::validarEsBisiesto(2024), "Confirmar año bisiesto correcto (2024)");
    PROBAR(!GestorDeValidaciones::validarEsBisiesto(2025), "Confirmar año no bisiesto correcto (2025)");

    PROBAR(GestorDeValidaciones::validarFecha("2024-02-29"), "Permitir 29 de febrero en año bisiesto");
    PROBAR(!GestorDeValidaciones::validarFecha("2025-02-29"), "Rechazar 29 de febrero en año normal");

    // Probar operador sobrecargado '>' y '<' (std::tie)
    Fecha inicio = {2026, 7, 25};
    Fecha fin = {2026, 7, 26};
    PROBAR(fin > inicio, "Operador Mayor Que: 2026-07-26 es mayor que 2026-07-25");
    PROBAR(inicio < fin, "Operador Menor Que: Verificar orden inverso jerarquico");

    std::cout << "\n==================================================\n";
    if (fallos == 0) {
        std::cout << COLOR_VERDE << "¡TODAS LAS PRUEBAS PASARON EXITOSAMENTE! (0 fallos)\n" << COLOR_RESET;
    } else {
        std::cout << COLOR_ROJO << "ALERTA: Se detectaron " << fallos << " errores en el codigo.\n" << COLOR_RESET;
    }
    std::cout << "==================================================\n";

    return fallos;
}
