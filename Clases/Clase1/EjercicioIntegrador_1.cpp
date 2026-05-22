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

#include <chrono>
#include <iostream>
#include <limits>
#include <new> // Incluimos la libreria para usar std::nothrow
#include <thread>

using std::cin;
using std::cout;
using std::endl;
using std::string;

void esperarSegundos() { std::this_thread::sleep_for(std::chrono::milliseconds(2500)); }

void limpiarPantalla() {
    cout << "\033[2J\033[1;1H" << std::flush; // Secuencia de escape ANSI para limpiar pantalla
}

void ingresarDatos(int &num) {
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
        // inicializamos la variable en false a cada vuelta de bucle
        flagTamanio = false;
        cout << "-----TAMANIO DEL ARRAY DINAMICO-------\n";
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

int main() {
    // variables
    int tamanio;
    bool flagTamanio = false;
    int opcion = 0;
    bool flagArrayCreado = false;

    // punteros
    int *ArrayPtr = nullptr;

    //* Comienzo del programa

    do {
        limpiarPantalla();
        cout << "\n\n-------MENU-------\n\n";
        cout << "1. Crear y llenar arreglo\n2. Mostrar arreglo\n";
        cout << "3. Encontrar número mayor\n4. Calcular promedio\n5. Salir\n";
        ingresarDatos(opcion);

        switch (opcion) {
            case 1:
                limpiarPantalla();
                cout << "\n\n-------CREAR ARRAY DINAMICO-------\n\n";
                ingresarTamanio(tamanio);
                ArrayPtr = crearArrayDinamico(tamanio);
                cout << "Array creado con exito\n";
                esperarSegundos();
                limpiarPantalla();
                cout << "-------LLENAR ARRAY DINAMICO-------\n\n";
                llenarArrayDinamico(tamanio, ArrayPtr);
                cout << "Array llenado con exito";
                flagArrayCreado = true;
                esperarSegundos();
                break;

            case 2:
                if (!flagArrayCreado) {
                    cout << "ADVERTENCIA: Aun no has creado el array dinamico\n";
                    cout << "Crea primero el array para usar estas funciones\n";
                    break;
                }
                esperarSegundos();
                limpiarPantalla();
                break;

            case 3:
                if (!flagArrayCreado) {
                    cout << "ADVERTENCIA: Aun no has creado el array dinamico\n";
                    cout << "Crea primero el array para usar estas funciones\n";
                    break;
                }
                esperarSegundos();
                limpiarPantalla();
                break;

            case 4:
                if (!flagArrayCreado) {
                    cout << "ADVERTENCIA: Aun no has creado el array dinamico\n";
                    cout << "Crea primero el array para usar estas funciones\n";
                    break;
                }
                esperarSegundos();
                limpiarPantalla();
                break;

            case 5:
                cout << "Saliendo...";
                esperarSegundos();
                limpiarPantalla();
                break;

            default:
                cout << "ERROR\n Ingresaste una opcion no disponible Por favor intente nuevamente\n";
                esperarSegundos();
                limpiarPantalla();
        }
    } while (opcion != 5);



    // Guardamos la direccion del arrayDinamico en nuestra variable dentro del main
    ArrayPtr = crearArrayDinamico(tamanio);


    // limpiamos lo que hay en ArrayPtr
    delete ArrayPtr;
    // Hacemos que no apunte a ningun lado para evitar un puntero colgante
    ArrayPtr = nullptr;

    return 0;
}