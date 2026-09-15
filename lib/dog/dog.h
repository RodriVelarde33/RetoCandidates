#ifndef DOG_H
#define DOG_H
using namespace std;
#include <string>

class Dog {
    private: 
    string name;
    int edad;
    int peso;


    public:
    //consturctor
    Dog(string name, int edad, int peso);

    //Getters
    string getName();
    int getEdad();
    int getPeso();

    //Setters
    void setName(string name);
    void setEdad(int edad);
    void setPeso(int peso);


    #endif

};
