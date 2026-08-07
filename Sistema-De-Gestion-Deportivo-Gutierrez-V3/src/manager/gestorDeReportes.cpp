#include "../../include/manager/gestorDeReportes.hpp"

std::vector<Jugador> GestorReportes::tablaDeGoleadoresTop10() {
    std::vector<Jugador> listaDeJugadores;

    // inicializamos en 0 por si no pasa las validaciones
    int difAnotaciones = 0;
    bool intercambiar = false;

    listaDeJugadores = GestorArchivosBinarios::listarRegistros<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES);

    if (listaDeJugadores.empty()) {
        return listaDeJugadores;
    }

    // Ordenamos por cantidad de anotaciones
    // Restamos 1 para no acceder a memoria indebida
    for (int e = 0; e < listaDeJugadores.size() - 1; e++) {
        for (int r = 0; r < listaDeJugadores.size() - e - 1; r++) {

            intercambiar = false;

            // Si el jugador 1 tiene menos anotaciones que el jugador 2;
            if (listaDeJugadores[r].getNumAnotaciones() < listaDeJugadores[r + 1].getNumAnotaciones()) {
                intercambiar = true;

                // Si poseen igual cantidad de anotaciones
            } // Aqui puedo colocar otros criterios de desempate a futuro

            // Si se cumple alguna condicion hacemos el intercambio
            if (intercambiar) {
                // Guardamos el jugador con menos puntos en una variable auxiliar
                Jugador registroTemporal = listaDeJugadores[r];

                // Luego movemos el equipo mayor a la posicion donde estaba el menor
                listaDeJugadores[r] = listaDeJugadores[r + 1];

                // colocamos en la nueva posicion al equipo con menos puntos
                listaDeJugadores[r + 1] = registroTemporal;
            }
        }
    }

    return listaDeJugadores;
}

std::vector<Jugador> GestorReportes::tablaDeTarjetasAmarillasTop10() {
    std::vector<Jugador> listaDeJugadores;

    // inicializamos en 0 por si no pasa las validaciones
    int difAnotaciones = 0;
    bool intercambiar = false;

    listaDeJugadores = GestorArchivosBinarios::listarRegistros<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES);

    if (listaDeJugadores.empty()) {
        return listaDeJugadores;
    }

    // Ordenamos por cantidad de anotaciones
    // Restamos 1 para no acceder a memoria indebida
    for (int e = 0; e < listaDeJugadores.size() - 1; e++) {
        for (int r = 0; r < listaDeJugadores.size() - e - 1; r++) {

            intercambiar = false;

            // Si el jugador 1 tiene menos anotaciones que el jugador 2;
            if (listaDeJugadores[r].getTarjetaAmarillas() < listaDeJugadores[r + 1].getTarjetaAmarillas()) {
                intercambiar = true;

                // Si poseen igual cantidad de anotaciones
            } // Aqui puedo colocar otros criterios de desempate a futuro

            // Si se cumple alguna condicion hacemos el intercambio
            if (intercambiar) {
                // Guardamos el jugador con menos puntos en una variable auxiliar
                Jugador registroTemporal = listaDeJugadores[r];

                // Luego movemos el equipo mayor a la posicion donde estaba el menor
                listaDeJugadores[r] = listaDeJugadores[r + 1];

                // colocamos en la nueva posicion al equipo con menos puntos
                listaDeJugadores[r + 1] = registroTemporal;
            }
        }
    }

    return listaDeJugadores;
}

std::vector<Jugador> GestorReportes::tablaDeTarjetasRojasTop10() {
    std::vector<Jugador> listaDeJugadores;

    // inicializamos en 0 por si no pasa las validaciones
    int difAnotaciones = 0;
    bool intercambiar = false;

    listaDeJugadores = GestorArchivosBinarios::listarRegistros<Jugador>(constantes::NOMBRE_ARCHIVO_JUGADORES);

    if (listaDeJugadores.empty()) {
        return listaDeJugadores;
    }

    // Ordenamos por cantidad de tarjetas rojas
    // Restamos 1 para no acceder a memoria indebida
    for (int e = 0; e < listaDeJugadores.size() - 1; e++) {
        for (int r = 0; r < listaDeJugadores.size() - e - 1; r++) {

            intercambiar = false;

            // Si el jugador 1 tiene menos anotaciones que el jugador 2;
            if (listaDeJugadores[r].getTarjetasRojas() < listaDeJugadores[r + 1].getTarjetasRojas()) {
                intercambiar = true;

                // Si poseen igual cantidad de anotaciones
            } // Aqui puedo colocar otros criterios de desempate a futuro

            // Si se cumple alguna condicion hacemos el intercambio
            if (intercambiar) {
                // Guardamos el jugador con menos puntos en una variable auxiliar
                Jugador registroTemporal = listaDeJugadores[r];

                // Luego movemos el equipo mayor a la posicion donde estaba el menor
                listaDeJugadores[r] = listaDeJugadores[r + 1];

                // colocamos en la nueva posicion al equipo con menos puntos
                listaDeJugadores[r + 1] = registroTemporal;
            }
        }
    }

    return listaDeJugadores;
}
