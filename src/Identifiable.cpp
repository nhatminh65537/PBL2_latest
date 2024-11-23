#include "Identifiable.h"
#include <iostream>

Identifiable::Identifiable(const std::string& ID)
{
    this->ID = ID;
}

Identifiable::~Identifiable()
{

}

const std::string& Identifiable::GetID() const {
    return this->ID;
}

void Identifiable::SetID(const std::string& ID) {
    this->ID = ID;
}
