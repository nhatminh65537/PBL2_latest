#include "Identifiable.h"

Identifiable::Identifiable(const string& ID)
{
    this->ID = ID;
}

Identifiable::~Identifiable()
{
    //dtor
}

const string& Identifiable::GetID() const {
    return this->ID;
}

void Identifiable::SetID(const string& ID) {
    this->ID = ID;
}
