#include "Appointment.h"

#include <algorithm>
#include <ranges>

Appointment::Appointment(const string& appointmentID,
                        const Datetime& startTime,
                         const string& customerID,
                         const string& stylistID,
                         const vector<Service>& serviceList) {
    this->ID = appointmentID;
    this->startTime = startTime;
    this->customerID = customerID;
    this->stylistID = stylistID;
    this->serviceList = serviceList;
}

Appointment::~Appointment()
{
    //dtor
}

void Appointment::Show() const {
    cout << "Appointment_ID: " << this->ID << '\n';
    cout << "StylistID: " << this->stylistID << '\n';
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

const string& Appointment::GetStylistID() const {
    return this->stylistID;
}

void Appointment::SetStylistID(const string& stylistID) {
    this->stylistID = stylistID;
}

const Datetime& Appointment::GetStartTime() const {
    return this->startTime;
}

void Appointment::SetStartTime(const Datetime& date) {
    this->startTime = date;
}

const vector<Service>& Appointment::GetServices() const {
    return this->serviceList;
}

void Appointment::SetServices(const vector<Service>& serviceList) {
    this->serviceList = serviceList;
}

ostream& operator<<(ostream& os, const Appointment& obj) {
    os << obj.ID << ' ' << obj.startTime << ' ' << obj.stylistID << ' ' << obj.customerID;
    for (const Service& service : obj.serviceList) {
        os << ' ' << service;
    }
    return os;
}

istream& operator>>(istream& is, Appointment& appointment) {
    is >> appointment.ID >> appointment.startTime >> appointment.stylistID >> appointment.customerID;

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