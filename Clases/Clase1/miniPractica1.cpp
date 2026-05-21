#include <iostream>
#include <limits>
#include <string>
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
    vector<Personas> Personas;

    void lleno() {
        if (estaLleno) {
            cout << "no se pueden montar mas personas";
        }
    }
};

// Usamos un puntero para ahorrar memoria ya que solo le pasamos al direccion
void añadirConPtr(Transporte *Trans) {
    Persona p;
    cout << "Bienvenido al " << Trans->nom << endl;
    cout << "Usted se ha montado con exito en el transporte, por favor espere su parada" << endl;
    cout << "Introduzca los siguientes datos para poder registralo: ";
    cout << "-----NOMBRE-----" << endl;
    ingresarDatos(p.nom);

    cout << "-----EDAD-----" << endl;
    ingresarDatos(p.edad);

    cout << "-----CEDULA-----" << endl;
    ingresarDatos(p.cedula);
}

int main() {
    // nombre, edad, profesion , estadocivil
    Persona Chofer = {"Jhon", "54", "12854674"};

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
    return 0;
}