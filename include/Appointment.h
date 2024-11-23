#ifndef APPOINTMENT_H
#define APPOINTMENT_H
#include <vector>
#include <string>
#include <iostream>
#include "Datetime.h"
#include "define.h"
#include "Identifiable.h"

using namespace std;

class Appointment : public Identifiable
{
    public:
        Appointment(const string& appointmentID = "null",
            const Datetime& startTime=Datetime::Now(),
            const string& customerID = "null",
            const string& stylistID = "null",
            const vector<Service>& serviceList = {});
        ~Appointment() override;
        void Show() const override;
        [[nodiscard]] const string& GetCustomerID() const;
        void SetCustomerID(const string& );
        [[nodiscard]] const string& GetStylistID() const;
        void SetStylistID(const string&);
        [[nodiscard]] const Datetime& GetStartTime() const;
        void SetStartTime(const Datetime&);
        [[nodiscard]] const vector<Service>& GetServices() const;
        void SetServices(const vector<Service>&);
        friend ostream& operator<<(ostream& os, const Appointment& appointment);
        friend istream& operator>>(istream& is, Appointment& appointment);
    private:
        Datetime startTime;
        string stylistID;
        string customerID;
        vector<Service> serviceList;
};
#endif // APPOINTMENT_H
