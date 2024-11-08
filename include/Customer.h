#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <vector>


#include "Appointment.h"
#include "Database.h"
#include "Member.h"
#include "define.h"

using namespace std;

class Customer : public Member {
    public:
        explicit Customer(const string& ID ="null",
            const string& firstName="null",
            const string& lastName="null",
            const bool& gender=false,const int& age=0,
            const string& phoneNumber="null",
            const string& username="null",
            const string& password="null");

            friend ostream& operator<<(ostream&, const Customer&);
            friend istream& operator>>(istream&, Customer&);
        ~Customer() override;
        void BookAppointment(Database<Appointment>& db,const vector<Service>& serviceList) const;
        void CancelAppointment(Database<Appointment>& db,const string& appointmentID) const;
        void ViewAppointment(const Database<Appointment>& db) const;
    private:
};

#endif // CUSTOMER_H
