#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <thread>

using std::cin;
using std::cout;
using std::endl;
using std::string;

struct Materia {
    string nombre;
    int ID;
};

struct Estudiante {
    string nombre;
    int ID;
    float nota;
};

struct Escuela {
    string nombre;
    int cantEstudiantes;
    Estudiante *arrayEstudiantes = nullptr;
    int cantMaterias;
    Materia *arrayMaterias = nullptr;
};

void limpiarPantalla() {
    // INTENTO 1: Usar códigos de escape ASCII/ANSI (El método más rápido y moderno)
    // Enviamos el comando de borrado. Si la terminal lo soporta, se limpiará al instante.
    std::cout << "\x1B[2J\x1B[H" << std::flush;

    // INTENTO 2: Fallback tradicional mediante comandos del Sistema Operativo
    // Si la terminal es antigua o no procesó el código ANSI, ejecutamos el comando nativo.
#ifdef _WIN32
    // Si estamos en Windows
    std::system("cls");
#else
    // Si estamos en Linux o macOS
    std::system("clear");
#endif
}

// funcion que se encarga de pausar el programa hasta que el usuario ingrese enter por la consola
void pausarPrograma() {
    // Limpia el búfer de entrada por si  quedaron caracteres (como '\n')
    std::cin.clear(); // Restablece los flags de error por si std::cin estaba en estado de fallo

    // Ignora cualquier carácter sobrante en el búfer hasta encontrar el salto de línea
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Mostramos el mensaje
    std::cout << "\nPresione Enter para continuar...";

    // Esperaa a que el usuario presione la tecla Enter
    std::cin.get();
}

void waitfor(int tiempo = 2500) { std::this_thread::sleep_for(std::chrono::milliseconds(tiempo)); }

template <typename Tipo1> //
void ingresarDatos(Tipo1 &texto, string mensaje) {
    bool flag = false;
    do {
        // limpiarPantalla();
        flag = false;
        cout << mensaje;
        cin >> texto;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            flag = true;
            cout << "Error Tipo de Dato Incorrecto\n";
            waitfor(3000);
        }
    } while (flag);
}

bool verificarPositivo(int num) {
    if (num <= 0) {
        return false;
    } else {
        return true;
    }
}

void ingresarSoloTexto(string &texto, string mensaje) {
    bool flag = false;
    bool esLetra = false;
    do {
        esLetra = false;
        flag = false;
        ingresarDatos(texto, mensaje);
        if (texto.empty()) {
            cout << "ERROR el campo no debe estar vacio\n";
            waitfor(3000);
            flag = true;
        } else {
            for (size_t e = 0; e < texto.size(); e++) {
                esLetra = isalpha(texto[e]);
                if (!esLetra) {
                    flag = true;
                    cout << "ERROR ingresa solo letras\n";
                    waitfor(3000);
                    break;
                }
            }
        }
    } while (flag);
}

void ingresarTamanio(int &tamanio, string mensaje) {
    bool positivo = false;
    do {
        positivo = false;
        ingresarDatos(tamanio, mensaje);
        positivo = verificarPositivo(tamanio);
        if (!positivo) {
            cout << "ERROR ingresa un tamanio valido\n";
            waitfor(3000);
        }
    } while (!positivo);
}

bool nombreRep(Escuela *ptr, string nombre, char c) {
    if (c == 'E') {
        for (size_t e = 0; e < ptr->cantEstudiantes; e++) {
            if (ptr->arrayEstudiantes[e].nombre == nombre) {
                return true;
            }
        }
    } else if (c == 'M') {
        for (size_t e = 0; e < ptr->cantMaterias; e++) {
            if (ptr->arrayMaterias[e].nombre == nombre) {
                return true;
            }
        }
    }
    // si no encuentra nada devuelve false
    return false;
}

bool idRep(Escuela *ptr, int id, char c) {
    if (c == 'E') {
        for (size_t e = 0; e < ptr->cantEstudiantes; e++) {
            if (ptr->arrayEstudiantes[e].ID == id) {
                return true;
            }
        }
    } else if (c == 'M') {
        for (size_t e = 0; e < ptr->cantMaterias; e++) {
            if (ptr->arrayMaterias[e].ID == id) {
                return true;
            }
        }
    }
    // si no encuentra nada devuelve false
    return false;
}

void ingresarID(int &ID, string mensaje) {
    bool positivo = false;
    do {
        positivo = false;
        ingresarDatos(ID, mensaje);
        positivo = verificarPositivo(ID);
        if (!positivo) {
            cout << "ERROR ingresa un ID valido (NUMERO POSITIVO)\n";
            waitfor(3000);
        }
    } while (!positivo);
}

void ingresarNotas(float &nota, string mensaje) {
    bool rango = true;
    do {
        rango = true;
        ingresarDatos(nota, mensaje);
        if (nota < 0 || nota > 20) {
            cout << "Error ingresa una nota en el rango 0-20\n";
            waitfor(3000);
            rango = false;
        }
    } while (!rango);
}

string convertirMayus(string texto) {
    std::transform(texto.begin(), texto.end(), texto.begin(), toupper);
    return texto;
}

void llenarEstudiantes(Escuela *ptr) {
    string nombre;
    int ID;
    float nota;
    bool flag = false;

    for (size_t e = 0; e < (ptr->cantEstudiantes); e++) {
        limpiarPantalla();
        flag = false;
        do {
            cout << "-------INGRESAR ESTUDIANTES-------\n\n\n";
            cout << "Estudiante " << e + 1 << ".\n";
            flag = false;
            ingresarSoloTexto(nombre, "Ingresar nombre: ");
            flag = nombreRep(ptr, nombre, 'E');
            if (flag) {
                cout << "Error Ese nombre ya Existe\n";
                waitfor(2000);
            }
        } while (flag);
        do {
            cout << "-------INGRESAR ESTUDIANTES-------\n\n\n";
            cout << "Estudiante " << e + 1 << ".\n";
            flag = false;
            ingresarID(ID, "Ingresar ID: ");
            flag = idRep(ptr, ID, 'E');
            if (flag) {
                cout << "Error Ese ID ya Existe\n";
                waitfor(2000);
            }
        } while (flag);

        cout << "\n\n-------INGRESAR ESTUDIANTES-------\n\n\n";
        cout << "Estudiante " << e + 1 << ".\n";
        flag = false;
        ingresarNotas(nota, "Ingresar Nota: ");
        ptr->arrayEstudiantes[e].nombre = nombre;
        ptr->arrayEstudiantes[e].ID = ID;
        ptr->arrayEstudiantes[e].nota = nota;
        waitfor(750);
    }
}

void llenarMaterias(Escuela *ptr) {
    string nombre;
    int ID;
    bool flag = false;
    for (size_t e = 0; e < (ptr->cantMaterias); e++) {
        limpiarPantalla();
        do {
            flag = false;
            cout << "-------INGRESAR MATERIAS-------\n\n\n";
            cout << "Materia " << e + 1 << ".\n";
            ingresarSoloTexto(nombre, "Ingresar nombre: ");
            flag = nombreRep(ptr, nombre, 'M');
            if (flag) {
                cout << "Error Ese nombre ya Existe\n";
                waitfor(2000);
            }
        } while (flag);
        do {
            flag = false;
            cout << "-------INGRESAR MATERIAS-------\n\n\n";
            cout << "Materia " << e + 1 << ".\n";
            ingresarID(ID, "Ingresar ID: ");
            flag = idRep(ptr, ID, 'M');
            if (flag) {
                cout << "Error Ese ID ya Existe\n";
                waitfor(2000);
            }
        } while (flag);

        ptr->arrayMaterias[e].nombre = nombre;
        ptr->arrayMaterias[e].ID = ID;
        waitfor(750);
    }
}

void menuEscuela(string &nombre, int &cantEst, int &cantMat) {
    limpiarPantalla();

    cout << "=======DATOS INICIALES DE TU ESCUELA=======\n\n";
    ingresarSoloTexto(nombre, "Nombre de tu Escuela: ");
    ingresarTamanio(cantEst, "Cantidad de Estudiantes: ");
    ingresarTamanio(cantMat, "Cantidad de Materias: ");

    cout << "\n|=== DATOS INGRESADOS CORRECTAMENTE ===|\n\n";
    waitfor(3000);
    limpiarPantalla();
}

void menuPrincipal() {
    cout << "\n-------MENU-------\n\n";
    cout << "1. Crear y llenar escuela \n2. Mostrar escuela \n3. Estudiante con mejor promedio\n";
    cout << "4. Promedio general \n5. Buscar por id \n6. Salir\n\n";
}

void LlenarEscuela(Escuela *ptr) {
    string nombre;
    int ID;

    llenarEstudiantes(ptr); // llenamos el array de estudiantes
    llenarMaterias(ptr);    // llenamos el array de Materias
}

Escuela *crearEscuela() {
    Escuela *nuevaEscuela = new Escuela;
    return nuevaEscuela;
}

void initEscuela(Escuela *ptr, string nomEsc, int cantEst, int cantMat) {
    // inicializamos el nombre y los arrays
    ptr->nombre = nomEsc;
    ptr->cantEstudiantes = cantEst;
    ptr->cantMaterias = cantMat;
    ptr->arrayEstudiantes = new Estudiante[(ptr->cantEstudiantes)];
    ptr->arrayMaterias = new Materia[(ptr->cantMaterias)];
}

void liberarEscuela(Escuela *&ptr) {
    // liberamos el array de estudiantes
    delete[] ptr->arrayEstudiantes;
    ptr->arrayEstudiantes = nullptr;

    // liberamos el array de materias
    delete[] ptr->arrayMaterias;
    ptr->arrayMaterias = nullptr;

    delete ptr;
    ptr = nullptr;
}

void mostrarEscuela(Escuela *ptr) {
    limpiarPantalla();
    cout << "\n======= LISTA DE MATERIAS =======\n\n";
    for (size_t e = 0; e < ptr->cantMaterias; e++) {
        cout << "MATERIA " << e + 1 << endl << endl;
        cout << "Nombre: " << ptr->arrayMaterias[e].nombre << endl;
        cout << "ID: " << ptr->arrayMaterias[e].ID << endl << endl << endl;
        waitfor(500);
    }
    pausarPrograma();
    cout << "\n\n======= LISTA DE ESTUDIANTES =======\n\n";
    for (size_t e = 0; e < ptr->cantEstudiantes; e++) {
        cout << "Estudiante " << e + 1 << endl << endl;
        cout << "Nombre: " << ptr->arrayEstudiantes[e].nombre << endl;
        cout << "ID: " << ptr->arrayEstudiantes[e].ID << endl;
        cout << "Nota: " << std::fixed << std::setprecision(2) << ptr->arrayEstudiantes[e].nota << endl << endl << endl;
        waitfor(500);
    }
    pausarPrograma();
}

float buscarMejorPromedio(Escuela *ptr) {
    float nuevoPromedio;
    float mejorPromedio = 0;
    for (size_t e = 0; e < ptr->cantEstudiantes; e++) {
        nuevoPromedio = ptr->arrayEstudiantes[e].nota;
        if (nuevoPromedio > mejorPromedio) {
            mejorPromedio = nuevoPromedio;
        }
    }
    return mejorPromedio;
}

float calcularPromedio(Escuela *ptr) {
    float promedio, suma = 0;

    for (size_t e = 0; e < ptr->cantEstudiantes; e++) {
        suma += ptr->arrayEstudiantes[e].nota;
    }
    promedio = (suma / ptr->cantEstudiantes);
    return promedio;
}

string buscarId(Escuela *ptr, int id) {
    bool flag = false;
    for (size_t e = 0; e < ptr->cantEstudiantes; e++) {
        if (id == ptr->arrayEstudiantes[e].ID) {
            return ptr->arrayEstudiantes[e].nombre;
        }
    }
    return "ERROR";
}

int main() {
    Escuela *ptrMiEscuela = nullptr;
    int cantEst = 0, cantMat = 0;
    string nombreEscuela = "";
    int opcion = 0;
    bool escuelaYaCreada = false;
    bool repetir;
    bool crearOtraEsc = true;
    string remplazarEsc;
    float promedioGeneral;
    float mejorPromedio;
    int id;

    do {
        id = 0;
        opcion = 0;
        promedioGeneral = 0;
        mejorPromedio = 0;
        repetir = false;
        limpiarPantalla(); // Limpiamos la Pantalla
        menuPrincipal();

        ingresarDatos(opcion, "Ingresar Opcion: ");
        switch (opcion) {

                // inicializar y llenar escuela
            case 1:
                // si la escuela ya ha sido inicializada
                if (escuelaYaCreada) {
                    do {
                        limpiarPantalla(); // Limpiamos la Pantalla
                        // inicializamos las variables
                        repetir = false;
                        crearOtraEsc = true;
                        ingresarDatos(remplazarEsc, "Ya ha sido creada una escuela\n quieres remplazarla? (S/N): ");
                        remplazarEsc = convertirMayus(remplazarEsc);
                        if (remplazarEsc == "S") {
                            liberarEscuela(ptrMiEscuela);
                            break;
                        } else if (remplazarEsc == "N") {
                            crearOtraEsc = false;
                        } else {
                            repetir = true;
                        }
                    } while (repetir);
                }

                if (!crearOtraEsc) {
                    cout << "Si no vas a modificar el array seras redirigido al menu principal\n";
                    waitfor(3500);
                    break;
                }

                // creamos e inicializamos la escuela
                ptrMiEscuela = crearEscuela();
                menuEscuela(nombreEscuela, cantEst, cantMat);
                // inicializamos la escuela
                initEscuela(ptrMiEscuela, nombreEscuela, cantEst, cantMat);
                escuelaYaCreada = true;

                // llenamos los arrays de la escuela
                LlenarEscuela(ptrMiEscuela);

                cout << "Escuela llenada con exito\n";
                waitfor(3500); // Esperamos un tiempo de 3.5 segundos
                break;

            // mostrar escuela
            case 2:
                if (!escuelaYaCreada) {
                    limpiarPantalla(); // Limpiamos la Pantalla
                    cout << "ADVERTENCIA: Aun no has creado la escuela\n";
                    cout << "Crea primero la Escuela para usar estas funciones\n";
                    waitfor(3500); // Esperamos un tiempo de 3.5 segundos
                    break;
                } else {
                    limpiarPantalla();
                    cout << "-------CONTENIDO DE LA ESCUELA-------\n\n";
                    mostrarEscuela(ptrMiEscuela);
                }
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            // mejor promedio
            case 3:
                mejorPromedio = 0;
                if (!escuelaYaCreada) {
                    limpiarPantalla(); // Limpiamos la Pantalla
                    cout << "ADVERTENCIA: Aun no has creado la escuela\n";
                    cout << "Crea primero la Escuela para usar estas funciones\n";
                    waitfor(3500); // Esperamos un tiempo de 3.5 segundos
                    break;
                } else {
                    limpiarPantalla();
                    mejorPromedio = buscarMejorPromedio(ptrMiEscuela);
                    cout << "Mejro Promedio: " << mejorPromedio << std::fixed << std::setprecision(3) << endl;
                    pausarPrograma(); // Esperamos que el usuario presione la tecla enter para continuar
                }
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            case 4:
                if (!escuelaYaCreada) {
                    limpiarPantalla(); // Limpiamos la Pantalla
                    cout << "ADVERTENCIA: Aun no has creado la escuela\n";
                    cout << "Crea primero la Escuela para usar estas funciones\n";
                    waitfor(3500); // Esperamos un tiempo de 3.5 segundos
                    break;
                } else {
                    limpiarPantalla();
                    cout << "-------CALCULAR PROMEDIO GENERAL-------\n\n";
                    promedioGeneral = calcularPromedio(ptrMiEscuela);
                    // fijamos el resultado a solo decimales y obligamos a que use 3 decimales
                    cout << "Promedio General: " << std::fixed << std::setprecision(3) << promedioGeneral << endl;
                    pausarPrograma(); // Esperamos que el usuario presione la tecla enter para continuar
                }
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            case 5:
                if (!escuelaYaCreada) {
                    limpiarPantalla(); // Limpiamos la Pantalla
                    cout << "ADVERTENCIA: Aun no has creado la escuela\n";
                    cout << "Crea primero la Escuela para usar estas funciones\n";
                    waitfor(3500); // Esperamos un tiempo de 3.5 segundos
                    break;
                } else {
                    string busqueda;
                    limpiarPantalla();
                    cout << "-------BUSCAR ESTUDIANTE-------\n\n";
                    ingresarID(id, "Ingresar ID: ");
                    busqueda = buscarId(ptrMiEscuela, id);

                    if (busqueda != "ERROR") {
                        cout << "ID: " << id << endl;
                        cout << "Nombre: " << busqueda << endl;
                    } else {
                        cout << "Error no se pudo encontrar el estudiante de id " << id << endl;
                    }
                    pausarPrograma(); // Esperamos que el usuario presione la tecla enter para continuar
                }
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            case 6:
                limpiarPantalla(); // Limpiamos la Pantalla
                cout << "Saliendo...";
                waitfor(3500);     // Esperamos un tiempo de 3.5 segundos
                limpiarPantalla(); // Limpiamos la Pantalla
                break;

            default:
                limpiarPantalla(); // Limpiamos la Pantalla
                cout << "ERROR opcion no disponible\n";
                waitfor(3500);     // Esperamos un tiempo de 3.5 segundos
                limpiarPantalla(); // Limpiamos la Pantalla
        }
    } while (opcion != 6);

    liberarEscuela(ptrMiEscuela);
    return 0;
}