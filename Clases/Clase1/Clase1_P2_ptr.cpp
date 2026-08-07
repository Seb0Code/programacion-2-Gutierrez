#include <iostream>
using std::cin;
using std::cout;
using std ::endl;
using std::string;

void funcion() { cout << "Esto es una funcion\n"; }

int main() {
    // Aritmetica de Punteros



    // Punteros a punteros
    int numero = 30;
    int *ptr = &numero;  // puntero normal
    int **ptr2 = &ptr;   // puntero a puntero
    int ***ptr3 = &ptr2; // puntero a puntero que apunta a otro puntero

    cout << "Valor de numero ('numero'): " << numero << endl;
    cout << "Direccion de numero ('&numero'): " << &numero << endl;
    cout << "Valor de ptr ('ptr'): " << ptr << endl;
    cout << "Direccion de ptr ('&ptr'): " << &ptr << endl;
    cout << "Valor de ptr2 ('ptr2'): " << ptr2 << endl;
    cout << "Direccion de ptr2 ('&ptr2'): " << &ptr2 << endl;
    cout << "Valor de ptr3 ('ptr3'): " << ptr3 << endl;
    cout << "Direccion de ptr3 ('&ptr3'): " << &ptr3 << endl << endl << endl;

    // Accediendo mediante punteros

    cout << "Valor de numero a través de puntero ('*ptr'): " << *ptr << endl;
    cout << "Valor de numero a traves de punteroDoble ('**ptr2'): " << **ptr2 << endl;
    cout << "Valor de numero a traves de punteroTriple ('***ptr3'): " << ***ptr3 << endl << endl << endl;

    // Ahora bien, que pasa si hacemos? Seran Equivalentes?

    cout << "Aqui podemos apreciar los niveles de desrefernciacion\nComo solo desreferenciamos una vez\nEl ptr no llega hasta el valor de numero, sino que accede a lo que hay en ";
    cout << "puntero\nEn este caso la direccion de numero:" << endl;
    cout << "*ptr2: " << *ptr2 << endl; // Lo desreferenciamos otra vez
    cout << "ptr: " << ptr << endl;
    cout << "&numero: " << &numero << endl;

    cout << "igual pasa si: " << endl;
    cout << "**ptr3: " << **ptr3 << endl;
    cout << "ptr: " << ptr << endl;

    cout << endl << "o" << endl << endl;
    cout << "*ptr3: " << *ptr3 << endl;
    cout << "ptr2: " << ptr2 << endl << endl << endl;

    // el nombre de una funcion actua como la direccion de memoria del inicio de la funcion
    // usamos reinterpreted_cast<type*>(function) para que la direccion pueda ser sacada a consola
    // cout << "Direccion de memoria de funcion(): " << reinterpret_cast<void *>(funcion) << endl;
    cout << "Direccion de memoria de funcion(): " << (void *)(funcion) << endl;

    // puntero a funcion
    cout << "Un puntero a funcion se declara de la siguiente forma:" << endl;
    cout << "tipoDeRetorno (*nombreDelPtr) (parametros que tiene la funcion)= function\n";
    cout << "Ejemplo: \nvoid (*ptrfuncion)(sin parametros)= funcion\n";
    void (*ptrfuncion)() = funcion;
    cout << "*ptrfuncion: " << (void *)ptrfuncion << endl;

    cout << "Llamada a la funcion a traves del puntero: \n funcion() -> ";
    ptrfuncion();
    return 0;
}