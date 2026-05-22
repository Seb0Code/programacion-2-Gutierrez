/*
 * EJERCICIO CLASE 1 - PUNTEROS Y MEMORIA DINÁMICA
 * ================================================
 *
 * ENUNCIADO SIMPLIFICADO:
 * Implementar un programa que permita:
 * 1. Crear un arreglo dinámico de números enteros
 * 2. Llenar el arreglo con valores ingresados por el usuario
 * 3. Mostrar todos los números del arreglo
 * 4. Encontrar el número mayor en el arreglo
 * 5. Calcular el promedio de todos los números
 * 6. Liberar la memoria correctamente
 *
 * REQUISITOS OBLIGATORIOS:
 * - Usar punteros y memoria dinámica (new/delete)
 * - Usar paso por referencia donde sea apropiado
 * - Validar entrada del usuario (tamaño del arreglo)
 * - Liberar toda la memoria asignada
 * - Asignar nullptr después de delete
 *
 * FUNCIONES QUE DEBE IMPLEMENTAR:
 * 1. int* crearArreglo(int tamanio)
 * 2. void llenarArreglo(int* arreglo, int tamanio)
 * 3. void mostrarArreglo(int* arreglo, int tamanio)
 * 4. int encontrarMayor(int* arreglo, int tamanio)
 * 5. float calcularPromedio(int* arreglo, int tamanio)
 * 6. void liberarArreglo(int*& arreglo)
 *
 * MENÚ SIMPLE:
 * 1. Crear y llenar arreglo
 * 2. Mostrar arreglo
 * 3. Encontrar número mayor
 * 4. Calcular promedio
 * 5. Salir
 *
 * NOTAS IMPORTANTES:
 * - Validar que el tamaño del arreglo sea positivo
 * - Verificar que new no retorne nullptr
 * - Usar buenas prácticas de programación
 * - Comentar el código apropiadamente
 *
 * CRITERIOS DE EVALUACIÓN:
 * - Correcta implementación de punteros (50%)
 * - Manejo correcto de memoria (30%)
 * - Funcionalidad del programa (20%)
 *
 * TIEMPO ESTIMADO: 1-2 horas
 * DIFICULTAD: Básica-Medio
 */

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

// Pasamos como parametros el tamaño y la direccion al primer elemento del array dinamico
void llenarArrayDinamico(int tamanio, int *ptr) {
    int num;

    // bucle que recorre todo el array
    for (size_t e = 0; e < tamanio; e++) {
        ingresarDatos(num);
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
    delete arrayDinamico;
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
    int promedio = 0;
    string crearNuevamente;
    bool crearArrayNuevamente = true;
    bool repetir = false;

    // punteros
    int *ArrayPtr = nullptr;

    //* Comienzo del programa

    do {
        limpiarPantalla(); // Limpiamos la Pantal
        cout << "\n\n-------MENU-------\n\n";
        cout << "1. Crear y llenar arreglo\n2. Mostrar arreglo\n";
        cout << "3. Encontrar numero mayor\n4. Calcular promedio\n5. Salir\n\n";
        ingresarDatos(opcion);

        switch (opcion) {
            case 1:
                limpiarPantalla(); // Limpiamos la Pantalla

                if (flagArrayCreado) {
                    do {
                        // inicializamos las variables
                        repetir = false;
                        crearArrayNuevamente = true;

                        cout << "Ya hay un array creado ¿quieres remplazarlo? (S/N)\n";
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

                cout << "-------LLENAR ARRAY DINAMICO-------\n\n";
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
                    cout << "-------CONTENIDO DEL ARRAY DINAMICO-------\n\n";
                    mostrarArrayDinamico(ArrayPtr, tamanio);
                    presionarTecla(); // Esperamos que el usuario presione la tecla enter para continuar
                }
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            case 3:
                if (!flagArrayCreado) {
                    limpiarPantalla(); // Limpiamos la Pantalla
                    cout << "ADVERTENCIA: Aun no has creado el array dinamico\n";
                    cout << "Crea primero el array para usar estas funciones\n";
                    esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                    break;
                } else {
                    numeroMayor = hallarNumeroMayor(ArrayPtr, tamanio);
                    cout << "Numero mayor: " << numeroMayor;
                    presionarTecla(); // Esperamos que el usuario presione la tecla enter para continuar
                }
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            case 4:
                if (!flagArrayCreado) {
                    limpiarPantalla(); // Limpiamos la Pantalla
                    cout << "ADVERTENCIA: Aun no has creado el array dinamico\n";
                    cout << "Crea primero el array para usar estas funciones\n";
                    esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                    break;
                } else {
                    cout << "-------CALCULAR PROMEDIO-------\n\n";
                    promedio = calcularPromedio(ArrayPtr, tamanio);
                    // fijamos el resultado a solo decimales y obligamos a que use 3 decimales
                    cout << "Promedio: " << std::fixed << std::setprecision(3) << promedio;
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
                cout << "ERROR\n Ingresaste una opcion no disponible Por favor intente nuevamente\n";
                esperarSegundos(); // Esperamos un tiempo de 3.5 segundos
                limpiarPantalla(); // Limpiamos la Pantalla
        }
    } while (opcion != 5);

    liberarMemoria(ArrayPtr);

    return 0;
}