/*
 * EJERCICIO CLASE 1 - PUNTEROS Y MEMORIA DINÁMICA
 * ================================================ */

#include <algorithm> // para hacer uso de transform y implmentar una transformacion de texto mas eficiente
#include <cctype>    // para el uso de std::toupper y tolower
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <new> // Incluimos la libreria para usar std::nothrow
#include <thread>

using std::cin;
using std::cout;
using std::endl;
using std::string;

void esperarSegundos() { std::this_thread::sleep_for(std::chrono::milliseconds(3500)); }

void presionarTecla() {
    cout << "Presione la tecla Enter para continuar...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
}

void limpiarPantalla() {
    cout << "\033[2J\033[1;1H" << std::flush; // Secuencia de escape ANSI para limpiar pantalla
    cout << endl << endl;
}

string convertirMayus(string texto) {
    std::transform(texto.begin(), texto.end(), texto.begin(), ::toupper);
    return texto;
}

template <class Tipo> void ingresarDatos(Tipo &num) {
    bool flag;
    do {
        flag = false;
        cout << "Ingresa Aqui: ";
        cin >> num;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            flag = true;
            cout << "\nERROR tipo de dato incorrecto";
        }
    } while (flag);
}

void ingresarTamanio(int &tamanio) {
    bool flagTamanio;
    // Bucle que ejecuta la accion de ingresar el tamaño hasta que el usuario ingrese un tamaño correcto
    do {
        limpiarPantalla();
        // inicializamos la variable en false a cada vuelta de bucle
        flagTamanio = false;
        cout << "\n-----TAMANIO DEL ARRAY DINAMICO-------\n";
        ingresarDatos(tamanio);

        // si ingresa un tamaño incorrecto se activa la bandera
        if (tamanio <= 0) {
            flagTamanio = true;
            cout << "ERROR\nIngrese un numero positivo";
            presionarTecla();
        }
    } while (flagTamanio);
}

int *crearArrayDinamico(int tamanio) {
    // creamos el array dinamico usando std::nothrow
    // El 'std::nothrow' hace que si falla la memoria, retorne nullptr
    int *nuevoArray = new (std::nothrow) int[tamanio];

    // si devolvio nullptr notificamos el error al usuario
    if (nuevoArray == nullptr) {
        cout << "Error al crear el arreglo dinamico por falta de memoria RAM" << endl;
    }
    // retornamos el array dinamico
    return nuevoArray;
}

void verficarPositivo(int &variable) {
    bool flag = false;

    do {
        limpiarPantalla();
        flag = false;
        ingresarDatos(variable);
        if (variable <= 0) {
            cout << "ERROR ingresa solo numero positivos\n";
            flag = true; // activamos la bandera
            esperarSegundos();
        }
    } while (flag);
}

// Pasamos como parametros el tamaño y la direccion al primer elemento del array dinamico
void llenarArrayDinamico(int tamanio, int *ptr) {
    int num;

    // bucle que recorre todo el array
    for (size_t e = 0; e < tamanio; e++) {
        verficarPositivo(num);
        *(ptr + e) = num;
    }
}

void mostrarArrayDinamico(int *arrayDinamico, int tamanio) {
    // bucle que recorre e imprime todo lo que hay en el array
    for (size_t e = 0; e < tamanio; e++) {
        cout << "Posicion " << e << ": " << *(arrayDinamico + e) << endl;
    }
}

// funcion que devuelve el numero mayor dentro del array
int hallarNumeroMayor(int *arrayDinamico, int tamanio) {
    int numeroReferencia = 0, numeroAnterior = 0;

    // bucle que recorre el array
    for (size_t e = 0; e < tamanio; e++) {
        numeroAnterior = *(arrayDinamico + e);
        if (numeroAnterior > numeroReferencia) {
            numeroReferencia = numeroAnterior;
        }
    }

    return numeroReferencia;
}

float calcularPromedio(int *arrayDinamico, int tamanio) {
    float suma = 0, promedio = 0;

    for (size_t e = 0; e < tamanio; e++) {
        // sumamos todos los valores
        suma += *(arrayDinamico + e);
    }
    // Calculamos el promedio
    promedio = suma / tamanio;

    // retornamos el valor
    return promedio;
}

void liberarMemoria(int *&arrayDinamico) {
    // limpiamos lo que hay en ArrayPtr
    delete[] arrayDinamico;
    // Hacemos que no apunte a ningun lado para evitar un puntero colgante
    arrayDinamico = nullptr;
}

int main() {
    // variables
    int tamanio;
    bool flagTamanio = false;
    int opcion = 0;
    bool flagArrayCreado = false;
    int numeroMayor = 0;
    float promedio = 0;
    string crearNuevamente;
    bool crearArrayNuevamente = true;
    bool repetir = false;

    // punteros
    int *ArrayPtr = nullptr;

    //* Comienzo del programa

    do {
        limpiarPantalla(); // Limpiamos la Pantal
        cout << "\n-------MENU-------\n\n";
        cout << "1. Crear y llenar arreglo de numero Positivos\n2. Mostrar arreglo\n";
        cout << "3. Encontrar numero mayor\n4. Calcular promedio\n5. Salir\n\n";
        ingresarDatos(opcion);

        switch (opcion) {
            case 1:
                if (flagArrayCreado) {
                    do {
                        limpiarPantalla(); // Limpiamos la Pantalla
                        // inicializamos las variables
                        repetir = false;
                        crearArrayNuevamente = true;

                        cout << "Ya hay un array creado quieres remplazarlo? (S/N)\n";
                        ingresarDatos(crearNuevamente);
                        crearNuevamente = convertirMayus(crearNuevamente);
                        if (crearNuevamente == "S") {
                            liberarMemoria(ArrayPtr);
                            break;
                        } else if (crearNuevamente == "N") {
                            crearArrayNuevamente = false;
                        } else {
                            repetir = true;
                        }
                    } while (repetir);
                }

                if (!crearArrayNuevamente) {
                    cout << "Si no vas a modificar el array seras redirigido al menu principal\n";
                    esperarSegundos();
                    break;
                }

                cout << "\n\n-------CREAR ARRAY DINAMICO-------\n\n";
                ingresarTamanio(tamanio);
                ArrayPtr = crearArrayDinamico(tamanio);
                cout << "Array creado con exito\n";
                esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                limpiarPantalla(); // Limpiamos la Pantalla
                cout << "\n\n-------LLENAR ARRAY DINAMICO-------\n\n";
                llenarArrayDinamico(tamanio, ArrayPtr);
                cout << "Array llenado con exito";
                flagArrayCreado = true;
                esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                break;

            case 2:
                if (!flagArrayCreado) {
                    limpiarPantalla(); // Limpiamos la Pantalla
                    cout << "ADVERTENCIA: Aun no has creado el array dinamico\n";
                    cout << "Crea primero el array para usar estas funciones\n";
                    esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                    break;
                } else {
                    limpiarPantalla();
                    cout << "-------CONTENIDO DEL ARRAY DINAMICO-------\n\n";
                    mostrarArrayDinamico(ArrayPtr, tamanio);
                    presionarTecla(); // Esperamos que el usuario presione la tecla enter para continuar
                }
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            case 3:
                numeroMayor = 0;
                if (!flagArrayCreado) {
                    limpiarPantalla(); // Limpiamos la Pantalla
                    cout << "ADVERTENCIA: Aun no has creado el array dinamico\n";
                    cout << "Crea primero el array para usar estas funciones\n";
                    esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                    break;
                } else {
                    limpiarPantalla();
                    numeroMayor = hallarNumeroMayor(ArrayPtr, tamanio);
                    cout << "Numero mayor: " << numeroMayor << endl;
                    presionarTecla(); // Esperamos que el usuario presione la tecla enter para continuar
                }
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            case 4:
                promedio = 0;
                if (!flagArrayCreado) {
                    limpiarPantalla(); // Limpiamos la Pantalla
                    cout << "ADVERTENCIA: Aun no has creado el array dinamico\n";
                    cout << "Crea primero el array para usar estas funciones\n";
                    esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                    break;
                } else {
                    limpiarPantalla();
                    cout << "-------CALCULAR PROMEDIO-------\n\n";
                    promedio = calcularPromedio(ArrayPtr, tamanio);
                    // fijamos el resultado a solo decimales y obligamos a que use 3 decimales
                    cout << "Promedio: " << std::fixed << std::setprecision(3) << promedio << endl;
                    presionarTecla(); // Esperamos que el usuario presione la tecla enter para continuar
                }
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            case 5:
                limpiarPantalla(); // Limpiamos la Pantalla
                cout << "Saliendo...";
                esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            default:
                limpiarPantalla(); // Limpiamos la Pantalla
                cout << "ERROR opcion no disponible\n";
                esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                limpiarPantalla(); // Limpiamos la Pantalla
        }
    } while (opcion != 5);

    liberarMemoria(ArrayPtr);

    return 0;
}