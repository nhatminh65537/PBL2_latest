#include "Identifiable.h"
#include <iostream>

Identifiable::Identifiable(const string& ID)
{
    this->ID = ID;
}

Identifiable::~Identifiable()
{

}

const string& Identifiable::GetID() const {
    return this->ID;
}

void Identifiable::SetID(const string& ID) {
    this->ID = ID;
}
