#include "../../include/utils/validaciones.hpp"
#include "../../include/models/fecha.hpp"
#include "../../include/utils/constantes.hpp"
#include "../../include/utils/formatos.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

// =======================================================================================================
// SOBRECARGAS PARA TEXTOS (Para std::string y puntero char (const char*))
// =======================================================================================================

bool GestorDeValidaciones::validarCadenaVacia(const char *texto) { return (texto == nullptr || *texto == '\0'); }

bool GestorDeValidaciones::validarCadenaVacia(const std::string &texto) { return texto.empty(); }

bool GestorDeValidaciones::validarEsCadenaVaciaOSoloEspacios(const char *texto) {
    if (validarCadenaVacia(texto))
        return true;
    return std::all_of(texto, texto + strlen(texto), [](unsigned char ch) { return std::isspace(ch); });
}

bool GestorDeValidaciones::validarEsCadenaVaciaOSoloEspacios(const std::string &texto) {
    return texto.empty() || std::all_of(texto.begin(), texto.end(), [](unsigned char ch) { return std::isspace(ch); });
}

bool GestorDeValidaciones::validarSoloNumeros(const char *texto) {
    if (validarCadenaVacia(texto))
        return false;
    return std::all_of(texto, texto + strlen(texto), [](unsigned char ch) { return std::isdigit(ch); });
}

bool GestorDeValidaciones::validarSoloNumeros(const std::string &texto) {
    if (texto.empty())
        return false;
    return std::all_of(texto.begin(), texto.end(), [](unsigned char ch) { return std::isdigit(ch); });
}

bool GestorDeValidaciones::validarSoloLetras(const char *texto) {
    if (validarCadenaVacia(texto))
        return false;
    return std::all_of(texto, texto + strlen(texto), [](unsigned char ch) { return std::isalpha(ch) || std::isspace(ch); });
}

bool GestorDeValidaciones::validarSoloLetras(const std::string &texto) {
    if (texto.empty())
        return false;
    return std::all_of(texto.begin(), texto.end(), [](unsigned char ch) { return std::isalpha(ch) || std::isspace(ch); });
}

bool GestorDeValidaciones::validarEsAlfanumericoConEspacios(const char *texto) {
    if (validarCadenaVacia(texto))
        return false;
    return std::all_of(texto, texto + strlen(texto), [](unsigned char ch) { return std::isalnum(ch) || std::isspace(ch); });
}

bool GestorDeValidaciones::validarEsAlfanumericoConEspacios(const std::string &texto) {
    if (texto.empty())
        return false;
    return std::all_of(texto.begin(), texto.end(), [](unsigned char ch) { return std::isalnum(ch) || std::isspace(ch); });
}

// =======================================================================================================
// VALIDACIONES DE ENTIDADES DE NEGOCIO SIMPLES
// =======================================================================================================

bool GestorDeValidaciones::validarTamano(const char *texto, const size_t tamano) {
    if (texto == nullptr) {
        return false;
    }
    return strlen(texto) == tamano;
}

bool GestorDeValidaciones::validarTamano(const std::string &texto, const size_t tamano) { return texto.length() == tamano; }

bool GestorDeValidaciones::validarMinuto(const int minuto, const int minutoMin, const int minutoMax) { return (minuto >= minutoMin && minuto <= minutoMax); }

bool GestorDeValidaciones::validarEsPositivo(const int num) { return num > 0; }

bool GestorDeValidaciones::validarId(const int id) { return id > 0; }

bool GestorDeValidaciones::validarIdParaAutogol(const int id) { return id >= 0; }

bool GestorDeValidaciones::validarEdad(const int edad) { return (edad >= 14 && edad <= 50); }

bool GestorDeValidaciones::validarDorsal(const int dorsal) { return (dorsal >= 1 && dorsal <= 99); }

bool GestorDeValidaciones::validarCedula(const char *cedula) {
    if (validarCadenaVacia(cedula) || !validarSoloNumeros(cedula)) {
        return false;
    }
    size_t longitud = strlen(cedula);
    return (longitud >= 7 && longitud <= 10);
}

bool GestorDeValidaciones::validarCedula(const std::string &cedula) { return validarCedula(cedula.c_str()); }

bool GestorDeValidaciones::validarNombreOApellido(const char *nombre) { return !validarCadenaVacia(nombre) && validarSoloLetras(nombre); }

bool GestorDeValidaciones::validarNombreOApellido(const std::string &nombre) { return !validarCadenaVacia(nombre) && validarSoloLetras(nombre); }

bool GestorDeValidaciones::validarNombreTorneo(const char *nombreTorneo) { return !validarCadenaVacia(nombreTorneo) && validarEsAlfanumericoConEspacios(nombreTorneo); }

bool GestorDeValidaciones::validarNombreTorneo(const std::string &nombreTorneo) { return !validarCadenaVacia(nombreTorneo) && validarEsAlfanumericoConEspacios(nombreTorneo); }

// =======================================================================================================
//   LOGICA DE FECHAS
// =======================================================================================================

bool GestorDeValidaciones::validarEsBisiesto(const int año) {
    if (año <= 0) {
        return false;
    }
    return (año % 4 == 0 && año % 100 != 0) || (año % 400 == 0);
}

bool GestorDeValidaciones::validarFecha(const char *fecha) {

    // Verificamos que no esté vacía y que tenga un tamaño de fecha valido
    if (validarCadenaVacia(fecha) || !validarTamano(fecha, constantes::TAMANO_FECHA - 1)) {
        return false;
    }

    // Verificamos que tenga los guiones
    if (fecha[4] != '-' || fecha[7] != '-') {
        return false;
    }

    // Verificar que sea YYYY-MM-DD
    for (size_t e = 0; e < constantes::TAMANO_FECHA - 1; e++) {
        if (e == 4 || e == 7) {
            continue;
        }
        if (!std::isdigit(static_cast<unsigned char>(fecha[e]))) {
            return false;
        }
    }

    // Transformamos la cadena de texto a formato fecha
    Fecha f = Formatos::convertirTextoAFecha(fecha);
    if (f.anio < 1) {
        return false;
    }

    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (validarEsBisiesto(f.anio)) {
        diasPorMes[1] = 29;
    }

    return (f.mes >= 1 && f.mes <= 12 && f.dia >= 1 && f.dia <= diasPorMes[f.mes - 1]);
}

bool GestorDeValidaciones::validarFecha(const std::string &fecha) { return validarFecha(fecha.c_str()); }

bool GestorDeValidaciones::validarFechaFin(const char *fechaFin, const char *fechaInicio) {

    // Validamos que las fechas sean validas (valga la redundancia xd)
    if (!validarFecha(fechaFin) || !validarFecha(fechaInicio))
        return false;

    Fecha fin = Formatos::convertirTextoAFecha(fechaFin);
    Fecha inicio = Formatos::convertirTextoAFecha(fechaInicio);

    // Comparamos con la sobrecarga de operadores (Equivale como al >=)
    return !(fin < inicio);
}

bool GestorDeValidaciones::validarFechaFin(const std::string &fechaFin, const std::string &fechaInicio) { return validarFechaFin(fechaFin.c_str(), fechaInicio.c_str()); }

bool GestorDeValidaciones::validarFechaDeRegistroDePartidos(const char *fechaPartido, const char *fechaInicioTorneo, const char *fechaFinTorneo) {

    // Verificamos si las fechas son validas
    if (!validarFecha(fechaPartido) || !validarFecha(fechaInicioTorneo) || !validarFecha(fechaFinTorneo))
        return false;

    // Convertimos a tipo fecha para comparar
    Fecha ptd = Formatos::convertirTextoAFecha(fechaPartido);
    Fecha inicio = Formatos::convertirTextoAFecha(fechaInicioTorneo);
    Fecha fin = Formatos::convertirTextoAFecha(fechaFinTorneo);

    // El partido debe estar entre en el rango del torneo (>= incio && <= al final)
    return !(ptd < inicio) && !(ptd > fin);
}

bool GestorDeValidaciones::validarFechaDeRegistroDePartidos(const std::string &fechaPartido, const std::string &fechaInicioTorneo, const std::string &fechaFinTorneo) {
    return validarFechaDeRegistroDePartidos(fechaPartido.c_str(), fechaInicioTorneo.c_str(), fechaFinTorneo.c_str());
}

bool GestorDeValidaciones::validarFechaDeRegistroDeJugadorOEquipo(const char *fechaRegistro, const char *fechaInicioTorneo) {

    // Verificamos si las fechas son validas
    if (!validarFecha(fechaRegistro) || !validarFecha(fechaInicioTorneo)) {
        return false;
    }

    Fecha rgt = Formatos::convertirTextoAFecha(fechaRegistro);
    Fecha inicio = Formatos::convertirTextoAFecha(fechaInicioTorneo);

    // No se puede registrar después de que el torneo arrancó
    if (!(rgt < inicio))
        return false;

    // Calcular la diferencia máxima permitida (6 meses)
    int totalDeMesesDiferencia = ((inicio.anio - rgt.anio) * 12) + (inicio.mes - rgt.mes);
    return std::abs(totalDeMesesDiferencia) <= 6;
}

bool GestorDeValidaciones::validarFechaDeRegistroDeJugadorOEquipo(const std::string &fechaRegistro, const std::string &fechaInicioTorneo) {
    return validarFechaDeRegistroDeJugadorOEquipo(fechaRegistro.c_str(), fechaInicioTorneo.c_str());
}