//
// Created by LONG on 11/8/2024.
//

#include "Stylist.h"

Stylist::Stylist(const string &ID, const string &firstName, const string &lastName,
    const bool &gender, const int &age, const string &phoneNumber, const string &username,
    const string &password, const Service& role)
    : Member(ID,firstName,lastName,gender,age,phoneNumber,username,password) {
    this->role = role;
}

Stylist::~Stylist() {

}

void Stylist::viewSchedule() const {
    // Tam thoi khach hang chua chon tho, nen tho chua co lich
}
