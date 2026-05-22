#include <chrono>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

template <class Tipo1> //
void ingresarDatos(Tipo1 &variable) {
    bool flag;
    do {
        flag = false;
        cout << "Ingresa aqui: ";
        cin >> variable;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "\nError tipo de dato incoreccto" << endl;
            flag = true;
        }
    } while (flag);
}

template <typename Tipo2> //
Tipo2 cambiarVar(Tipo2 var) {
    ingresarDatos(var);
    return var;
}

struct Persona {
    string nom;
    string edad;
    string cedula;
};

struct Transporte {
    string nom;
    short velocidadMAX = 0;
    short numPersonas = 0;
    bool estaLleno = false;
    short maxCapacidad = 0;
    vector<Persona> Personas;

    Persona var;

    void añadirPersonas(Transporte *Trans) {
        Persona p;
        cout << "Bienvenido a " << Trans->nom << endl;
        cout << "Introduzca los siguientes datos para poder registralo: ";
        cout << "\n-----NOMBRE-----" << endl;
        ingresarDatos(p.nom);

        cout << "-----EDAD-----" << endl;
        ingresarDatos(p.edad);

        cout << "-----CEDULA-----" << endl;
        ingresarDatos(p.cedula);
        Personas.push_back(p);
        numPersonas++;
        cout << "Usted se ha montado con exito en el transporte, por favor espere su parada" << endl;
    }

    bool lleno() {
        if (numPersonas == maxCapacidad) {
            return true;
        } else
            return false;
    }
};

int main() {
    // nombre, edad, profesion , estadocivil
    Persona Chofer = {"Jhon", "54", "12854674"};
    Persona p;

    // Nombre, velocidad, numdePersonasABordo, estalleno?, capacidadMAX
    Transporte Bus = {"Bus", 60, 0, false, 30};
    Transporte Moto = {"Moto", 80, 0, false, 2};
    Transporte Carrito = {"Carrito", 60, 0, false, 5};
    Transporte Avion = {"Avion", 500, 0, false, 80};
    Transporte Barco = {"Barco", 250, 0, false, 250};

    // Punteros de transportes
    Transporte *BusPtr = &Bus;
    Transporte *MotoPtr = &Moto;
    Transporte *CarritoPtr = &Carrito;
    Transporte *AvionPtr = &Avion;
    Transporte *BarcoPtr = &Barco;

    // vectores normales

    // vectores de punteros
    vector<Transporte *> TransportesPtr = {BusPtr, MotoPtr, CarritoPtr, AvionPtr, BarcoPtr};

    cout << endl << "Tamanio del objeto Bus: " << sizeof(Bus) << " bytes" << endl;
    cout << "Tamanio del objeto Moto: " << sizeof(Moto) << " bytes" << endl;
    cout << "Tamanio del objeto Avion: " << sizeof(Avion) << " bytes" << endl;
    cout << "Tamanio del objeto Barco: " << sizeof(Barco) << " bytes" << endl;
    cout << "Tamanio del objeto Carrito: " << sizeof(Carrito) << " bytes" << endl;

    int opcion = 0;

    do {
        cout << "En que transporte desea viajar?\n1. Bus\n2. Carrito\n3. Avion\n4. Barco\n5. Moto" << endl;
        opcion = 0;
        ingresarDatos(opcion);
        switch (opcion) {
            case 1:
                if (!Bus.lleno()) {
                    Bus.añadirPersonas(BusPtr);
                } else
                    cout << "El bus esta lleno" << endl;
                break;

            case 2:
                if (!Carrito.lleno()) {
                    Carrito.añadirPersonas(CarritoPtr);
                } else
                    cout << "El Carrito esta lleno" << endl;
                break;

            case 3:
                if (!Avion.lleno()) {
                    Avion.añadirPersonas(AvionPtr);
                } else
                    cout << "El Avion esta lleno" << endl;
                break;

            case 4:
                if (!Barco.lleno()) {
                    Barco.añadirPersonas(BarcoPtr);
                } else
                    cout << "El Barco esta lleno" << endl;
                break;

            case 5:
                if (!Moto.lleno()) {
                    Moto.añadirPersonas(MotoPtr);
                } else
                    cout << "La Moto esta llena" << endl;
                break;

            case 6:
                cout << "Saliendo..." << endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                break;

            default:
                cout << "ERROR opcion invalida" << endl;
        }
    } while (opcion != 6);

    return 0;
}