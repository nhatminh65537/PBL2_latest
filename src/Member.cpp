#include <iostream>
#include "Member.h"
#include "define.h"

using namespace std;

Member::Member(const string& ID,const string &firstName, const string &lastName, const bool &gender, const int &age,
    const string &phoneNumber, const string &username, const string &password) {
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

string Member::GetFirstName() const {
    return this->firstName;
}

void Member::SetFirstName(const string &val) {
    this->firstName = val;
}

string Member::GetLastName() const {
    return this->lastName;
}

void Member::SetLastName(const string &val) {
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

string Member::GetPhoneNumber() const {
    return this->phoneNumber;
}

void Member::SetPhoneNumber(const string &val) {
    this->phoneNumber = val;
}

string Member::GetUserName() const {
    return this->username;
}

void Member::SetUserName(const string &val) {
    this->username = val;
}

string Member::GetPassword() const {
    return this->password;
}

void Member::SetPassword(const string &val) {
    this->password = val;
}

void Member::Show() const {
    cout << "ID: "  << this->ID << '\n';
    cout << "Username: "  << this->username << '\n';
    cout << "Full name: "  << this->GetFullName() << '\n';
    cout << "Gender: "  << (this->gender==1 ? "Male" : "Female") << '\n';
    cout << "Age: "  << this->age << '\n';
    cout << "Phone: "  << this->phoneNumber << '\n';
}


string Member::GetFullName() const {
    return this->GetFirstName() + ' ' + this->GetLastName();
}

ostream& operator<<(ostream& os, const Member& obj) {
    os << obj.ID << ' ' << Replace(obj.firstName,' ','-')  << ' ' << Replace(obj.lastName,' ','-') << ' '
    << obj.username << ' ' << obj.gender << ' '
    << obj.age << ' ' << obj.phoneNumber;
    return os;
}

istream& operator>>(istream& is, Member& obj) {
    is >> obj.ID >> obj.firstName >> obj.lastName >> obj.username >> obj.gender >> obj.age >> obj.phoneNumber;
    return is;
}

