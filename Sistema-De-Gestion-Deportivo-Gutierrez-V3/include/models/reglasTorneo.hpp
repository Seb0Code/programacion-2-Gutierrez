#ifndef REGLAS_TORNEO_HPP
#define REGLAS_TORNEO_HPP

#include "../../include/utils/constantes.hpp"
#include <cstring>
#include <string>

class ReglasTorneo {
  private:
    char nombreDeporte[constantes::TAMANO_DEPORTE]{};
    int minJugadores{};
    int maxJugadores{};
    int minutoMinimo{};
    int minutoMaximo{};
    bool permiteEmpate{};
    bool tarjetasHabilitadas{};

  public:
    ReglasTorneo() = default;

    // Constructor parametrizado
    ReglasTorneo(const std::string &nomDep, int minJ, int maxJ, int minM, int maxM, bool empate, bool tarjetas)
        : minJugadores(minJ), maxJugadores(maxJ), minutoMinimo(minM), minutoMaximo(maxM), permiteEmpate(empate), tarjetasHabilitadas(tarjetas) {
        std::strncpy(nombreDeporte, nomDep.c_str(), constantes::TAMANO_DEPORTE - 1);
        nombreDeporte[constantes::TAMANO_DEPORTE - 1] = '\0';
    }

    // ------------------------------------------------------------------------------------------------- //
    //    GETERS                                                                                         //
    // ------------------------------------------------------------------------------------------------- //
    const char *getNombreDeporte() const { return nombreDeporte; }

    int getMinJugadores() const { return minJugadores; }

    int getMaxJugadores() const { return maxJugadores; }

    int getMinutoMinimo() const { return minutoMinimo; }

    int getMinutoMaximo() const { return minutoMaximo; }

    bool getPermiteEmpate() const { return permiteEmpate; }

    bool getTarjetasHabilitadas() const { return tarjetasHabilitadas; }
};

#endif