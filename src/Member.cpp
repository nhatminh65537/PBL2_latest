#include <iostream>
#include "Member.h"
#include "define.h"

Member::Member(const std::string& ID,const std::string &firstName, const std::string &lastName, const bool &gender, const int &age,
    const std::string &phoneNumber, const std::string &username, const std::string &password) {
    this->ID = ID;
    this->firstName = firstName;
    this->lastName = lastName;
    this->gender = gender;
    this->age = age;
    this->phoneNumber = phoneNumber;
    this->username = username;
    this->password = password;
}

Member::~Member(){

}

std::string Member::GetFirstName() const {
    return this->firstName;
}

void Member::SetFirstName(const std::string &val) {
    this->firstName = val;
}

std::string Member::GetLastName() const {
    return this->lastName;
}

void Member::SetLastName(const std::string &val) {
    this->lastName = val;
}

bool Member::GetGender() const {
    return this->gender;
}

void Member::SetGender(const bool &val) {
    this->gender = val;
}

int Member::GetAge() const {
    return this->age;
}

void Member::SetAge(const int &val) {
    this->age = val;
}

std::string Member::GetPhoneNumber() const {
    return this->phoneNumber;
}

void Member::SetPhoneNumber(const std::string &val) {
    this->phoneNumber = val;
}

std::string Member::GetUserName() const {
    return this->username;
}

void Member::SetUserName(const std::string &val) {
    this->username = val;
}

std::string Member::GetPassword() const {
    return this->password;
}

void Member::SetPassword(const std::string &val) {
    this->password = val;
}

void Member::Show() const {
    std::cout << "ID: "  << this->ID << '\n';
    std::cout << "Username: "  << this->username << '\n';
    std::cout << "Full name: "  << this->GetFullName() << '\n';
    std::cout << "Gender: "  << (this->gender==1 ? "Male" : "Female") << '\n';
    std::cout << "Age: "  << this->age << '\n';
    std::cout << "Phone: "  << this->phoneNumber << '\n';
}


std::string Member::GetFullName() const {
    return this->GetFirstName() + ' ' + this->GetLastName();
}

std::ostream& operator<<(std::ostream& os, const Member& obj) {
    os << obj.ID << ' ' << Replace(obj.firstName,' ','-')  << ' ' << Replace(obj.lastName,' ','-') << ' '
    << obj.username << ' ' << obj.gender << ' '
    << obj.age << ' ' << obj.phoneNumber;
    return os;
}

std::istream& operator>>(std::istream& is, Member& obj) {
    is >> obj.ID >> obj.firstName >> obj.lastName >> obj.username >> obj.gender >> obj.age >> obj.phoneNumber;
    return is;
}

