#include "Appointment.h"

#include <algorithm>
#include <ranges>

Appointment::Appointment(const std::string& appointmentID,
                        const Datetime& startTime,
                         const std::string& customerID,
                         const std::string& stylistID,
                         const std::vector<Service>& serviceList) {
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
    std::cout << "Appointment_ID: " << this->ID << '\n';
    std::cout << "StylistID: " << this->stylistID << '\n';
    std::cout << "CustomerID: " << this->customerID << '\n';
    std::cout << "ServiceList: ";
    for (size_t i = 0; i < this->serviceList.size(); i++) {
        std::cout << ServiceToString(this->serviceList[i]);
        if (i < this->serviceList.size() - 1)   std::cout << ", ";
    }
    std::cout << '\n';
}

const std::string& Appointment::GetCustomerID() const {
    return this->customerID;
}

void Appointment::SetCustomerID(const std::string& customerID) {
    this->customerID = customerID;
}

const std::string& Appointment::GetStylistID() const {
    return this->stylistID;
}

void Appointment::SetStylistID(const std::string& stylistID) {
    this->stylistID = stylistID;
}

const Datetime& Appointment::GetStartTime() const {
    return this->startTime;
}

void Appointment::SetStartTime(const Datetime& date) {
    this->startTime = date;
}

const std::vector<Service>& Appointment::GetServices() const {
    return this->serviceList;
}

void Appointment::SetServices(const std::vector<Service>& serviceList) {
    this->serviceList = serviceList;
}

std::ostream& operator<<(std::ostream& os, const Appointment& obj) {
    os << obj.ID << ' ' << obj.startTime << ' ' << obj.stylistID << ' ' << obj.customerID;
    for (const Service& service : obj.serviceList) {
        os << ' ' << service;
    }
    return os;
}

std::istream& operator>>(std::istream& is, Appointment& appointment) {
    is >> appointment.ID >> appointment.startTime >> appointment.stylistID >> appointment.customerID;

    appointment.serviceList.clear();
    std::string serviceStr;
    getline(is, serviceStr);
    std::vector<std::string> serviceList = Split(serviceStr,' ');

    for (const std::string& service : serviceList) {
        appointment.serviceList.push_back(static_cast<Service>(stoi(service)));
    }
    std::ranges::sort(appointment.serviceList);
    return is;
}