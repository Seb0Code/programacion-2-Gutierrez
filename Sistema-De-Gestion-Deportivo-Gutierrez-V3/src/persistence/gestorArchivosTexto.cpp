/**
 * @file GestorArchivosTexto.cpp
 * @brief Implementación de la clase GestorArchivosTexto para la manipulación de archivos de texto y CSV.
 */

#include "../../include/persistence/GestorArchivosTexto.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

// =======================================================================================
// Implementación de Métodos Auxiliares Privados de Apertura de Flujos
// =======================================================================================

bool GestorArchivosTexto::abrirArchivoParaEscrituraYLectura(const std::string &rutaArchivo, std::fstream &archivo) {
    archivo.open(rutaArchivo, std::ios::in | std::ios::out);
    if (!verificarOperacion(archivo)) {
        // Si no existe o falló al abrirse con in|out, intentamos crearlo con out|in|app o trunc
        archivo.clear();
        archivo.open(rutaArchivo, std::ios::out | std::ios::in | std::ios::app);
    }
    return verificarOperacion(archivo);
}

bool GestorArchivosTexto::abrirArchivoParaEscritura(const std::string &rutaArchivo, std::ofstream &archivo, bool modoAppend) {
    if (modoAppend) {
        archivo.open(rutaArchivo, std::ios::out | std::ios::app);
    } else {
        archivo.open(rutaArchivo, std::ios::out | std::ios::trunc);
    }
    return verificarOperacion(archivo);
}

bool GestorArchivosTexto::abrirArchivoParaLectura(const std::string &rutaArchivo, std::ifstream &archivo) {
    archivo.open(rutaArchivo, std::ios::in);
    return verificarOperacion(archivo);
}

// =======================================================================================
// Implementación de Operaciones sobre Archivos de Texto (.txt, .log)
// =======================================================================================

std::vector<std::string> GestorArchivosTexto::leerLineas(const std::string &rutaArchivo) {
    std::vector<std::string> lineas;
    std::ifstream archivo;

    if (!abrirArchivoParaLectura(rutaArchivo, archivo)) {
        return lineas; // Retorna vector vacío si el archivo no existe o no se pudo abrir
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        lineas.push_back(linea);
    }

    archivo.close();
    return lineas;
}

bool GestorArchivosTexto::escribirLinea(const std::string &rutaArchivo, const std::string &linea, const bool modoAppend) {
    std::ofstream archivo;
    if (!abrirArchivoParaEscritura(rutaArchivo, archivo, modoAppend)) {
        return false;
    }

    archivo << linea << "\n";

    bool operacionExitosa = verificarOperacion(archivo);
    archivo.close();
    return operacionExitosa;
}

bool GestorArchivosTexto::escribirLineas(const std::string &rutaArchivo, const std::vector<std::string> &lineas, bool modoAppend) {
    std::ofstream archivo;
    if (!abrirArchivoParaEscritura(rutaArchivo, archivo, modoAppend)) {
        return false;
    }

    for (const std::string &linea : lineas) {
        archivo << linea << "\n";
        if (!verificarOperacion(archivo)) {
            archivo.close();
            return false;
        }
    }

    archivo.close();
    return true;
}

// =======================================================================================
// Implementación de Operaciones sobre Archivos CSV (.csv)
// =======================================================================================

// Una matriz hecha con vectores
std::vector<std::vector<std::string>> GestorArchivosTexto::leerCSV(const std::string &rutaArchivo, char delimitador) {
    std::vector<std::vector<std::string>> matriz;
    std::ifstream archivo;

    if (!abrirArchivoParaLectura(rutaArchivo, archivo)) {
        return matriz; // Matriz vacía
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        // Si la linea no esta vacia tokenizamos
        if (!linea.empty()) {
            // Dividimos la linea de texto y lo almacenamos en un vector para que pueda ser alamacenado
            // en la matriz
            matriz.push_back(dividirTexto(linea, delimitador));
        }
    }

    archivo.close();
    return matriz;
}

bool GestorArchivosTexto::escribirCSV(const std::string &rutaArchivo, const std::vector<std::vector<std::string>> &matriz, char delimitador) {
    std::ofstream archivo;
    // Los CSV usualmente se sobrescriben al actualizar la configuración
    if (!abrirArchivoParaEscritura(rutaArchivo, archivo, false)) {
        return false;
    }

    for (size_t e = 0; e < matriz.size(); ++e) {
        for (size_t r = 0; r < matriz[e].size(); ++r) {
            archivo << matriz[e][r];
            // Escribir delimitador si no es la última columna de la fila
            if (r < matriz[e].size() - 1) {
                archivo << delimitador;
            }
        }
        archivo << "\n"; // Salto de línea al terminar la fila
        if (!verificarOperacion(archivo)) {
            archivo.close();
            return false;
        }
    }

    archivo.close();
    return true;
}

// =======================================================================================
// Implementación de Métodos de Validación y Utilidades de Archivo
// =======================================================================================

std::vector<std::string> GestorArchivosTexto::dividirTexto(const std::string &texto, char delimitador) {
    std::vector<std::string> tokens;
    std::stringstream ss(texto);
    std::string token;

    while (std::getline(ss, token, delimitador)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool GestorArchivosTexto::validarExisteArchivo(const std::string &rutaArchivo) { return std::filesystem::exists(rutaArchivo); }

bool GestorArchivosTexto::validarArchivoVacio(const std::string &rutaArchivo) {
    if (std::filesystem::exists(rutaArchivo) && std::filesystem::file_size(rutaArchivo) == 0) {
        return true;
    }
    return false;
}

bool GestorArchivosTexto::limpiarArchivo(const std::string &rutaArchivo) {
    // Abrir en modo std::ios::trunc borra todo el contenido inmediatamente dejando el archivo en 0 bytes
    std::ofstream archivo(rutaArchivo, std::ios::trunc);
    bool limpiado = archivo.is_open();
    if (limpiado) {
        archivo.close();
    }
    return limpiado;
}

// ? Agregar un parametro bool omitirencabezado seria buena idea
size_t GestorArchivosTexto::contarLineas(const std::string &rutaArchivo) {
    std::ifstream archivo;
    if (!abrirArchivoParaLectura(rutaArchivo, archivo)) {
        return 0;
    }

    size_t contador = 0;
    std::string linea;
    while (std::getline(archivo, linea)) {
        contador++;
    }

    archivo.close();
    return contador;
}