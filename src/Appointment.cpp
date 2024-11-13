#include "Appointment.h"

#include <algorithm>
#include <ranges>

Appointment::Appointment(const string& appointmentID,
                         const string& customerID,
                         const vector<Service>& serviceList) {
    this->ID = appointmentID;
    this->customerID = customerID;
    this->serviceList = serviceList;
}

Appointment::~Appointment()
{
    //dtor
}

void Appointment::Show() const {
    cout << "Appointment_ID: " << this->ID << '\n';
    cout << "CustomerID: " << this->customerID << '\n';
    cout << "ServiceList: ";
    for (size_t i = 0; i < this->serviceList.size(); i++) {
        cout << ServiceToString(this->serviceList[i]);
        if (i < this->serviceList.size() - 1)   cout << ", ";
    }
    cout << '\n';
}

const string& Appointment::GetCustomerID() const {
    return this->customerID;
}

void Appointment::SetCustomerID(const string& customerID) {
    this->customerID = customerID;
}


ostream& operator<<(ostream& os, const Appointment& obj) {
    os << obj.ID << ' ' << obj.customerID;
    for (const Service& service : obj.serviceList) {
        os << ' ' << service;
    }
    return os;
}

istream& operator>>(istream& is, Appointment& appointment) {
    is >> appointment.ID >> appointment.customerID;

    appointment.serviceList.clear();
    string serviceStr;
    getline(is, serviceStr);
    vector<string> serviceList = Split(serviceStr,' ');

    for (const string& service : serviceList) {
        appointment.serviceList.push_back(static_cast<Service>(stoi(service)));
    }
    ranges::sort(appointment.serviceList);
    return is;
}