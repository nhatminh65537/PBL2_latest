#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <vector>
#include "define.h"
#include "Member.h"
#include "Appointment.h"

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
        void BookAppointment(const vector<Service>& serviceList) const;
        void CancelAppointment(const string& appointmentID) const;
        void ViewAppointment() const;
    private:
};

#endif // CUSTOMER_H
