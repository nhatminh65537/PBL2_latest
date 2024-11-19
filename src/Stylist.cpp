//
// Created by LONG on 11/8/2024.
//

#include "Stylist.h"
#include <Appointment.h>
#include "Database.h"

Database<Customer>& dbCustomer = Database<Customer>::Connect(CUSTOMERS_FILE);

Stylist::Stylist(const string &ID, const string &firstName, const string &lastName,
    const bool &gender, const int &age, const string &phoneNumber, const string &username,
    const string &password)
    : Member(ID,firstName,lastName,gender,age,phoneNumber,username,password) {
}

Stylist::~Stylist() {

}

void Stylist::viewScheduleByDay(const Datetime& dt) const {
    cout << "Schedule for: " << dt.GetDay() << "/" << dt.GetMonth() << "/" << dt.GetYear() << '\n';
    bool hasAppointments = false;

    for (const auto& appointment : schedule) {
        if (appointment.GetStartTime().GetDay() == dt.GetDay() &&
            appointment.GetStartTime().GetMonth() == dt.GetMonth() &&
            appointment.GetStartTime().GetYear() == dt.GetYear()) {
            hasAppointments = true;
            cout << "Schedule with " << dbCustomer.Get(appointment.GetCustomerID()) << "\n";
            cout << appointment.GetStartTime().GetMinute() << " " << appointment.GetStartTime().GetHour() << "\n";
        }
    }
    if(!hasAppointments) {
        cout << "No Appointment!\n";
    }
}

void Stylist::viewSchedule() const {
    // Tam thoi khach hang chua chon tho, nen tho chua co lich
}
