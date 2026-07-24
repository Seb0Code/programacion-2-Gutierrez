// Sport G&C Tournaments (SISTEMA DE GESTION DE TORNEOS DEPORTIVOS)

// ============================================//
//   1. INCLUDES Y DIRECTIVAS                   //
// ============================================//
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <locale>
#include <sstream>
#include <string>
#include <thread>



namespace fs = std::filesystem;

// ============================================//
//   2. STRUCTS                                //
// ============================================//



struct Jugador {
    // Datos básicos
    int ID;
    int idEquipo;
    char nombre[TAMANO_NOMBRE];
    char cedula[TAMANO_CEDULA];
    char posicion[TAMANO_POSICION];
    int edad;
    int numeroDorsal;
    char fechaRegistro[TAMANO_FECHA]; // fecha de registro del jugador en formato YYYY-MM-DD
    // bool suspendido;

    // Estadísticas individuales (se actualizan al registrar goles en partidos)
    int anotaciones;
    int tarjetaAmarillas;
    int tarjetasRojas;

    // Metadata de control
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Equipo {
    // Datos básicos
    int ID;
    char nombre[TAMANO_NOMBRE];
    char ciudad[TAMANO_MENSAJE_ERROR];
    char entrenador[TAMANO_NOMBRE];
    char fechaRegistro[TAMANO_FECHA]; // fecha de registro del equipo en Formato: YYYY-MM-DD
    int numJugadores = 0;             // Numero de jugadores del equipo

    // Estadísticas del torneo
    int puntos;
    int jugados = 0; // Partidos jugados
    int victorias;
    int empates;
    int derrotas;
    int anotacionAFavor;
    int anotacionEnContra;

    // Relaciones: IDs de partidos en que participó este equipo
    int partidosIDs[50]; // Máximo 50 partidos por equipo
    int cantidadPartidos;

    // Metadata de control
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

struct Anotacion {
    int idJugador;                         // ID del jugador que anotó (0 = desconocido / gol en contra)
    int minuto;                            // Minuto del partido en que se anotó (1 - 120)
    char equipo[TAMANO_LOCAL_O_VISITANTE]; // "LOCAL" o "VISITANTE"
};

struct tarjetaAmarilla {
    int idJugador;                         // ID del jugador que anotó (0 = desconocido / gol en contra)
    int minuto;                            // Minuto del partido en que se anotó (1 - 120)
    char equipo[TAMANO_LOCAL_O_VISITANTE]; // "LOCAL" o "VISITANTE"
};

struct tarjetaRoja {
    int idJugador;                         // ID del jugador que anotó (0 = desconocido / gol en contra)
    int minuto;                            // Minuto del partido en que se anotó (1 - 120)
    char equipo[TAMANO_LOCAL_O_VISITANTE]; // "LOCAL" o "VISITANTE"
};

struct Partido {
    // Datos básicos
    int ID;
    int idEquipoLocal;
    int idEquipoVisitante;
    char fecha[TAMANO_FECHA];
    char estado[TAMANO_ESTADO]; // "PROGRAMADO", "JUGADO", "CANCELADO"
    char descripcion[TAMANO_DESCRIPCION];

    // Marcador global (para acceso rápido sin recorrer el array de goles)
    int anotacionesLocal;
    int anotacionesVisitante;

    // Tarjetas por equipo
    int tarjetasAmaLocal;
    int tarjetasAmaVisitante;
    int tarjetasRojasLocal;
    int tarjetasRojasVisitante;

    // Detalle de goles (NUEVO en Proyecto 2)
    Anotacion anotaciones[MAX_ANOTACIONES];           // Máximo 22 goles por partido
    tarjetaAmarilla tarjetaA[MAX_TARJETAS_AMARILLAS]; // Máximo 30 tarjetas A por partido
    tarjetaRoja tarjetaR[MAX_TARJETAS_ROJAS];         // Máximo 8 tarjetas R por partido
    int numAnotaciones;
    int numtarjetaAma;
    int numTarjetasRojas;

    // Metadata de control
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaUltimaModificacion;
};

// Una estructura para los informes de las referencias

struct ReferenciaRota {
    char tipoDeReferencia[20];
    int idOrigen;
    int idReferenciaRota;
};

struct InformeIntegridad {
    int equiposVerificados = 0;
    int jugadoresVerificados = 0;
    int partidosVerificados = 0;
    int totalDeReferenciasRotas = 0;
    ReferenciaRota rotas[MAX_RESULTADOS];
};

// ============================================//
//   3. FUNCIONES aUXILIARES                   //
// ============================================//

// grupo de funciones que no inciden como tal en el sistema pero que mejoran su funcionamiento
namespace auxiliares {

    namespace {
        std::string normalizarEntradaAMayus(const std::string &valor) {
            std::string copia = valor;
            std::transform(copia.begin(), copia.end(), copia.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
            return copia;
        }

        bool esCancelacion(const std::string &valor) {
            std::string copia = normalizarEntradaAMayus(valor);
            return copia == "CANCELAR" || copia == "SALIR";
        }
    } // namespace

    // funcion para ingresar cualquier tipo de dato
    template <typename var1> //
    bool ingresarDatos(var1 &variable, const char *mensaje, bool *cancelado = nullptr, bool (*ptrValidador)(var1, char *) = nullptr) {
        bool flag = false;
        char mensajeError[TAMANO_MENSAJE_ERROR];
        std::string entrada;

        do {
            mensajeError[0] = '\0';
            flag = false;

            if (cancelado != nullptr) {
                *cancelado = false;
            }

            std::cout << mensaje /*<< " (escriba 'cancelar' para cancelar): "*/ << std::flush;
            std::cin.clear();
            std::getline(std::cin, entrada);

            // Verificamos si quiere cancelar
            if (esCancelacion(entrada)) {
                if (cancelado != nullptr) {
                    *cancelado = true;
                }
                return false;
            }

            // Intenamos convertir al ripo de dato
            std::stringstream ss(entrada);
            if (!(ss >> variable)) {
                std::cout << "Error Tipo de Dato Incorrecto\n\n";
                waitfor(3000);
                continue;
            }

            if (ss >> std::ws && !ss.eof()) {
                std::cout << "Error Tipo de Dato Incorrecto\n\n";
                waitfor(3000);
                continue;
            }

            // Verificamos la validacion enviada
            if (ptrValidador != nullptr) {
                flag = !ptrValidador(variable, mensajeError);
                if (flag) {
                    std::cout << mensajeError << std::endl << std::endl;
                    waitfor(3500);
                }
            }
        } while (flag);

        return true;
    }

    // funcion para ingresar cadenas de texto
    bool ingresarCadena(char *texto, size_t tamañoMaximo, const char *mensaje, bool *cancelado = nullptr, bool (*ptrValidador)(const char *, char *) = nullptr) {
        const int TAMANO_MENSAJE_ERROR = 150;
        bool flag = false;
        char mensajeError[TAMANO_MENSAJE_ERROR];
        std::string entrada;

        do {
            mensajeError[0] = '\0';
            flag = false;

            if (cancelado != nullptr) {
                *cancelado = false;
            }

            std::cout << mensaje << std::flush;
            std::cin.clear();
            std::getline(std::cin, entrada);

            if (esCancelacion(entrada)) {
                if (cancelado != nullptr) {
                    *cancelado = true;
                }
                return false;
            }

            if (entrada.size() >= tamañoMaximo) {
                std::cout << "ERROR: Excediste el limite de caracteres permitido (" << tamañoMaximo - 1 << "). Intente de nuevo.\n\n";
                auxiliares::waitfor(3000);
                flag = true;
                continue;
            }

            std::strncpy(texto, entrada.c_str(), tamañoMaximo - 1);
            texto[tamañoMaximo - 1] = '\0';

            if (ptrValidador != nullptr) {
                if (!ptrValidador(texto, mensajeError)) {
                    flag = true;
                    std::cout << mensajeError << std::endl << std::endl;
                    waitfor(3500);
                }
            }
        } while (flag);

        return true;
    }

} // namespace auxiliares

// ============================================//
//   5. CAPA DE logica                         //
// ============================================//

namespace logica {

    bool existeArchivo(const char *nombreArchivo) {
        // Creamos el archivo y le colocamos la cabecera
        std::ifstream comprobar;

        // intentamos abrir el archivo
        comprobar.open(nombreArchivo, std::ios::binary);

        // Si el archivo no se pudo leer/abrir significa que no existe
        if (!comprobar.good()) {
            comprobar.close();
            return false;
        }
        comprobar.close();
        return true;
    }

    // Abre el archivo y retorna el header si el archivo existe y si tiene header
    ArchivoHeader leerHeader(const char *nombreArchivo) {

        // si no existe el archivo devolvemos el header lleno de -1
        ArchivoHeader header, headerError = {-1, -1, -1, -1};

        if (!existeArchivo(nombreArchivo)) {

            return headerError;

        } else { // Si existe el archivo
            // abrimos el archivo en modo lectura
            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que el archivo abrió correctamente
            if (!archivo.is_open()) {
                return headerError;
            }

            // ubicamos el puntero de lectura al principio por seguridad
            archivo.seekg(0, std::ios::beg);

            // Leemos solo el header
            archivo.read(reinterpret_cast<char *>(&header), sizeof(ArchivoHeader));

            // Verificamos si la lectura fue correcta
            if (archivo.fail()) {
                archivo.close();
                return headerError;
            }

            archivo.close();
            return header;
        }
    }

    bool obtenerInformacionTorneo(Torneo &torneo) {
        // Buscamos y leemos el nombre del torneo

        std::ifstream archivoTorneo;
        archivoTorneo.open(NOMBRE_ARCHIVO_TORNEO, std::ios::binary);

        if (!archivoTorneo.is_open()) {
            return false;
        }

        // Movemos el puntero de lectura al principio
        archivoTorneo.seekg(0, std::ios::beg);

        // Leemos el torneo
        archivoTorneo.read(reinterpret_cast<char *>(&torneo), sizeof(Torneo));

        // Verificamos si la lectura fue exitosa
        if (archivoTorneo.fail()) {
            archivoTorneo.close();
            return false;
        }

        // Cerramos el archivo
        archivoTorneo.close();
        return true;
    }

    namespace {

        bool prepararOperacion(const char *nombreArchivo, ArchivoHeader &header) {
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            header = leerHeader(nombreArchivo);
            return header.cantidadRegistros != -1;
        }

        // Abre un archivo binario en modo lectura/escritura y confirma que se abrió.
        bool abrirParaEscritura(const char *nombreArchivo, std::fstream &archivo) {
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
            return archivo.is_open();
        }

        bool abrirParaLectura(const char *nombreArchivo, std::ifstream &archivo) {
            archivo.open(nombreArchivo, std::ios::binary);
            return archivo.is_open();
        }

        void definirFormato(Torneo &torneo, int opcion) {
            if (opcion == 1) {
                std::strncpy(torneo.formato, "GRUPOS", TAMANO_FORMATO);
            } else if (opcion == 2) {
                std::strncpy(torneo.formato, "ELIMINATORIA", TAMANO_FORMATO);
            }
        }

        // Crea el archivo si no existe y escribe un ArchivoHeader en cero
        // Retorna true si el archivo quedó listo para usar
        bool inicializarArchivo(const char *nombreArchivo) {

            // Si el archivo existe (se puede abrir para leer) simplemente devolvemos true y cerramos el archivo
            if (existeArchivo(nombreArchivo)) {
                return true;

            } else { // Si el archivo no existe lo creamos
                std::ofstream archivo;
                archivo.open(nombreArchivo, std::ios::binary);

                if (!archivo.is_open()) {
                    // Devolvemos false porque ya que el archivo NO quedó listo para usar (no existe)
                    return false;
                }

                // Si el archivo es de torneo, inicializamos un registro Torneo vacío
                if (std::strcmp(nombreArchivo, NOMBRE_ARCHIVO_TORNEO) == 0) {
                    Torneo torneoVacio{};
                    torneoVacio.inicializado = false;
                    torneoVacio.fechaCreacion = std::time(nullptr);
                    torneoVacio.fechaUltimaModificacion = std::time(nullptr);

                    archivo.seekp(0, std::ios::beg);
                    archivo.write(reinterpret_cast<const char *>(&torneoVacio), sizeof(Torneo));
                    if (archivo.fail()) {
                        archivo.close();
                        return false;
                    }

                    archivo.close();
                    return true;
                }

                // inicializamos un archivo Header
                ArchivoHeader nuevo;
                nuevo.cantidadRegistros = 0;
                nuevo.proximoID = 1;
                nuevo.registrosActivos = 0;
                nuevo.version = 0;

                // Movemos el puntero de escritura al inicio por seguridad
                archivo.seekp(0, std::ios::beg);

                // escribimos el header en el archivo binario
                archivo.write(reinterpret_cast<const char *>(&nuevo), sizeof(ArchivoHeader));

                // Verificamos que se haya escrito bien
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // cerramos el archivo
                archivo.close();
                return true;
            }
        }

        // recibe el header y se actualiza el header del archivo
        bool actualizarHeader(const char *nombreArchivo, ArchivoHeader header) {

            // Si no existe el archivo devolvemos false
            if (!existeArchivo(nombreArchivo)) {
                return false;

            } else { // Si existe el archivo cambiamos el header
                std::fstream archivo;

                if (!abrirParaEscritura(nombreArchivo, archivo)) {
                    return false;
                }

                // Movemos el puntero de escritura al inicio por seguridad
                archivo.seekp(0, std::ios::beg);

                // Escribimos el nuevo header
                archivo.write(reinterpret_cast<const char *>(&header), sizeof(ArchivoHeader));

                // Verificamos si la escritura fue correcta
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                archivo.close();
                return true;
            }
        }

        bool inicializarSistemaArchivos() {
            return inicializarArchivo(NOMBRE_ARCHIVO_TORNEO) && inicializarArchivo(NOMBRE_ARCHIVO_EQUIPOS) && inicializarArchivo(NOMBRE_ARCHIVO_JUGADORES) &&
                   inicializarArchivo(NOMBRE_ARCHIVO_PARTIDOS);
        }

        template <class struct1> //
        int buscarIndicePorID(const char *nombreArchivo, int ID) {
            int indice = -1;

            // Si el archivo no existe devolvemos -1
            if (!existeArchivo(nombreArchivo)) {
                return indice;
            }

            if (ID < 0) {
                return -1;
            }

            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return -1;
            }

            // Movemos el puntero de lectura despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            struct1 registroTemporal;
            int contadorIndice = 0;
            bool encontrado = false;

            // Realizamos la busqueda
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct1))) {
                if (archivo.fail()) {
                    archivo.close();
                    return -1;
                }

                if (registroTemporal.ID == ID) {
                    encontrado = true;
                    break;
                }
                contadorIndice++;
            }

            archivo.close();

            return encontrado ? contadorIndice : -1;
        }

        template <class struct2> //
        bool existeID(const char *nombreArchivo, const int ID) {

            // Verificamos que existe el archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::ifstream archivo;
            struct2 registroTemporal;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // movemos el puntero de indice despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Buscamos el ID en los registros
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct2))) {

                // Verificamos si se produjo un fallo
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // Nos saltamos los equipos eliminados
                if (registroTemporal.eliminado) {
                    continue;
                }

                // Si encontramos una coincidencia devolvemos true
                if (registroTemporal.ID == ID) {
                    archivo.close();
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        // tipo5 es la estructura y tipo6 es el tipo de dato dentro de la estructura, uno de los miembros
        template <class struct3, class var2> //
        bool cadenaDuplicada(const char *nombreArchivo, const char *cadena, var2 struct3::*miembro) {

            // Verificamos si el archivo existe
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // Realizamos una copia de la variable para poder convertirla a minuscula sin problemas
            char nombreBusquedaAux[TAMANO_NOMBRE];
            std::strncpy(nombreBusquedaAux, cadena, TAMANO_NOMBRE);
            auxiliares::toMinus(nombreBusquedaAux);
            struct3 registroTemporal;

            // movemos el puntero de indice despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // buscamos en el binario
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct3))) {

                // verificamos que no hayan errores en la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // Nos saltamos los equipos eliminados
                if (registroTemporal.eliminado) {
                    continue;
                }

                // buscamos si el nombre está duplicado
                if (std::strcmp(registroTemporal.*miembro, cadena) == 0) {
                    // * DESCRIPCION:
                    // como *miembro es un puntero es decir guarda una direccion de memoria, la desreferenciamos con '*'
                    // por lo tanto lo que indica es que dentro de la estructura real (que aun no sabemos cual es)
                    // vamos a acceder al campo que nos indica miembro (el que pasaron como argumento)
                    // si la estructura es equipo y el miembro es nombre entrenador entonces hace equipo::nombreEntrenador
                    // es decir creamos una variable de tipo equipo y le pasamos como propiedad el nombre del entrenador
                    archivo.close();
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        template <class struct4, class var3> //
        bool cadenaDuplicadaParaActualizar(const char *nombreArchivo, const char *cadena, const int idRegistro, var3 struct4::*miembro) {

            // si el archivo no existe devolvemos false
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // creamos una copia para no modificar las variables reales
            char copiaNuevaCadena[TAMANO_NOMBRE];
            std::strncpy(copiaNuevaCadena, cadena, TAMANO_NOMBRE);
            auxiliares::toMinus(copiaNuevaCadena);
            struct4 registroTemporal;

            // movemos el puntero de indice despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Realizamos la busqueda
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct4))) {
                // verificamos si no ocurrio un fallo en la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // Nos saltamos los equipos eliminados
                if (registroTemporal.eliminado) {
                    continue;
                }

                // Si estamos revisando el registro al que pertenecemos lo ignoramos
                if (registroTemporal.ID == idRegistro) {
                    continue;
                }

                // creamos una copia de la cadena real para hacer la comparacion en minusculas
                char cadenaAux[TAMANO_NOMBRE];
                std::strncpy(cadenaAux, registroTemporal.*miembro, TAMANO_NOMBRE);
                auxiliares::toMinus(cadenaAux);

                // comparamos los nombres si es igual devolmemos true sino false
                if (std::strcmp(cadenaAux, copiaNuevaCadena) == 0) {
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        // Funcion para cualquier estructura
        template <class struct5> //
        bool buscarRegistrosPorId(const char *nombreArchivo, struct5 &buscado, const int ID) {
            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            if (ID <= 0) {
                return false;
            }

            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // Buscamos el indice fisico
            int indice = buscarIndicePorID<struct5>(nombreArchivo, ID);

            // Si no encontró nada es decir el indice vale -1 retornamos false
            if (indice == -1) {
                return false;
            }

            // calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(struct5);

            // movemos el punero a esa posicion
            archivo.seekg(posicion, std::ios::beg);

            // Leemos el registro
            archivo.read(reinterpret_cast<char *>(&buscado), sizeof(struct5));

            // verificamos que no se hayan producido errores en la lectura
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // verificamos que no esté eliminado
            if (buscado.eliminado) {
                return false;
            }

            archivo.close();

            /*
            // debug
            std::cout << "\n sizeof(ArchivoHeader): " << sizeof(ArchivoHeader);
            std::cout << "\n Indice: " << indice;
            std::cout << "\n sizeof(Equipo): " << sizeof(struct5);
            std::cout << "\n Posicion: " << posicion;
            std::cout << "\n Calculo: " << sizeof(ArchivoHeader) << " + " << indice << " * " << sizeof(struct5);
            auxiliares::pausarPrograma();*/
            return true;
        }

        template <class struct6> //
        int buscarRegistrosPorSucadena(const char *nombreArchivo, struct6 resultados[], const char *subcadena, const int maxResultados) {
            int cantidadDeRegistrosEncontrados = 0;
            int error = -1;
            struct6 registroTemporal;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return error;
            }

            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // Realizamos copias para no dañar las variables originales
            char copiaRegistro[TAMANO_NOMBRE];
            char copiaSubcadena[TAMANO_NOMBRE];
            std::strncpy(copiaSubcadena, subcadena, TAMANO_NOMBRE);
            auxiliares::toMinus(copiaSubcadena);

            // Ubicamos el puntero de posicion despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Realizamos la lectura del archivo
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct6))) {

                // Verificamos que no haya error al hacer la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return error;
                }

                // Si encontramos un registro eliminado lo saltamos
                if (registroTemporal.eliminado) {
                    continue;
                }

                // Si aún no llegamos a la cantidad maxima de registros hacemos la comparacion
                if (cantidadDeRegistrosEncontrados < maxResultados) {

                    // hacemos una copia del nombre del registro
                    std::strncpy(copiaRegistro, registroTemporal.nombre, TAMANO_NOMBRE);
                    auxiliares::toMinus(copiaRegistro);

                    // lo pasamos a minus para comparar mejor
                    // Buscamos si la subcadena coincide con la copia usando std::strstr
                    if (std::strstr(copiaRegistro, copiaSubcadena) != nullptr) {
                        resultados[cantidadDeRegistrosEncontrados] = registroTemporal;
                        cantidadDeRegistrosEncontrados++;
                    }

                } else {
                    // sino salimos del bucle
                    break;
                }
            }

            archivo.close();
            return cantidadDeRegistrosEncontrados;
        }

    } // namespace

    template <class struct7> //
    int listarRegistros(const char *nombreArchivo, struct7 resultados[], const int maxRegistros) {
        int cantidadDeRegistrosEncontrados = 0;
        int error = -1;
        struct7 registroTemporal;
        ArchivoHeader header = leerHeader(nombreArchivo);

        // verificamos que si existe ese archivo
        if (!existeArchivo(nombreArchivo)) {
            return error;
        }

        std::ifstream archivo;
        if (!abrirParaLectura(nombreArchivo, archivo)) {
            return false;
        }

        // Verificamos que el header se haya leido correctamente
        if (header.cantidadRegistros == -1) {
            return false;
        }

        // Ubicamos el puntero de posicion despues del header
        archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

        // Realizamos la lectura del archivo
        while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(struct7))) {

            // Verificamos si no hubo un falló en la lectura
            if (archivo.fail()) {
                archivo.close();
                return error;
            }

            // Si encontramos un archivo que esta eliminado lo saltamos
            if (registroTemporal.eliminado) {
                continue;
            }

            // Si aún no llegamos a la cantidad maxima de registros hacemos la comparacion
            if (cantidadDeRegistrosEncontrados < maxRegistros) {

                resultados[cantidadDeRegistrosEncontrados] = registroTemporal;
                cantidadDeRegistrosEncontrados++;

            } else {
                // sino salimos del bucle
                break;
            }
        }

        // Verificamos que se hayan leido todos los registros
        if (header.registrosActivos > cantidadDeRegistrosEncontrados) {
            return error;
        }

        archivo.close();
        return cantidadDeRegistrosEncontrados;
    }

    namespace equipos {

        bool registrarEquipo(const char *nombreArchivo, Equipo &nuevoEquipo) {
            ArchivoHeader header;
            if (!prepararOperacion(nombreArchivo, header)) {
                return false;
            }
            std::fstream archivo;
            if (!abrirParaEscritura(nombreArchivo, archivo)) {
                return false;
            }

            // movemos el puntero de escritura al final
            archivo.seekp(0, std::ios::end);

            // colocamos el equipo como activo (es decir que no ha sido eliminado)
            nuevoEquipo.eliminado = false;

            // Inicializamos las estadísticas
            nuevoEquipo.jugados = 0;
            nuevoEquipo.victorias = 0;
            nuevoEquipo.empates = 0;
            nuevoEquipo.derrotas = 0;
            nuevoEquipo.puntos = 0;
            nuevoEquipo.anotacionAFavor = 0;
            nuevoEquipo.anotacionEnContra = 0;
            nuevoEquipo.numJugadores = 0;
            nuevoEquipo.cantidadPartidos = 0;

            // Asignamos el ID
            nuevoEquipo.ID = header.proximoID;

            // Asignamos la fecha de creacion del equipo
            nuevoEquipo.fechaCreacion = time(nullptr);           // Esto toma la fecha de en que se creo la variable
            nuevoEquipo.fechaUltimaModificacion = time(nullptr); // colocamos la misma porque cuando fue cuando se creó

            // Escribimos la estructura
            archivo.write(reinterpret_cast<const char *>(&nuevoEquipo), sizeof(Equipo));

            // Verificamos que no se haya producido un error
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Actualizamos el header
            header.cantidadRegistros++;
            header.proximoID++;
            header.registrosActivos++;
            // header.version; // ? Por el momento lo de dejaré asi pq nose que hacer
            actualizarHeader(nombreArchivo, header);

            archivo.close();
            return true;
        }

        int tablaDePosiciones(const char *nombreArchivo, Equipo registros[], const int maxEquipos) {
            // inicializamos en 0 por si no pasa las validaciones
            int difPtsEq1 = 0, difPtsEq2 = 0;
            bool intercambiar = false;
            int cantidadDeRegistros = 0;
            int error = -1;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return error;
            }

            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // Mediante la funcion listarRegistros obtenemos los registros de equipos guardados y la cantidad
            cantidadDeRegistros = listarRegistros<Equipo>(nombreArchivo, registros, maxEquipos);

            // verificamos que la funcion no devuelva error
            if (cantidadDeRegistros == error) {
                return error;
            }

            // Ordenamos por cantidad de puntos de mayor a menor
            // Restamos 1 para no acceder a memoria indebida
            for (int e = 0; e < cantidadDeRegistros - 1; e++) {
                for (int r = 0; r < cantidadDeRegistros - e - 1; r++) {

                    // incializamos esta bandera en false, se activa si los equipos
                    // requieren que se interambien por los criterios de cada condicion
                    intercambiar = false;

                    //* Condicion 1
                    // Si el equipo 1 tiene menos puntos que el equipo 2;
                    if (registros[r].puntos < registros[r + 1].puntos) {
                        intercambiar = true;

                        // Si poseen igual cantidad de puntos
                    } else if ((registros[r].puntos) == (registros[r + 1].puntos)) {

                        // Calculamos diferencia de puntos
                        difPtsEq1 = (registros[r].anotacionAFavor) - (registros[r].anotacionEnContra);
                        difPtsEq2 = (registros[r + 1].anotacionAFavor) - (registros[r + 1].anotacionEnContra);

                        // *Condicion 2
                        // Si el equipo de la izquierda tiene menor diferencia de puntos
                        // lo ubicamos a la deracha es decir lo bajamos una posicion
                        if (difPtsEq1 < difPtsEq2) {
                            intercambiar = true;

                            // Si la diferencia de goles es igual tambien
                        } else if (difPtsEq1 == difPtsEq2) {

                            //* Condicion 3
                            // comparamos los puntos a favor
                            if (registros[r].anotacionAFavor < registros[r + 1].anotacionAFavor) {
                                intercambiar = true;

                                // Si los puntos a favor son iguales desempatamos por victorias
                            } else if (registros[r].anotacionAFavor == registros[r + 1].anotacionAFavor) {

                                // * Condicion 4
                                // comparamos las victorias
                                if (registros[r].victorias < registros[r + 1].victorias) {
                                    intercambiar = true;
                                }
                            }
                        }
                    }

                    // Si se cumple alguna condicion hacemos el intercambio
                    if (intercambiar) {
                        // Guardamos el equipo con menos puntos en una variable auxiliar
                        Equipo registroTemporal = registros[r];

                        // Luego movemos el equipo mayor a la posicion donde estaba el menor
                        registros[r] = registros[r + 1];

                        // colocamos en la nueva posicion al equipo con menos puntos
                        registros[r + 1] = registroTemporal;
                    }
                }
            }

            archivo.close();
            return cantidadDeRegistros;
        }

        bool actualizarEquipo(const char *nombreArchivo, const int ID, const char *nombre, const char *entrenador, const char *ciudad) {

            bool existe = false;
            int error = -1;

            // Verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            // Verifiquemos que los valores no esten vacios
            if (validadores::charVacio(nombre) && validadores::charVacio(entrenador) && validadores::charVacio(ciudad)) {
                return false;
            }

            // Utilizamos una variable auxiliar
            Equipo equipoAuxiliar;

            // Buscamos el registro con el ID
            existe = buscarRegistrosPorId<Equipo>(nombreArchivo, equipoAuxiliar, ID);

            // Verificamos que lo haya encontrado
            if (!existe) {
                return false;
            }

            // Abrimos el archivo
            std::fstream archivo;
            if (!abrirParaEscritura(nombreArchivo, archivo)) {
                return false;
            }

            // Verificamos que no estén duplicados
            if (cadenaDuplicadaParaActualizar<Equipo>(nombreArchivo, nombre, ID, &Equipo::nombre) ||
                cadenaDuplicadaParaActualizar<Equipo>(nombreArchivo, entrenador, ID, &Equipo::entrenador)) {
                return false;
            }

            // Si no está vacío lo actualizamos
            if (!validadores::charVacio(nombre)) {
                std::strncpy(equipoAuxiliar.nombre, nombre, TAMANO_NOMBRE);
            }
            if (!validadores::charVacio(entrenador)) {
                std::strncpy(equipoAuxiliar.entrenador, entrenador, TAMANO_NOMBRE);
            }
            if (!validadores::charVacio(ciudad)) {
                std::strncpy(equipoAuxiliar.ciudad, ciudad, TAMANO_NOMBRE);
            }

            // buscamos el indice fisico del archivo para guardarlo
            int indice = buscarIndicePorID<Equipo>(nombreArchivo, ID);

            // Verificamos que no devuelva error
            if (indice == error) {
                return false;
            }

            // Calculamos la posición
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Equipo);

            // Nos movemos a esa posicion
            archivo.seekp(posicion, std::ios::beg);

            // Modificamos la fecha de ultima modificacion del registro
            equipoAuxiliar.fechaUltimaModificacion = std::time(nullptr);

            // Sobreescribimos esa posicion
            archivo.write(reinterpret_cast<const char *>(&equipoAuxiliar), sizeof(Equipo));

            // Si hubo error de lectura
            if (archivo.fail()) {
                return false;
            }

            // Cerramos el archivo
            archivo.close();
            return true;
        }

        bool eliminarEquipo(const char *nombreArchivo, const int ID) {
            ArchivoHeader header;
            if (!prepararOperacion(nombreArchivo, header)) {
                return false;
            }
            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // Buscamos el indice fisico del equipo
            int indice = buscarIndicePorID<Equipo>(nombreArchivo, ID);

            // Verificamos si no se encontró ningun equipo con ese ID
            if (indice == -1) {
                return false;
            }

            Equipo registroTemporal;

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Equipo);

            // Movemos el puntero de lectura al indice de posicion
            archivo.seekg(posicion, std::ios::beg);

            // Leemos el registro
            archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Equipo));

            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            /*
            // Abrimos el archivo de partidos para hacer la verificacion
            std::ifstream archivoPartidos;
            if (!abrirParaLectura(NOMBRE_ARCHIVO_PARTIDOS, archivo)) {
                    return false;
            }


            Partido partidoTemporal;

            // * Verificamos que no tenga partidos asociados
            while (archivoPartidos.read(reinterpret_cast<char *>(&partidoTemporal), sizeof(Partido))) {
                if (archivoPartidos.fail()) {
                    archivoPartidos.close();
                    archivo.close();
                    return false;
                }

                if ((partidoTemporal.idEquipoLocal == registroTemporal.ID) || (partidoTemporal.idEquipoVisitante == registroTemporal.ID)) {
                    archivoPartidos.close();
                    archivo.close();
                    return false;
                }
            }

            archivoPartidos.close();

            // Abrimos el archivo de jugadores para hacer la verificacion
            std::ifstream archivoJugadores;
            archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary);

            if (!archivoJugadores.is_open()) {
                archivo.close();
                return false;
            }

            Jugador jugadorTemporal;

            // * Verificamos que no tenga jugadores asociados
            while (archivoJugadores.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {
                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                if (jugadorTemporal.idEquipo == registroTemporal.ID) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }
            }

            archivoJugadores.close();
            */
            // Reabrimos el archivo de equipos para actualizar el registro
            archivo.close();
            std::fstream archivoEquipos;
            if (!abrirParaEscritura(nombreArchivo, archivoEquipos)) {
                return false;
            }

            archivoEquipos.seekp(posicion, std::ios::beg);
            registroTemporal.eliminado = true;
            archivoEquipos.write(reinterpret_cast<const char *>(&registroTemporal), sizeof(Equipo));
            if (archivoEquipos.fail()) {
                archivoEquipos.close();
                return false;
            }
            archivoEquipos.close();
            header.registrosActivos--;
            actualizarHeader(nombreArchivo, header);
            return true;
        }

    } // namespace equipos

    namespace jugadores {

        bool DorsalDuplicado(const char *nombreArchivo, const int dorsal, const int idEquipo) {

            // Verificamos que exista el archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // Movemos el puntero de lectura despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            Jugador jugadorTemporal;

            // Leemos el archivo
            while (archivo.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {

                // Verificamos si no hubo error en la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // Si encontramos coincidencia y pertence al mismo equipo
                if ((jugadorTemporal.numeroDorsal == dorsal) && (jugadorTemporal.idEquipo == idEquipo)) {
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        bool dorsalDuplicadoParaActualizar(const char *nombreArchivo, const int dorsal, const int IDJugador, const int idEquipo) {

            // Verificamos que exista el archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // Movemos el puntero de lectura despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            Jugador jugadorTemporal;

            // Leemos el archivo
            while (archivo.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {

                // Verificamos si no hubo error en la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // Saltamos al jugador que está siendo actualizado
                if (jugadorTemporal.ID == IDJugador) {
                    continue;
                }

                // Si encontramos coincidencia y pertence al mismo equipo
                if ((jugadorTemporal.numeroDorsal == dorsal) && (jugadorTemporal.idEquipo == idEquipo)) {
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        bool cedulaRepetida(const char *nombreArchivo, const char *cedula) {

            // Verificamos que exista el archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            // Movemos el puntero de lectura despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            Jugador jugadorTemporal;

            // Leemos el archivo
            while (archivo.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {

                // Verificamos si no hubo error en la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                if (std::strcmp(jugadorTemporal.cedula, cedula) == 0) {
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        bool registrarJugador(const char *nombreArchivo, Jugador &nuevoJugador) {

            int error = -1;

            // verificamos primero que el archivo existe
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            // Leemos el header para obtener los datos
            ArchivoHeader header = leerHeader(nombreArchivo);

            // Validamos que el header no devuelva error
            if (header.cantidadRegistros == error) {
                return error;
            }

            std::fstream archivo;
            if (!abrirParaEscritura(nombreArchivo, archivo)) {
                return false;
            }

            // * Inicializamos las estadisticas
            nuevoJugador.anotaciones = 0;
            nuevoJugador.tarjetaAmarillas = 0;
            nuevoJugador.tarjetasRojas = 0;

            // * Inicializamos las fechas de creacion y ultima modificacion
            nuevoJugador.fechaCreacion = std::time(nullptr);
            nuevoJugador.fechaUltimaModificacion = std::time(nullptr);

            // * Colocamos el jugador como activo y le colocamos el ID
            nuevoJugador.ID = header.proximoID;
            nuevoJugador.eliminado = false;

            // * Actualizamos el header
            header.cantidadRegistros++;
            header.registrosActivos++;
            header.proximoID++;

            // * Movemos el puntero de escritura al final
            archivo.seekp(0, std::ios::end);

            // * Registramos el nuevo jugador
            archivo.write(reinterpret_cast<const char *>(&nuevoJugador), sizeof(Jugador));

            // Verficamos que no haya errores con la escritura
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo de jugadores
            archivo.close();

            // * Actualziamos el header
            actualizarHeader(nombreArchivo, header);


            // * Aumentamos el numero de jugadores del equipo

            // buscamos el indice fisico del equipo al que pertence el jugador
            int indice = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, nuevoJugador.idEquipo);

            // Verificamos que indice no arroje error
            if (indice == error) {
                return false;
            }

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Equipo);

            std::fstream archivoEquipo;
            if (!abrirParaEscritura(NOMBRE_ARCHIVO_EQUIPOS, archivoEquipo)) {
                return false;
            }

            // Nos movemos a la posicion del registro (puntero de lectura)
            archivoEquipo.seekg(posicion, std::ios::beg);

            Equipo equipoTemporal;

            // Leemos el equipo
            archivoEquipo.read(reinterpret_cast<char *>(&equipoTemporal), sizeof(Equipo));

            // Verificamos que no haya errores de lectura
            if (archivoEquipo.fail()) {
                archivo.close();
                return false;
            }

            // aumentamos en 1 el numero de jugadores
            equipoTemporal.numJugadores++;

            // Ahora nos movemos a la posicion del registro (puntero de escritura)
            archivoEquipo.seekp(posicion, std::ios::beg);

            // Sobreescribmos el registro
            archivoEquipo.write(reinterpret_cast<const char *>(&equipoTemporal), sizeof(Equipo));

            // Verificamos que no hubo errores
            if (archivoEquipo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo de equipo
            archivoEquipo.close();
            return true;
        }

        int listarJugadoresPorEquipo(const char *nombreArchivo, const int idEquipo, Jugador resultados[], int maxResultados) {

            int error = -1;
            int cantidadDeJugadoresEncontrados = 0;
            ArchivoHeader header;
            if (!prepararOperacion(nombreArchivo, header)) {
                return false;
            }
            std::ifstream archivo;
            if (!abrirParaLectura(nombreArchivo, archivo)) {
                return false;
            }

            Equipo equipoTemporal;
            Jugador jugadorTemporal;

            // Buscamos si el equipo solicitado realmente existe
            bool existe = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoTemporal, idEquipo);

            // Verificamos si el equipo existe
            if (!existe) {
                return error;
            }

            // Si el equipo no tiene jugadores
            if (equipoTemporal.numJugadores == 0) {
                return 0;
            }

            // Movemos el puntero despues del header
            archivo.seekg(sizeof(header), std::ios::beg);

            // buscamos los jugadores
            while (archivo.read(reinterpret_cast<char *>(&jugadorTemporal), sizeof(Jugador))) {

                // Verificamos que la lectura no arroje error
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // Si aun nos superamos el maximo de resultados permitido
                if (cantidadDeJugadoresEncontrados < maxResultados) {

                    // Si el jugador no pertence al equipo lo ignoramos
                    if (jugadorTemporal.idEquipo != idEquipo) {
                        continue;
                    }

                    if (jugadorTemporal.eliminado) {
                        continue;
                    }

                    // Guardamos los jugadores
                    resultados[cantidadDeJugadoresEncontrados] = jugadorTemporal;

                    // aumentamos el contador de jugadores encontrados
                    cantidadDeJugadoresEncontrados++;

                } else { // si no rompemos el bucle
                    break;
                }
            }

            return cantidadDeJugadoresEncontrados;
        }

        bool actualizarJugador(const char *nombreArchivo, int ID, const char *nombre, int edad, int numeroDorsal, const char *posicion, const char *cedula) {

            bool existe = false;
            int error = -1;

            // Verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            // Verifiquemos que los valores no esten vacios
            if (validadores::charVacio(nombre) && edad == 0 && numeroDorsal == 0 && validadores::charVacio(posicion) && validadores::charVacio(cedula)) {
                return false;
            }

            // Utilizamos una variable auxiliar
            Jugador jugadorAuxiliar;

            // Buscamos el registro con el ID
            existe = buscarRegistrosPorId<Jugador>(nombreArchivo, jugadorAuxiliar, ID);

            // Verificamos que lo haya encontrado
            if (!existe) {
                return false;
            }

            // Abrimos el archivo
            std::fstream archivo;
            if (!abrirParaEscritura(nombreArchivo, archivo)) {
                return false;
            }

            // Verificamos que no estén duplicados
            if (cadenaDuplicadaParaActualizar<Jugador>(nombreArchivo, nombre, ID, &Jugador::nombre) ||
                cadenaDuplicadaParaActualizar<Jugador>(nombreArchivo, cedula, ID, &Jugador::cedula) ||
                dorsalDuplicadoParaActualizar(nombreArchivo, numeroDorsal, ID, jugadorAuxiliar.idEquipo)) {
                return false;
            }

            char mensajeError[TAMANO_MENSAJE_ERROR];
            // Si no está vacío lo actualizamos
            if (!validadores::charVacio(nombre)) {
                std::strncpy(jugadorAuxiliar.nombre, nombre, TAMANO_NOMBRE);
            }
            if (validadores::validarEdad(edad, mensajeError)) {
                jugadorAuxiliar.edad = edad;
            }
            if (validadores::validarDorsal(numeroDorsal, mensajeError)) {
                jugadorAuxiliar.numeroDorsal = numeroDorsal;
            }
            if (!validadores::charVacio(posicion)) {
                std::strncpy(jugadorAuxiliar.posicion, posicion, TAMANO_POSICION);
            }
            if (!validadores::charVacio(cedula)) {
                std::strncpy(jugadorAuxiliar.cedula, cedula, TAMANO_CEDULA);
            }

            // buscamos el indice fisico del archivo para guardarlo
            int indice = buscarIndicePorID<Jugador>(nombreArchivo, ID);

            // Verificamos que no devuelva error
            if (indice == error) {
                return false;
            }

            // Calculamos la posición del registro
            std::streampos pos;
            pos = sizeof(ArchivoHeader) + indice * sizeof(Jugador);

            // Nos movemos a esa posición en el flujo de escritura
            archivo.seekp(pos, std::ios::beg);

            // Modificamos la fecha de ultima modificacion del registro
            jugadorAuxiliar.fechaUltimaModificacion = std::time(nullptr);

            // Sobreescribimos esa posicion
            archivo.write(reinterpret_cast<const char *>(&jugadorAuxiliar), sizeof(Jugador));

            // Si hubo error de lectura
            if (archivo.fail()) {
                return false;
            }

            // Cerramos el archivo
            archivo.close();
            return true;
        }

        bool eliminarJugador(const char *nombreArchivo, int ID) {

            int error = -1;
            ArchivoHeader header;
            if (!prepararOperacion(nombreArchivo, header)) {
                return false;
            }

            // * Borramos el Jugador

            // Buscamos el indice fisico del jugador
            int indice = buscarIndicePorID<Jugador>(nombreArchivo, ID);

            // Verificamos si no se encontró ningun jugador con ese ID
            if (indice == error) {
                return false;
            }

            std::fstream archivo;
            if (!abrirParaEscritura(nombreArchivo, archivo)) {
                return false;
            }

            Jugador registroTemporal;

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Jugador);

            // Movemos el puntero de lectura al indice de posicion
            archivo.seekg(posicion);

            // * Leemos el registro
            archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Jugador));

            // Verificamos que no haya fallado
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Lo colocamos como eliminado
            registroTemporal.eliminado = true;

            // Movemos el puntero de escritura al indice de posicion
            archivo.seekp(posicion, std::ios::beg);

            // * Sobreescribimos el registro
            archivo.write(reinterpret_cast<const char *>(&registroTemporal), sizeof(Jugador));

            // Verificamos que no haya fallado
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo
            archivo.close();

            // Reducimos el numero de jugadores activos
            header.registrosActivos--;

            //  *Actualizamos el header
            actualizarHeader(nombreArchivo, header);


            // * Disminuimos el contador de jugadores del equipo correspondiente

            Equipo equipoTemporal;
            bool existeEquipo = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoTemporal, registroTemporal.idEquipo);

            // si tiene equipo disminuimos el numero de jugadoress del equipo, sino no hacemos nada
            if (existeEquipo && equipoTemporal.numJugadores > 0) {
                equipoTemporal.numJugadores--;
            }

            // Guardamos la informacion en el equipo
            int indiceEquipo = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoTemporal.ID);
            if (indiceEquipo == error) {
                return false;
            }

            std::fstream archivoEquipo;
            if (!abrirParaEscritura(NOMBRE_ARCHIVO_EQUIPOS, archivoEquipo)) {
                return false;
            }

            std::streampos posicionEquipo = sizeof(ArchivoHeader) + indiceEquipo * sizeof(Equipo);
            archivoEquipo.seekp(posicionEquipo, std::ios::beg);
            archivoEquipo.write(reinterpret_cast<const char *>(&equipoTemporal), sizeof(Equipo));
            if (archivoEquipo.fail()) {
                archivoEquipo.close();
                return false;
            }
            archivoEquipo.close();

            return true;
        }
    } // namespace jugadores

    namespace partidos {

        int minJugadoresPorDeporte() {
            Torneo torneo;
            obtenerInformacionTorneo(torneo);
            for (size_t e = 0; e < validadores::totalDeportes; e++) {
                if (std::strcmp(validadores::Deportes[e], torneo.deporte) == 0) {
                    return validadores::MinimoJugadoresPorDeporte[e];
                }
            }
            return 1;
        }

        const char *estadoPartidos[] = {"PROGRAMADO", "JUGADO", "CANCELADO"};

        bool hayPartidoProgramadoEntre2(const char *nombreArchivo, const Partido &partido) {

            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);
            if (!archivo.is_open()) {
                return false;
            }

            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            Partido pAux;
            while (archivo.read(reinterpret_cast<char *>(&pAux), sizeof(Partido))) {
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                bool tienePartidoEntreSi = ((pAux.idEquipoLocal == partido.idEquipoLocal) && (pAux.idEquipoVisitante == partido.idEquipoVisitante)) ||
                                           ((pAux.idEquipoLocal == partido.idEquipoVisitante) && (pAux.idEquipoVisitante == partido.idEquipoLocal));

                if (tienePartidoEntreSi && (std::strcmp(pAux.estado, estadoPartidos[0]) == 0)) {
                    archivo.close();
                    return true;
                }
            }

            archivo.close();
            return false;
        }

        // Retorna array de partidos con ese estado ("PROGRAMADO", "JUGADO", "CANCELADO")
        int listarPartidosPorSuEstado(const char *nombreArchivo, Partido resultados[], const char *estado, const int maxResultados) {
            int cantidadDeRegistrosEncontrados = 0;
            int error = -1;
            Partido registroTemporal;
            ArchivoHeader header;

            if (!existeArchivo(nombreArchivo)) {
                return error;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);
            if (!archivo.is_open()) {
                return error;
            }

            header = leerHeader(nombreArchivo);
            if (header.cantidadRegistros == error) {
                archivo.close();
                return error;
            }

            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Partido))) {
                if (archivo.fail()) {
                    archivo.close();
                    return error;
                }

                if (registroTemporal.eliminado) {
                    continue;
                }

                if (cantidadDeRegistrosEncontrados < maxResultados) {
                    if (std::strcmp(registroTemporal.estado, estado) == 0) {
                        resultados[cantidadDeRegistrosEncontrados] = registroTemporal;
                        cantidadDeRegistrosEncontrados++;
                    }
                } else {
                    break;
                }
            }

            // Verificamos que se hayan leido todos los registros
            if (header.registrosActivos < cantidadDeRegistrosEncontrados) {
                archivo.close();
                return error;
            }

            archivo.close();
            return cantidadDeRegistrosEncontrados;
        }

        bool programarPartido(const char *nombreArchivo, Partido &nuevoPartido) {
            int error = -1;

            // * Validaciones

            // Verificamos que exista el archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            // Leemos el header del archivo de Equipos
            ArchivoHeader headerEquipos = leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que el header se haya leido correctamente
            if (headerEquipos.cantidadRegistros == error) {
                return false;
            }

            // Si no hay al menos 2 equipos no se puede programar un partido
            if (headerEquipos.registrosActivos <= 1) {
                return false;
            }

            // No se puede programar un partido entre el mismo equipo
            if (nuevoPartido.idEquipoLocal == nuevoPartido.idEquipoVisitante) {
                return false;
            }

            Equipo eqLocal, eqVisitante;

            // Si los equipos no existen
            bool existeEquipoLocal = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, nuevoPartido.idEquipoLocal);
            bool existeEquipoVisitante = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, nuevoPartido.idEquipoVisitante);

            if ((!existeEquipoLocal) || (!existeEquipoVisitante)) {
                return false;
            }

            ArchivoHeader header = leerHeader(nombreArchivo);
            if (header.cantidadRegistros == error) {
                return false;
            }

            if (hayPartidoProgramadoEntre2(nombreArchivo, nuevoPartido)) {
                return false;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
            if (!archivo.is_open()) {
                return false;
            }

            int minimoRequerido = minJugadoresPorDeporte();
            /*
            // Verificar que ambos equipos tengan suficientes jugadores
            if (eqLocal.numJugadores < minimoRequerido || eqVisitante.numJugadores < minimoRequerido) {
                return false;
            }*/

            // * Procedemos a programar el partido
            nuevoPartido.ID = header.proximoID;

            // Asignamos los valores iniciales
            std::strncpy(nuevoPartido.estado, estadoPartidos[0], TAMANO_ESTADO); // PROGRAMADO
            nuevoPartido.anotacionesLocal = 0;
            nuevoPartido.anotacionesVisitante = 0;
            nuevoPartido.numAnotaciones = 0;
            nuevoPartido.eliminado = false;

            // Asignamos los valores de tiempo
            nuevoPartido.fechaCreacion = std::time(nullptr);
            nuevoPartido.fechaUltimaModificacion = std::time(nullptr);

            // Movemos el puntero de escritura al final para programar el nuevo partido
            archivo.seekp(0, std::ios::end);

            // * Guardamos el registro en el archivo
            archivo.write(reinterpret_cast<const char *>(&nuevoPartido), sizeof(Partido));

            // Verificamos que la escritura no arroje errores
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo
            archivo.close();

            // Modificamos los datos del header
            header.proximoID++;
            header.cantidadRegistros++;
            header.registrosActivos++;

            // Actualizamos el header
            actualizarHeader(nombreArchivo, header);
            return true;
        }

        // TODO:   Victoria local  → local  +3 pts, +1 victoria  / visitante +1 derrota
        // TODO:   Empate          → ambos  +1 pt,  +1 empate
        // TODO:   Victoria visit. → visit. +3 pts, +1 victoria  / local     +1 derrota
        bool registrarResultado(const char *nombreArchivo, Partido registroPartido) {
            int error = -1;

            Torneo torneo;
            if (!obtenerInformacionTorneo(torneo)) {
                return false;
            }

            // * Validaciones

            if (!existeArchivo(nombreArchivo) || !existeArchivo(NOMBRE_ARCHIVO_EQUIPOS) || !existeArchivo(NOMBRE_ARCHIVO_JUGADORES)) {
                return false;
            }

            // Leemos el header del archivo de Equipos
            ArchivoHeader headerEquipos = leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que el header se haya leido correctamente
            if (headerEquipos.cantidadRegistros == error) {
                return false;
            }

            // Si no hay al menos 2 equipos no se puede programar un partido
            if (headerEquipos.registrosActivos <= 1) {
                return false;
            }

            //  Ahora leemos el header de partidos para verificar que hayan partidos
            ArchivoHeader headerPartidos = leerHeader(nombreArchivo);

            // Verificamos que el header se haya leido correctamente
            if (headerPartidos.cantidadRegistros == error) {
                return false;
            }

            // Si no hay partidos activos no hacemos nada
            if (headerPartidos.registrosActivos == 0) {
                return false;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que esté abierto
            if (!archivo.is_open()) {
                return false;
            }

            Partido registroTemporal;
            int contador = 0;

            // Movemos el puntero de lectura
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Verificamos que haya partidos en estado programado
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Partido))) {

                // Verificamos que la lectura haya sido correcta
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                // Verificamos si el estado es programado
                if (std::strcmp(registroTemporal.estado, estadoPartidos[0]) == 0) {
                    contador++;
                }
            }

            // Verificamos que hay partidos
            if (contador <= 0) {
                archivo.close();
                return false;
            }
            archivo.clear(); // Limpiamos el estado de error de eof

            // Por seguridad
            if (registroPartido.anotacionesLocal < 0 || registroPartido.anotacionesVisitante < 0) {
                archivo.close();
                return false;
            }

            // Ademas verificamos que el numAnotaciones no sea mayor que el maximo
            if (registroPartido.numAnotaciones > MAX_ANOTACIONES) {
                archivo.close();
                return false;
            }

            if (registroPartido.numtarjetaAma > MAX_TARJETAS_AMARILLAS) {
                archivo.close();
                return false;
            }

            if (registroPartido.numTarjetasRojas > MAX_TARJETAS_ROJAS) {
                archivo.close();
                return false;
            }

            archivo.close();

            // * 1. Leemos el partido y verificamos que exsite

            // verificamos que el partido si exista (en estado programado);
            Partido nuevoPartido;
            bool existePartido = buscarRegistrosPorId<Partido>(nombreArchivo, nuevoPartido, registroPartido.ID);

            // Verificamos si encontro el partido
            if (!existePartido) {
                return false;
            }

            // Si encontró el partido verificamos que esté en estado programado
            if (std::strcmp(nuevoPartido.estado, estadoPartidos[0]) != 0) {
                return false;
            }

            // * 2. Leemos cada Equipo de los binarios

            Equipo eqLocal, eqVisitante;
            bool existe;

            existe = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, nuevoPartido.idEquipoLocal);

            // Verificamos por seguridad
            if (!existe) {
                archivo.close();
                return false;
            }

            existe = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, nuevoPartido.idEquipoVisitante);

            // Verificamos por seguridad
            if (!existe) {
                return false;
            }

            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
            // Verificamos que esté abierto
            if (!archivo.is_open()) {
                return false;
            }

            // * 3. Determinamos el resultado del partido y actualizamos las estadisticas en memoria de cada equipo

            // Si ocurre un empate en deportes donde no se permiten empates retornamos false
            if (registroPartido.anotacionesLocal == registroPartido.anotacionesVisitante) {
                if (std::strcmp(torneo.deporte, "BALONCESTO") == 0 || std::strcmp(torneo.deporte, "TENIS") == 0 || std::strcmp(torneo.deporte, "VOLEIBOL") == 0 ||
                    std::strcmp(torneo.deporte, "BEISBOL") == 0 || std::strcmp(torneo.deporte, "SOFTBOL") == 0) {
                    archivo.close();
                    return false;
                }
            }

            // Actualizamos las anotaciones/tarjetas del partido
            nuevoPartido.anotacionesLocal = registroPartido.anotacionesLocal;
            nuevoPartido.anotacionesVisitante = registroPartido.anotacionesVisitante;

            nuevoPartido.tarjetasAmaLocal = registroPartido.tarjetasAmaLocal;
            nuevoPartido.tarjetasAmaVisitante = registroPartido.tarjetasAmaVisitante;

            nuevoPartido.tarjetasRojasLocal = registroPartido.tarjetasRojasLocal;
            nuevoPartido.tarjetasRojasVisitante = registroPartido.tarjetasRojasVisitante;

            // Si el equipo local ganó
            if (registroPartido.anotacionesLocal > registroPartido.anotacionesVisitante) {
                eqLocal.victorias++; // Aumentamos las victorias del local
                eqLocal.puntos += 3; // Aumentamos los puntos del local

                eqVisitante.derrotas++; // Aumentamos las derrotas del visitante

                // Si el equipo visitante ganó
            } else if (registroPartido.anotacionesVisitante > registroPartido.anotacionesLocal) {
                eqVisitante.victorias++; // Aumentamos las victorias
                eqVisitante.puntos += 3; // Aumentamos los puntos

                eqLocal.derrotas++; // Aumentamos las derrotas

                // Si empataron en un deporte permitido
            } else {
                eqLocal.empates++;
                eqLocal.puntos += 1;

                eqVisitante.empates++;
                eqVisitante.puntos += 1;
            }

            // Añadimos los puntos a favor y en contra
            eqLocal.anotacionAFavor += registroPartido.anotacionesLocal;
            eqLocal.anotacionEnContra += registroPartido.anotacionesVisitante;
            eqLocal.jugados++;

            eqVisitante.anotacionAFavor += registroPartido.anotacionesVisitante;
            eqVisitante.anotacionEnContra += registroPartido.anotacionesLocal;
            eqVisitante.jugados++;

            // * 4. Agregamos el id del partido al array de cada equipo y aumentamos el numero de partidos
            if (eqLocal.cantidadPartidos >= 50 || eqVisitante.cantidadPartidos >= 50) {
                archivo.close();
                return false;
            }
            eqLocal.partidosIDs[eqLocal.cantidadPartidos] = registroPartido.ID;
            eqVisitante.partidosIDs[eqVisitante.cantidadPartidos] = registroPartido.ID;
            eqLocal.cantidadPartidos++;
            eqVisitante.cantidadPartidos++;

            // Agregamos la fecha de modificacion
            eqLocal.fechaUltimaModificacion = std::time(nullptr);
            eqVisitante.fechaUltimaModificacion = std::time(nullptr);

            // * 5. Registramos goles/tarjetas y detalle de goles/tarjetas
            nuevoPartido.numAnotaciones = registroPartido.numAnotaciones;
            nuevoPartido.numtarjetaAma = registroPartido.numtarjetaAma;
            nuevoPartido.numTarjetasRojas = registroPartido.numTarjetasRojas;

            // Puedo usar std::copy pero solo usaré for por el momento
            for (int e = 0; e < nuevoPartido.numAnotaciones; e++) {
                nuevoPartido.anotaciones[e] = registroPartido.anotaciones[e];
            }

            for (int e = 0; e < nuevoPartido.numtarjetaAma; e++) {
                nuevoPartido.tarjetaA[e] = registroPartido.tarjetaA[e];
            }

            for (int e = 0; e < nuevoPartido.numTarjetasRojas; e++) {
                nuevoPartido.tarjetaR[e] = registroPartido.tarjetaR[e];
            }

            // * 6. Actualizamos las estadisticas de los jugadores por cada gol / tarjeta

            Jugador jugadorAux;

            // Modificamos los goles
            for (int e = 0; e < registroPartido.numAnotaciones; e++) {

                // Si fue un autogol, saltamos esta iteracion
                if (registroPartido.anotaciones[e].idJugador == 0) {
                    continue;
                }

                // Buscamos el indice
                int indiceBuscado = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, nuevoPartido.anotaciones[e].idJugador);

                // Si ocurrió un error detenemos el proceso
                if (indiceBuscado == error) {
                    archivo.close();
                    return false;
                }

                std::fstream archivoJugadores;
                archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary | std::ios::in | std::ios::out);

                // Verificamos que esté abierto
                if (!archivoJugadores.is_open()) {
                    archivo.close();
                    return false;
                }

                // Calculamos la posicion a la que nos vamos  a mover
                std::streampos posicion = sizeof(ArchivoHeader) + indiceBuscado * sizeof(Jugador);

                // Nos movemos a esa posicion
                archivoJugadores.seekg(posicion, std::ios::beg);

                // Leemos ese registro
                archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                // modificamos los valores
                jugadorAux.anotaciones++;

                // Nos movemos a la posicion nuevamente
                archivoJugadores.seekp(posicion, std::ios::beg);

                // Escribimos el jugador
                archivoJugadores.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));

                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                archivoJugadores.close();
            }

            // Modificamos las tarjetas amarillas
            for (int e = 0; e < registroPartido.numtarjetaAma; e++) {
                // Buscamos el indice
                int indiceBuscado = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, nuevoPartido.tarjetaA[e].idJugador);

                // Si ocurrió un error detenemos el proceso
                if (indiceBuscado == error) {
                    archivo.close();
                    return false;
                }

                std::fstream archivoJugadores;
                archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary | std::ios::in | std::ios::out);

                // Verificamos que esté abierto
                if (!archivoJugadores.is_open()) {
                    archivo.close();
                    return false;
                }

                // Calculamos la posicion a la que nos vamos  a mover
                std::streampos posicion = sizeof(ArchivoHeader) + indiceBuscado * sizeof(Jugador);

                // Nos movemos a esa posicion
                archivoJugadores.seekg(posicion, std::ios::beg);

                // Leemos ese registro
                archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                // modificamos los valores
                jugadorAux.tarjetaAmarillas++;

                // Nos movemos a la posicion nuevamente
                archivoJugadores.seekp(posicion, std::ios::beg);

                // Escribimos el jugador
                archivoJugadores.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));

                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                archivoJugadores.close();
            }

            // Modificamos las tarjetas rojas
            for (int e = 0; e < registroPartido.numTarjetasRojas; e++) {
                // Buscamos el indice
                int indiceBuscado = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, nuevoPartido.tarjetaR[e].idJugador);

                // Si ocurrió un error detenemos el proceso
                if (indiceBuscado == error) {
                    archivo.close();
                    return false;
                }

                std::fstream archivoJugadores;
                archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary | std::ios::in | std::ios::out);

                // Verificamos que esté abierto
                if (!archivoJugadores.is_open()) {
                    archivo.close();
                    return false;
                }

                // Calculamos la posicion a la que nos vamos  a mover
                std::streampos posicion = sizeof(ArchivoHeader) + indiceBuscado * sizeof(Jugador);

                // Nos movemos a esa posicion
                archivoJugadores.seekg(posicion, std::ios::beg);

                // Leemos ese registro
                archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                // modificamos los valores
                jugadorAux.tarjetasRojas++;

                // Nos movemos a la posicion nuevamente
                archivoJugadores.seekp(posicion, std::ios::beg);

                // Escribimos el jugador
                archivoJugadores.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));

                if (archivoJugadores.fail()) {
                    archivoJugadores.close();
                    archivo.close();
                    return false;
                }

                archivoJugadores.close();
            }

            // * 7. Cambiamos el estado del partido a jugado
            std::strncpy(nuevoPartido.estado, estadoPartidos[1], TAMANO_ESTADO); // JUGADO
            nuevoPartido.fechaUltimaModificacion = std::time(nullptr);

            // * 8. Escribimos cada equipo en su repectivo archivo

            // Buscamos el indice de cada equipo
            int indiceLocal = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal.ID);
            int indiceVisitante = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante.ID);

            // Verificamos que no devuelvan error
            if (indiceLocal == error || indiceVisitante == error) {
                archivo.close();
                return false;
            }

            // Abrimos el archivo de equipos
            std::fstream archivoEquipos;
            archivoEquipos.open(NOMBRE_ARCHIVO_EQUIPOS, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que el archivo abrió
            if (!archivoEquipos.is_open()) {
                archivo.close();
                return false;
            }

            // Calculamos las posiciones
            std::streampos posicionLocal = sizeof(ArchivoHeader) + indiceLocal * sizeof(Equipo);
            std::streampos posicionVisitante = sizeof(ArchivoHeader) + indiceVisitante * sizeof(Equipo);

            // * Añadimos al Equipo Local

            // Nos movemos a la posicion
            archivoEquipos.seekp(posicionLocal, std::ios::beg);

            // Escribimos el equipo
            archivoEquipos.write(reinterpret_cast<const char *>(&eqLocal), sizeof(Equipo));

            // Verificamos si no dió error la escritura
            if (archivoEquipos.fail()) {
                archivoEquipos.close();
                archivo.close();
                return false;
            }

            // * Añadimos al Equipo Visitante

            // Nos movemos a la posicion
            archivoEquipos.seekp(posicionVisitante, std::ios::beg);

            // Escribimos el equipo
            archivoEquipos.write(reinterpret_cast<const char *>(&eqVisitante), sizeof(Equipo));

            // Verificamos si no dió error
            if (archivoEquipos.fail()) {
                archivoEquipos.close();
                archivo.close();
                return false;
            }

            // cerramos el archivo
            archivoEquipos.close();
            archivo.close();

            // * 9. Guardamos el partido
            int indice = buscarIndicePorID<Partido>(nombreArchivo, nuevoPartido.ID);

            // Verificamos que no devuelva error
            if (indice == error) {
                return false;
            }

            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);
            // Verificamos que esté abierto
            if (!archivo.is_open()) {
                return false;
            }

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Partido);

            // Nos movemos a esa posicion
            archivo.seekp(posicion, std::ios::beg);

            // Sobreescribimos el partido
            archivo.write(reinterpret_cast<const char *>(&nuevoPartido), sizeof(Partido));

            // Verificamos que no devuelva error
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Cerramos el archivo
            archivo.close();
            return true;
        }

        int listarPartidosPorEquipo(const char *nombreArchivo, const int idEquipo, Partido resultados[], int maxResultados) {
            int cantidadDeRegistrosEncontrados = 0;
            int error = -1;
            Partido registroTemporal;
            ArchivoHeader header;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return error;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que se haya abierto correctamente
            if (!archivo.is_open()) {
                return error;
            }

            // Leemos el header
            header = leerHeader(NOMBRE_ARCHIVO_PARTIDOS);

            // Ubicamos el puntero de posicion despues del header
            archivo.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Realizamos la lectura del archivo
            while (archivo.read(reinterpret_cast<char *>(&registroTemporal), sizeof(Partido))) {

                // Verificamos si no hubo un fallo en la lectura
                if (archivo.fail()) {
                    archivo.close();
                    return error;
                }

                // Si encontramos un archivo que esta eliminado lo saltamos
                if (registroTemporal.eliminado) {
                    continue;
                }

                // Si aún no llegamos a la cantidad maxima de registros hacemos la comparacion
                if (cantidadDeRegistrosEncontrados < maxResultados) {

                    // comparamos con los estados
                    if (registroTemporal.idEquipoLocal == idEquipo || registroTemporal.idEquipoVisitante == idEquipo) {
                        resultados[cantidadDeRegistrosEncontrados] = registroTemporal;
                        cantidadDeRegistrosEncontrados++;
                    }

                } else {
                    // sino salimos del bucle
                    break;
                }
            }

            // Verificamos que se hayan leido todos los registros
            if (header.registrosActivos < cantidadDeRegistrosEncontrados) {
                return error;
            }

            archivo.close();
            return cantidadDeRegistrosEncontrados;
        }

        bool cancelarPartido(const char *nombreArchivo, const int idPartido) {
            int error = -1;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return false;
            }

            std::fstream archivo;
            archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos que el archivo este abierto
            if (!archivo.is_open()) {
                return false;
            }

            // * Buscar y leer el Partido
            Partido pAux;
            int indice = buscarIndicePorID<Partido>(nombreArchivo, idPartido);

            // Verificamos que el indice no sea paramtro de error (que sea -1)
            if (indice == error) {
                archivo.close();
                return false;
            }

            // Calculamos la posicion
            std::streampos posicion = sizeof(ArchivoHeader) + indice * sizeof(Partido);

            // Movemos el puntero de lectura a esa posicion
            archivo.seekg(posicion, std::ios::beg);

            archivo.read(reinterpret_cast<char *>(&pAux), sizeof(Partido));

            // Verificamos que no se produjo un error
            if (archivo.fail()) {
                archivo.close();
                return false;
            }

            // Por seguridad (No sea el caso de que haya un bug o un error no se)
            if (pAux.eliminado) {
                return false;
            }
            archivo.close();

            // Si está jugado debemos revertir todo
            if (std::strcmp(pAux.estado, estadoPartidos[1]) == 0) {

                // * 1. Buscamos los equipos para revertir las estadisticas

                Equipo eqLocal, eqVisitante;

                // Buscamos el indice fisico
                int indiceLocal = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, pAux.idEquipoLocal);
                int indiceVisitante = buscarIndicePorID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, pAux.idEquipoVisitante);

                // Verificamos que la lectura de los indices fue correcta
                if (indiceLocal == error || indiceVisitante == error) {
                    return false;
                }

                std::fstream archivoEquipos;
                archivoEquipos.open(NOMBRE_ARCHIVO_EQUIPOS, std::ios::binary | std::ios::in | std::ios::out);

                // Verificamos que esté abierto
                if (!archivoEquipos.is_open()) {
                    return false;
                }

                // Calculamos la posicion
                std::streampos posicionLocal = sizeof(ArchivoHeader) + indiceLocal * sizeof(Equipo);
                std::streampos posicionVisitante = sizeof(ArchivoHeader) + indiceVisitante * sizeof(Equipo);

                // Obtenemos el equipo local primero
                archivoEquipos.seekg(posicionLocal, std::ios::beg);
                archivoEquipos.read(reinterpret_cast<char *>(&eqLocal), sizeof(Equipo));

                // Verificamos si falló
                if (archivoEquipos.fail()) {
                    archivoEquipos.close();
                    return false;
                }

                // Luego el equipo visitante
                archivoEquipos.seekg(posicionVisitante, std::ios::beg);
                archivoEquipos.read(reinterpret_cast<char *>(&eqVisitante), sizeof(Equipo));

                // Verificamos si falló
                if (archivoEquipos.fail()) {
                    archivoEquipos.close();
                    return false;
                }

                // Si el equipo local ganó
                if (pAux.anotacionesLocal > pAux.anotacionesVisitante) {
                    eqLocal.victorias--; // Revertimos las victorias del local
                    eqLocal.puntos -= 3; // Revertimos los puntos del local

                    eqVisitante.derrotas--; // Revertimos las derrotas del visitante

                    // Si el equipo visitante ganó
                } else if (pAux.anotacionesVisitante > pAux.anotacionesLocal) {
                    eqVisitante.victorias--; // Revertimos las victorias
                    eqVisitante.puntos -= 3; // Revertimos los puntos

                    eqLocal.derrotas--; // Revertimos las derrotas

                    // Si empataron en un deporte permitido
                } else {
                    eqLocal.empates--;   // Revertimos los empates
                    eqLocal.puntos -= 1; // Revertimos los puntos

                    eqVisitante.empates--;   // Revertimos los empates
                    eqVisitante.puntos -= 1; // Revertimos los puntos
                }

                // Quitamos los puntos a favor y en contra
                eqLocal.anotacionAFavor -= pAux.anotacionesLocal;
                eqLocal.anotacionEnContra -= pAux.anotacionesVisitante;
                eqLocal.jugados--;

                eqVisitante.anotacionAFavor -= pAux.anotacionesVisitante;
                eqVisitante.anotacionEnContra -= pAux.anotacionesLocal;
                eqVisitante.jugados--;

                // Borramos el id en la posicion del arreglo en la que esté y luego lo reducimos (para local y visitante)
                for (int e = 0; e < eqLocal.cantidadPartidos; e++) {
                    if (eqLocal.partidosIDs[e] == idPartido) {
                        for (int r = e; r < eqLocal.cantidadPartidos - 1; r++) {
                            eqLocal.partidosIDs[r] = eqLocal.partidosIDs[r + 1];
                        }
                        eqLocal.partidosIDs[eqLocal.cantidadPartidos - 1] = -1;
                        eqLocal.cantidadPartidos--;
                        break;
                    }
                }
                for (int e = 0; e < eqVisitante.cantidadPartidos; e++) {
                    if (eqVisitante.partidosIDs[e] == idPartido) {
                        for (int r = e; r < eqVisitante.cantidadPartidos - 1; r++) {
                            eqVisitante.partidosIDs[r] = eqVisitante.partidosIDs[r + 1];
                        }
                        eqVisitante.partidosIDs[eqVisitante.cantidadPartidos - 1] = -1;
                        eqVisitante.cantidadPartidos--;
                        break;
                    }
                }

                // Agregamos la fecha de modificacion
                eqLocal.fechaUltimaModificacion = std::time(nullptr);
                eqVisitante.fechaUltimaModificacion = std::time(nullptr);

                // * 2. Buscamos los jugadores para revertir estadisticas

                // Limpiamos los goles y los detalles de cada gol
                for (int e = 0; e < pAux.numAnotaciones; e++) {

                    if (pAux.anotaciones[e].idJugador == 0) {
                        continue;
                    }

                    Jugador jugadorAux;

                    // Buscamos el índice jugador que realió la anotacion
                    int indiceJugador = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, pAux.anotaciones[e].idJugador);
                    if (indiceJugador == error) {
                        return false;
                    }

                    // Abrimos el archivo de jugadores
                    std::fstream archivoJugadores;
                    archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary | std::ios::out | std::ios::in);

                    // Verificamos si falló
                    if (archivoJugadores.fail()) {
                        archivoEquipos.close();
                        return false;
                    }

                    // Calculamos la posicion
                    std::streampos posicionJugador = sizeof(ArchivoHeader) + indiceJugador * sizeof(Jugador);

                    // Movemos el puntero de lectura
                    archivoJugadores.seekg(posicionJugador, std::ios::beg);

                    // Realizamos la lectura
                    archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        return false;
                    }

                    // Modificamos el valor
                    jugadorAux.anotaciones--;

                    // Movemos el puntero de escritura
                    archivoJugadores.seekp(posicionJugador, std::ios::beg);

                    // Sobreescribimos el archivo
                    archivoJugadores.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        return false;
                    }
                    pAux.anotaciones[e] = {0, 0, 0};

                    archivoJugadores.close();
                }

                pAux.anotacionesLocal = 0;
                pAux.anotacionesVisitante = 0;
                pAux.numAnotaciones = 0;

                // Limpiamos las tarjetas amarillas y los detalles de cada tarjeta Amarilla
                for (int e = 0; e < pAux.numtarjetaAma; e++) {

                    if (pAux.tarjetaA[e].idJugador == 0) {
                        continue;
                    }

                    Jugador jugadorAux;

                    // Buscamos el índice jugador que realió la anotacion
                    int indiceJugador = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, pAux.tarjetaA[e].idJugador);
                    if (indiceJugador == error) {
                        return false;
                    }

                    // Abrimos el archivo de jugadores
                    std::fstream archivoJugadores;
                    archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary | std::ios::out | std::ios::in);

                    // Verificamos si falló
                    if (archivoJugadores.fail()) {
                        archivoEquipos.close();
                        return false;
                    }

                    // Calculamos la posicion
                    std::streampos posicionJugador = sizeof(ArchivoHeader) + indiceJugador * sizeof(Jugador);

                    // Movemos el puntero de lectura
                    archivoJugadores.seekg(posicionJugador, std::ios::beg);

                    // Realizamos la lectura
                    archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        return false;
                    }

                    // Modificamos el valor
                    jugadorAux.tarjetaAmarillas--;

                    // Movemos el puntero de escritura
                    archivoJugadores.seekp(posicionJugador, std::ios::beg);

                    // Sobreescribimos el archivo
                    archivoJugadores.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        return false;
                    }
                    pAux.tarjetaA[e] = {0, 0, 0};

                    archivoJugadores.close();
                }

                pAux.tarjetasAmaLocal = 0;
                pAux.tarjetasAmaVisitante = 0;
                pAux.numtarjetaAma = 0;

                // Limpiamos las tarjetas rojas y los detalles de cada tarjeta roja
                for (int e = 0; e < pAux.numTarjetasRojas; e++) {

                    Jugador jugadorAux;

                    if (pAux.tarjetaR[e].idJugador == 0) {
                        continue;
                    }

                    // Buscamos el índice jugador que realió la anotacion
                    int indiceJugador = buscarIndicePorID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, pAux.tarjetaR[e].idJugador);
                    if (indiceJugador == error) {
                        return false;
                    }

                    // Abrimos el archivo de jugadores
                    std::fstream archivoJugadores;
                    archivoJugadores.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary | std::ios::out | std::ios::in);

                    // Verificamos si falló
                    if (archivoJugadores.fail()) {
                        archivoEquipos.close();
                        return false;
                    }

                    // Calculamos la posicion
                    std::streampos posicionJugador = sizeof(ArchivoHeader) + indiceJugador * sizeof(Jugador);

                    // Movemos el puntero de lectura
                    archivoJugadores.seekg(posicionJugador, std::ios::beg);

                    // Realizamos la lectura
                    archivoJugadores.read(reinterpret_cast<char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        return false;
                    }

                    // Modificamos el valor
                    jugadorAux.tarjetasRojas--;

                    // Movemos el puntero de escritura
                    archivoJugadores.seekp(posicionJugador, std::ios::beg);

                    // Sobreescribimos el archivo
                    archivoJugadores.write(reinterpret_cast<const char *>(&jugadorAux), sizeof(Jugador));

                    // Verificamos que no falló
                    if (archivoJugadores.fail()) {
                        archivoJugadores.close();
                        archivoEquipos.close();
                        return false;
                    }
                    pAux.tarjetaR[e] = {0, 0, 0};

                    archivoJugadores.close();
                }

                pAux.tarjetasRojasLocal = 0;
                pAux.tarjetasRojasVisitante = 0;
                pAux.numTarjetasRojas = 0;

                // * Sobreescribimos los equipos

                // Primero el equipo local
                archivoEquipos.seekp(posicionLocal, std::ios::beg);
                archivoEquipos.write(reinterpret_cast<const char *>(&eqLocal), sizeof(Equipo));

                // Verificamos si falló
                if (archivoEquipos.fail()) {
                    archivoEquipos.close();
                    return false;
                }

                // Luego el equipo visitante
                archivoEquipos.seekp(posicionVisitante, std::ios::beg);
                archivoEquipos.write(reinterpret_cast<const char *>(&eqVisitante), sizeof(Equipo));

                // Verificamos si falló
                if (archivoEquipos.fail()) {
                    archivoEquipos.close();
                    return false;
                }

                // Cerramos el archivo
                archivoEquipos.close();

                archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

                // Verificamos que el archivo este abierto
                if (!archivo.is_open()) {
                    return false;
                }

                // Colocamos el partido como cancelado
                std::strncpy(pAux.estado, estadoPartidos[2], TAMANO_ESTADO);

                pAux.fechaUltimaModificacion = std::time(nullptr);
                // pAux.eliminado = true;

                // Movemos el puntero de escritura a la posicion
                archivo.seekp(posicion, std::ios::beg);

                // Sobreescribimos el archivo
                archivo.write(reinterpret_cast<const char *>(&pAux), sizeof(Partido));

                // Verificamos que no se produjo un error
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                archivo.close();

                return true;

                // Si está programado solo lo colocamos como eliminado
            } else if (std::strcmp(pAux.estado, estadoPartidos[0]) == 0) {

                archivo.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

                // Verificamos que el archivo este abierto
                if (!archivo.is_open()) {
                    return false;
                }

                // Colocamos el partido como cancelado
                std::strncpy(pAux.estado, estadoPartidos[2], TAMANO_ESTADO);

                // pAux.eliminado = true;
                pAux.fechaUltimaModificacion = std::time(nullptr);

                // Movemos el puntero de escritura a la posicion
                archivo.seekp(posicion, std::ios::beg);

                // Sobreescribimos el archivo
                archivo.write(reinterpret_cast<const char *>(&pAux), sizeof(Partido));

                // Verificamos que no se produjo un error
                if (archivo.fail()) {
                    archivo.close();
                    return false;
                }

                ArchivoHeader headerPartidos = leerHeader(nombreArchivo);
                if (headerPartidos.cantidadRegistros == -1) {
                    return false;
                }
                headerPartidos.registrosActivos--;
                actualizarHeader(nombreArchivo, headerPartidos);

                archivo.close();
                return true;

            } else {

                archivo.close();
                return false; // Si está cancelado devolvemos error
            }
        }

        /*bool eliminarPartido() {
            //
        }*/
    } // namespace partidos

    namespace reportes {

        int tablaDeGoleadoresTop10(const char *nombreArchivo, Jugador registros[]) {
            // inicializamos en 0 por si no pasa las validaciones
            int difAnotaciones = 0;
            bool intercambiar = false;
            int cantidadDeRegistros = 0;
            int error = -1;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return error;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que se haya abierto correctamente
            if (!archivo.is_open()) {
                return error;
            }

            // Mediante la funcion listarRegistros obtenemos los registros de equipos guardados y la cantidad
            cantidadDeRegistros = listarRegistros<Jugador>(nombreArchivo, registros, MAX_RESULTADOS);

            // verificamos que la funcion no devuelva error
            if (cantidadDeRegistros == error) {
                return error;
            }

            // Ordenamos por cantidad de anotaciones
            // Restamos 1 para no acceder a memoria indebida
            for (int e = 0; e < cantidadDeRegistros - 1; e++) {
                for (int r = 0; r < cantidadDeRegistros - e - 1; r++) {

                    intercambiar = false;

                    // Si el jugador 1 tiene menos anotaciones que el jugador 2;
                    if (registros[r].anotaciones < registros[r + 1].anotaciones) {
                        intercambiar = true;

                        // Si poseen igual cantidad de anotaciones
                    } // Aqui puedo colocar otros criterios de desempate a futuro

                    // Si se cumple alguna condicion hacemos el intercambio
                    if (intercambiar) {
                        // Guardamos el jugador con menos puntos en una variable auxiliar
                        Jugador registroTemporal = registros[r];

                        // Luego movemos el equipo mayor a la posicion donde estaba el menor
                        registros[r] = registros[r + 1];

                        // colocamos en la nueva posicion al equipo con menos puntos
                        registros[r + 1] = registroTemporal;
                    }
                }
            }

            archivo.close();
            return cantidadDeRegistros;
        }

        int tablaDeTarjetasAmarillasTop10(const char *nombreArchivo, Jugador registros[]) {
            // inicializamos en 0 por si no pasa las validaciones
            bool intercambiar = false;
            int cantidadDeRegistros = 0;
            int error = -1;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return error;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que se haya abierto correctamente
            if (!archivo.is_open()) {
                return error;
            }

            // Mediante la funcion listarRegistros obtenemos los registros de equipos guardados y la cantidad
            cantidadDeRegistros = listarRegistros<Jugador>(nombreArchivo, registros, MAX_RESULTADOS);

            // verificamos que la funcion no devuelva error
            if (cantidadDeRegistros == error) {
                return error;
            }

            // Ordenamos por cantidad de anotaciones
            // Restamos 1 para no acceder a memoria indebida
            for (int e = 0; e < cantidadDeRegistros - 1; e++) {
                for (int r = 0; r < cantidadDeRegistros - e - 1; r++) {

                    intercambiar = false;

                    // Si el jugador 1 tiene menos anotaciones que el jugador 2;
                    if (registros[r].tarjetaAmarillas < registros[r + 1].tarjetaAmarillas) {
                        intercambiar = true;

                        // Si poseen igual cantidad de anotaciones
                    } // Aqui puedo colocar otros criterios de desempate a futuro

                    // Si se cumple alguna condicion hacemos el intercambio
                    if (intercambiar) {
                        // Guardamos el jugador con menos puntos en una variable auxiliar
                        Jugador registroTemporal = registros[r];

                        // Luego movemos el equipo mayor a la posicion donde estaba el menor
                        registros[r] = registros[r + 1];

                        // colocamos en la nueva posicion al equipo con menos puntos
                        registros[r + 1] = registroTemporal;
                    }
                }
            }

            archivo.close();
            return cantidadDeRegistros;
        }

        int tablaDeTarjetasRojasTop10(const char *nombreArchivo, Jugador registros[]) {
            // inicializamos en 0 por si no pasa las validaciones
            bool intercambiar = false;
            int cantidadDeRegistros = 0;
            int error = -1;

            // verificamos que si existe ese archivo
            if (!existeArchivo(nombreArchivo)) {
                return error;
            }

            std::ifstream archivo;
            archivo.open(nombreArchivo, std::ios::binary);

            // Verificamos que se haya abierto correctamente
            if (!archivo.is_open()) {
                return error;
            }

            // Mediante la funcion listarRegistros obtenemos los registros de equipos guardados y la cantidad
            cantidadDeRegistros = listarRegistros<Jugador>(nombreArchivo, registros, MAX_RESULTADOS);

            // verificamos que la funcion no devuelva error
            if (cantidadDeRegistros == error) {
                return error;
            }

            // Ordenamos por cantidad de tarjetas rojas
            // Restamos 1 para no acceder a memoria indebida
            for (int e = 0; e < cantidadDeRegistros - 1; e++) {
                for (int r = 0; r < cantidadDeRegistros - e - 1; r++) {

                    intercambiar = false;

                    // Si el jugador 1 tiene menos anotaciones que el jugador 2;
                    if (registros[r].tarjetasRojas < registros[r + 1].tarjetasRojas) {
                        intercambiar = true;

                        // Si poseen igual cantidad de anotaciones
                    } // Aqui puedo colocar otros criterios de desempate a futuro

                    // Si se cumple alguna condicion hacemos el intercambio
                    if (intercambiar) {
                        // Guardamos el jugador con menos puntos en una variable auxiliar
                        Jugador registroTemporal = registros[r];

                        // Luego movemos el equipo mayor a la posicion donde estaba el menor
                        registros[r] = registros[r + 1];

                        // colocamos en la nueva posicion al equipo con menos puntos
                        registros[r + 1] = registroTemporal;
                    }
                }
            }

            archivo.close();
            return cantidadDeRegistros;
        }

    } // namespace reportes

    namespace mantenimiento {

        bool copiarArchivoBinario(const char *rutaOriginal, const char *rutaDestino) {
            std::ifstream archivoOrigen(rutaOriginal, std::ios::binary);
            if (!archivoOrigen.is_open()) {
                return false;
            }

            std::ofstream archivoDestino(rutaDestino, std::ios::binary | std::ios::trunc);
            if (!archivoDestino.is_open()) {
                archivoOrigen.close();
                return false;
            }

            archivoDestino << archivoOrigen.rdbuf(); // Copia todo directamente
            archivoOrigen.close();
            archivoDestino.close();

            // return true;
            return !archivoOrigen.bad() && !archivoDestino.fail();
        }

        // Lo coloqué bool para que retornemos false si ocurrió un error
        bool verificarIntegridadReferencial(InformeIntegridad &informe) {

            // * Paso 1. Recorrer ArchivoJugadores.bin

            /*// Abrimos el archivo de jugadores
            std::ifstream archivo;
            archivo.open(NOMBRE_ARCHIVO_JUGADORES, std::ios::binary);*/
            ArchivoHeader headerEquipos = leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            /*// Verificamos que se haya abierto correctamente
            if (!archivo.is_open()) {
                return false;
            }*/

            if (headerEquipos.registrosActivos > 0) {
                informe.equiposVerificados = headerEquipos.registrosActivos;
            }

            Jugador listaDeJugadores[MAX_RESULTADOS];
            int cantJugadores = listarRegistros<Jugador>(NOMBRE_ARCHIVO_JUGADORES, listaDeJugadores, MAX_RESULTADOS);

            if (cantJugadores == MI_ERROR) {
                return false;
            }

            // Por cada jugador
            for (size_t e = 0; e < cantJugadores; e++) {

                // Buscamos el equipo del jugador para ver si existe
                Equipo eqAux;
                bool existeEquipo = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqAux, listaDeJugadores[e].idEquipo);

                if (!existeEquipo && informe.totalDeReferenciasRotas < MAX_RESULTADOS) {
                    ReferenciaRota refAux;
                    std::strncpy(refAux.tipoDeReferencia, "JUGADOR", TAMANO_FORMATO);
                    refAux.idOrigen = listaDeJugadores[e].ID;
                    refAux.idReferenciaRota = listaDeJugadores[e].idEquipo;
                    informe.rotas[informe.totalDeReferenciasRotas] = refAux;
                    informe.totalDeReferenciasRotas++;
                }

                informe.jugadoresVerificados++;
            }

            // Ahora recorremos los partidos
            Partido listaDePartidos[MAX_RESULTADOS];
            int cantPartidos = listarRegistros<Partido>(NOMBRE_ARCHIVO_PARTIDOS, listaDePartidos, MAX_RESULTADOS);

            if (cantPartidos == MI_ERROR) {
                return false;
            }

            // Revismaos cada partido
            for (size_t e = 0; e < cantPartidos; e++) {
                Equipo eqAux;
                bool existe = false;

                // Buscamos el equipo local
                existe = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqAux, listaDePartidos[e].idEquipoLocal);

                if (!existe && informe.totalDeReferenciasRotas < MAX_RESULTADOS) {
                    ReferenciaRota refAux;
                    std::strncpy(refAux.tipoDeReferencia, "PARTIDO_LOCAL", TAMANO_FORMATO);
                    refAux.idOrigen = listaDePartidos[e].ID;
                    refAux.idReferenciaRota = listaDePartidos[e].idEquipoLocal;
                    informe.rotas[informe.totalDeReferenciasRotas] = refAux;
                    informe.totalDeReferenciasRotas++;
                }

                // Buscamos el equipo visitante
                existe = buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqAux, listaDePartidos[e].idEquipoVisitante);

                if (!existe && informe.totalDeReferenciasRotas < MAX_RESULTADOS) {
                    ReferenciaRota refAux;
                    std::strncpy(refAux.tipoDeReferencia, "PARTIDO_VISITANTE", TAMANO_FORMATO);
                    refAux.idOrigen = listaDePartidos[e].ID;
                    refAux.idReferenciaRota = listaDePartidos[e].idEquipoVisitante;
                    informe.rotas[informe.totalDeReferenciasRotas] = refAux;
                    informe.totalDeReferenciasRotas++;
                }

                // Recorremos por cada gol
                for (size_t r = 0; r < listaDePartidos[e].numAnotaciones; r++) {

                    // Si es un autogol por ejemplo
                    if (listaDePartidos[e].anotaciones[r].idJugador <= 0) {
                        continue;
                    }

                    Jugador jugadorAux;

                    bool existe = buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, listaDePartidos[e].anotaciones[r].idJugador);

                    // si no se encontró nada
                    if (!existe && informe.totalDeReferenciasRotas < MAX_RESULTADOS) {
                        ReferenciaRota refAux;
                        std::strncpy(refAux.tipoDeReferencia, "GOL", TAMANO_FORMATO);
                        refAux.idOrigen = listaDePartidos[e].ID;
                        refAux.idReferenciaRota = listaDePartidos[e].anotaciones[r].idJugador;
                        informe.rotas[informe.totalDeReferenciasRotas] = refAux;
                        informe.totalDeReferenciasRotas++;
                    }
                }

                informe.partidosVerificados++;
            }

            return true;
            auxiliares::pausarPrograma();
        }

        bool crearBackup(std::string &nombreCarpeta) {

            // 1. Obtenemos el tiempo actual
            std::time_t tiempoActual = std::time(nullptr);
            std::tm *infoTiempo = std::localtime(&tiempoActual);

            // 2. Formatear el nombre de la carpeta (backup_YYYY-MM-DD_HH-MM)
            char bufferNombre[50];
            std::strftime(bufferNombre, sizeof(bufferNombre), "backup_%Y-%m-%d_%H-%M", infoTiempo);
            nombreCarpeta = bufferNombre;

            // Creamos la carpeta
            if (!fs::create_directory(RUTA_BACKUPS + nombreCarpeta)) {
                // std::cerr << "Error: No se pudo crear la carpeta de respaldo.\n";
                return false;
            }

            // Variable Auxiliar
            std::string rutaAux;

            // 3. Copiar los 4 archivos uno por uno
            rutaAux = RUTA_BACKUPS + nombreCarpeta + "/equipos.bin";
            bool ok1 = copiarArchivoBinario(NOMBRE_ARCHIVO_EQUIPOS, rutaAux.c_str());
            rutaAux = RUTA_BACKUPS + nombreCarpeta + "/partidos.bin";
            bool ok2 = copiarArchivoBinario(NOMBRE_ARCHIVO_PARTIDOS, rutaAux.c_str());
            rutaAux = RUTA_BACKUPS + nombreCarpeta + "/jugadores.bin";
            bool ok3 = copiarArchivoBinario(NOMBRE_ARCHIVO_JUGADORES, rutaAux.c_str());
            rutaAux = RUTA_BACKUPS + nombreCarpeta + "/torneo.bin";
            bool ok4 = copiarArchivoBinario(NOMBRE_ARCHIVO_TORNEO, rutaAux.c_str());

            // 4. Verificar e informar el resultado
            if (ok1 && ok2 && ok3 && ok4) {
                return true;
            } else {
                return false;
            }

            return true;
        }
    } // namespace mantenimiento

} // namespace logica

// ============================================//
//   6. PRESENTACION                           //
// ============================================//

namespace presentacion {

    bool crearCarpeta(fs::path rutaDatos, std::string nombre) {
        auxiliares::limpiarPantalla();

        try {
            // create_directories crea toda la ruta y devuelve false si ya existe
            if (fs::create_directories(rutaDatos)) {
                std::cout << "\n Carpeta '" << nombre << "' creada en: " << rutaDatos << std::endl;
                return true;
            } else {
                // Entra aqui si la carpeta ya existia (no hace nada ni da error)
                std::cout << "\n La carpeta '" << nombre << "' ya existe. No se realizaron cambios." << std::endl;
                return true;
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "\n Error de permisos o sistema: " << e.what() << std::endl;
            return false;
        }
    }

    void mensajeSalida() {
        auxiliares::limpiarPantalla();
        std::cout << "\n Saliendo... \n";
        auxiliares::waitfor(2500);
    }

    void mensajeMenuPrincipal() {
        auxiliares::limpiarPantalla();
        std::cout << "\n Volviendo al Menú Principal... \n";
        auxiliares::waitfor(2500);
    }

    void mensajeMenuAnterior() {
        auxiliares::limpiarPantalla();
        std::cout << "\n Volviendo al menu anterior... \n";
        auxiliares::waitfor(2500);
    }

    void mensajeDefault() {
        auxiliares::limpiarPantalla();
        std::cout << "\n Error: Has ingresado una opcion inválida. Intentalo nuevamente.\n";
        auxiliares::waitfor(2500);
    }

    namespace equipos {

        // Recolectamos los datos para registrar el equipo
        void RegistrarEquipos(const char *nombreArchivo) {
            bool flagError = false;
            bool cancelado = false;
            Equipo nuevo;
            char confirmacion;

            // Buscamos y leemos el nombre del torneo
            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error del Sistema \n";
                auxiliares::pausarPrograma();
                return;
            }

            // Recolectamos el nombre del Equipo
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarCadena(nuevo.nombre, TAMANO_NOMBRE, " Ingrese el nombre del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                validadores::Nombres)) {
                    std::cout << "\n Registro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Validamos nombre duplicado
                if (logica::cadenaDuplicada<Equipo>(nombreArchivo, nuevo.nombre, &Equipo::nombre)) {
                    std::cerr << "Error, el nombre '" << nuevo.nombre << "' ya está en uso\n";
                    flagError = true;
                    auxiliares::waitfor(3000);
                    continue;
                }
                auxiliares::waitfor(1200);
            } while (flagError);


            // Recolectamos el nombre del entrenador del nuevo Equipo
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarCadena(nuevo.entrenador, TAMANO_NOMBRE, " Ingrese el nombre del Entrenador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                validadores::Nombres)) {
                    std::cout << "\n Registro cancelado por el usuario. \n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Validamos nombre duplicado
                if (logica::cadenaDuplicada(NOMBRE_ARCHIVO_EQUIPOS, nuevo.entrenador, &Equipo::entrenador)) {
                    std::cerr << "Error, el nombre '" << nuevo.entrenador << "' ya direge otro equipo\n";
                    flagError = true;
                    auxiliares::waitfor(3000);
                    continue;
                }
                auxiliares::waitfor(1200);
            } while (flagError);

            // Recolectamos la fecha de registro del equipo
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " Fecha de Inicio del Torneo: " << torneo.fechaInicio << std::endl;
            if (!auxiliares::ingresarCadena(nuevo.fechaRegistro, TAMANO_FECHA, " Ingrese la fecha de Registro del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado,
                                            validadores::fechaValidaRegistroDeJugadorOEquipo)) {
                std::cout << "\n Registro cancelado por el usuario. \n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(1200);
            auxiliares::limpiarPantalla();

            // Recolectamos la ciudad del Equipo
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO EQUIPO           ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(nuevo.ciudad, TAMANO_NOMBRE, " Ingrese el nombre de la Ciudad del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado,
                                            validadores::Nombres)) {
                std::cout << "\n Registro cancelado por el usuario. \n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(1200);
            auxiliares::limpiarPantalla();

            // Pedimos la confirmacion al usuario
            auxiliares::ingresarDatos(confirmacion, " ¿Está seguro de que desea registrar este equipo? (S/N): ");

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                auxiliares::limpiarPantalla();
                bool registrado = logica::equipos::registrarEquipo(nombreArchivo, nuevo);
                if (registrado) {
                    // Si el equipo se creo conn éxito
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║        EQUIPO REGISTRADO CON ÉXITO        ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                    // Mostramos los datos ingresados
                    std::cout << " Torneo: " << torneo.nombre << std::endl;
                    std::cout << " Nombre del Equipo: " << nuevo.nombre << std::endl;
                    std::cout << " ID del Equipo: " << nuevo.ID << std::endl;
                    std::cout << " Entrenador del Equipo: " << nuevo.entrenador << std::endl;
                    std::cout << " Ciudad del Equipo: " << nuevo.ciudad << std::endl;
                    std::cout << " Fecha de Registro del Equipo: " << nuevo.fechaRegistro << std::endl;
                } else {
                    std::cerr << "\n Se produjo un error a la hora de registrar el equipo \n";
                }

            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                auxiliares::limpiarPantalla();
                std::cout << "\n Registro de Equipo Cancelado\n";
            } else {
                auxiliares::limpiarPantalla();
                std::cerr << "\n Error: No se ingresó una opción correcta (S/N)\n";
                std::cout << "\n Registro de Equipo Cancelado\n";
            }
            auxiliares::pausarPrograma();
        }

        void buscarEquipoPorID(const char *nombreArchivo) {
            int ID = 0;
            bool cancelado = false;
            int error = -1;
            bool equipoEncontrado = false;
            Equipo equipoBuscado;
            auxiliares::limpiarPantalla();

            // Leemos el header del archivo de equipo para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = logica::leerHeader(nombreArchivo);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\n Error del Sistema!\n";
                std::cout << " Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << " No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Pedimos los datos de busqueda
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║        BUSQUEDA DE EQUIPOS POR ID         ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarDatos(ID, " Ingrese el ID (escriba 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "\n Busqueda cancelada por el usuario. \n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo
            equipoEncontrado = logica::buscarRegistrosPorId<Equipo>(nombreArchivo, equipoBuscado, ID);

            // Limpiamos la pantalla
            auxiliares::limpiarPantalla();
            std::cout << "\n Buscando...\n";
            auxiliares::waitfor(1500);

            // Si no fue encontrado enviamos mensaje de error de busqueda, si se encontro mostramos los datos
            if (!equipoEncontrado) {
                std::cout << "\n El equipo de ID " << ID << " no fue encontrado\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║             EQUIPO ENCONTRADO             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Información del Equipo:\n";
                std::cout << "    ID del Equipo:       " << equipoBuscado.ID << "\n";
                std::cout << "    Nombre:              " << equipoBuscado.nombre << "\n";
                std::cout << "    Entrenador:          " << equipoBuscado.entrenador << "\n";
                std::cout << "    Ciudad Origen:       " << equipoBuscado.ciudad << "\n";
                std::cout << "    Fecha de Registro:   " << equipoBuscado.fechaRegistro << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Estadísticas en el Torneo:\n";
                std::cout << "    Puntos Totales:    " << equipoBuscado.puntos << "\n";
                std::cout << "    Victorias:         " << equipoBuscado.victorias << "\n";
                std::cout << "    Empates:           " << equipoBuscado.empates << "\n";
                std::cout << "    Derrotas:          " << equipoBuscado.derrotas << "\n";
                std::cout << "    Puntos a Favor:    " << equipoBuscado.anotacionAFavor << "\n";
                std::cout << "    Puntos en Contra:  " << equipoBuscado.anotacionEnContra << "\n";
            }
            std::cout << "-------------------------------------------------------------\n\n";
            auxiliares::pausarPrograma();
        }

        void buscarEquiposPorSubCadena(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            bool cancelado = false;
            int cantEquiposEncontrados = 0;
            int error = -1;
            char subcadena[TAMANO_NOMBRE];
            const int maxResultados = MAX_RESULTADOS;
            Equipo resultados[maxResultados];

            // Leemos el header del archivo de equipo para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = logica::leerHeader(nombreArchivo);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\n Error del Sistema!\n";
                std::cout << " Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << " No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Solicitamos los datos de busqueda
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║      BUSQUEDA DE EQUIPOS POR NOMBRE       ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(subcadena, TAMANO_NOMBRE, " Escribe el nombre (o parte del nombre) del equipo que buscas (ingresa 'cancelar' para cancelar): ",
                                            &cancelado, validadores::Nombres)) {
                std::cout << "\n Busqueda cancelada por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Limpiamos la pantalla
            auxiliares::limpiarPantalla();

            std::cout << "\n Buscando... \n";

            // Llamamos a la funcion de busqueda
            cantEquiposEncontrados = logica::buscarRegistrosPorSucadena<Equipo>(nombreArchivo, resultados, subcadena, maxResultados);

            auxiliares::waitfor(1500);
            auxiliares::limpiarPantalla();

            // Si no se enocontro ningun equipo
            if (cantEquiposEncontrados <= 0) {
                std::cout << "\n No se encontro ninguna coincidencia con: '" << subcadena << "'\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          RESULTADOS ENCONTRADOS           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "----------------------------------------------------------------------------\n";
                std::cout << " Se encontraron " << cantEquiposEncontrados << " coincidencia(s):\n";
                std::cout << "----------------------------------------------------------------------------\n";

                for (int e = 0; e < cantEquiposEncontrados; e++) {
                    std::cout << std::endl << e + 1 << ".\n";
                    std::cout << "   Nombre:                   " << resultados[e].nombre << "\n";
                    std::cout << "   ID:                       " << resultados[e].ID << "\n";
                    std::cout << "   Entrenador:               " << resultados[e].entrenador << "\n";
                    std::cout << "   Ciudad:                   " << resultados[e].ciudad << "\n";
                    std::cout << "   Cantidad de Partidos:     " << resultados[e].cantidadPartidos << "\n";
                    std::cout << "   Fecha de Registro:        " << resultados[e].fechaRegistro << "\n";
                }
                std::cout << "---------------------------------------------------------------------------\n";
            }

            auxiliares::pausarPrograma();
        }

        void listarEquipos(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            // Inicializamos las variables a utilizar
            int cantEquipos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Equipo listaDeEquipos[maxResultados];

            // llamamos a la funcion que nos devuelve la lista de punteros
            cantEquipos = logica::listarRegistros<Equipo>(nombreArchivo, listaDeEquipos, maxResultados);

            // Si no se consiguieron equipos
            if (cantEquipos == 0) {
                std::cout << " No hay equipos registrados en el sistema actualmente.\n";
            } else {

                // Listamos todos los equipos
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            LISTA DE EQUIPOS               ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << "----------------------------------------------------------------------------------------- \n";
                std::cout << " " << std::left << std::setw(4) << "N°"
                          << " | " << std::setw(6) << "ID"
                          << " | " << std::setw(35) << "Nombre"
                          << " | " << std::setw(30) << "Ciudad"
                          << " | " << "Puntos" << "\n";

                std::cout << "----------------------------------------------------------------------------------------- \n";

                for (int e = 0; e < cantEquipos; e++) {
                    // Filas de datos con exactamente los mismos anchos modificados
                    std::cout << " " << std::left << std::setw(4) << (e + 1) << " | " << std::setw(6) << listaDeEquipos[e].ID << " | " << std::setw(35) << listaDeEquipos[e].nombre
                              << " | " << std::setw(30) << listaDeEquipos[e].ciudad << " | " << listaDeEquipos[e].puntos << "\n";
                }

                std::cout << "----------------------------------------------------------------------------------------- \n";
            }

            auxiliares::pausarPrograma();
        }

        void mostrarTablaDePosiciones(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1200);

            // Inicializamos las variables a utilizar
            int cantEquipos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Equipo tablaDePosiciones[maxResultados];

            // Buscamos y leemos el nombre del torneo
            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            // Armamos la tabla de posiciones
            cantEquipos = logica::equipos::tablaDePosiciones(nombreArchivo, tablaDePosiciones, maxResultados);

            auxiliares::waitfor(500);

            // CArgando la tabla de posiciones
            std::cout << "\nCargando la tabla de posiciones...\n";

            auxiliares::waitfor(1200);

            // Si no hay resultados no mostramos nada, sino mostramos la tabla con los equipos
            if (cantEquipos == 0) {
                std::cout << "\nNo hay Equipos Disponibles\n";
            } else {

                // Mostramos la tabla de posiciones
                auxiliares::toMayus(torneo.nombre);
                std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║                             TABLA DE POSICIONES                                     ║\n";
                std::cout << "║               " << std::left << std::setw(70) << torneo.nombre << "║\n";
                std::cout << "╠════╦═══════════════════════════════════════════════╦═════╦═══╦═══╦═══╦════╦════╦════╣\n";
                std::cout << "║ #  ║ Equipo                                        ║ PTS ║ J ║ G ║ E ║ D  ║ GF ║ GC ║\n";
                std::cout << "╠════╬═══════════════════════════════════════════════╬═════╬═══╬═══╬═══╬════╬════╬════╣\n";

                for (int e = 0; e < cantEquipos; e++) {
                    std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(45) << tablaDePosiciones[e].nombre << " ║ " << std::right
                              << std::setw(3) << tablaDePosiciones[e].puntos << " ║ " << std::right << std::setw(1) << tablaDePosiciones[e].jugados << " ║ " << std::right
                              << std::setw(1) << tablaDePosiciones[e].victorias << " ║ " << std::right << std::setw(1) << tablaDePosiciones[e].empates << " ║ " << std::right
                              << std::setw(2) << tablaDePosiciones[e].derrotas << " ║ " << std::right << std::setw(2) << tablaDePosiciones[e].anotacionAFavor << " ║ " << std::right
                              << std::setw(2) << tablaDePosiciones[e].anotacionEnContra << " ║\n";
                }
                std::cout << "╚════╩═══════════════════════════════════════════════╩═════╩═══╩═══╩═══╩════╩════╩════╝\n";
                std::cout << "\nReferencia: PTS=Puntos  J=Jugados  G=Ganados  E=Empatados\n";
                std::cout << "            D=Derrotas  GF=puntos a Favor  GC=puntos en Contra\n\n";
            }

            auxiliares::pausarPrograma();
        }

        void eliminatorias() {
            // En proceso..
        }

        void actualizarEquipo(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            // * Datos Actualizables:
            // * Nombre del Equipo, Nombre del Entrenador, Nombre de la Ciudad

            // Variables Editables
            char nombreAux[TAMANO_NOMBRE] = "";
            char entrenadorAux[TAMANO_NOMBRE] = "";
            char ciudadAux[TAMANO_NOMBRE] = "";

            // Demás Variables
            bool actualizado = false;
            bool cancelado = false;
            bool flagError = false;
            bool salir = false;
            char confirmacion;
            int idEquipo = 0;
            int error = -1;
            int opcion = -1;

            // Leemos el header del archivo de equipo para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = logica::leerHeader(nombreArchivo);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Pedimos el ID del equipo a modificar
            do {
                auxiliares::limpiarPantalla();
                flagError = false;

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            ACTUALIZAR EQUIPOS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Pedimos el ID del equipo que desean actualizar
                if (!auxiliares::ingresarDatos(idEquipo, "Ingresa el ID del equipo que desea actualizar (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                    std::cout << "\n Operación Cancelada por el usuario. \n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // si no Existe el ID
                if (!logica::existeID<Equipo>(nombreArchivo, idEquipo)) {
                    std::cout << "Error el ID '" << idEquipo << "' no pertenece a ningún equipo registrado\n";
                    auxiliares::pausarPrograma();
                    flagError = true;
                }

            } while (flagError);

            // Mostramos el menu para que el usuario eliga
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                salir = false;

                // Presentamos un menu corto de los datos que puede actualizar para que eliga
                auxiliares::limpiarPantalla();
                std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
                std::cout << "   ║          MENÚ ACTUALIZAR EQUIPOS          ║\n";
                std::cout << "   ╠═══════════════════════════════════════════╣\n";
                std::cout << "   ║  1. Nombre del equipo                     ║\n";
                std::cout << "   ║  2. Nombre del Entrenador                 ║\n";
                std::cout << "   ║  3. Nombre de la Ciudad                   ║\n";
                // std::cout << "   ║  4. Listar equipos                        ║\n";
                std::cout << "   ║  0. Salir                                 ║\n";
                std::cout << "   ╚═══════════════════════════════════════════╝\n";
                std::cout << std::endl;

                // Pedimos la opcion
                if (!auxiliares::ingresarDatos(opcion, " Ingrese una opción: ")) {
                    std::cout << "\n Operación Cancelada por el usuario. \n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Estructura del switch
                switch (opcion) {

                    // Salida
                    case 0:
                        salir = true;
                        break;

                    // Actualizar nombre
                    case 1:
                        do {
                            flagError = false;
                            auxiliares::limpiarPantalla();
                            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                            std::cout << "       ║        ACTUALIZAR NOMBRE DEL EQUIPO       ║\n";
                            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                            if (!auxiliares::ingresarCadena(nombreAux, TAMANO_NOMBRE, "Ingrese el nuevo nombre del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                            validadores::Nombres)) {
                                std::cout << "\n Operación Cancelada por el Usuario \n";
                                auxiliares::pausarPrograma();
                                return;
                            }

                            if (logica::cadenaDuplicadaParaActualizar<Equipo>(nombreArchivo, nombreAux, idEquipo, &Equipo::nombre)) {
                                std::cout << "El nombre '" << nombreAux << "' ya le pertence a otro equipo\n";
                                flagError = true;
                            }

                            auxiliares::waitfor(1500);
                            auxiliares::limpiarPantalla();
                        } while (flagError);
                        break;

                    // Actualizar nombre del entrenador
                    case 2:
                        do {
                            flagError = false;
                            auxiliares::limpiarPantalla();
                            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                            std::cout << "       ║     ACTUALIZAR NOMBRE DEL ENTRENADOR      ║\n";
                            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                            if (!auxiliares::ingresarCadena(entrenadorAux, TAMANO_NOMBRE, "Ingrese el nuevo nombre del Entrenador del Equipo (ingrese 'cancelar' para cancelar): ",
                                                            &cancelado, validadores::Nombres)) {
                                std::cout << "\n Operación Cancelada por el Usuario \n";
                                auxiliares::pausarPrograma();
                                return;
                            }

                            if (logica::cadenaDuplicadaParaActualizar<Equipo>(nombreArchivo, entrenadorAux, idEquipo, &Equipo::entrenador)) {
                                std::cout << "Ya el entrenador '" << entrenadorAux << "' dirige otro equipo\n";
                                flagError = true;
                            }
                            auxiliares::waitfor(1500);
                            auxiliares::limpiarPantalla();
                        } while (flagError);
                        break;

                    // Actualizar nombre de la ciudad
                    case 3:

                        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                        std::cout << "       ║       ACTUALIZAR NOMBRE DE LA CIUDAD      ║\n";
                        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                        if (!auxiliares::ingresarCadena(ciudadAux, TAMANO_NOMBRE,
                                                        "Ingrese el nuevo nombre de la Ciudad del Equipo (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::Nombres)) {
                            std::cout << "\n Operación Cancelada por el Usuario \n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        auxiliares::waitfor(1500);
                        auxiliares::limpiarPantalla();
                        break;

                    default:
                        mensajeDefault();
                }

            } while (opcion != 0);

            // Si pidió salir
            if (salir) {
                auxiliares::limpiarPantalla();
                auxiliares::waitfor(500);
                std::cout << "\n Saliendo... \n";
                auxiliares::waitfor(1000);
                auxiliares::limpiarPantalla();
            }

            // Si no se modificó ningun parametro
            if (validadores::charVacio(nombreAux) && validadores::charVacio(entrenadorAux) && validadores::charVacio(ciudadAux)) {
                std::cout << "\n No se realizaron cambios.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Pedimos la confirmación
            auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea aplicar estos cambios? (S/N): ");

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                actualizado = logica::equipos::actualizarEquipo(nombreArchivo, idEquipo, nombreAux, entrenadorAux, ciudadAux);
                if (!actualizado) {
                    std::cout << "Se produjo un error a la hora de actualizar el equipo\n";
                } else {

                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << "           Equipo Actualizado con Éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";

                    if (!validadores::charVacio(nombreAux)) {
                        std::cout << "Nuevo Nombre del Equipo: " << nombreAux << std::endl;
                    }

                    if (!validadores::charVacio(nombreAux)) {
                        std::cout << "Nuevo Nombre del Entrenador del Equipo: " << entrenadorAux << std::endl;
                    }

                    if (!validadores::charVacio(nombreAux)) {
                        std::cout << "Nuevo Nombre de la Ciudad del Equipo: " << ciudadAux << std::endl;
                    }
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "Actualización de Datos Cancelada\n";
            } else {
                std::cout << "Error: No se ingresó una opción correcta (S/N)\n";
                std::cout << "Actualización de Datos Cancelada\n";
            }
            auxiliares::pausarPrograma();
        }

        void eliminarEquipo(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            int error = -1;
            bool eliminado = false;
            bool encontrado = false;
            bool cancelado = false;
            int ID = 0;
            char confirmacion;
            Equipo equipoAux;

            // Leemos el header del archivo de equipo para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = logica::leerHeader(nombreArchivo);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Eliminación Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             ELIMINAR EQUIPOS              ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Pedimos el ID del equipo que desean actualizar
            if (!auxiliares::ingresarDatos(ID, "Ingresa el ID del equipo que desea eliminar (ingresa 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "\nRegistro cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // si no Existe el ID
            if (!logica::existeID<Equipo>(nombreArchivo, ID)) {
                std::cerr << "Error el ID '" << ID << "' no pertenece a ningún equipo registrado\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(1500);
            auxiliares::limpiarPantalla();

            // buscamos el equipo mediante el ID ingresado
            encontrado = logica::buscarRegistrosPorId<Equipo>(nombreArchivo, equipoAux, ID);

            // si no se encontró ningun equipo
            if (!encontrado) {
                std::cerr << "Error, no se encontró el equipo que se desea eliminar";
                auxiliares::pausarPrograma();
                return;
            }

            /*
            // Si tiene partidos jugados
            if (equipoAux.cantidadPartidos > 0) {
                std::cout << " ADVERTENCIA: El equipo tiene " << equipoAux.cantidadPartidos << " partidos asociados.\n";
                std::cout << " No puede ser eliminado hasta que cancele los partidos del equipo\n";
                auxiliares::pausarPrograma();
                return;
            }*/

            // ? ofrecer opcion de cancelar todos los partidos

            /*
            // Si tiene jugadores
            if (equipoAux.numJugadores > 0) {
                std::cout << " ADVERTENCIA: El equipo tiene " << equipoAux.numJugadores << " jugadores.\n";
                std::cout << " No puede ser eliminado\n";
                auxiliares::pausarPrograma();
                return;
            }*/

            // ? ofrecer opcion de eliminar todos los jugadores?

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             ELIMINAR EQUIPOS              ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << "Equipo Seleccionado: \n\n";
            std::cout << "Nombre: " << equipoAux.nombre << std::endl;
            std::cout << "Entrenador: " << equipoAux.entrenador << std::endl;
            std::cout << "Ciudad: " << equipoAux.ciudad << std::endl;

            auxiliares::ingresarDatos(confirmacion, "¿Desea eliminar el equipo? (S/N): ");
            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                eliminado = logica::equipos::eliminarEquipo(nombreArchivo, ID);
                if (!eliminado) {
                    std::cout << "Se produjo un error a la hora de eliminar el equipo\n";
                    std::cout << "Por favor revise que el equipo a eliminar no tenga partidos asociados ni jugadores registrados\n";
                } else {
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << "           Equipo Eliminado con Éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "Eliminacion de Equipo Cancelada\n";
            } else {
                std::cerr << "Error: No se ingresó una opción correcta (S/N)\n";
                std::cout << "Eliminacion de Equipo Cancelada\n";
            }
            auxiliares::pausarPrograma();
        }

    } // namespace equipos

    namespace jugadores {

        void registrarJugador(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int error = -1;
            Jugador nuevo;
            bool flagError = false;
            char confirmacion;
            bool cancelado = false;
            int opcion = 0;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Registro Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Leemos el torneo
            Torneo torneo;
            logica::obtenerInformacionTorneo(torneo);

            // Recolectamos el ID del equipo
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarDatos(nuevo.idEquipo, "Ingrese el ID del equipo al que pertenece el jugador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                /// Si el ID no existe dentro de los equipos
                if (!logica::existeID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, nuevo.idEquipo)) {
                    std::cout << "Error el ID '" << nuevo.idEquipo << "' no pertenece a ningun equipo\n";
                    flagError = true;
                    auxiliares::waitfor(2500);
                    continue;
                }
                auxiliares::waitfor(1500);
            } while (flagError);

            // Recolectamos el nombre del Jugador
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarCadena(nuevo.nombre, TAMANO_NOMBRE, "Ingrese el nombre del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                validadores::Nombres)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Validamos nombre duplicado
                if (logica::cadenaDuplicada<Jugador>(nombreArchivo, nuevo.nombre, &Jugador::nombre)) {
                    std::cout << "Error, el nombre '" << nuevo.nombre << "' ya está en uso.\n";
                    flagError = true;
                    auxiliares::waitfor(3000);
                    continue;
                }
                auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la Edad
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarDatos(nuevo.edad, "Ingrese la edad del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::validarEdad)) {
                std::cout << "\nRegistro cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(1500);

            // Recolectamos la cedula
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarCadena(nuevo.cedula, TAMANO_CEDULA, "Ingrese la cedula del jugador (ingrese 'cancelar' para cancelar): ", nullptr, validadores::Cedulas)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Validamos nombre duplicado
                if (logica::cadenaDuplicada(nombreArchivo, nuevo.cedula, &Jugador::cedula)) {
                    std::cout << " Error, la cedula '" << nuevo.cedula << "' ya le pertenece a otro jugador\n";
                    flagError = true;
                    auxiliares::waitfor(3000);
                    continue;
                }
                auxiliares::waitfor(2000);
            } while (flagError);

            // Recolectamos la Posicion
            do {
                flagError = false;
                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte actual del Torneo: " << torneo.deporte << "\n\n";

                const char **matrizDeporteActual = nullptr;

                for (size_t i = 0; i < validadores::totalDeportes; i++) {
                    // Recorremos el array de matrices y verificamos lo que hay en la posicion 0
                    if (std::strcmp(validadores::MapaDeportes[i][0], torneo.deporte) == 0) {
                        matrizDeporteActual = validadores::MapaDeportes[i];
                        break;
                    }
                }

                // Mostramos las posiciones disponibles de esa fila
                std::cout << "\n Seleccione la posición del jugador:\n";

                // Para saber el numero de posiciones del deporte
                int contadorPosiciones = 0;

                for (size_t j = 1; matrizDeporteActual[j] != nullptr; j++) {
                    std::cout << " " << j << ". " << matrizDeporteActual[j] << "\n";
                    contadorPosiciones++;
                }
                std::cout << "\n";

                if (!auxiliares::ingresarDatos(opcion, "Seleccione una opción (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos que esté en el rango de opciones
                if (opcion >= 1 && opcion <= contadorPosiciones) {
                    // si es correcta guardamos la posicion
                    std::strncpy(nuevo.posicion, matrizDeporteActual[opcion], TAMANO_POSICION);
                } else {
                    std::cerr << "Error: Opción inválida. Por favor, intente de nuevo.\n";
                    flagError = true;
                    auxiliares::waitfor(2000);
                }
            } while (flagError);


            // Recolectamos la fecha de registro del jugador
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " Fecha de Inicio del Torneo: " << torneo.fechaInicio << std::endl;
            if (!auxiliares::ingresarCadena(nuevo.fechaRegistro, TAMANO_FECHA, " Ingrese la fecha de Registro del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                            validadores::fechaValidaRegistroDeJugadorOEquipo)) {
                std::cout << "\n Registro cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(2000);
            auxiliares::limpiarPantalla();

            // Recolectamos el dorsal del Jugador
            do {
                flagError = false;
                Equipo equipoBuscado;
                logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, nuevo.idEquipo);
                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          REGISTRAR NUEVO JUGADOR          ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                if (!auxiliares::ingresarDatos(nuevo.numeroDorsal, "Ingrese el Dorsal del jugador (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::validarDorsal)) {
                    std::cout << "\nRegistro cancelado por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                if (logica::jugadores::DorsalDuplicado(nombreArchivo, nuevo.numeroDorsal, nuevo.idEquipo)) {
                    std::cout << "Error el dorsal '" << nuevo.numeroDorsal << "' ya está ocupado en el equipo '" << equipoBuscado.nombre << "'.\n";
                    flagError = true;
                }
                auxiliares::waitfor(2000);
                auxiliares::limpiarPantalla();
            } while (flagError);

            // Pedimos la confirmacion al usuario
            auxiliares::limpiarPantalla();
            auxiliares::ingresarDatos(confirmacion, "¿Está seguro de que desea registrar este jugador? (S/N): ");

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                // agregamos el jugador
                bool registrado = logica::jugadores::registrarJugador(nombreArchivo, nuevo);
                auxiliares::waitfor(1200);
                auxiliares::limpiarPantalla();
                // Si no se agregó
                if (!registrado) {
                    std::cerr << "Error al registrar al jugador.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║       JUGADOR REGISTRADO CON ÉXITO        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Torneo: " << torneo.nombre;
                std::cout << "\n ID del Jugador: " << nuevo.ID;
                std::cout << "\n Nombre del Jugador: " << nuevo.nombre;
                std::cout << "\n Cédula: " << nuevo.cedula;
                std::cout << "\n Edad: " << nuevo.edad << " años";
                std::cout << "\n Posición: " << nuevo.posicion;
                std::cout << "\n Dorsal: " << nuevo.numeroDorsal;
                std::cout << "\n ID del Equipo asignado: " << nuevo.idEquipo;
                std::cout << "\n Fecha de Registro: " << nuevo.fechaRegistro;
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << " Registro de Jugador Cancelado.\n";
            } else {
                std::cerr << " ERROR: Opción incorrecta (S/N).\nRegistro de Jugador Cancelado.\n";
            }
            auxiliares::pausarPrograma();
        }

        void buscarJugadorID(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int ID = 0;
            int error = -1;
            bool encontrado = false;
            bool cancelado = false;
            Jugador jugadorBuscado;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║       BUSQUEDA DE JUGADORES POR ID        ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            if (!auxiliares::ingresarDatos(ID, "Ingrese el ID (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "\nRegistro Cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            encontrado = logica::buscarRegistrosPorId<Jugador>(nombreArchivo, jugadorBuscado, ID);

            auxiliares::waitfor(800);
            auxiliares::limpiarPantalla();
            std::cout << "\nBuscando...\n";
            auxiliares::waitfor(1500);


            // si no encontro un jugador
            if (!encontrado) {
                std::cerr << "Error no hay ningun jugador registrado con el ID '" << ID << "'\n";
            } else {


                // Buscamos el equipo del jugador
                Equipo equipoBuscado;
                encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, jugadorBuscado.idEquipo);

                // Verificamos que el equipo fue enoncontrado
                if (!encontrado) {
                    std::cout << "\nError del Sistema!\n";
                    std::cout << "Busqueda Cancelada\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Listamos los datos
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            JUGADOR ENCONTRADO             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Torneo:                 " << auxiliares::toMayus(torneo.nombre) << "\n";
                std::cout << "  Deporte:                " << torneo.deporte << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Informacion del Jugador: \n";
                std::cout << "    ID del Jugador:       " << jugadorBuscado.ID << "\n";
                std::cout << "    Nombre:               " << jugadorBuscado.nombre << "\n";
                std::cout << "    Cédula:               " << jugadorBuscado.cedula << "\n";
                std::cout << "    Edad:                 " << jugadorBuscado.edad << " años \n";
                std::cout << "    Posición:             " << jugadorBuscado.posicion << "\n";
                std::cout << "    Dorsal:               " << jugadorBuscado.numeroDorsal << "\n";
                std::cout << "    Fecha de Registro:    " << jugadorBuscado.fechaRegistro << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Informacion del Equipo: \n";
                std::cout << "    ID Equipo:            " << jugadorBuscado.idEquipo << "\n";
                std::cout << "    Equipo:               " << equipoBuscado.nombre << "\n";
                std::cout << "-------------------------------------------------------------\n";
                std::cout << "  Estadísticas en el Torneo:\n";
                std::cout << "    Anotaciones:          " << jugadorBuscado.anotaciones << "\n";
                std::cout << "    Tarjetas Amarillas:   " << jugadorBuscado.tarjetaAmarillas << "\n";
                std::cout << "    Tarjetas Rojas:       " << jugadorBuscado.tarjetasRojas << "\n";
                std::cout << "-------------------------------------------------------------\n";
            }
            auxiliares::pausarPrograma();
        }

        void buscarJugadorPorNombre(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            char subcadena[TAMANO_NOMBRE];
            int cantJugadoresEncontrados = 0;
            int error = -1;
            bool cancelado = false;
            bool encontrado = false;
            const int maxResultados = MAX_RESULTADOS;
            Jugador resultados[maxResultados];

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║      BÚSQUEDA DE JUGADORES POR NOMBRE     ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            if (!auxiliares::ingresarCadena(subcadena, 100, "Escribe el nombre (o parte del nombre) del jugador que buscas (escribe 'cancelar' para cancelar): ", &cancelado,
                                            validadores::Nombres)) {
                std::cout << "\nRegistro Cancelado por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1200);
            std::cout << "Buscando..." << std::endl;
            auxiliares::limpiarPantalla();
            auxiliares::waitfor(500);

            // llamamos a la funcion buscar por nombre y almacenamos el resultado
            cantJugadoresEncontrados = logica::buscarRegistrosPorSucadena<Jugador>(nombreArchivo, resultados, subcadena, maxResultados);

            // Si no encontró nada
            if (cantJugadoresEncontrados <= 0) {
                std::cout << "\nNo se encontraron jugadores que coincidan con '" << subcadena << "'.\n";
            } else {

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║          COINCIDENCIAS ENCONTRADAS        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Se encontraron " << cantJugadoresEncontrados << " jugador(es):\n";

                for (int e = 0; e < cantJugadoresEncontrados; e++) {

                    // Buscamos el nombre del Equipo
                    Equipo equipoBuscado;
                    encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, resultados[e].idEquipo);

                    // Si no se encontró el equipo
                    if (!encontrado) {
                        std::cerr << "Error del Sistema!";
                        std::cout << "Busqueda Cancelada";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << "-------------------------------------------------------------\n";
                    std::cout << "  ID:                    " << resultados[e].ID << "\n";
                    std::cout << "  Nombre:                " << resultados[e].nombre << "\n";
                    std::cout << "  Cédula:                " << resultados[e].cedula << "\n";
                    std::cout << "  Dorsal:                [" << resultados[e].numeroDorsal << "]\n";
                    std::cout << "  Edad:                  " << resultados[e].edad << " años \n";
                    std::cout << "  Posición:              " << resultados[e].posicion << "\n";
                    std::cout << "  Nombre del Equipo:     " << equipoBuscado.nombre << "\n";
                    std::cout << "  ID Equipo:             " << resultados[e].idEquipo << "\n";
                    std::cout << "  Anotaciones:           " << resultados[e].anotaciones << "\n";
                    std::cout << "  Tarjetas Amarillas:    " << resultados[e].tarjetaAmarillas << "\n";
                    std::cout << "  Tarjetas Rojas:    " << resultados[e].tarjetasRojas << "\n";
                }
                std::cout << "-------------------------------------------------------------\n";
            }

            std::cout << "\n";
            auxiliares::pausarPrograma();
        }

        void mostrarJugadoresPorEquipo(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int idEquipo = 0;
            int cantJugadoresEncontrados = 0;
            const int maxResultados = MAX_RESULTADOS;
            Jugador listaDeJugadores[maxResultados];
            int error = -1;
            bool cancelado = false;
            bool encontrado = false;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Registro Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << " No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << " No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Pedimos el ID del equipo a consultar
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║      MOSTRAR JUGADORES POR EQUIPO         ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            if (!auxiliares::ingresarDatos(idEquipo, "Ingrese el ID del Equipo (ingresa 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "\n Operación Cancelada por el usuario.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo primero
            Equipo equipoBuscado;
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, idEquipo);

            // Si no encontramos un equipo con ese ID enviamos error
            if (!encontrado) {
                std::cerr << "\n Error: El equipo con ID '" << idEquipo << "' no existe.\n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1000);
            std::cout << "\n Buscando jugadores...\n\n";

            // obtenemos la lista de punteros
            cantJugadoresEncontrados = logica::jugadores::listarJugadoresPorEquipo(nombreArchivo, idEquipo, listaDeJugadores, maxResultados);

            if (cantJugadoresEncontrados < 0) {
                std::cerr << "\n Error del Sistema! \n";
                std::cout << " Operacion Cancelada \n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1000);

            // Si no obtenemos nada
            if (cantJugadoresEncontrados == 0) {
                std::cout << "El equipo '" << equipoBuscado.nombre << "' actualmente no tiene jugadores registrados.\n";
            } else {
                std::cout << "╔═════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║ EQUIPO: " << std::left << std::setw(71) << equipoBuscado.nombre << " ║\n";
                std::cout << "║ ID DEL EQUIPO: " << std::left << std::setw(64) << equipoBuscado.ID << " ║\n";
                std::cout << "╠════╦══════════════════════════════════════════╦═══════════════╦═════╦═══════════╣\n";
                std::cout << "║ ID ║ Nombre                                   ║ Posición      ║ Edad║ Dorsal    ║\n";
                std::cout << "╠════╬══════════════════════════════════════════╬═══════════════╬═════╬═══════════╣\n";

                // Imprimimos los jugadores
                for (int e = 0; e < cantJugadoresEncontrados; e++) {
                    std::cout << "║ " << std::right << std::setw(2) << listaDeJugadores[e].ID << " ║ " << std::left << std::setw(40) << listaDeJugadores[e].nombre << " ║ "
                              << std::left << std::setw(13) << listaDeJugadores[e].posicion << " ║ " << std::right << std::setw(3) << listaDeJugadores[e].edad << " ║ ["
                              << std::right << std::setw(2) << listaDeJugadores[e].numeroDorsal << "]      ║\n";
                }
                std::cout << "╚════╩══════════════════════════════════════════╩═══════════════╩═════╩═══════════╝\n";
                std::cout << " Total de jugadores en el equipo: " << cantJugadoresEncontrados << "\n";
            }

            std::cout << "\n";
            auxiliares::pausarPrograma();
        }

        void mostrarListaDeJugadores(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int cantJugadoresEncontrados = 0;
            int error = -1;
            const int maxResultados = MAX_RESULTADOS;
            Jugador listaDeJugadores[maxResultados];
            bool encontrado = false;

            // Leemos el header del archivo de Equipos para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operación Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::waitfor(1000);

            // Llamamos a tu función lógica
            cantJugadoresEncontrados = logica::listarRegistros<Jugador>(nombreArchivo, listaDeJugadores, maxResultados);

            // Validamos si el sistema tiene jugadores cargados
            if (cantJugadoresEncontrados == 0) {
                std::cout << "No existen jugadores registrados en el sistema actualmente.\n";
            } else {

                std::cout << "Cargando todos los jugadores...\n\n";
                auxiliares::limpiarPantalla();
                auxiliares::waitfor(1000);

                std::cout << "╔═══════════════════════════════════════════════════════════════════════════════════╗\n";
                std::cout << "║ SPORT G&C TOURNAMENTS                                                             ║\n";
                std::cout << "║ TORNEO: " << std::left << std::setw(73) << torneo.nombre << " ║\n";
                std::cout << "║ LISTA DE JUGADORES REGISTRADOS                                                    ║\n";
                std::cout << "╠════╦══════════════════════╦══════════════════════╦═══════════════╦═════╦══════════╣\n";
                std::cout << "║ ID ║ Nombre               ║ Equipo               ║ Posición      ║ Edad║ Dorsal   ║\n";
                std::cout << "╠════╬══════════════════════╬══════════════════════╬═══════════════╬═════╬══════════╣\n";

                // Imprimimos cada jugador en el sistema
                for (int e = 0; e < cantJugadoresEncontrados; e++) {

                    // Buscamos el equipo en cada iteracion
                    Equipo equipoBuscado;
                    encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, listaDeJugadores[e].idEquipo);

                    if (!encontrado) {
                        auxiliares::limpiarPantalla();
                        std::cout << "Error del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << "║ " << std::right << std::setw(2) << listaDeJugadores[e].ID << " ║ " << std::left << std::setw(20) << listaDeJugadores[e].nombre << " ║ "
                              << std::left << std::setw(20) << equipoBuscado.nombre << " ║ " << std::left << std::setw(13) << listaDeJugadores[e].posicion << " ║ " << std::right
                              << std::setw(3) << listaDeJugadores[e].edad << " ║ [" << std::right << std::setw(2) << listaDeJugadores[e].numeroDorsal << "]     ║\n";
                }
                std::cout << "╚════╩══════════════════════╩══════════════════════╩═══════════════╩═════╩══════════╝\n";
                std::cout << " Total de jugadores registrados en el sistema: " << cantJugadoresEncontrados << "\n";
            }

            std::cout << "\n";
            auxiliares::pausarPrograma();
        }

        void actualizarJugador(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            // * Datos Actualizables:
            // * Nombre, Edad, Dorsal, Posicion, Cédula

            // Variables Editables
            char nombreAux[TAMANO_NOMBRE] = "";
            char posicionAux[TAMANO_POSICION] = "";
            char cedulaAux[TAMANO_CEDULA] = "";
            int edadAux = 0;
            int dorsalAux = 0;

            // Demás Variables
            char confirmacion;
            bool actualizado = false;
            bool flagError = false;
            bool cancelado = false;
            bool salir = false;
            int idJugador = 0;
            int error = -1;
            int opcion = -1;

            // Leemos el header del archivo de jugadores para saber el numero de registros activos
            ArchivoHeader headerJugadores = logica::leerHeader(nombreArchivo);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            // Pedimos el ID del jugador a modificar
            do {
                auxiliares::limpiarPantalla();
                flagError = false;

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           ACTUALIZAR JUGADORES            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Pedimos el ID del jugador que desean actualizar
                if (!auxiliares::ingresarDatos(idJugador, "Ingresa el ID del jugador al que desea actualizarsus datos (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\n Operación Cancelada por el usuario. \n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // si no Existe el ID
                if (!logica::existeID<Jugador>(nombreArchivo, idJugador)) {
                    std::cout << "Error el ID '" << idJugador << "' no pertenece a ningún jugador registrado\n";
                    auxiliares::pausarPrograma();
                    flagError = true;
                }

            } while (flagError);

            // Mostramos el menu para que el usuario eliga
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                salir = false;
                bool existe = false;
                Jugador jugadorAuxiliar;
                Equipo equipoAuxiliar;

                // Presentamos un menu corto de los datos que puede actualizar para que eliga
                auxiliares::limpiarPantalla();
                std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
                std::cout << "   ║         MENÚ ACTUALIZAR JUGADORES         ║\n";
                std::cout << "   ╠═══════════════════════════════════════════╣\n";
                std::cout << "   ║  1. Nombre del Jugador                    ║\n";
                std::cout << "   ║  2. Edad del Jugador                      ║\n";
                std::cout << "   ║  3. Dorsal del Jugador                    ║\n";
                std::cout << "   ║  4. Posicion del Jugador                  ║\n";
                std::cout << "   ║  5. Cédula del Jugador                    ║\n";
                // ? Equipo al que pertenece??
                std::cout << "   ║  0. Salir                                 ║\n";
                std::cout << "   ╚═══════════════════════════════════════════╝\n";
                std::cout << std::endl;

                // Pedimos la opcion
                if (!auxiliares::ingresarDatos(opcion, " Ingrese una opción: ")) {
                    std::cout << "\n Operación Cancelada por el usuario. \n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Estructura del switch
                switch (opcion) {

                    // Salida
                    case 0:
                        salir = true;
                        break;

                    // Actualizar Nombre
                    case 1:

                        do {
                            flagError = false;
                            auxiliares::limpiarPantalla();
                            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                            std::cout << "       ║             ACTUALIZAR NOMBRE             ║\n";
                            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                            if (!auxiliares::ingresarCadena(nombreAux, TAMANO_NOMBRE, " Ingrese el nuevo nombre del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                            validadores::Nombres)) {
                                std::cout << "\n Operación Cancelada por el Usuario \n";
                                auxiliares::pausarPrograma();
                                return;
                            }

                            if (logica::cadenaDuplicadaParaActualizar<Jugador>(nombreArchivo, nombreAux, idJugador, &Jugador::nombre)) {
                                std::cout << " El nombre '" << nombreAux << "' ya le pertence a otro Jugador\n";
                                flagError = true;
                            }

                            auxiliares::waitfor(1500);
                            auxiliares::limpiarPantalla();
                        } while (flagError);
                        break;

                    // Actualizar Edad
                    case 2:

                        auxiliares::limpiarPantalla();

                        // Recolectamos la Edad
                        std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                        std::cout << "       ║              ACTUALIZAR EDAD              ║\n";
                        std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                        if (!auxiliares::ingresarDatos(edadAux, " Ingrese la nueva edad del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::validarEdad)) {
                            std::cout << "\n Operación Cancelada por el Usuario \n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        auxiliares::waitfor(1200);
                        auxiliares::limpiarPantalla();

                        break;

                    // Actualizar Numero de Dorsal
                    case 3:

                        // Recolectamos el dorsal
                        do {
                            flagError = false;
                            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                            std::cout << "       ║             ACTUALIZAR DORSAL             ║\n";
                            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                            // Pedimos el dorsal
                            if (!auxiliares::ingresarDatos(dorsalAux, " Ingrese el nuevo dorsal del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                           validadores::validarDorsal)) {
                                std::cout << "\n Operación Cancelada por el Usuario \n";
                                auxiliares::pausarPrograma();
                                return;
                            }
                            auxiliares::waitfor(1000);

                            // Buscamos al jugador para obtener el id de su equipo
                            existe = logica::buscarRegistrosPorId<Jugador>(nombreArchivo, jugadorAuxiliar, idJugador);

                            // Verificamos si existe
                            if (!existe) {
                                std::cerr << "\n Error del Sistema! \n";
                                auxiliares::pausarPrograma();
                                return;
                            }

                            // Buscamos el equipo para obtener sus datos
                            existe = logica::buscarRegistrosPorId<Equipo>(nombreArchivo, equipoAuxiliar, jugadorAuxiliar.idEquipo);

                            // Verificamos si existe
                            if (!existe) {
                                std::cerr << "\n Error del Sistema! \n";
                                auxiliares::pausarPrograma();
                                return;
                            }

                            // Si el dorsal ingresado esta duplicado y es distinto del dorsal actual del jugador
                            if (logica::jugadores::dorsalDuplicadoParaActualizar(nombreArchivo, dorsalAux, idJugador, jugadorAuxiliar.idEquipo)) {
                                std::cerr << " Error: El dorsal '" << dorsalAux << "' ya está en uso en el equipo '" << equipoAuxiliar.nombre << "'.\n";
                                auxiliares::waitfor(2000);
                                flagError = true;
                                continue;
                            }

                            auxiliares::waitfor(1200);
                            auxiliares::limpiarPantalla();
                        } while (flagError);
                        break;

                    // Actualizar Posicion del Jugador
                    case 4: {

                        // Recolectamos la posición
                        do {
                            opcion = 0;
                            flagError = false;
                            auxiliares::limpiarPantalla();
                            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                            std::cout << "       ║           ACTUALIZAR LA POSICIÓN          ║\n";
                            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                            std::cout << " Deporte actual del Torneo: " << torneo.deporte << "\n\n";

                            const char **matrizDeporteActual = nullptr;

                            for (size_t i = 0; i < validadores::totalDeportes; i++) {
                                // Recorremos el array de matrices y verificamos lo que hay en la posicion 0
                                if (std::strcmp(validadores::MapaDeportes[i][0], torneo.deporte) == 0) {
                                    matrizDeporteActual = validadores::MapaDeportes[i];
                                    break;
                                }
                            }

                            // Mostramos las posiciones disponibles de esa fila
                            std::cout << " Seleccione la posición del jugador: \n";

                            // Para saber el numero de posiciones del deporte
                            int contadorPosiciones = 0;

                            for (size_t j = 1; matrizDeporteActual[j] != nullptr; j++) {
                                std::cout << " " << j << ". " << matrizDeporteActual[j] << "\n";
                                contadorPosiciones++;
                            }
                            std::cout << "\n";

                            auxiliares::ingresarDatos(opcion, "Seleccione una opción: ");

                            // Verificamos que esté en el rango de opciones
                            if (opcion >= 1 && opcion <= contadorPosiciones) {
                                // si es correcta guardamos la posicion
                                std::strncpy(posicionAux, matrizDeporteActual[opcion], TAMANO_POSICION);
                            } else {
                                std::cerr << " Error: Opción inválida. Por favor, intente de nuevo.\n";
                                flagError = true;
                                auxiliares::waitfor(2000);
                            }
                        } while (flagError);

                        break;
                    }
                    // Actualizar Cédula
                    case 5:

                        do {
                            existe = false;
                            flagError = false;
                            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                            std::cout << "       ║            ACTUALIZAR LA CEDULA           ║\n";
                            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                            // Pedimos la cedula
                            if (!auxiliares::ingresarCadena(cedulaAux, TAMANO_CEDULA, " Ingrese la nueva cedula del Jugador (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                            validadores::Cedulas)) {
                                std::cout << "\n Operación Cancelada por el Usuario \n";
                                auxiliares::pausarPrograma();
                                return;
                            }

                            // Buscamos el jugador
                            existe = logica::buscarRegistrosPorId<Jugador>(nombreArchivo, jugadorAuxiliar, idJugador);

                            // Verificamos si lo encontró
                            if (!existe) {
                                std::cerr << "\n Error del Sistema! \n";
                                auxiliares::pausarPrograma();
                                return;
                            }

                            // Verificamos que la cedula no está repetida y no le pertenezca al jugador
                            if (logica::jugadores::cedulaRepetida(nombreArchivo, cedulaAux) && (std::strcmp(cedulaAux, jugadorAuxiliar.cedula) != 0)) {
                                std::cout << " La cédula '" << cedulaAux << "' ya le pertence a otro jugador";
                                auxiliares::waitfor(1500);
                                flagError = true;
                                continue;
                            }

                            auxiliares::waitfor(1500);
                            auxiliares::limpiarPantalla();

                        } while (flagError);
                        break;

                        // Mensaje de Default
                    default:
                        mensajeDefault();
                }

            } while (opcion != 0);

            // Si pidió salir
            if (salir) {
                auxiliares::limpiarPantalla();
                auxiliares::waitfor(500);
                std::cout << "\n Saliendo... \n";
                auxiliares::waitfor(1000);
                auxiliares::limpiarPantalla();
            }

            // Si no se modificó ningun parametro
            if (validadores::charVacio(nombreAux) && edadAux == 0 && dorsalAux == 0 && validadores::charVacio(posicionAux) && validadores::charVacio(cedulaAux)) {
                std::cout << "\n No se realizaron cambios.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Confirmación de los cambios
            auxiliares::limpiarPantalla();
            std::cout << "\n";
            auxiliares::ingresarDatos(confirmacion, " ¿Está seguro de que desea actualizar el jugador? (S/N): ");

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

                // Llamamos a la lógica correspondiente de Jugadores
                actualizado = logica::jugadores::actualizarJugador(nombreArchivo, idJugador, nombreAux, edadAux, dorsalAux, posicionAux, cedulaAux);

                // Si no pudo ser actualizado
                if (!actualizado) {
                    std::cout << "\n Se produjo un error a la hora de actualizar el jugador\n";
                } else {
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << "           Jugador Actualizado con Éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n\n";
                    if (!validadores::charVacio(nombreAux)) {
                        std::cout << " Nuevo Nombre:   " << nombreAux << "\n";
                    }
                    if (edadAux >= 0) {
                        std::cout << " Nueva Edad:     " << edadAux << " años" << "\n";
                    }
                    if (dorsalAux >= 0) {
                        std::cout << " Nuevo Dorsal:   " << dorsalAux << "\n";
                    }
                    if (!validadores::charVacio(posicionAux)) {
                        std::cout << " Nueva Posición: " << posicionAux << "\n";
                    }
                    if (!validadores::charVacio(cedulaAux)) {
                        std::cout << " Nueva Cedula:   " << cedulaAux << "\n";
                    }
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "Actualización de Datos Cancelada\n";
            } else {
                std::cerr << "Error: No se ingresó una opción correcta (S/N)\n";
                std::cout << "Actualización de Datos Cancelada\n";
            }
            auxiliares::pausarPrograma();
        }

        void eliminarJugador(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int ID = 0;
            char confirmacion;
            int error = -1;
            bool cancelado = false;
            Jugador jugadorBuscado;
            bool encontrado = false;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Operacion Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            ELIMINAR JUGADOR               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            // Recolectamos el ID
            if (!auxiliares::ingresarDatos(ID, "Ingresa el ID del jugador que deseas eliminar (ingresa 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                std::cout << "\nOperacion Cancelada por el usuario\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos al jugador para mostrar sus datos en pantalla antes de continuar
            encontrado = logica::buscarRegistrosPorId<Jugador>(nombreArchivo, jugadorBuscado, ID);

            // Si no encontro el jugador con ese ID
            if (!encontrado) {
                std::cerr << "\nError: El ID '" << ID << "' no pertenece a ningún jugador registrado.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Verificamos que no tenga puntos anotados
            if (jugadorBuscado.anotaciones > 0) {
                std::cout << "\nError el jugador " << jugadorBuscado.nombre << " no puede tener anotaciones en el torneo\n";
            }

            // Buscamos el equipo al que pertenece
            Equipo equipoBuscado;
            encontrado = logica::buscarRegistrosPorId<Equipo>(nombreArchivo, equipoBuscado, jugadorBuscado.idEquipo);

            // Si no ecnontro el equipo del jugador
            if (!encontrado) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Mostramos el jugador que se va a eliminar
            std::cout << "\n Se eliminará el siguiente jugador del sistema:\n";
            std::cout << " -----------------------------------------------\n";
            std::cout << " Nombre:     " << jugadorBuscado.nombre << "\n";
            std::cout << " Posición:   " << jugadorBuscado.posicion << "\n";
            std::cout << " Dorsal:     " << jugadorBuscado.numeroDorsal << "\n";
            std::cout << " Anotaciones:     " << jugadorBuscado.anotaciones << "\n";
            std::cout << " Equipo:     " << equipoBuscado.nombre << "\n";
            std::cout << " -----------------------------------------------\n\n";

            auxiliares::ingresarDatos(confirmacion, "¿Está seguro de eliminar este jugador? (S/N): ");
            auxiliares::waitfor(1500);
            auxiliares::limpiarPantalla();
            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

                // Llamamos a la logica
                bool eliminado = logica::jugadores::eliminarJugador(nombreArchivo, ID);

                if (eliminado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "           Jugador eliminado con éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cerr << "\nError: No se pudo eliminar al jugador.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\nElimnación de datos cancelada\n";
            } else {
                std::cerr << "\nError: Opción inválida (S/N).\nEliminación de datos cancelada.\n";
            }
            auxiliares::pausarPrograma();
        }

    } // namespace jugadores

    namespace partidos {

        void programarPartido(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int error = -1;
            bool cancelado = false;
            bool encontrado = false;
            Equipo eqLocal, eqVisitante;

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            // Leemos el header del archivo de equipo para saber el numero de reisgtros activos
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerEquipos.cantidadRegistros == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay mas equipos activos registrados
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "\nNo hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            if (headerEquipos.registrosActivos <= 1) {
                std::cout << "\nNo es posible programar un partido con solo un equipo\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Calculamos el minimo de jugadores en el deporte actual para organizar un partido
            Partido nuevoPartido;
            bool flagError = false;
            char confirmacion;
            bool programado = false;

            // Recolectamos el ID LOCAL
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarDatos(nuevoPartido.idEquipoLocal, " Ingrese el ID del equipo local (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\n Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Si el ID no corresponde a ningun equipo
                if (!logica::existeID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, nuevoPartido.idEquipoLocal)) {
                    std::cerr << " Error el ID '" << nuevoPartido.idEquipoLocal << "' no está asociado a ningún equipo\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                }

            } while (flagError);

            // Recolectamos el ID VISItante
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                if (!auxiliares::ingresarDatos(nuevoPartido.idEquipoVisitante, " Ingrese el ID del equipo visitante (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\n Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Si el ID no corresponde a ningun equipo
                if (!logica::existeID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, nuevoPartido.idEquipoVisitante)) {
                    std::cerr << "Error el ID '" << nuevoPartido.idEquipoVisitante << "' no está asociado a ningún equipo\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                }

                // Si el ID VISITANTE es el mismo que el ID del local
                if (nuevoPartido.idEquipoLocal == nuevoPartido.idEquipoVisitante) {
                    std::cerr << " Error: no se puede programar un partido entre un mismo equipo\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                }
            } while (flagError);

            auxiliares::limpiarPantalla();

            // Buscamos el equipo local
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, nuevoPartido.idEquipoLocal);

            // Verificamos si fue encontrado
            if (!encontrado) {
                std::cerr << "\n Error del Sistema! \n";
                std::cout << " Operación Cancelada \n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo visitante
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, nuevoPartido.idEquipoVisitante);

            // Verificamos si fue encontrado
            if (!encontrado) {
                std::cerr << "\n Error del Sistema! \n";
                std::cout << " Operación Cancelada \n";
                auxiliares::pausarPrograma();
                return;
            }

            /*
            // Si no cumplen con el minimo de jugadores
            if ((eqLocal.numJugadores < minimoDeJugadores) || (eqVisitante.numJugadores < minimoDeJugadores)) {
                std::cout << "Error no se puede programar un partido.\n";
                std::cout << "Los equipos no cumplen con el minimo de jugadores establecido\n\n";
                std::cout << "Deporte: " << torneo.deporte << std::endl;
                std::cout << "Minimo de Jugadores por Equipo: " << minimoDeJugadores << std::endl;
                std::cout << "Numero de Jugadores de '" << eqLocal.nombre << "': " << eqLocal.numJugadores << std::endl;
                std::cout << "Numero de Jugadores de '" << eqVisitante.nombre << "': " << eqVisitante.numJugadores << std::endl;
                auxiliares::pausarPrograma();
                return;
            }*/

            // Abrimos el archivo de Partidos
            Partido partidoAux;
            std::fstream archivoPartidos;
            archivoPartidos.open(nombreArchivo, std::ios::binary | std::ios::in | std::ios::out);

            // Verificamos si abrió el archivo
            if (!archivoPartidos.is_open()) {
                std::cerr << "\n Error del Sistema!\n";
                std::cout << " Operación Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Ubicamos el puntero de lectura despues del header
            archivoPartidos.seekg(sizeof(ArchivoHeader), std::ios::beg);

            // Si ya tienen un partido programado
            while (archivoPartidos.read(reinterpret_cast<char *>(&partidoAux), sizeof(Partido))) {

                // Verificamos que la lectura sea correcta
                if (archivoPartidos.fail()) {
                    std::cerr << "\n Error del Sistema! \n";
                    std::cout << " Operación Cancelada \n";
                    auxiliares::pausarPrograma();
                    return;
                }

                bool partidoEntreSi = (((partidoAux.idEquipoLocal == nuevoPartido.idEquipoLocal) && (partidoAux.idEquipoVisitante == nuevoPartido.idEquipoVisitante)) ||
                                       ((partidoAux.idEquipoLocal == nuevoPartido.idEquipoVisitante) && (partidoAux.idEquipoVisitante == nuevoPartido.idEquipoLocal)));
                if (partidoEntreSi && (std::strcmp(partidoAux.estado, logica::partidos::estadoPartidos[0]) == 0)) {
                    std::cerr << " Error ya hay un partido programado entre el equipo " << eqLocal.nombre << "' y '" << eqVisitante.nombre << std::endl;
                    auxiliares::pausarPrograma();
                    return;
                }
            }

            // Cerramos el archivo de Partidos
            archivoPartidos.close();

            // Pedimos la fecha
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " Fecha de Inicio del Torneo: " << torneo.fechaInicio << std::endl;
            if (!auxiliares::ingresarCadena(nuevoPartido.fecha, TAMANO_FECHA, " Ingrese la fecha del partido (YYYY-MM-DD) (ingrese 'cancelar' para cancelar): ", &cancelado,
                                            validadores::fechaValidaRegistroDePartidos)) {
                std::cout << "\n Operacion Cancelada por el Usuario\n";
                auxiliares::pausarPrograma();
                return;
            }
            auxiliares::waitfor(750);
            auxiliares::limpiarPantalla();

            // Pedimos la descripcion
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║           PROGRAMAR PARTIDO               ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(nuevoPartido.descripcion, TAMANO_DESCRIPCION, " Ingrese la descripción del partido (ingrese 'cancelar' para cancelar): ", &cancelado,
                                            validadores::nombreTorneo)) {
                std::cout << "\n Operacion Cancelada por el Usuario\n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::waitfor(750);
            auxiliares::limpiarPantalla();

            std::cout << "\n";
            auxiliares::ingresarDatos(confirmacion, " Confirme la programación del partido (S/N): ");
            auxiliares::waitfor(750);
            auxiliares::limpiarPantalla();

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                programado = logica::partidos::programarPartido(nombreArchivo, nuevoPartido);

                if (programado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "                ¡Partido programado con éxito!\n";
                    std::cout << "                " << eqLocal.nombre << "  VS  " << eqVisitante.nombre << "\n";
                    std::cout << "                Fecha: " << nuevoPartido.fecha << std::endl;
                    std::cout << "                ID Asignado: " << nuevoPartido.ID << std::endl;
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cerr << "\n Se produjo un error a la hora de programar el partido.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\n La programación del partido ha sido cancelada.\n";
            } else {
                std::cerr << "\n Error: Opción inválida (S/N).\nLa programación del partido ha sido cancelada.";
            }

            auxiliares::pausarPrograma();
        }

        void registrarResultado(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int error = -1;

            // Variables
            bool flagError = false;
            char confirmacion;
            bool cancelado = false;
            bool encontrado = false;
            Partido registroPartido, partidoAux;

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\n Error del Sistema!\n";
                std::cout << " Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << " No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << " No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Recopilamos el ID del partido a registrar
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                if (!auxiliares::ingresarDatos(registroPartido.ID, " Ingrese el ID del partido a registrar (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\n Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Buscamos el partido
                encontrado = logica::buscarRegistrosPorId<Partido>(nombreArchivo, partidoAux, registroPartido.ID);
                if (!encontrado) {
                    std::cerr << " Error: El ID de partido '" << registroPartido.ID << "' no está asociado a ningún partido.\n";
                    auxiliares::pausarPrograma();
                    return;
                } else if (std::strcmp(partidoAux.estado, logica::partidos::estadoPartidos[0]) != 0) {
                    std::cerr << " Error: El partido ya fue JUGADO o no se encuentra en estado PROGRAMADO.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                }

            } while (flagError);

            // Buscamos los equipos
            Equipo eqLocal, eqVisitante;

            // Buscamos el equipo local
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, partidoAux.idEquipoLocal);

            // Verificamos que fue encontrado
            if (!encontrado) {
                std::cerr << "\n Error del Sistema!\n";
                std::cout << " Operacion Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo visitante
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, partidoAux.idEquipoVisitante);

            // Verificamos que fue encontrado
            if (!encontrado) {
                std::cerr << "\n Error del Sistema!\n";
                std::cout << " Operacion Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            if (eqLocal.cantidadPartidos >= 50 && eqVisitante.cantidadPartidos >= 50) {
                std::cerr << "\n Error: El equipo local de ID '" << eqLocal.ID << "' y nombre '" << eqLocal.nombre << " ha llegado al maximo de partidos (50) ";
                std::cerr << "\n Error: El equipo visitante de ID '" << eqVisitante.ID << "' y nombre '" << eqVisitante.nombre << " ha llegado al maximo de partidos (50) ";
                std::cout << "\n Operacion Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            } else if (eqLocal.cantidadPartidos >= 50) {
                std::cerr << "\n Error: El equipo local de ID '" << eqLocal.ID << "' y nombre '" << eqLocal.nombre << " ha llegado al maximo de partidos (50) \n";
                std::cout << " Operacion Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            } else if (eqVisitante.cantidadPartidos >= 50) {
                std::cerr << "\n Error: El equipo visitante de ID '" << eqVisitante.ID << "' y nombre '" << eqVisitante.nombre << " ha llegado al maximo de partidos (50) \n";
                std::cout << " Operacion Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Recolectamos las anotaciones del partido
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           REGISTRAR ANOTACIONES           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << " Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo local
                if (!auxiliares::ingresarDatos(registroPartido.anotacionesLocal, "Número de Anotaciones del Equipo Local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();

                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << " Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo Visitante
                if (!auxiliares::ingresarDatos(registroPartido.anotacionesVisitante,
                                               " Número de Anotaciones del Equipo Visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos que sean positivos
                if (registroPartido.anotacionesLocal < 0 || registroPartido.anotacionesVisitante < 0) {
                    std::cerr << "Error: El número de anotaciones no puede ser un valor negativo.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                    // Validamos el empate
                } else if (registroPartido.anotacionesLocal == registroPartido.anotacionesVisitante) {

                    if (std::strcmp(torneo.deporte, "BALONCESTO") == 0 || std::strcmp(torneo.deporte, "TENIS") == 0 || std::strcmp(torneo.deporte, "VOLEIBOL") == 0 ||
                        std::strcmp(torneo.deporte, "BEISBOL") == 0 || std::strcmp(torneo.deporte, "SOFTBOL") == 0) {
                        std::cout << "Error: En el deporte " << torneo.deporte << " no se permiten empates. Registre el marcador final con prórroga.\n";
                        auxiliares::waitfor(3000);
                        flagError = true;
                        continue;
                    }
                }

                // Obtenemos el numero de anotaciones totales
                registroPartido.numAnotaciones = registroPartido.anotacionesLocal + registroPartido.anotacionesVisitante;

                // Verificamos que el numero de anotaciones no supere el máximo permitido
                if (registroPartido.numAnotaciones > MAX_ANOTACIONES) {
                    std::cout << "Error: El número de anotaciones no puede ser mayor al maximo permitido (" << MAX_ANOTACIONES << ").\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }

                // * Registramos el detalle de cada gol

                // Registramos las anotaciones del local
                for (int e = 0; e < registroPartido.anotacionesLocal; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Anotaciones del Equipo Local (" << eqLocal.nombre << ") ---------- \n";
                    std::cout << " Anotacion " << e + 1 << "/" << registroPartido.anotacionesLocal << "\n";

                    // Pedimos el minuto en el que anotó el gol
                    if (!auxiliares::ingresarDatos(registroPartido.anotaciones[e].minuto, " Ingrese el minuto en el que se anotó (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                   validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    int idJugador = 0;
                    do {
                        idJugador = 0;

                        flagError = false;
                        // Pedimos el ID del jugador que anotó el gol
                        if (!auxiliares::ingresarDatos(idJugador, " Ingrese el ID del jugador que anotó (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                       validadores::IDvalidoParaAutogol)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        // Si el id de l jugador no es de autogol
                        if (idJugador != 0) {
                            // Verificamos si el id
                            if (!logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, idJugador)) {
                                std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador \n";
                                flagError = true;
                                auxiliares::pausarPrograma();
                                continue;
                            }

                            Jugador jugadorAux;
                            logica::buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador);

                            if (jugadorAux.idEquipo != eqLocal.ID) {
                                std::cerr << "\n Error: El jugador '" << jugadorAux.nombre << "' de ID '" << idJugador << "' no pertenece a el equipo local (" << eqLocal.nombre
                                          << ") \n";
                                flagError = true;
                                auxiliares::pausarPrograma();
                                continue;
                            }
                        }

                    } while (flagError);
                    registroPartido.anotaciones[e].idJugador = idJugador;
                    std::strncpy(registroPartido.anotaciones[e].equipo, "LOCAL", TAMANO_LOCAL_O_VISITANTE);
                }

                // Registramos las anotaciones del visitante
                for (int e = registroPartido.anotacionesLocal; e < registroPartido.numAnotaciones; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR RESULTADO            ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Anotaciones del Equipo Visitante (" << eqVisitante.nombre << ") ---------- \n";
                    std::cout << " Anotacion " << (e - registroPartido.anotacionesLocal) + 1 << "/" << registroPartido.anotacionesVisitante;

                    // Pedimos el minuto en el que anotó el gol
                    if (!auxiliares::ingresarDatos(registroPartido.anotaciones[e].minuto, " Ingrese el minuto en el que se anotó (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                   validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    int idJugador = 0;

                    do {
                        idJugador = 0;
                        flagError = false;
                        // Pedimos el ID del jugador que anotó el gol
                        if (!auxiliares::ingresarDatos(registroPartido.anotaciones[e].idJugador,
                                                       " Ingrese el ID del jugador que anotó (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::IDvalidoParaAutogol)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        // Si el id de l jugador no es de autogol
                        if (idJugador != 0) {
                            // Verificamos si el id excisste
                            if (!logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, idJugador)) {
                                std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador \n";
                                flagError = true;
                                auxiliares::pausarPrograma();
                                continue;
                            }

                            Jugador jugadorAux;
                            logica::buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador);

                            if (jugadorAux.idEquipo != eqVisitante.ID) {
                                std::cerr << "\n Error: El jugador '" << jugadorAux.nombre << "' de ID '" << idJugador << "' no pertenece a el equipo Visitante ("
                                          << eqVisitante.nombre << ") \n";
                                flagError = true;
                                auxiliares::pausarPrograma();
                                continue;
                            }
                        }

                    } while (flagError);
                    registroPartido.anotaciones[e].idJugador = idJugador;
                    std::strncpy(registroPartido.anotaciones[e].equipo, "VISITANTE", TAMANO_LOCAL_O_VISITANTE);
                }

            } while (flagError);

            // Recoletamos las tarjetas amarillas
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║             REGISTRAR TARJETAS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << " Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo local
                if (!auxiliares::ingresarDatos(registroPartido.tarjetasAmaLocal,
                                               " Número de Tarjetas Amarillas del Equipo Local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();

                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << " Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo Visitante
                if (!auxiliares::ingresarDatos(registroPartido.tarjetasAmaVisitante,
                                               " Número de Tarjetas Amarillas del Equipo Visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos que sean positivos
                if (registroPartido.tarjetasAmaLocal < 0 || registroPartido.tarjetasAmaVisitante < 0) {
                    std::cerr << " Error: El número de tarjetas amarillas no puede ser un valor negativo.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }

                // Obtenemos el numero de tarjetas Amarillas totales
                registroPartido.numtarjetaAma = registroPartido.tarjetasAmaLocal + registroPartido.tarjetasAmaVisitante;

                // Verificamos que el numero de TARJETAS amarillas no supere el máximo permitido
                if (registroPartido.numtarjetaAma > MAX_TARJETAS_AMARILLAS) {
                    std::cerr << " Error: El número de tarjetas Amarillas no puede ser mayor al maximo permitido (" << MAX_TARJETAS_AMARILLAS << ").\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }

                // * Registramos el detalle de cada tarjeta amarillas

                // Registramos las tarjetas amarillas del local
                for (int e = 0; e < registroPartido.tarjetasAmaLocal; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Tarjetas Amarillas del Equipo Local (" << eqLocal.nombre << ") ---------- \n";
                    std::cout << " Tarjeta A " << e + 1 << "/" << registroPartido.tarjetasAmaLocal;

                    // Pedimos el minuto en el que anotó el gol
                    if (!auxiliares::ingresarDatos(registroPartido.tarjetaA[e].minuto,
                                                   " Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }


                    int idJugador = 0;
                    do {
                        idJugador = 0;
                        flagError = false;
                        // Pedimos el ID del jugador que le sacaron la tarjeta amarilla
                        if (!auxiliares::ingresarDatos(idJugador, " Ingrese el ID del jugador que tiene tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                       validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        if (!logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, idJugador)) {
                            std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                        Jugador jugadorAux;
                        logica::buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador);

                        if (jugadorAux.idEquipo != eqLocal.ID) {
                            std::cerr << "\n Error: El jugador '" << jugadorAux.nombre << "' de ID '" << idJugador << "' no pertenece a el equipo local (" << eqLocal.nombre
                                      << ") \n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);

                    registroPartido.tarjetaA[e].idJugador = idJugador;
                    std::strncpy(registroPartido.tarjetaA[e].equipo, "LOCAL", TAMANO_LOCAL_O_VISITANTE);
                }

                // Registramos las tarjetas amarillas del visitante
                for (int e = registroPartido.tarjetasAmaLocal; e < registroPartido.numtarjetaAma; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Tarjetas Amarillas del Equipo Visitante (" << eqVisitante.nombre << ") ---------- \n";
                    std::cout << " Tarjeta A " << (e - registroPartido.tarjetasAmaLocal) + 1 << "/" << registroPartido.tarjetasAmaVisitante;

                    // Pedimos el minuto en el que anotó el gol
                    if (!auxiliares::ingresarDatos(registroPartido.tarjetaA[e].minuto,
                                                   " Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    int idJugador = 0;
                    do {
                        idJugador = 0;
                        flagError = false;
                        // Pedimos el ID del jugador que tiene tarjeta amarilla
                        if (!auxiliares::ingresarDatos(idJugador, " Ingrese el ID del jugador que tiene la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                       validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        if (!logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, idJugador)) {
                            std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                        Jugador jugadorAux;
                        logica::buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador);

                        if (jugadorAux.idEquipo != eqVisitante.ID) {
                            std::cerr << "\n Error: El jugador '" << jugadorAux.nombre << "' de ID '" << idJugador << "' no pertenece a el equipo Visitante (" << eqVisitante.nombre
                                      << ") \n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);
                    registroPartido.tarjetaA[e].idJugador = idJugador;
                    std::strncpy(registroPartido.tarjetaA[e].equipo, "VISITANTE", TAMANO_LOCAL_O_VISITANTE);
                }

            } while (flagError);

            // Recolectamos las tarjetas rojas
            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║             REGISTRAR TARJETAS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << " Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo local
                if (!auxiliares::ingresarDatos(registroPartido.tarjetasRojasLocal, " Número de Tarjetas Rojas del Equipo Local (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();

                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " Deporte Actual del Torneo: " << torneo.deporte << "\n";
                std::cout << " Partido: " << eqLocal.nombre << " VS " << eqVisitante.nombre << "\n\n";

                // Pedimos las del equipo Visitante
                if (!auxiliares::ingresarDatos(registroPartido.tarjetasRojasVisitante,
                                               " Número de Tarjetas Rojas del Equipo Visitante (ingrese 'cancelar' para cancelar): ", &cancelado)) {
                    std::cout << " Operacion Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos que sean positivos
                if (registroPartido.tarjetasRojasLocal < 0 || registroPartido.tarjetasRojasVisitante < 0) {
                    std::cerr << " Error: El número de tarjetas amarillas no puede ser un valor negativo.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }

                // Obtenemos el numero de tarjetas Rojas totales
                registroPartido.numTarjetasRojas = registroPartido.tarjetasRojasLocal + registroPartido.tarjetasRojasVisitante;

                // Verificamos que el numero de tarjetas Rojas no supere el máximo permitido
                if (registroPartido.numTarjetasRojas > MAX_TARJETAS_ROJAS) {
                    std::cerr << " Error: El número de tarjetas Rojas no puede ser mayor al maximo permitido (" << MAX_TARJETAS_ROJAS << ").\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }

                // * Registramos el detalle de cada tarjeta roja

                // Registramos las tarjetas rojas del local
                for (int e = 0; e < registroPartido.tarjetasRojasLocal; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();
                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Tarjetas Rojas del Equipo Local (" << eqLocal.nombre << ") ---------- \n";
                    std::cout << " Tarjeta R " << e + 1 << "/" << registroPartido.tarjetasRojasLocal;

                    // Pedimos el minuto en el que se produjo la tarjeta roja
                    if (!auxiliares::ingresarDatos(registroPartido.tarjetaR[e].minuto,
                                                   " Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    int idJugador = 0;
                    do {
                        idJugador = 0;
                        flagError = false;
                        // Pedimos el ID del jugador que le sacaron la tarjeta roja
                        if (!auxiliares::ingresarDatos(idJugador, " Ingrese el ID del jugador que tiene tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                       validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        // Si no existe un jugador con ese ID
                        if (!logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, idJugador)) {
                            std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                        Jugador jugadorAux;
                        logica::buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador);

                        if (jugadorAux.idEquipo != eqLocal.ID) {
                            std::cerr << "\n Error: El jugador '" << jugadorAux.nombre << "' de ID '" << idJugador << "' no pertenece a el equipo local (" << eqLocal.nombre
                                      << ") \n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);

                    registroPartido.tarjetaR[e].idJugador = idJugador;
                    std::strncpy(registroPartido.tarjetaR[e].equipo, "LOCAL", TAMANO_LOCAL_O_VISITANTE);
                }

                // Registramos las tarjetas rojas del visitante
                for (int e = registroPartido.tarjetasRojasLocal; e < registroPartido.numTarjetasRojas; e++) {
                    auxiliares::waitfor(300);
                    auxiliares::limpiarPantalla();

                    flagError = false;
                    std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                    std::cout << "       ║            REGISTRAR TARJETAS             ║\n";
                    std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                    std::cout << "\n ---------- Detalle de las Tarjetas Rojas del Equipo Visitante (" << eqVisitante.nombre << ") ---------- \n";
                    std::cout << " Tarjeta R " << (e - registroPartido.tarjetasRojasLocal) + 1 << "/" << registroPartido.tarjetasRojasVisitante;

                    // Pedimos el minuto el que le sacaron la tarjeta roja
                    if (!auxiliares::ingresarDatos(registroPartido.tarjetaR[e].minuto,
                                                   " Ingrese el minuto en el que se produjo la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::minuto)) {
                        std::cout << " Operación Cancelada por el Usuario\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    int idJugador = 0;
                    do {
                        idJugador = 0;
                        flagError = false;
                        // Pedimos el ID del jugador que le sacaron la tarjeta Roja
                        if (!auxiliares::ingresarDatos(idJugador, "Ingrese el ID del jugador que tiene la tarjeta (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                       validadores::IDvalido)) {
                            std::cout << " Operación Cancelada por el Usuario\n";
                            auxiliares::pausarPrograma();
                            return;
                        }

                        if (!logica::existeID<Jugador>(NOMBRE_ARCHIVO_JUGADORES, idJugador)) {
                            std::cerr << "\n Error el ID ingresado no pertenece a ningún jugador\n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                        Jugador jugadorAux;
                        logica::buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, idJugador);

                        if (jugadorAux.idEquipo != eqVisitante.ID) {
                            std::cerr << "\n Error: El jugador '" << jugadorAux.nombre << "' de ID '" << idJugador << "' no pertenece a el equipo Visitante (" << eqVisitante.nombre
                                      << ") \n";
                            flagError = true;
                            auxiliares::pausarPrograma();
                            continue;
                        }

                    } while (flagError);

                    registroPartido.tarjetaR[e].idJugador = idJugador;
                    std::strncpy(registroPartido.tarjetaR[e].equipo, "VISITANTE", TAMANO_LOCAL_O_VISITANTE);
                }

            } while (flagError);

            std::strncpy(registroPartido.estado, logica::partidos::estadoPartidos[1], TAMANO_ESTADO);

            // Mostramos el marcador
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║          RESUMEN DEL MARCADOR             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            std::cout << " " << eqLocal.nombre << " " << registroPartido.anotacionesLocal << "  -  " << registroPartido.anotacionesVisitante << " " << eqVisitante.nombre << "\n\n";

            auxiliares::ingresarDatos(confirmacion, " ¿Está seguro de registrar este resultado definitivo? (S/N): ");
            auxiliares::limpiarPantalla();

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                // Invocamos tu función lógica corregida
                bool registrado = logica::partidos::registrarResultado(nombreArchivo, registroPartido);

                if (registrado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "                  ¡Resultado registrado con éxito!\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                    std::cout << " Partido ID:  " << registroPartido.ID << "\n";
                    std::cout << " Estado:      " << "JUGADO\n";
                    std::cout << " Marcador:    " << eqLocal.nombre << " " << registroPartido.anotacionesLocal << "  -  " << registroPartido.anotacionesVisitante << " "
                              << eqVisitante.nombre << "\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cerr << "\nError: No se pudo registrar el partido.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\nRegistro de resultado cancelado.\n";
            } else {
                std::cerr << "\nError: Opción inválida (S/N).\nRegistro de resultado cancelado.\n";
            }
            auxiliares::pausarPrograma();
        }

        void buscarPartidoPorID(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            bool cancelado = false;
            bool encontrado = false;
            int error = -1;
            bool flagError = false;
            Partido partidoBuscado;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << "No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            int idPartido = 0;

            do {
                flagError = false;
                auxiliares::limpiarPantalla();

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           BUSCAR PARTIDO POR ID           ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Recolectamos el ID
                if (!auxiliares::ingresarDatos(idPartido, " Ingrese el ID del partido que desea consultar (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::limpiarPantalla();
                auxiliares::waitfor(500);

                // Buscamos el partido mediante el ID
                encontrado = logica::buscarRegistrosPorId<Partido>(nombreArchivo, partidoBuscado, idPartido);

                // Si no existe, avisamos y salimos
                if (!encontrado) {
                    std::cerr << "\n Error: El ID de partido '" << idPartido << "' no existe en el sistema.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }
                auxiliares::waitfor(1200);
            } while (flagError);

            Equipo eqLocal, eqVisitante;

            // Buscamos los equipos para mostrar nombres reales en la presentacion

            // Buscamos el Local
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, partidoBuscado.idEquipoLocal);

            // Si no existe, avisamos y salimos
            if (!encontrado) {
                std::cerr << "\n Error del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el visitante
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, partidoBuscado.idEquipoVisitante);

            // Si no existe, avisamos y salimos
            if (!encontrado) {
                std::cerr << "\n Error del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(500);
            std::cout << "Buscando..." << std::endl;
            auxiliares::waitfor(1200);
            auxiliares::limpiarPantalla();

            std::cout << "\n╔══════════════════════════════════════════════════╗\n";
            std::cout << "║              DETALLE DE PARTIDO                  ║\n";
            std::cout << "╠══════════════════════════════════════════════════╣\n";
            std::cout << "║ " << std::left << std::setw(14) << "ID Partido" << ": " << std::setw(32) << partidoBuscado.ID << "║\n";
            std::cout << "║ " << std::left << std::setw(14) << "Estado" << ": " << std::setw(32) << partidoBuscado.estado << "║\n";
            std::cout << "║ " << std::left << std::setw(14) << "Fecha" << ": " << std::setw(32) << partidoBuscado.fecha << "║\n";
            std::cout << "║                                                  ║\n";
            std::cout << "║  " << std::left << std::setw(20) << eqLocal.nombre << " " << partidoBuscado.anotacionesLocal << " - " << partidoBuscado.anotacionesVisitante << "  "
                      << std::setw(20) << eqVisitante.nombre << " ║\n";
            std::cout << "║      (Local)                  (Visitante)        ║\n";
            std::cout << "║                                                  ║\n";
            std::cout << "║ Notas: " << std::left << std::setw(41) << partidoBuscado.descripcion << "║\n";
            std::cout << "║                                                  ║\n";
            std::cout << "╚══════════════════════════════════════════════════╝\n\n";

            auxiliares::pausarPrograma();
        }

        void listarTodosLosPartidos(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();
            int error = -1;
            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << "No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            int cantPartidos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Partido listaDePartidos[maxResultados];

            // Obtenemos la lista de partidos
            cantPartidos = logica::listarRegistros<Partido>(nombreArchivo, listaDePartidos, maxResultados);

            if (cantPartidos == 0) {
                std::cout << "No se encontró ningún partido\n";
                auxiliares::pausarPrograma();
                return;
            } else if (cantPartidos == -1) {
                std::cerr << "\nError del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║            LISTADO DE PARTIDOS            ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado"
                      << "Encuentro y Resultado\n";
            std::cout << "--------------------------------------------------------------------------------\n";

            // Recorremos para imprimir los mensajes
            for (int e = 0; e < cantPartidos; e++) {

                // Buscamos los equipos
                bool encontrado = false;

                Equipo eqLocal, eqVisitante;

                // Buscamos el equipo local
                encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, listaDePartidos[e].idEquipoLocal);

                // Verificamos que si lo encontró
                if (!encontrado) {
                    std::cerr << "\nError del Sistema!\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Buscamos el equipo visitante
                encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, listaDePartidos[e].idEquipoVisitante);

                // Verificamos que si lo encontró
                if (!encontrado) {
                    std::cerr << "\nError del Sistema!\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                std::cout << std::left << std::setw(6) << listaDePartidos[e].ID << std::left << std::setw(14) << listaDePartidos[e].fecha << std::left << std::setw(13)
                          << listaDePartidos[e].estado;
                std::cout << eqLocal.nombre << " " << listaDePartidos[e].anotacionesLocal << "  -  " << listaDePartidos[e].anotacionesVisitante << " " << eqVisitante.nombre
                          << "\n";
            }
            std::cout << "--------------------------------------------------------------------------------\n\n";


            auxiliares::pausarPrograma();
        }

        void buscarPartidosPorEquipo(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            int idEquipo = -1;
            bool cancelado = false;
            int cantPartidos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Partido listaDePartidos[maxResultados];
            int error = -1;
            bool flagError = false;
            bool encontrado = false;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << "No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            do {
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        HISTORIAL DE PARTIDOS POR EQUIPO   ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Pedimos al usuario el id
                if (!auxiliares::ingresarDatos(idEquipo, " Ingrese el ID del equipo a consultar (ingresa 'cancelar' para cancelar): ", &cancelado, validadores::IDvalido)) {
                    std::cout << "\n Operación Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                // Verificamos si ese id existe
                if (!logica::existeID<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, idEquipo)) {
                    std::cout << "El id " << idEquipo << " no le pertenece a ningún equipo\n";
                    flagError = true;
                    auxiliares::pausarPrograma();
                    continue;
                }

            } while (flagError);


            auxiliares::limpiarPantalla();
            auxiliares::waitfor(500);
            std::cout << "\nBuscando...\n";
            auxiliares::waitfor(1200);
            auxiliares::limpiarPantalla();

            // Validamos si el equipo existe para poder usarlo
            Equipo equipoBuscado;
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoBuscado, idEquipo);

            if (!encontrado) {
                std::cout << "\n Error del sistema.\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Inicializamos la cantidad de partidos en 0 y buscamos la lista de partidos
            cantPartidos = logica::partidos::listarPartidosPorEquipo(nombreArchivo, idEquipo, listaDePartidos, maxResultados);

            if (cantPartidos == error) {
                std::cerr << "\n Error: Ocurrió un error a la hora de mostrar la lista de partidos por equipo.\n\n";
            } else if (cantPartidos == 0) {
                // Si la lógica creó el arreglo pero el equipo no tiene partidos
                std::cout << "\n El equipo '" << equipoBuscado.nombre << "' no tiene partidos registrados todavia.\n\n";
            } else {
                // Entra aquí si se encontraron partidos para el equipo
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PARTIDOS ENCONTRADOS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n";
                std::cout << " Historial para: " << equipoBuscado.nombre << "\n\n";
                std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado"
                          << "Encuentro y Resultado\n";
                std::cout << "--------------------------------------------------------------------------------\n";

                // Recorremos e imprimimos todos los partidos del equipo
                for (int e = 0; e < cantPartidos; e++) {

                    // Buscamos los equipos
                    Equipo eqLocal, eqVisitante;

                    // Buscamos el local
                    encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, listaDePartidos[e].idEquipoLocal);

                    // Verificamos que fue encontrado
                    if (!encontrado) {
                        std::cerr << "\nError del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    // buscamos el Visitante
                    encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, listaDePartidos[e].idEquipoVisitante);

                    // Verificamos que fue encontrado
                    if (!encontrado) {
                        std::cerr << "\nError del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << std::left << std::setw(6) << listaDePartidos[e].ID << std::left << std::setw(14) << listaDePartidos[e].fecha << std::left << std::setw(13)
                              << listaDePartidos[e].estado;
                    std::cout << eqLocal.nombre << " " << listaDePartidos[e].anotacionesLocal << "  -  " << listaDePartidos[e].anotacionesVisitante << " " << eqVisitante.nombre
                              << "\n";
                }
                std::cout << "--------------------------------------------------------------------------------\n\n";
            }

            auxiliares::pausarPrograma();
        }

        void listarPartidosPorEstado(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            int idEquipo = -1;
            bool cancelado = false;
            int cantPartidos = 0;
            const int maxResultados = MAX_RESULTADOS;
            Partido listaDePartidos[maxResultados];
            int error = -1;
            bool flagError = false;
            bool encontrado = false;
            char estado[TAMANO_ESTADO];
            int opcion = -1;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay partidos activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << "No hay ningún partido programado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            do {
                auxiliares::limpiarPantalla();
                flagError = false;
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║         BUSCAR PARTIDOS POR ESTADO        ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                std::cout << "Estados de partido consultables: \n\n";
                std::cout << "---------------------------------------------\n";
                std::cout << " 0. PROGRAMADO\n 1. JUGADO\n 2. CANCELADO\n\n";
                std::cout << "---------------------------------------------\n";
                auxiliares::ingresarDatos(opcion, "Ingrese el tipo de estado de partido que desea consultar (ingrese 'canelar' para cancelar): ", &cancelado,
                                          validadores::Positivo);

                switch (opcion) {
                    case 0: // Configuramos en PROGRAMADO
                        std::strncpy(estado, logica::partidos::estadoPartidos[0], TAMANO_ESTADO);
                        break;

                    case 1: // Configuramos en JUGADO
                        std::strncpy(estado, logica::partidos::estadoPartidos[1], TAMANO_ESTADO);
                        break;

                    case 2: // Configuramos en CANCELADO
                        std::strncpy(estado, logica::partidos::estadoPartidos[2], TAMANO_ESTADO);
                        break;

                    default:
                        std::cout << "Ingrese una opcion correcta\n";
                        flagError = true;
                        auxiliares::pausarPrograma();
                }
            } while (flagError);

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(500);
            std::cout << "Buscando..." << std::endl;
            auxiliares::waitfor(1200);
            auxiliares::limpiarPantalla();

            // Listamos todos los partidos
            cantPartidos = logica::partidos::listarPartidosPorSuEstado(nombreArchivo, listaDePartidos, estado, maxResultados);

            if (cantPartidos == -1) {
                std::cerr << "\n Error del Sistema.\n";
            } else if (cantPartidos == 0) {
                std::cout << "\n No se encontro ningun partido en estado '" << estado << "' actualmente.\n\n";
            } else {
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║           PARTIDOS ENCONTRADOS            ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n";
                std::cout << " Partidos en estado: " << estado << "\n\n";

                std::cout << std::left << std::setw(6) << "ID" << std::left << std::setw(14) << "Fecha" << std::left << std::setw(13) << "Estado"
                          << "Encuentro y Resultado\n";
                std::cout << "--------------------------------------------------------------------------------\n";

                // Bucle de impresión de registros
                for (int e = 0; e < cantPartidos; e++) {
                    // Buscamos los equipos
                    Equipo eqLocal, eqVisitante;

                    // Buscamos el local
                    encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, listaDePartidos[e].idEquipoLocal);

                    // Verificamos que fue encontrado
                    if (!encontrado) {
                        std::cerr << "\nError del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    // buscamos el Visitante
                    encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, listaDePartidos[e].idEquipoVisitante);

                    // Verificamos que fue encontrado
                    if (!encontrado) {
                        std::cerr << "\nError del Sistema!\n";
                        auxiliares::pausarPrograma();
                        return;
                    }

                    std::cout << std::left << std::setw(6) << listaDePartidos[e].ID << std::left << std::setw(14) << listaDePartidos[e].fecha << std::left << std::setw(13)
                              << listaDePartidos[e].estado;
                    std::cout << eqLocal.nombre << " " << listaDePartidos[e].anotacionesLocal << "  -  " << listaDePartidos[e].anotacionesVisitante << " " << eqVisitante.nombre
                              << "\n";
                }
                std::cout << "--------------------------------------------------------------------------------\n\n";
            }

            auxiliares::pausarPrograma();
        }

        void cancelarPartido(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            int idPartido = -1;
            bool cancelado = false;
            char confirmacion;
            Partido partidoAuxiliar;
            Equipo eqLocal, eqVisitante;
            bool existe = false;
            bool flagError = false;

            // Pedimos  id
            do {
                flagError = false;
                auxiliares::limpiarPantalla();

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║             CANCELAR PARTIDOS             ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n";
                if (!auxiliares::ingresarDatos(idPartido, " Ingrese el ID del Partido que desea cancelar (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << "\n Operación cancelada por el usuario.\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::limpiarPantalla();
                auxiliares::waitfor(250);
                std::cout << "\n Procesando solicitud...";
                auxiliares::waitfor(1200);
                auxiliares::limpiarPantalla();

                // Buscamos el Partido para mostrar los datos de ese partido antes de eliminarlo
                existe = logica::buscarRegistrosPorId<Partido>(nombreArchivo, partidoAuxiliar, idPartido);

                // Verificamos si existe
                if (!existe) {
                    std::cerr << "\n Error: El ID '" << idPartido << "' no pertenece a ningún partido registrado.\n";
                    flagError = true;
                    auxiliares::pausarPrograma();
                }

            } while (flagError);


            // Buscamos el equipo local
            existe = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, partidoAuxiliar.idEquipoLocal);

            // Verificamos si existe
            if (!existe) {
                std::cerr << "\n Error del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el equipo visitante
            existe = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, partidoAuxiliar.idEquipoVisitante);

            // Verificamos si existe
            if (!existe) {
                std::cerr << "\n Error del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║             CANCELAR PARTIDOS             ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << "\n Se borrará el registro del partido: \n\n";
            std::cout << " Encuentro: " << eqLocal.nombre << "  " << partidoAuxiliar.anotacionesLocal << "   -   " << partidoAuxiliar.anotacionesVisitante << "  "
                      << eqVisitante.nombre << std::endl;
            std::cout << " Fecha: " << partidoAuxiliar.fecha << std::endl;
            std::cout << " ID: " << partidoAuxiliar.ID << std::endl;
            std::cout << " Estado del Partido: " << partidoAuxiliar.estado << "\n\n";

            auxiliares::ingresarDatos(confirmacion, " ¿Está seguro de eliminar el registro de este partido? (S/N): ");

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(1000);

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                // Llamamos a la logica
                bool partidoCancelado = logica::partidos::cancelarPartido(nombreArchivo, idPartido);

                if (partidoCancelado) {
                    std::cout << "\n------------------------------------------------------------------------------\n";
                    std::cout << "           Partido Cancelado con Éxito\n";
                    std::cout << "------------------------------------------------------------------------------\n";
                } else {
                    std::cerr << "\n Error: No se pudo cancelar el Partido debido a un Error del Sistema.\n";
                }

            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << " Operación Cancelada!\n";
            } else {
                std::cerr << " Error: No se ingresó una opción válida.\n Operación Cancelada!\n";
            }
            auxiliares::pausarPrograma();
        }

    } // namespace partidos

    namespace reportes {

        void tablaTop10Anotadores(const char *nombreArchivo) {

            int error = -1;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            // Pedimos a la lógica los jugadores ordenados por goles (usa la función existente)
            Jugador tablaAnotadores[MAX_RESULTADOS];
            int cantidad = logica::reportes::tablaDeGoleadoresTop10(nombreArchivo, tablaAnotadores);
            if (cantidad <= 0) {
                std::cout << "\n No hay jugadores disponibles \n";
                auxiliares::pausarPrograma();
                return;
            }

            int limite = (cantidad > 10) ? 10 : cantidad;

            // Mostramos la tabla de posiciones
            auxiliares::toMayus(torneo.nombre);
            std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                             TABLA DE GOLEADORES TOP 10                                                      ║\n";
            std::cout << "║               " << std::left << std::setw(93) << torneo.nombre << " ║\n";
            std::cout << "╠════╦══════════════════════════════════════════╦═════╦═════════════════════════╦═══════════╦═════╦════╦══════╣\n";
            std::cout << "║ #  ║ Jugadores                                ║ ID  ║    Nombre del Equipo    ║ ID Equipo ║  A  ║ PJ ║ Prom ║\n";
            std::cout << "╠════╬══════════════════════════════════════════╬═════╬═════════════════════════╬═══════════╬═════╬════╬══════╣\n";

            for (int e = 0; e < limite; e++) {
                // Buscamos el equipo al que pertenece
                Equipo equipoAuxiliar;
                bool existe = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, tablaAnotadores[e].idEquipo);
                float promedio = (((float)(tablaAnotadores[e].anotaciones)) / ((float)(equipoAuxiliar.jugados)));
                std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(40) << tablaAnotadores[e].nombre << " ║ " << std::left << std::setw(3)
                          << tablaAnotadores[e].ID << " ║ " << std::left << std::setw(23) << equipoAuxiliar.nombre << " ║ " << std::left << std::setw(9)
                          << tablaAnotadores[e].idEquipo << " ║ " << std::left << std::setw(3) << tablaAnotadores[e].anotaciones << " ║ " << std::right << std::setw(2)
                          << equipoAuxiliar.jugados << " ║ " << std::left << std::setw(4) << ((promedio == 0.0) ? 0.0 : promedio) << " ║ \n";
            }
            std::cout << "╚════╩══════════════════════════════════════════╩═════╩═════════════════════════╩═══════════╩═════╩════╩══════╝\n";
            std::cout << "\nReferencia: A = Anotaciones  PJ = PartidosJugados  Prom=Promedio\n\n";
            auxiliares::pausarPrograma();
        }

        void tablaTop10TarjetasAmarillas(const char *nombreArchivo) {

            int error = -1;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            // Pedimos a la lógica los jugadores ordenados por goles (usa la función existente)
            Jugador tablaTarjetasAma[MAX_RESULTADOS];
            int cantidad = logica::reportes::tablaDeGoleadoresTop10(nombreArchivo, tablaTarjetasAma);
            if (cantidad <= 0) {
                std::cout << "\n No hay jugadores disponibles \n";
                auxiliares::pausarPrograma();
                return;
            }

            int limite = (cantidad > 10) ? 10 : cantidad;

            // Mostramos la tabla de posiciones
            auxiliares::toMayus(torneo.nombre);
            std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                          TABLA DE TARJETAS AMARILLAS TOP 10                                                 ║\n";
            std::cout << "║               " << std::left << std::setw(93) << torneo.nombre << " ║\n";
            std::cout << "╠════╦══════════════════════════════════════════╦═════╦═════════════════════════╦═══════════╦═════╦════╦══════╣\n";
            std::cout << "║ #  ║ Jugadores                                ║ ID  ║    Nombre del Equipo    ║ ID Equipo ║  T  ║ PJ ║ Prom ║\n";
            std::cout << "╠════╬══════════════════════════════════════════╬═════╬═════════════════════════╬═══════════╬═════╬════╬══════╣\n";

            for (int e = 0; e < limite; e++) {
                // Buscamos el equipo al que pertenece
                Equipo equipoAuxiliar;
                bool existe = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, tablaTarjetasAma[e].idEquipo);
                float promedio = (((float)(tablaTarjetasAma[e].tarjetaAmarillas)) / ((float)(equipoAuxiliar.jugados)));
                std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(40) << tablaTarjetasAma[e].nombre << " ║ " << std::left
                          << std::setw(3) << tablaTarjetasAma[e].ID << " ║ " << std::left << std::setw(23) << equipoAuxiliar.nombre << " ║ " << std::left << std::setw(9)
                          << tablaTarjetasAma[e].idEquipo << " ║ " << std::left << std::setw(3) << tablaTarjetasAma[e].anotaciones << " ║ " << std::right << std::setw(2)
                          << equipoAuxiliar.jugados << " ║ " << std::left << std::setw(4) << ((promedio == 0.0) ? 0.0 : promedio) << " ║ \n";
            }
            std::cout << "╚════╩══════════════════════════════════════════╩═════╩═════════════════════════╩═══════════╩═════╩════╩══════╝\n";
            std::cout << "\nReferencia: T = Numero de Tarjetas  PJ = PartidosJugados  Prom=Promedio\n\n";
            auxiliares::pausarPrograma();
        }

        void tablaTop10TarjetasRojas(const char *nombreArchivo) {

            int error = -1;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerJugadores = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerJugadores.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << "No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerJugadores.registrosActivos == 0) {
                std::cout << "No hay ningún jugador registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            Jugador tablaTarjetasRojas[MAX_RESULTADOS];
            int cantidad = logica::reportes::tablaDeGoleadoresTop10(nombreArchivo, tablaTarjetasRojas);
            if (cantidad <= 0) {
                std::cout << "\n No hay jugadores disponibles \n";
                auxiliares::pausarPrograma();
                return;
            }

            int limite = (cantidad > 10) ? 10 : cantidad;

            // Mostramos la tabla de posiciones
            auxiliares::toMayus(torneo.nombre);
            std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                                   TABLA DE TARJETAS ROJAS TOP 10                                            ║\n";
            std::cout << "║               " << std::left << std::setw(93) << torneo.nombre << " ║\n";
            std::cout << "╠════╦══════════════════════════════════════════╦═════╦═════════════════════════╦═══════════╦═════╦════╦══════╣\n";
            std::cout << "║ #  ║ Jugadores                                ║ ID  ║    Nombre del Equipo    ║ ID Equipo ║  T  ║ PJ ║ Prom ║\n";
            std::cout << "╠════╬══════════════════════════════════════════╬═════╬═════════════════════════╬═══════════╬═════╬════╬══════╣\n";

            for (int e = 0; e < limite; e++) {
                // Buscamos el equipo al que pertenece
                Equipo equipoAuxiliar;
                bool existe = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, equipoAuxiliar, tablaTarjetasRojas[e].idEquipo);
                float promedio = (((float)(tablaTarjetasRojas[e].tarjetaAmarillas)) / ((float)(equipoAuxiliar.jugados)));
                std::cout << "║ " << std::right << std::setw(2) << (e + 1) << " ║ " << std::left << std::setw(40) << tablaTarjetasRojas[e].nombre << " ║ " << std::left
                          << std::setw(3) << tablaTarjetasRojas[e].ID << " ║ " << std::left << std::setw(23) << equipoAuxiliar.nombre << " ║ " << std::left << std::setw(9)
                          << tablaTarjetasRojas[e].idEquipo << " ║ " << std::left << std::setw(3) << tablaTarjetasRojas[e].anotaciones << " ║ " << std::right << std::setw(2)
                          << equipoAuxiliar.jugados << " ║ " << std::left << std::setw(4) << ((promedio == 0.0) ? 0.0 : promedio) << " ║ \n";
            }
            std::cout << "╚════╩══════════════════════════════════════════╩═════╩═════════════════════════╩═══════════╩═════╩════╩══════╝\n";
            std::cout << "\nReferencia: T = Numero de Tarjetas  PJ = PartidosJugados  Prom=Promedio\n\n";
            auxiliares::pausarPrograma();
        }

        void fichaTecnica(const char *nombreArchivo) {
            int error = -1;
            bool flagError = false;
            bool encontrado = false;
            bool cancelado = false;

            // Leemos el header del archivo de jugadores para saber el numero de reisgtros activos
            ArchivoHeader headerPartidos = logica::leerHeader(nombreArchivo);
            ArchivoHeader headerEquipos = logica::leerHeader(NOMBRE_ARCHIVO_EQUIPOS);

            // Verificamos que la lectura del header fue correcta
            if (headerPartidos.cantidadRegistros == error || headerEquipos.registrosActivos == error) {
                std::cerr << "\nError del Sistema!\n";
                std::cout << "Busqueda Cancelada\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay equipos disponibles
            if (headerEquipos.registrosActivos == 0) {
                std::cout << " No hay ningún equipo registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Si no hay jugadores activos registrados
            if (headerPartidos.registrosActivos == 0) {
                std::cout << " No hay ningún partido registrado actualmente\n";
                auxiliares::pausarPrograma();
                return;
            }

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            //
            int idPartido = 0;
            Partido partidoBuscado;

            do {
                flagError = false;
                auxiliares::limpiarPantalla();

                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║               FICHA TECNICA               ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Recolectamos el ID
                if (!auxiliares::ingresarDatos(idPartido, " Ingrese el ID del partido que desea consultar (ingrese 'cancelar' para cancelar): ", &cancelado,
                                               validadores::IDvalido)) {
                    std::cout << " Operación Cancelada por el Usuario\n";
                    auxiliares::pausarPrograma();
                    return;
                }

                auxiliares::limpiarPantalla();
                auxiliares::waitfor(500);

                // Buscamos el partido mediante el ID
                encontrado = logica::buscarRegistrosPorId<Partido>(nombreArchivo, partidoBuscado, idPartido);

                // Si no existe, avisamos y salimos
                if (!encontrado) {
                    std::cerr << "\n Error: El ID de partido '" << idPartido << "' no existe en el sistema.\n";
                    auxiliares::waitfor(2000);
                    flagError = true;
                    continue;
                }
                auxiliares::waitfor(1200);
            } while (flagError);

            Equipo eqLocal, eqVisitante;

            // Buscamos el Local
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqLocal, partidoBuscado.idEquipoLocal);

            // Si no existe, avisamos y salimos
            if (!encontrado) {
                std::cerr << "\n Error del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            // Buscamos el visitante
            encontrado = logica::buscarRegistrosPorId<Equipo>(NOMBRE_ARCHIVO_EQUIPOS, eqVisitante, partidoBuscado.idEquipoVisitante);

            // Si no existe, avisamos y salimos
            if (!encontrado) {
                std::cerr << "\n Error del Sistema!\n";
                auxiliares::pausarPrograma();
                return;
            }

            auxiliares::limpiarPantalla();
            auxiliares::waitfor(500);
            std::cout << "\n Cargando Ficha Técnica... \n" << std::endl;
            auxiliares::waitfor(1200);
            auxiliares::limpiarPantalla();

            // Mostrar ficha tecnica

            // Imprimimos la ficha con más detalle: goles, tarjetas y nombre del torneo
            std::cout << "\n╔════════════════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                      FICHA TÉCNICA DEL PARTIDO                         ║\n";
            std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
            std::cout << "║ Torneo: " << std::left << std::setw(60) << torneo.nombre << "   ║\n";
            std::cout << "║ Partido ID: " << std::left << std::setw(10) << partidoBuscado.ID << " Fecha: " << std::left << std::setw(14) << partidoBuscado.fecha
                      << " Estado: " << std::left << std::setw(12) << partidoBuscado.estado << "    ║\n";
            std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
            std::cout << "║ " << std::left << std::setw(28) << eqLocal.nombre << std::right << std::setw(3) << partidoBuscado.anotacionesLocal << "  -  " << std::setw(3)
                      << partidoBuscado.anotacionesVisitante << "   " << std::left << std::setw(28) << eqVisitante.nombre << " ║\n";
            std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
            std::cout << "║ Notas: " << std::left << std::setw(63) << partidoBuscado.descripcion << " ║\n";
            std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
            std::cout << "║ GOLES:                                                                 ║\n";
            if (partidoBuscado.numAnotaciones <= 0) {
                std::cout << "║   Ningun gol registrado                                                  ║\n";
            } else {
                for (int e = 0; e < partidoBuscado.numAnotaciones; e++) {
                    Anotacion anotacion = partidoBuscado.anotaciones[e];
                    // Obtener nombre del jugador si existe
                    char nombreJugador[TAMANO_NOMBRE] = "Jugador desconocido";
                    if (anotacion.idJugador > 0) {
                        Jugador jugadorAux;
                        if (logica::buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, anotacion.idJugador)) {
                            std::strncpy(nombreJugador, jugadorAux.nombre, TAMANO_NOMBRE - 1);
                            nombreJugador[TAMANO_NOMBRE - 1] = '\0';
                        }
                    } else if (anotacion.idJugador == 0) {
                        std::strncpy(nombreJugador, "AUTOGOL", TAMANO_NOMBRE);
                    }
                    std::string etiqueta = "[" + std::string(anotacion.equipo) + "]";
                    std::cout << "║  " << std::left << std::setw(12) << etiqueta << " Min. " << std::right << std::setw(3) << anotacion.minuto << " - " << std::left
                              << std::setw(45) << nombreJugador << "║\n";
                }
            }
            std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
            std::cout << "║ TARJETAS AMARILLAS:                                                     ║\n";
            if (partidoBuscado.numtarjetaAma <= 0) {
                std::cout << "║   Ninguna tarjeta amarilla registrada                                   ║\n";
            } else {
                for (int e = 0; e < partidoBuscado.numtarjetaAma; e++) {
                    tarjetaAmarilla tarjetaAma = partidoBuscado.tarjetaA[e];
                    char nombreJugador[TAMANO_NOMBRE] = "Jugador desconocido";
                    if (tarjetaAma.idJugador > 0) {
                        Jugador jugadorAux;
                        if (logica::buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, tarjetaAma.idJugador)) {
                            std::strncpy(nombreJugador, jugadorAux.nombre, TAMANO_NOMBRE - 1);
                            nombreJugador[TAMANO_NOMBRE - 1] = '\0';
                        }
                    }
                    std::string etiqueta = "[" + std::string(tarjetaAma.equipo) + "]";
                    std::cout << "║  " << std::left << std::setw(10) << etiqueta << " Min. " << std::right << std::setw(3) << tarjetaAma.minuto << " - " << std::left
                              << std::setw(44) << nombreJugador << "║\n";
                }
            }
            std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
            std::cout << "║ TARJETAS ROJAS:                                                         ║\n";
            if (partidoBuscado.numTarjetasRojas <= 0) {
                std::cout << "║   Ninguna tarjeta roja registrada                                       ║\n";
            } else {
                for (int e = 0; e < partidoBuscado.numTarjetasRojas; e++) {
                    tarjetaRoja tarjetaRoja = partidoBuscado.tarjetaR[e];
                    char nombreJugador[TAMANO_NOMBRE] = "Jugador desconocido";
                    if (tarjetaRoja.idJugador > 0) {
                        Jugador jugadorAux;
                        if (logica::buscarRegistrosPorId<Jugador>(NOMBRE_ARCHIVO_JUGADORES, jugadorAux, tarjetaRoja.idJugador)) {
                            std::strncpy(nombreJugador, jugadorAux.nombre, TAMANO_NOMBRE - 1);
                            nombreJugador[TAMANO_NOMBRE - 1] = '\0';
                        }
                    }
                    std::string etiqueta = "[" + std::string(tarjetaRoja.equipo) + "]";
                    std::cout << "║  " << std::left << std::setw(10) << etiqueta << " Min. " << std::right << std::setw(3) << tarjetaRoja.minuto << " - " << std::left
                              << std::setw(44) << nombreJugador << "║\n";
                }
            }
            std::cout << "╚════════════════════════════════════════════════════════════════════════╝\n\n";

            auxiliares::pausarPrograma();
        }

    } // namespace reportes

    namespace mantenimiento {

        void verificarIntegridadReferencial() {
            auxiliares::limpiarPantalla();
            std::cout << "\n Verificando integridad referencial...\n";
            auxiliares::waitfor(1000);

            InformeIntegridad informe;

            // Si no
            if (!logica::mantenimiento::verificarIntegridadReferencial(informe)) {
                std::cerr << "\n Error del Sistema!\n";
            }

            auxiliares::limpiarPantalla();

            std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
            std::cout << "║          INFORME DE INTEGRIDAD REFERENCIAL            ║\n";
            std::cout << "╠═══════════════════════════════════════════════════════╣\n";
            std::cout << "║  Equipos verificados   : " << std::left << std::setw(29) << informe.equiposVerificados << "║\n";
            std::cout << "║  Jugadores verificados : " << std::left << std::setw(29) << informe.jugadoresVerificados << "║\n";
            std::cout << "║  Partidos verificados  : " << std::left << std::setw(29) << informe.partidosVerificados << "║\n";
            std::cout << "║                                                       ║\n";
            std::cout << "║  Referencias rotas encontradas: " << std::left << std::setw(22) << informe.totalDeReferenciasRotas << "║\n";

            if (informe.totalDeReferenciasRotas == 0) {
                std::cout << "║  Estado: OK - SISTEMA INTEGRO                         ║\n";
            } else {
                std::cout << "║  Estado: ERROR - SE ENCONTRARON REFERENCIAS ROTAS     ║\n";
            }
            std::cout << "╚═══════════════════════════════════════════════════════╝\n\n";

            for (size_t e = 0; e < informe.totalDeReferenciasRotas; e++) {
                std::cout << " ROTO: " << informe.rotas[e].tipoDeReferencia << " ID " << informe.rotas[e].idOrigen << " apunta a ID " << informe.rotas[e].idReferenciaRota
                          << " (no existe)\n";
            }

            auxiliares::pausarPrograma();
        }

        void crearBackups(const char *nombreArchivo) {
            auxiliares::limpiarPantalla();

            bool backupCreado = false;
            char confirmacion;

            auxiliares::ingresarDatos(confirmacion, " ¿Está seguro de crear el backup? (S/N): ");

            auxiliares::waitfor(750);
            auxiliares::limpiarPantalla();

            if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {

                // Creamos la carperta de backups
                bool creado = crearCarpeta(nombreArchivo, "Backups");

                if (!creado) {
                    std::cerr << "\n Error del Sistema!\n ";
                }

                std::string nombreCarpeta, rutaBackup;
                backupCreado = logica::mantenimiento::crearBackup(nombreCarpeta);
                rutaBackup = RUTA_BACKUPS + nombreCarpeta;

                if (backupCreado) {
                    std::cout << "\n Respaldo creado con exito!\n";
                    // std::cout << " Ruta del backup: " << fs::absolute(nombreCarpeta) << "\n";
                    std::cout << "\n Backup creado correctamente en: " << rutaBackup << std::endl;
                } else {
                    std::cerr << " Error: Se produjo un error a la hora de crear el backup.\n";
                }
            } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                std::cout << "\n Creación de Backup cancelado.\n";
            } else {
                std::cerr << "\n Error: Opción inválida (S/N).\n Creación de Backup cancelado.\n";
            }
            auxiliares::pausarPrograma();
        }
    } // namespace mantenimiento

    namespace menu {

        void datosInicialesTorneo() {

            // Leemos el torneo para ver si ya ha sido inicializado
            std::fstream archivoTorneo;
            archivoTorneo.open(NOMBRE_ARCHIVO_TORNEO, std::ios::binary | std::ios::in | std::ios::out);

            Torneo torneoAux; // Para lectura

            // Verificamos que abrió correctamente
            if (!archivoTorneo.is_open()) {
                std::cerr << " Error del sistema!\n";
                auxiliares::pausarPrograma();
                std::exit(1);
            }

            // Movemos el puntero de lectura al inicio
            archivoTorneo.seekg(0, std::ios::beg);

            // Verificamos si el archivo contiene un registro Torneo válido
            archivoTorneo.seekg(0, std::ios::end);
            auto tamañoArchivo = archivoTorneo.tellg();
            archivoTorneo.seekg(0, std::ios::beg);

            if (tamañoArchivo >= static_cast<std::streamoff>(sizeof(Torneo))) {
                archivoTorneo.read(reinterpret_cast<char *>(&torneoAux), sizeof(Torneo));
                if (archivoTorneo.fail()) {
                    std::cerr << " Error del Sistema!\n";
                    std::exit(1);
                }
            } else {
                torneoAux = Torneo{};
                torneoAux.inicializado = false;
            }

            // Si ya esta inicializado no hacemos nada
            if (torneoAux.inicializado) {
                std::cout << "\n Iniciando el torneo " << torneoAux.nombre << "... \n";
                auxiliares::waitfor(2000);
                return;
            }

            // variables auxiliares
            bool cancelado = false;
            bool deporteValido = false;
            Torneo torneo;
            int opcionFormato = 0;
            bool opcionValida = false;

            // * Aqui se recopilan los datos iniciales del torneo

            // Ingresar Nombre
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(torneo.nombre, TAMANO_NOMBRE, " Nombre del Torneo (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::nombreTorneo)) {
                std::cout << "\n Operación Cancelada por el Usuario \n";
                auxiliares::waitfor(1000);
                std::cout << "\n Saliendo del Programa... \n";
                auxiliares::waitfor(1500);
                std::exit(1);
            }
            auxiliares::waitfor(1500);

            // Ingresar Deporte
            do {
                char mensajeError[TAMANO_MENSAJE_ERROR];
                deporteValido = false;

                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                // Mostrar deportes disponibles
                std::cout << " Deportes disponibles:\n";
                for (size_t e = 0; e < validadores::totalDeportes; e++) {
                    std::cout << " - " << validadores::Deportes[e] << std::endl;
                }
                std::cout << std::endl;

                if (!auxiliares::ingresarCadena(torneo.deporte, TAMANO_DEPORTE, " Deporte del Torneo (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                validadores::nombreTorneo)) {
                    std::cout << "\n Operación Cancelada por el Usuario \n";
                    auxiliares::waitfor(1000);
                    std::cout << "\n Saliendo del Programa... \n";
                    auxiliares::waitfor(1500);
                    std::exit(1);
                }

                // Convertimos a mayus
                auxiliares::toMayus(torneo.deporte);

                if (!validadores::existeDeporte(torneo.deporte, mensajeError)) {
                    std::cerr << " Error: " << mensajeError << std::endl;
                    auxiliares::waitfor(2000);
                } else {
                    deporteValido = true;
                }

            } while (!deporteValido);

            auxiliares::waitfor(1500);

            // Ingresar Formato
            do {
                opcionValida = true;
                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
                std::cout << " 1. Formato de Grupos (Todos contra todos)\n";
                std::cout << " 2. Formato de Eliminatoria Directa\n";
                std::cout << "--------------------------------------------------\n";
                if (!auxiliares::ingresarDatos(opcionFormato, " Seleccione el formato (1 o 2) (ingrese 'cancelar' para cancelar): ", &cancelado, validadores::Positivo)) {
                    std::cout << "\n Operación Cancelada por el Usuario \n";
                    auxiliares::waitfor(1000);
                    std::cout << "\n Saliendo del Programa... \n";
                    auxiliares::waitfor(1500);
                    std::exit(1);
                }
                if (opcionFormato != 1 && opcionFormato != 2) {
                    std::cerr << " Opcion invalida. Intente de nuevo.\n";
                    opcionValida = false;
                    auxiliares::pausarPrograma();
                }
            } while (!opcionValida);

            // desde la logica definimos el tipo de torneo en base a la opcion ingresada
            logica::definirFormato(torneo, opcionFormato);
            auxiliares::waitfor(1500);

            // Ingresar Fecha de Inicio del torneo
            auxiliares::limpiarPantalla();
            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";
            if (!auxiliares::ingresarCadena(torneo.fechaInicio, TAMANO_FECHA, " Fecha De Inicio del Torneo (ingrese 'cancelar' para cancelar): ", &cancelado,
                                            validadores::validarFecha)) {
                std::cout << "\n Operación Cancelada por el Usuario \n";
                auxiliares::waitfor(1000);
                std::cout << "\n Saliendo del Programa... \n";
                auxiliares::waitfor(1500);
                std::exit(1);
            }
            auxiliares::waitfor(1500);

            bool fechaValida = false;
            do {

                // Ingresar Fecha de Finalizacion de Torneo
                auxiliares::limpiarPantalla();
                std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
                std::cout << "       ║        DATOS INICIALES DEL TORNEO         ║\n";
                std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

                if (!auxiliares::ingresarCadena(torneo.fechaFin, TAMANO_FECHA, " Fecha de Finalización del Torneo (ingrese 'cancelar' para cancelar): ", &cancelado,
                                                validadores::ValidarFechaFin)) {
                    std::cout << "\n Operación Cancelada por el Usuario \n";
                    auxiliares::waitfor(1000);
                    std::cout << "\n Saliendo del Programa... \n";
                    auxiliares::waitfor(1500);
                    std::exit(1);
                }

                fechaValida = validadores::validarFechaFinalEnDatosIniciales(torneo.fechaInicio, torneo.fechaFin);

                // Si la fecha no es valida
                if (!fechaValida) {
                    std::cerr << "\n Error: La fecha ingresada es antes de la fecha de inicio del torneo \n";
                    std::cout << " Ingrese una fecha despues de " << torneo.fechaInicio;
                    auxiliares::pausarPrograma();
                }

            } while (!fechaValida);

            auxiliares::waitfor(1500);
            auxiliares::limpiarPantalla();

            // Movemos el puntero de escritura al inicio
            archivoTorneo.seekp(0, std::ios::beg);

            // guardamos los datos en el fichero binario
            archivoTorneo.write(reinterpret_cast<const char *>(&torneo), sizeof(Torneo));

            // Verificamos que la escritura fue correcta
            if (archivoTorneo.fail()) {
                std::cerr << " Error del Sistema!\n";
                std::exit(1);
            }


            std::cout << "\n       ╔═══════════════════════════════════════════╗\n";
            std::cout << "       ║       NUEVO TORNEO CREADO CON ÉXITO       ║\n";
            std::cout << "       ╚═══════════════════════════════════════════╝\n\n";

            std::cout << " Nombre: " << torneo.nombre << std::endl;
            std::cout << " Deporte: " << torneo.deporte << std::endl;
            std::cout << " Formato: " << torneo.formato << std::endl;
            std::cout << " Fecha de inicio del torneo: " << torneo.fechaInicio << std::endl;
            std::cout << " Fecha de Finalización del torneo: " << torneo.fechaFin;

            archivoTorneo.close();
            auxiliares::pausarPrograma();
        }

        void Principal() {
            auxiliares::limpiarPantalla();

            Torneo torneo;
            bool obtenido = logica::obtenerInformacionTorneo(torneo);

            if (!obtenido) {
                std::cerr << "\n Error de Sistema! \n";
                auxiliares::pausarPrograma();
                return;
            }

            std::cout << "\n   ╔══════════════════════════════════════════════════════════════╗\n";
            std::cout << "   ║                    Sport G&C Tournaments                     ║\n";
            std::cout << "   ║  Torneo:  " << std::left << std::setw(50) << torneo.nombre << " ║\n";
            std::cout << "   ║  Deporte: " << std::left << std::setw(18) << torneo.deporte << " | Formato: " << std::left << std::setw(21) << torneo.formato << "║\n";
            std::cout << "   ╠══════════════════════════════════════════════════════════════╣\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "1. Gestión de Equipos" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "2. Gestión de Jugadores" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(61) << "3. Gestión de Partidos" << "║\n";
            // std::cout << "   ║  " << std::left << std::setw(60) << "4. Tabla de Posiciones" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "4. Reportes" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "5. Mantenimiento" << "║\n";
            std::cout << "   ║  " << std::left << std::setw(60) << "0. Salir" << "║\n";
            std::cout << "   ╚══════════════════════════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void GestionDeEquipos() {
            auxiliares::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║          GESTIÓN DE EQUIPOS               ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Registrar equipo                      ║\n";
            std::cout << "   ║  2. Buscar equipo                         ║\n";
            std::cout << "   ║  3. Actualizar equipo                     ║\n";
            std::cout << "   ║  4. Listar equipos                        ║\n";
            std::cout << "   ║  5. Eliminar equipo                       ║\n";
            std::cout << "   ║  0. Volver al menú principal              ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void menuBuscarEquipo() {
            auxiliares::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║          BUSQUEDA DE EQUIPOS              ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Buscar equipo por ID                  ║\n";
            std::cout << "   ║  2. Buscar equipo por nombre              ║\n";
            std::cout << "   ║  0. Volver al menú anterior               ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void menuBuscarJugador() {
            auxiliares::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║          BUSQUEDA DE JUGADORES            ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Buscar jugador por ID                 ║\n";
            std::cout << "   ║  2. Buscar jugador por nombre             ║\n";
            std::cout << "   ║  0. Volver al menú anterior               ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void listarPartidos() {
            auxiliares::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║              LISTAR PARTIDOS              ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Listar todos los partidos             ║\n";
            std::cout << "   ║  2. Listar partidos por su estado         ║\n";
            std::cout << "   ║  0. Volver al menú anterior               ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void buscarPartidos() {
            auxiliares::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║          BUSQUEDA DE PARTIDOS             ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Buscar partido por equipo             ║\n";
            std::cout << "   ║  2. Buscar partido por ID                 ║\n";
            std::cout << "   ║  0. Volver al menú anterior               ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void GestionDePartidos() {
            auxiliares::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║        GESTIÓN DE PARTIDOS                ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Programar partido                     ║\n";
            std::cout << "   ║  2. Registrar resultado                   ║\n";
            std::cout << "   ║  3. Buscar partido                        ║\n";
            std::cout << "   ║  4. Listar partidos                       ║\n";
            std::cout << "   ║  5. Cancelar partido                      ║\n";
            std::cout << "   ║  0. Volver al menú principal              ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void GestionDeJugadores() {
            auxiliares::limpiarPantalla();
            std::cout << "\n   ╔═══════════════════════════════════════════╗\n";
            std::cout << "   ║        GESTIÓN DE JUGADORES               ║\n";
            std::cout << "   ╠═══════════════════════════════════════════╣\n";
            std::cout << "   ║  1. Registrar jugador                     ║\n";
            std::cout << "   ║  2. Buscar jugador                        ║\n";
            std::cout << "   ║  3. Actualizar jugador                    ║\n";
            std::cout << "   ║  4. Listar jugadores (todos)              ║\n";
            std::cout << "   ║  5. Listar jugadores por equipo           ║\n";
            std::cout << "   ║  6. Eliminar jugador                      ║\n";
            std::cout << "   ║  0. Volver al menú principal              ║\n";
            std::cout << "   ╚═══════════════════════════════════════════╝\n";
            std::cout << std::endl;
        }

        void Reportes() {
            std::cout << " ╔═══════════════════════════════════════════╗\n";
            std::cout << " ║              REPORTES                     ║\n";
            std::cout << " ╠═══════════════════════════════════════════╣\n";
            std::cout << " ║  1. Tabla de posiciones                   ║\n";
            std::cout << " ║  2. Tabla de goleadores (Top 10)          ║\n";
            std::cout << " ║  3. Tabla de tarjetas amarillas (Top 10)  ║\n";
            std::cout << " ║  4. Tabla de tarjetas rojas (Top 10)      ║\n";
            std::cout << " ║  5. Ficha técnica de partido              ║\n";
            std::cout << " ║  0. Volver al menú principal              ║\n";
            std::cout << " ╚═══════════════════════════════════════════╝\n";
        }

        void Mantenimiento() {
            std::cout << " ╔═══════════════════════════════════════════╗\n";
            std::cout << " ║           MANTENIMIENTO                   ║\n";
            std::cout << " ╠═══════════════════════════════════════════╣\n";
            std::cout << " ║  1. Verificar integridad referencial      ║\n";
            std::cout << " ║  2. Crear backup de datos                 ║\n";
            std::cout << " ║  0. Volver                                ║\n";
            std::cout << " ╚═══════════════════════════════════════════╝\n";
        }

    } // namespace menu

} // namespace presentacion

// ============================================//
//   7. MAIN                                   //
// ============================================//

int main() {

    // Llamamos a la función de configuración de Idioma al inicio
    auxiliares::configurarIdioma();

    // Creamos una carpeta de datos;
    presentacion::crearCarpeta(NOMBRE_CARPETA_DATOS, "datos");
    auxiliares::waitfor(3000);

    // Inicializamos los ficheros
    if (!logica::inicializarSistemaArchivos()) {
        std::cerr << " Error inesperado del sistema \n";
        return 1;
    }

    // Variables Estaticas
    int opcionMenu = -1;     // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionSubMenu = -1;  // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionMenuBusq = -1; // declaramos en -1 para evitar que coincida con una de las opciones
    int opcionMenuListar = -1;
    char confirmacion;

    /*
    // Calcular tamaño
    std::ifstream archivoPrueba;
    archivoPrueba.open(NOMBRE_ARCHIVO_EQUIPOS);
    if (!archivoPrueba.is_open()) {
        return 1;
    }
    archivoPrueba.seekg(0, std::ios::end);
    std::streampos posicion = archivoPrueba.tellg();
    std::cout << "Tamano archivo equipos original: " << posicion << std::endl;
    archivoPrueba.close();

    // Calcular tamaño
    archivoPrueba;
    archivoPrueba.open(RUTA_BACKUPS + "backup_2026-07-17_08-33/" + "equipos.bin");
    if (!archivoPrueba.is_open()) {
        return 1;
    }
    archivoPrueba.seekg(0, std::ios::end);
    posicion = archivoPrueba.tellg();
    std::cout << "Tamano archivo equipos backup: " << posicion << std::endl;
    archivoPrueba.close();

    // Calcular tamaño
    archivoPrueba;
    archivoPrueba.open(NOMBRE_ARCHIVO_JUGADORES);
    if (!archivoPrueba.is_open()) {
        return 1;
    }
    archivoPrueba.seekg(0, std::ios::end);
    posicion = archivoPrueba.tellg();
    std::cout << "Tamano archivo jugadores original: " << posicion << std::endl;
    archivoPrueba.close();

    // Calcular tamaño
    archivoPrueba;
    archivoPrueba.open(RUTA_BACKUPS + "backup_2026-07-17_08-33/" + "jugadores.bin");
    if (!archivoPrueba.is_open()) {
        return 1;
    }
    archivoPrueba.seekg(0, std::ios::end);
    posicion = archivoPrueba.tellg();
    std::cout << "Tamano archivo jugadores backup: " << posicion << std::endl;
    archivoPrueba.close();

    // Calcular tamaño
    archivoPrueba;
    archivoPrueba.open(NOMBRE_ARCHIVO_PARTIDOS);
    if (!archivoPrueba.is_open()) {
        return 1;
    }
    archivoPrueba.seekg(0, std::ios::end);
    posicion = archivoPrueba.tellg();
    std::cout << "Tamano archivo partidos original: " << posicion << std::endl;
    archivoPrueba.close();

    // Calcular tamaño
    archivoPrueba;
    archivoPrueba.open(RUTA_BACKUPS + "backup_2026-07-17_08-33/" + "partidos.bin");
    if (!archivoPrueba.is_open()) {
        return 1;
    }
    archivoPrueba.seekg(0, std::ios::end);
    posicion = archivoPrueba.tellg();
    std::cout << "Tamano archivo partidos backup: " << posicion << std::endl;
    archivoPrueba.close();

    // Calcular tamaño
    archivoPrueba;
    archivoPrueba.open(NOMBRE_ARCHIVO_TORNEO);
    if (!archivoPrueba.is_open()) {
        return 1;
    }
    archivoPrueba.seekg(0, std::ios::end);
    posicion = archivoPrueba.tellg();
    std::cout << "Tamano archivo torneo original: " << posicion << std::endl;
    archivoPrueba.close();

    // Calcular tamaño
    archivoPrueba;
    archivoPrueba.open(RUTA_BACKUPS + "backup_2026-07-17_08-33/" + "torneo.bin");
    if (!archivoPrueba.is_open()) {
        return 1;
    }
    archivoPrueba.seekg(0, std::ios::end);
    posicion = archivoPrueba.tellg();
    std::cout << "Tamano archivo torneo backup: " << posicion << std::endl;
    archivoPrueba.close();

    return 0;*/

    // Inicio del Programa
    presentacion::menu::datosInicialesTorneo();

    // Estructura del switch
    do {

        // inicializamos las variables para evitar conflictos
        opcionMenu = -1;
        opcionSubMenu = -1;
        opcionMenuListar = -1;

        // Presentamos el menu principal
        presentacion::menu::Principal();
        auxiliares::ingresarDatos(opcionMenu, " Seleccione una opcion: ", nullptr, validadores::Positivo);
        auxiliares::limpiarPantalla();
        auxiliares::waitfor(1200);
        switch (opcionMenu) {

            // Salida del Programa
            case 0:
                auxiliares::limpiarPantalla();
                auxiliares::waitfor(500);
                auxiliares::ingresarDatos(confirmacion, " ¿Está seguro de que desea salir del programa? (S/N): ");
                if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'S') {
                    presentacion::mensajeSalida();
                } else if (std::toupper(static_cast<unsigned char>(confirmacion)) == 'N') {
                    std::cout << " Salida del Programa Cancelada\n"; // Forzamos la vuelta del bucle
                    opcionMenu = -1;
                    auxiliares::waitfor(2000);
                    auxiliares::limpiarPantalla();
                } else {
                    std::cerr << " Error: No se ingresó una opción correcta (S/N) \n";
                    std::cout << " Salida del Programa Cancelada \n";
                    auxiliares::waitfor(2000);
                    opcionMenu = -1;
                }
                break;

            // Gestión de Equipos
            case 1:
                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Gestión de Equipos... \n";
                auxiliares::waitfor(1200);

                do {
                    auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    opcionMenuBusq = -1;
                    presentacion::menu::GestionDeEquipos();
                    auxiliares::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ", nullptr, validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0: // Volver al menu Principal
                            presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Registrar nuevos equipos
                            presentacion::equipos::RegistrarEquipos(NOMBRE_ARCHIVO_EQUIPOS);
                            break;

                        case 2: // Menu Buscar equipos
                            do {
                                auxiliares::waitfor(800);
                                opcionMenuBusq = -1;
                                presentacion::menu::menuBuscarEquipo();
                                auxiliares::ingresarDatos(opcionMenuBusq, " Seleccione una opcion: ", nullptr, validadores::Positivo);

                                switch (opcionMenuBusq) {
                                    case 0: // Volver al menu anterior
                                        presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Busqueda por ID
                                        presentacion::equipos::buscarEquipoPorID(NOMBRE_ARCHIVO_EQUIPOS);
                                        break;

                                    case 2: // Busqueda por nombre
                                        presentacion::equipos::buscarEquiposPorSubCadena(NOMBRE_ARCHIVO_EQUIPOS);
                                        break;

                                    default:
                                        presentacion::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);

                            break;

                        case 3: // Actualizar Equipos
                            presentacion::equipos::actualizarEquipo(NOMBRE_ARCHIVO_EQUIPOS);
                            break;

                        case 4: // Listar Equipos
                            presentacion::equipos::listarEquipos(NOMBRE_ARCHIVO_EQUIPOS);
                            break;

                        case 5: // Eliminar Equipos
                            presentacion::equipos::eliminarEquipo(NOMBRE_ARCHIVO_EQUIPOS);
                            break;

                        default:
                            presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);

                break;

            // Gestión de Jugadores
            case 2:
                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Gestión de Jugadores... \n";
                auxiliares::waitfor(1200);

                do {
                    auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::GestionDeJugadores();
                    auxiliares::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ", nullptr, validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0: // Volver al menu Principal
                            presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Registrar nuevos jugadores
                            presentacion::jugadores::registrarJugador(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 2: // Menu Buscar jugadores
                            do {
                                opcionMenuBusq = -1;
                                presentacion::menu::menuBuscarJugador();
                                auxiliares::ingresarDatos(opcionMenuBusq, " Seleccione una opcion: ", nullptr, validadores::Positivo);

                                switch (opcionMenuBusq) {
                                    case 0: // Volver al menu anterior
                                        presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Busqueda por ID
                                        presentacion::jugadores::buscarJugadorID(NOMBRE_ARCHIVO_JUGADORES);
                                        break;

                                    case 2: // Busqueda por nombre
                                        presentacion::jugadores::buscarJugadorPorNombre(NOMBRE_ARCHIVO_JUGADORES);
                                        break;

                                    default:
                                        presentacion::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);
                            break;

                        case 3: // Actualizar Jugador
                            presentacion::jugadores::actualizarJugador(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 4: // Listar todos los jugadores
                            presentacion::jugadores::mostrarListaDeJugadores(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 5: // Listar Jugadores por Equipos
                            presentacion::jugadores::mostrarJugadoresPorEquipo(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 6: // Eliminar jugador
                            presentacion::jugadores::eliminarJugador(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        default:
                            presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);
                break;

            // Gestión de Partidos
            case 3:
                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Gestión de Partidos... \n";
                auxiliares::waitfor(1200);

                do {
                    auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::GestionDePartidos();
                    auxiliares::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ", nullptr, validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0: // Volver al menú principal
                            presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Programar Partidos
                            presentacion::partidos::programarPartido(NOMBRE_ARCHIVO_PARTIDOS);
                            break;

                        case 2: // Registrar el Resultado de un partido
                            presentacion::partidos::registrarResultado(NOMBRE_ARCHIVO_PARTIDOS);
                            break;

                        case 3: // Buscar partidos
                            do {
                                opcionMenuBusq = -1;
                                presentacion::menu::buscarPartidos();
                                auxiliares::ingresarDatos(opcionMenuBusq, " Seleccione una opcion: ", nullptr, validadores::Positivo);

                                switch (opcionMenuBusq) {
                                    case 0: // Volver al menu anterior
                                        presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Busqueda por Equipo
                                        presentacion::partidos::buscarPartidosPorEquipo(NOMBRE_ARCHIVO_PARTIDOS);
                                        break;

                                    case 2: // Busqueda por ID
                                        presentacion::partidos::buscarPartidoPorID(NOMBRE_ARCHIVO_PARTIDOS);
                                        break;

                                    default:
                                        presentacion::mensajeDefault();
                                }
                            } while (opcionMenuBusq != 0);
                            break;

                        case 4: // Listar Partidos
                            do {
                                opcionMenuListar = -1;
                                presentacion::menu::listarPartidos();
                                auxiliares::ingresarDatos(opcionMenuListar, " Seleccione una opcion: ", nullptr, validadores::Positivo);

                                switch (opcionMenuListar) {
                                    case 0: // Volver al menu anterior
                                        presentacion::mensajeMenuAnterior();
                                        break;

                                    case 1: // Listar todos los partidos
                                        presentacion::partidos::listarTodosLosPartidos(NOMBRE_ARCHIVO_PARTIDOS);
                                        break;

                                    case 2: // Listar partidos por estado
                                        presentacion::partidos::listarPartidosPorEstado(NOMBRE_ARCHIVO_PARTIDOS);
                                        break;

                                    default:
                                        presentacion::mensajeDefault();
                                }
                            } while (opcionMenuListar != 0);
                            break;

                        case 5: // Cancelar Partidos
                            presentacion::partidos::cancelarPartido(NOMBRE_ARCHIVO_PARTIDOS);
                            break;

                        default:
                            presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);

                break;

            // Reportes
            case 4:
                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Reportes... \n";
                auxiliares::waitfor(1200);

                do {
                    auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::Reportes();
                    auxiliares::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ", nullptr, validadores::Positivo);

                    switch (opcionSubMenu) {

                        case 0: // Salida
                            presentacion::mensajeMenuPrincipal();
                            break;

                        case 1: // Tabla de Posiciones
                            auxiliares::waitfor(500);
                            auxiliares::limpiarPantalla();
                            std::cout << "\n Ingresando al apartado de Tabla de Posiciones... \n";
                            auxiliares::waitfor(1200);
                            auxiliares::limpiarPantalla();
                            presentacion::equipos::mostrarTablaDePosiciones(NOMBRE_ARCHIVO_EQUIPOS);
                            break;

                        case 2: // Tabla de Anotadores
                            auxiliares::waitfor(500);
                            auxiliares::limpiarPantalla();
                            std::cout << "\n Ingresando al apartado de Tabla del Top 10 Anotadores... \n";
                            auxiliares::waitfor(1200);
                            auxiliares::limpiarPantalla();
                            presentacion::reportes::tablaTop10Anotadores(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 3: // Tabla de Tarjetas Amarillas
                            auxiliares::waitfor(500);
                            auxiliares::limpiarPantalla();
                            std::cout << "\n Ingresando al apartado de Tabla del Top 10 Tarjetas Amarillas... \n";
                            auxiliares::waitfor(1200);
                            auxiliares::limpiarPantalla();
                            presentacion::reportes::tablaTop10TarjetasAmarillas(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 4: // Tabla de Tarjetas Rojas
                            auxiliares::waitfor(500);
                            auxiliares::limpiarPantalla();
                            std::cout << "\n Ingresando al apartado de Tabla del Top 10 Tarjetas Rojas... \n";
                            auxiliares::waitfor(1200);
                            auxiliares::limpiarPantalla();
                            presentacion::reportes::tablaTop10TarjetasRojas(NOMBRE_ARCHIVO_JUGADORES);
                            break;

                        case 5: // Ficha Técnica
                            auxiliares::waitfor(500);
                            auxiliares::limpiarPantalla();
                            std::cout << "\n Ingresando a la ficha técnica del partido... \n";
                            auxiliares::waitfor(1200);
                            auxiliares::limpiarPantalla();
                            presentacion::reportes::fichaTecnica(NOMBRE_ARCHIVO_PARTIDOS);
                            break;

                        default:
                            presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);

                break;

            // Mantenimiento
            case 5:
                auxiliares::waitfor(500);
                auxiliares::limpiarPantalla();
                std::cout << "\n Ingresando al apartado de Mantenimiento... \n";
                auxiliares::waitfor(1200);

                do {
                    auxiliares::limpiarPantalla();
                    opcionSubMenu = -1;
                    presentacion::menu::Mantenimiento();
                    auxiliares::ingresarDatos(opcionSubMenu, " Seleccione una opcion: ", nullptr, validadores::Positivo);

                    switch (opcionSubMenu) {
                        case 0:
                            presentacion::mensajeMenuPrincipal();
                            break;

                        case 1:
                            presentacion::mantenimiento::verificarIntegridadReferencial();
                            break;

                        case 2:
                            presentacion::mantenimiento::crearBackups(RUTA_BACKUPS.c_str());
                            break;

                        default:
                            presentacion::mensajeDefault();
                    }
                } while (opcionSubMenu != 0);

                break;

            // Si no se selecciona una opcion correcta enviamos un mensaje de aviso
            default:
                presentacion::mensajeDefault();
        }
        // El bucle se repite si el usuario no eligió la opcion de salir en el menu Principal
    } while (opcionMenu != 0);

    return 0;
}