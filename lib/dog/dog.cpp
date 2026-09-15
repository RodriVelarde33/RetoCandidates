#include "dog.h"

Dog::Dog(string name, int edad, int peso) {
    this->name = name;
    this->edad = edad;
    this->peso = peso;
}

string Dog::getName() {
    return name;
}

int Dog::getEdad() {
    return edad;
}

int Dog::getPeso() {
    return peso;
}

void Dog::setName(string name) {
    this->name = name;
}

void Dog::setEdad(int edad) {
    this->edad = edad;
}

void Dog::setPeso(int peso) {
    this->peso = peso;
}