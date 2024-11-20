//
// Created by LONG on 11/8/2024.
//
#include <define.h>
#include "Member.h"
#include "Appointment.h"
#include "define.h"

using namespace std;

class Stylist : public Member {
public:
    Stylist(const string& ID="null",
            const string& firstName="null",
            const string& lastName="null",
            const bool& gender=false,const int& age=0,
            const string& phoneNumber="null",
            const string& username="null",
            const string& password="null");
    ~Stylist() override;
    bool Login(const string&,const string&) const override;
    bool Logout() const override;

    void ViewSchedule() const;
    void ViewScheduleByDay(const Datetime& dt=Datetime::Now()) const;

    friend ostream& operator<<(ostream&,const Stylist&);
    friend istream& operator>>(istream&,Stylist&);

    vector<Appointment> schedule;
    private:

};
