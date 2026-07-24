#ifndef TORNEO_HPP
#define TORNEO_HPP

#include "../../include/utils/constantes.hpp"

class Torneo {

  private:
    char nombre[constantes::TAMANO_NOMBRE];     // Nombre del torneo
    char deporte[constantes::TAMANO_DEPORTE];   // Deporte del torneo
    char formato[constantes::TAMANO_FORMATO];   // "GRUPOS" o "ELIMINATORIA"
    char fechaInicio[constantes::TAMANO_FECHA]; // Formato YYYY-MM-DD
    char fechaFin[constantes::TAMANO_FECHA];    // Formato YYYY-MM-DD

    // Metadata de control
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;

    // Variable para ver si los datos ya están inicializados
    bool inicializado;

  public:
    // ---------------------------------------------------------------------------------------------- //
    //   GETERS                                                                                       //
    // ---------------------------------------------------------------------------------------------- //
    char *getNombre();
    char *getDeporte();
    char *getFormato();
    char *getFechaInicio();
    char *getFechaFin();

    // ---------------------------------------------------------------------------------------------- //
    //   SETTERS                                                                                       //
    // ---------------------------------------------------------------------------------------------- //
};

#endif