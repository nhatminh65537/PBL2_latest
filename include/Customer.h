#ifndef CUSTOMER_H
#define CUSTOMER_H
#include <vector>
#include "define.h"
#include "Member.h"
#include "Appointment.h"


class Customer : public Member {
    public:
        explicit Customer(const std::string& ID ="null",
        const std::string& firstName="null",
        const std::string& lastName="null",
        const bool& gender=false,const int& age=0,
        const std::string& phoneNumber="null",
        const std::string& username="null",
        const std::string& password="null");

        ~Customer() override;

        friend std::ostream& operator<<(std::ostream&, const Customer&);
        friend std::istream& operator>>(std::istream&, Customer&);

        bool Login(const std::string &, const std::string &) const override;
        bool Logout() const override;

        void BookAppointment(const Datetime& date,const std::vector<Service>& serviceList) const;
        void CancelAppointment(const std::string& appointmentID) const;
        void ViewAppointment() const;
    private:
};

#endif // CUSTOMER_H
