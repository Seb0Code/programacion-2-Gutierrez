#include "../../include/models/torneo.hpp"

const char *Torneo::getDeporte() { return deporte; }

const char *Torneo::getNombre() { return nombre; }

const char *Torneo::getFormato() { return formato; }

const char *Torneo::getFechaInicio() { return fechaInicio; }

const char *Torneo::getFechaFin() { return fechaFin; }

size_t Torneo::getTamanoTorneo() { return sizeof(Torneo); }