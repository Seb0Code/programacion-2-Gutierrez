#include "../../include/utils/validaciones.hpp"
#include "../../include/utils/constantes.hpp"
#include <cstring>

void separarFechaEnPartes(const char *fecha, int &año, int &mes, int &dia) {
    año = (fecha[0] - '0') * 1000 + (fecha[1] - '0') * 100 + (fecha[2] - '0') * 10 + (fecha[3] - '0');
    mes = (fecha[5] - '0') * 10 + (fecha[6] - '0');
    dia = (fecha[8] - '0') * 10 + (fecha[9] - '0');
}

int convertirFechaANumeroEntero(int año, int mes, int dia) { return (año * 10000) + (mes * 100) + dia; }

// -------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------

bool GestorDeValidaciones::validarTamano(const char *texto, const size_t tamano) {
    size_t tamanoCadena = strlen(texto);
    return tamanoCadena == tamano;
}

inline bool GestorDeValidaciones::validarCadenaVacia(const char *texto) { return (texto == nullptr || *texto == '\0'); }

bool GestorDeValidaciones::validarSoloNumeros(const char *texto) {

    size_t e = 0;
    bool esNum = true;

    // validamos que no esté vacío
    if (GestorDeValidaciones::validarCadenaVacia(texto)) {
        return false;
    }

    while (*(texto + e) != '\0') {
        esNum = std::isdigit(static_cast<unsigned char>(*(texto + e)));
        if (!esNum) {
            return false;
        }
        e++;
    }
    return true;
}

bool GestorDeValidaciones::validarSoloLetras(const char *texto) {

    size_t e = 0;
    bool esLetra = true;
    bool esEspacio = true;

    // validamos que no esté vacío
    if (GestorDeValidaciones::validarCadenaVacia(texto)) {
        return false;
    }

    while (*(texto + e) != '\0') {
        esLetra = std::isalpha(static_cast<unsigned char>(*(texto + e)));
        esEspacio = std::isspace(static_cast<unsigned char>(*(texto + e)));
        if (!esLetra && !esEspacio) {
            return false;
        }
        e++;
    }
    return true;
}

bool GestorDeValidaciones::validarEsBisiesto(const int año) {
    // Verificamos que no sea 0 o negativo
    if (año <= 0) {
        return false;
    }
    // Aplicamos la regla de divisibilidad del año bisiesto
    return (año % 4 == 0 && año % 100 != 0) || (año % 400 == 0);
}

bool GestorDeValidaciones::validarEsAlfanumericoConEspacios(const char *texto) {
    size_t i = 0;
    while (texto[i] != '\0') {
        // revisa si es letra o número y si es espacio ' '
        if (!std::isalnum(texto[i]) && texto[i] != ' ') {
            return false;
        }
        i++;
    }
    return true;
}

bool GestorDeValidaciones::validarMinuto(const int variable) {

    if (variable < MINUTO_MINIMO || variable > MINUTO_MAXIMO) {
        return false;
    }
    return true;
}

bool GestorDeValidaciones::validarEsPositivo(const int num) {
    if (num < 0) {
        return false;
    }
    return true;
}

bool GestorDeValidaciones::validarId(const int id) {
    if (id <= 0) {
        return false;
    }
    return true;
}

bool GestorDeValidaciones::validarIdParaAutogol(const int id) {
    if (id < 0) {
        return false;
    }
    return true;
}

bool GestorDeValidaciones::validarEdad(const int edad) {
    // la edad no puede ser negativa ni igual a 0, tampoco puede ser mayor a 120
    if (edad < 14 || edad > 50) {
        // asignamos la siguiente cadena de texto a el array de char
        return false;
    }
    return true;
}

bool GestorDeValidaciones::validarDorsal(const int dorsal) {
    if (dorsal < 1 || dorsal > 99) {
        // std:strncpy copia el mensaje del segundo parametro dentro de un const char*
        return false;
    }
    return true;
}

bool GestorDeValidaciones::validarFecha(const char *fecha) {
    int dia = 0, mes = 0, año = 0;
    // array de los dias de cada mes
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // validamos que no esté vacío
    if (GestorDeValidaciones::validarCadenaVacia(fecha)) {
        return false; // si esta vacio devolvemos que la fecha no es válida
    }

    // Validación de tamaño y guiones (YYYY-MM-DD)
    if (!GestorDeValidaciones::validarTamano(fecha, 10) || fecha[4] != '-' || fecha[7] != '-') {
        return false;
    }

    // Verificamos que los demás caracteres sean numéricos
    for (size_t e = 0; e < 10; e++) {
        if (e == 4 || e == 7)
            continue;
        if (!std::isdigit(static_cast<unsigned char>(fecha[e]))) {
            return false;
        }
    }

    separarFechaEnPartes(fecha, año, mes, dia);
    // si es año bisiesto febrero pasa a tener 29 dias
    if (mes == 2 && GestorDeValidaciones::validarEsBisiesto(año)) {
        diasPorMes[1] = 29;
    }

    // validar que los meses esten en el rango y que los años no sean negativos
    if (mes < 1 || mes > 12 || año < 1) {
        return false;
    }

    // validar que el dia
    if (dia < 1 || dia > diasPorMes[mes - 1]) {
        return false;
    }

    return true;
}

// Esta funcion se usa para validar que una fecha no sea menor en el tiempo que otra
bool GestorDeValidaciones::validarFechaFin(const char *fechaFin, const char *fechaInicio) {

    // si la fecha final no es valida
    if (!GestorDeValidaciones::validarFecha(fechaFin)) {
        return false;
    }

    // declaramos los valores a comparar
    int añoFin, mesFin, diaFin;
    int añoIni, mesIni, diaIni;

    // Almacenamos las fechas en variables int
    separarFechaEnPartes(fechaFin, añoFin, mesFin, diaFin);
    separarFechaEnPartes(fechaInicio, añoIni, mesIni, diaIni);

    int numIni = convertirFechaANumeroEntero(añoIni, mesIni, diaIni);
    int numFin = convertirFechaANumeroEntero(añoFin, mesFin, diaFin);

    if (numFin < numIni) {
        return false;
    }
    return true;
}

bool GestorDeValidaciones::validarFechaDeRegistroDePartidos(const char *fechaPartido, const char *fechaInicioTorneo, const char *fechaFinTorneo) {

    // si las fechas no son validas
    if (!GestorDeValidaciones::validarFecha(fechaPartido) || !GestorDeValidaciones::validarFecha(fechaInicioTorneo) || !GestorDeValidaciones::validarFecha(fechaFinTorneo)) {
        return false;
    }

    // declaramos los valores a comparar
    int añoFin, mesFin, diaFin;
    int añoIni, mesIni, diaIni;
    int añoPtd, mesPtd, diaPtd;

    // Almacenamos las fechas en variables int
    separarFechaEnPartes(fechaFinTorneo, añoFin, mesFin, diaFin);
    separarFechaEnPartes(fechaInicioTorneo, añoIni, mesIni, diaIni);
    separarFechaEnPartes(fechaPartido, añoPtd, mesPtd, diaPtd);

    // llevamos cada fecha a expresion de un solo numero
    int numIni = convertirFechaANumeroEntero(añoIni, mesIni, diaIni);
    int numFin = convertirFechaANumeroEntero(añoFin, mesFin, diaFin);
    int numPtd = convertirFechaANumeroEntero(añoPtd, mesPtd, diaPtd);

    // Validamos los limites
    if (numPtd < numIni || numPtd > numFin) {
        return false;
    }

    return true;
}

bool GestorDeValidaciones::validarFechaDeRegistroDeJugadorOEquipo(const char *fechaRegistro, const char *fechaInicioTorneo) {

    // si la fecha final no es valida
    if (!GestorDeValidaciones::validarFecha(fechaRegistro) || !GestorDeValidaciones::validarFecha(fechaRegistro)) {
        return false;
    }

    // declaramos los valores a comparar
    int añoRgt, mesRgt, diaRgt;
    int añoIni, mesIni, diaIni;

    // Almacenamos las fechas en variables int
    separarFechaEnPartes(fechaRegistro, añoRgt, mesRgt, diaRgt);
    separarFechaEnPartes(fechaInicioTorneo, añoIni, mesIni, diaIni);

    int numIni = convertirFechaANumeroEntero(añoIni, mesIni, diaIni);
    int numRgt = convertirFechaANumeroEntero(añoRgt, mesRgt, diaRgt);

    // Verificamos que el registro no sea despues de iniciar el torneo del torneo
    if (numRgt >= numIni) {
        return false;
    }

    // Calculamos la diferencia de tiempo en meses entre ambas fechas
    int difAños = añoIni - añoRgt;
    int difMeses = (mesIni - mesRgt);
    int totalDeMesesDiferencia = (difAños * 12) + difMeses;

    // Verificamos que el registro no sea mas de 6 meses antes
    if (std::abs(totalDeMesesDiferencia) > 6) {
        return false;
    }
    return true;
}

bool Cedulas(const char *cedula) {
    const size_t tamañoMin = 7, tamañomax = 10;

    // validamos que no esté vacío
    if (GestorDeValidaciones::validarCadenaVacia(cedula)) {
        return false; // si esta vacio devolvemos que la fecha no es válida
    }

    // verificamos que solo tenga numeros
    if (!GestorDeValidaciones::validarSoloNumeros(cedula)) {
        return false;
    }

    // medimos la longitud
    size_t longitud = strlen(cedula);

    // si la longitud esta fuera del rando
    if (longitud < tamañoMin || longitud > tamañomax) {
        return false;
    }

    return true;
}

bool GestorDeValidaciones::validarNombreOApellido(const char *nombre) {
    // validamos que no esté vacío
    if (GestorDeValidaciones::validarCadenaVacia(nombre)) {
        return false; // si esta vacio devolvemos que la fecha no es válida
    }

    // validamos que solo contenga letras
    if (!GestorDeValidaciones::validarSoloLetras(nombre)) {
        return false;
    }
    return true;
}

bool nombreTorneo(const char *nombreTorneo) {
    // validamos que no esté vacío
    if (GestorDeValidaciones::validarCadenaVacia(nombreTorneo)) {
        return false; // si esta vacio devolvemos que la fecha no es válida
    }

    //
    if (!GestorDeValidaciones::validarEsAlfanumericoConEspacios(nombreTorneo)) {
        return false;
    }

    return true;
}
