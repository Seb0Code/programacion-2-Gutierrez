#include <iostream>
#include <limits>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;

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

// template <class Tipo3> //

int main() {
    // variables auxiliares
    int varI;
    float varF;
    string varS;

    // Variables y punteros de tipo 'int'
    int num = 10;
    int *numPtr = &num;

    // Variables y punteros de tipo 'float'
    float decimal = 2.5;
    float *decimalPtr = &decimal;

    // Variables y punteros de tipo 'string'
    string texto = "GitHub.com";
    string *textoPtr = &texto;

    // Un arreglo de caracteres de tipo puntero que puede apuntar a un string
    char *cadenaPtr = &(texto[0]);

    // system("cls");
    cout << "\n\n\n1.----------PRACtICA BASICA DE PUNtEROS----------\n\n\n";

    cout << "----------ACCEDIENDO A LOS VALORES DE NUM Y NUMPtR----------\n\n";
    cout << "Valor original de num: " << num << endl;
    cout << "Direccion de num: " << &num << endl;
    cout << "tamanio de num (en bytes): " << sizeof(num) << endl;
    cout << "Lo que almacena numPtr: " << numPtr << endl;
    cout << "Donde apunta numPtr: " << *numPtr << endl;
    cout << "tamanio de numPtr (en bytes): " << sizeof(numPtr) << endl;

    cout << "\n----------ACCEDIENDO A LOS VALORES DE DECIMAL Y DECIMALPtR----------\n\n";
    cout << "Valor original de decimal: " << decimal << endl;
    cout << "Direccion de decimal: " << &decimal << endl;
    cout << "tamanio de decimal (en bytes): " << sizeof(decimal) << endl;
    cout << "Lo que almacena decimalPtr: " << decimalPtr << endl;
    cout << "Donde apunta decimalPtr: " << *decimalPtr << endl;
    cout << "tamanio de decimalPtr (en bytes): " << sizeof(decimalPtr) << endl;

    cout << "\n----------ACCEDIENDO A LOS VALORES DE tEXtO Y tEXtOPtR----------\n\n";
    cout << "Valor original de texto: " << texto << endl;
    cout << "Direccion de texto: " << &texto << endl;
    cout << "tamanio de texto (en bytes): " << sizeof(texto) << endl;
    cout << "Lo que almacena textoPtr: " << textoPtr << endl;
    cout << "Donde apunta textoPtr: " << *textoPtr << endl;
    cout << "tamanio de textoPtr (en bytes): " << sizeof(textoPtr) << endl;

    cout << "\n\n\n\n----------MODIFICANDO LOS VALORES MENDIANTE LAS VARIABLES----------\n\n\n\n";

    // llamamos a la funcion para cambiar el valor de num
    cout << "Valor de num antes: " << num << endl;
    num = cambiarVar(num);
    cout << "Valor de num despues: " << num << endl << endl;

    // llamamos a la funcion para cambiar el valor de decimal
    cout << "Valor de decimal antes: " << decimal << endl;
    decimal = cambiarVar(decimal);
    cout << "Valor de decimal despues: " << decimal << endl << endl;

    // llamamos a la funcion para cambiar el valor de texto
    cout << "Valor de texto antes: " << texto << endl;
    texto = cambiarVar(texto);
    cout << "Valor de texto: " << texto << endl << endl;

    cout << "\n\n\n\n----------MODIFICANDO LOS VALORES MENDIANTE LOS PUNTEROS----------\n\n\n\n";

    // llamamos a la funcion para cambiar el valor de num desde numPtr
    cout << "Valor de num antes: " << num << endl;
    ingresarDatos(varI);
    *numPtr = varI;
    cout << "Valor de num despues: " << num << endl << endl;

    // llamamos a la funcion para cambiar el valor de decimal desde decimalPtr
    cout << "Valor de decimal antes: " << decimal << endl;
    ingresarDatos(varF);
    *decimalPtr = varF;
    cout << "Valor de decimal despues: " << decimal << endl << endl;

    // llamamos a la funcion para cambiar el valor de texto desde textoPtr
    cout << "Valor de texto antes: " << texto << endl;
    ingresarDatos(varS);
    *textoPtr = varS;
    cout << "Valor de texto despues: " << texto << endl << endl;

    return 0;
}