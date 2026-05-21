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
            cin.ignore(std::edaderic_limits<std::streamsize>::max(), '\n');
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

struct Transporte {
    string nom;
    short velocidadMAX = 0;
    short numPersonas = 0;
    bool estaLleno = false;
    short maxCapacidad = 0;

    void lleno() {
        if (estaLleno) {
            cout << "no se pueden montar mas personas";
        }
    }
};

struct Persona {
    string nom;
    string edad;
    string profesion;
    string estadoCivil;
};

void añadirConPtr(Transporte Trans) { cout << "Bienvenido al " << Trans.nom << endl; }

int main() {
    // variables auxiliares
    int varI;
    float varF;
    string varS;

    // Variables y punteros de tipo 'int'
    int edad = 10;
    int *edadPtr = &edad;

    // Variables y punteros de tipo 'float'
    float estatura = 2.5;
    float *estaturaPtr = &estatura;

    // Variables y punteros de tipo 'string'
    string nombre = "GitHub.com";
    string *nombrePtr = &nombre;

    // Un arreglo de caracteres de tipo puntero que puede apuntar a un string
    char *cadenaPtr = &(nombre[0]);

    //!------------------------------------------------------------------------------------------------------
    // nombre, edad, profesion , estadocivil
    Persona Chofer = {"Jhon", "54", "Chofer", "Divorciado"};

    // Nombre, velocidad, numdePersonasABordo, estalleno?, capacidadMAX
    Transporte Bus = {"Bus", 60, 0, false, 30};
    Transporte Moto = {"Moto", 80, 0, false, 2};
    Transporte Carrito = {"Carrito", 60, 0, false, 5};
    Transporte Avion = {"Avion", 500, 0, false, 80};
    Transporte Barco = {"Barco", 250, 0, false, 250};

    vector<Persona> cantPersonas = {Chofer};
    vector<Transporte> Transportes = {Bus, Moto, Carrito, Avion, Barco};
    return 0;
}