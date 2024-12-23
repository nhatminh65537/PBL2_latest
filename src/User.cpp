#include <iostream>
#include "User.h"
#include "define.h"

User::User(const std::string& ID,const std::string &firstName, const std::string &lastName, const std::string &username, const std::string &password, const bool &gender,
    const std::string &phoneNumber,const int& role) {
    this->ID = ID;
    this->firstName = firstName;
    this->lastName = lastName;
    this->gender = gender;
    this->phoneNumber = phoneNumber;
    this->username = username;
    this->password = password;
    this->role = role;
}

User::~User(){

}

std::string User::GetFirstName() const {
    return this->firstName;
}

void User::SetFirstName(const std::string &val) {
    this->firstName = val;
}

std::string User::GetLastName() const {
    return this->lastName;
}

void User::SetLastName(const std::string &val) {
    this->lastName = val;
}

bool User::GetGender() const {
    return this->gender;
}

void User::SetGender(const bool &val) {
    this->gender = val;
}

std::string User::GetPhoneNumber() const {
    return this->phoneNumber;
}

void User::SetPhoneNumber(const std::string &val) {
    this->phoneNumber = val;
}

std::string User::GetUserName() const {
    return this->username;
}

void User::SetUserName(const std::string &val) {
    this->username = val;
}

std::string User::GetPassword() const {
    return this->password;
}

void User::SetPassword(const std::string &val) {
    this->password = val;
}

int User::GetRole() const {
    return this->role;
}

void User::SetRole(const int &role) {
    this->role = role;
}


void User::Show() const {
    std::cout << "ID: "  << this->ID << '\n';
    std::cout << "Username: "  << this->username << '\n';
    std::cout << "Full name: "  << this->GetFullName() << '\n';
    std::cout << "Gender: "  << (this->gender==1 ? "Male" : "Female") << '\n';
    std::cout << "Phone: "  << this->phoneNumber << '\n';
    std::cout << "Role: "  << (this->role==1 ? "Customer" : this->role == 2 ? "Stylist" : "Admin") << '\n';
}


std::string User::GetFullName() const {
    return Replace(this->GetFirstName(),'-',' ') + ' ' + this->GetLastName();
}

std::ostream& operator<<(std::ostream& os, const User& obj) {
    os << obj.ID << ' ' << Replace(Strip(obj.firstName),' ','-')  << ' ' << Replace(Strip(obj.lastName),' ','-') << ' '
    << obj.username << ' ' << obj.password << ' ' << obj.gender << ' ' << obj.phoneNumber << ' ' << obj.role;
    return os;
}

std::istream& operator>>(std::istream& is, User& obj) {
    is >> obj.ID >> obj.firstName >> obj.lastName >> obj.username >> obj.password >>  obj.gender >> obj.phoneNumber >> obj.role;
    obj.firstName = Replace(obj.firstName,'-',' ');
    obj.lastName = Replace(obj.lastName,'-',' ');
    return is;
}

