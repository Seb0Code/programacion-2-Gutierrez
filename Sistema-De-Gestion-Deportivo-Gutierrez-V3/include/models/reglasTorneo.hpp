#ifndef REGLAS_TORNEO_HPP
#define REGLAS_TORNEO_HPP

#include "../../include/utils/constantes.hpp"
#include <string>

class ReglasTorneo {
  private:
    std::string nombreDeporte;
    int minJugadores;
    int maxJugadores;
    int minutoMinimo;
    int minutoMaximo;
    bool permiteEmpate;
    bool tarjetasHabilitadas;

  public:
    ReglasTorneo() = default;

    // Constructor parametrizado
    ReglasTorneo(std::string nomDep, int minJ, int maxJ, int minM, int maxM, bool empate, bool tarjetas)
        : nombreDeporte(nomDep), minJugadores(minJ), maxJugadores(maxJ), minutoMinimo(minM), minutoMaximo(maxM), permiteEmpate(empate), tarjetasHabilitadas(tarjetas) {}

    // ------------------------------------------------------------------------------------------------- //
    //    GETERS                                                                                         //
    // ------------------------------------------------------------------------------------------------- //
    int getMinJugadores() const { return minJugadores; }

    int getMaxJugadores() const { return maxJugadores; }

    int getMinutoMinimo() const { return minutoMinimo; }

    int getMinutoMaximo() const { return minutoMaximo; }

    bool getPermiteEmpate() const { return permiteEmpate; }

    bool getTarjetasHabilitadas() const { return tarjetasHabilitadas; }
};

#endif
