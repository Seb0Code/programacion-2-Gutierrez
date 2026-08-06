#include "../../include/models/torneo.hpp"
#include "../../include/manager/operacionesTorneo.hpp"
#include "../../include/persistence/gestorArchivosTexto.hpp"
#include "../../include/utils/formatos.hpp"
#include "../../include/utils/validaciones.hpp"
#include <iomanip>
#include <iostream>

Torneo::Torneo(const char *nom, const char *dep, const char *form, const char *fechaI, const char *fechaF) : Torneo() {
    Formatos::copiarCadena(nombre, nom, sizeof(nombre));
    Formatos::copiarCadena(deporte, dep, sizeof(deporte));
    Formatos::copiarCadena(formato, form, sizeof(formato));
    Formatos::copiarCadena(fechaInicio, fechaI, sizeof(fechaInicio));
    Formatos::copiarCadena(fechaFin, fechaF, sizeof(fechaFin));

    const time_t ahora = std::time(nullptr);
    fechaCreacion = ahora;
    fechaUltimaModificacion = ahora;
    inicializado = true;
}

const char *Torneo::getDeporte() const { return deporte; }

const char *Torneo::getNombre() const { return nombre; }

const char *Torneo::getFormato() const { return formato; }

const char *Torneo::getFechaInicio() const { return fechaInicio; }

const char *Torneo::getFechaFin() const { return fechaFin; }

time_t Torneo::getFechaCreacion() const { return fechaCreacion; }

time_t Torneo::getFechaUltimaModificacion() const { return fechaUltimaModificacion; }

std::string Torneo::getFechaCreacionFormateada() const {

    // Transformamos la fecha a un string y lo devolvemos
    std::string fechaFormateada = OperacionesTorneo::convertirFechaTimeAString(fechaCreacion);
    return fechaFormateada;
}

std::string Torneo::getFechaUltimaModificacionFormateada() const {
    // Transformamos la fecha a un string y lo devolvemos
    std::string fechaFormateada = OperacionesTorneo::convertirFechaTimeAString(fechaUltimaModificacion);
    return fechaFormateada;
}

bool Torneo::getInicializado() const { return inicializado; }

size_t Torneo::getTamano() { return sizeof(Torneo); }

bool Torneo::setNombre(const char *nom) {

    // si está vacía no realizamos el cambio o si no es un nombre valido
    if (GestorDeValidaciones::validarCadenaVacia(nom) || !GestorDeValidaciones::validarNombreTorneo(nom) || std::strlen(nom) >= sizeof(nombre)) {
        return false;
    }

    // Copiamos el nombre
    Formatos::copiarCadena(nombre, nom, sizeof(nombre));
    return true;
}

bool Torneo::setDeporte(const int idDep) {

    // Le pedimos a operaciones torneo que busque el deporte
    std::string nombreDeporte = OperacionesTorneo::buscarDeporteEnConfig(idDep);

    if (nombreDeporte == "ERROR") {
        return false;
    }

    if (nombreDeporte.size() >= constantes::TAMANO_DEPORTE) {
        return false;
    }

    // Convertimos el string a cadenas de c y copiamos el nuevo nombre del deporte
    const char *nombreDeporte_cstr = nombreDeporte.c_str();
    Formatos::copiarCadena(deporte, nombreDeporte_cstr, sizeof(deporte));
    return true;
}

bool Torneo::setFormato(const int idForm) {

    // Le pedimos a operaciones torneo que busque el formato
    std::string nombreFormato = OperacionesTorneo::buscarFormatoEnConfig(idForm);

    if (nombreFormato == "ERROR") {
        return false;
    }

    if (nombreFormato.size() >= constantes::TAMANO_FORMATO) {
        return false;
    }

    // Convertimos el string a cadenas de c y copiamos el nuevo nombre del formato
    const char *nombreFormato_cstr = nombreFormato.c_str();
    Formatos::copiarCadena(formato, nombreFormato_cstr, sizeof(formato));
    return true;
}

bool Torneo::setFechaInicio(const char *fechaI) {

    // Validamos la fecha
    if (!GestorDeValidaciones::validarFecha(fechaI)) {
        return false;
    }

    // Validamos que cumpla con el tamaño valido
    if (std::strlen(fechaI) != constantes::TAMANO_FECHA - 1) {
        return false;
    }

    Formatos::copiarCadena(fechaInicio, fechaI, sizeof(fechaInicio));
    return true;
}

bool Torneo::setFechaFin(const char *fechaF) {
    // Validamos la fecha
    if (!GestorDeValidaciones::validarFechaFin(fechaF, fechaInicio)) {
        return false;
    }

    // Validamos que cumpla con el tamaño valido
    if (std::strlen(fechaF) != constantes::TAMANO_FECHA - 1) {
        return false;
    }

    // Copiamos al atributo original
    Formatos::copiarCadena(fechaFin, fechaF, sizeof(fechaFin));
    return true;
}

ReglasTorneo Torneo::getReglasTorneo() const { return reglas; }

bool Torneo::setFechaCreacion(const time_t fechaC) {
    // Si time_t es con signo negativo (antes de 1970) o excede rangos soportados
    if (fechaC == static_cast<std::time_t>(-1)) {
        return false;
    }

    // Intentar convertir a estructura de tiempo local
    std::tm *f = std::localtime(&fechaC);
    if (f == nullptr) {
        return false;
    }

    // Cambiamos la fecha
    fechaCreacion = fechaC;
    return true;
}

bool Torneo::setFechaUltimaModificacion(const time_t fechaUM) {
    // Si time_t es con signo negativo (antes de 1970) o excede rangos soportados
    if (fechaUM == static_cast<std::time_t>(-1)) {
        return false;
    }

    // Intentar convertir a estructura de tiempo local
    std::tm *f = std::localtime(&fechaUM);
    if (f == nullptr) {
        return false;
    }

    // Cambiamos la fecha
    fechaUltimaModificacion = fechaUM;
    return true;
}

bool Torneo::setInicializado(const bool ini) {
    inicializado = ini;
    return true;
}
