#ifndef APPOINTMENT_H
#define APPOINTMENT_H
#include <vector>
#include <string>
#include <iostream>

#include "define.h"
#include "Identifiable.h"

using namespace std;

class Appointment : public Identifiable
{
    public:
        Appointment(const string& appointmentID = "null",
            const string& customerID = "null",
            const vector<Service>& serviceList = {});
        ~Appointment() override;
        void Show() const override;
        [[nodiscard]] const string& GetCustomerID() const;
        void SetCustomerID(const string& );
        friend ostream& operator<<(ostream& os, const Appointment& appointment);
        friend istream& operator>>(istream& is, Appointment& appointment);
    private:
        string customerID;
        vector<Service> serviceList;
};
#endif // APPOINTMENT_H
