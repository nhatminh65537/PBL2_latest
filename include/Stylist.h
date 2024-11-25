//
// Created by LONG on 11/8/2024.
//
#ifndef Stylist_h
#define Stylist_h
#include <define.h>
#include "Member.h"
#include "Appointment.h"
#include "define.h"

class Stylist : public Member {
public:
    Stylist(const std::string& ID="null",
            const std::string& firstName="null",
            const std::string& lastName="null",
            const bool& gender=false,const int& age=0,
            const std::string& phoneNumber="null",
            const std::string& username="null",
            const std::string& password="null");
    ~Stylist() override;
    bool Login(const std::string&,const std::string&) const override;
    bool Logout() const override;

    void ViewSchedule() const;
    void ViewScheduleByDay(const Datetime& dt=Datetime::Now()) const;

    friend std::ostream& operator<<(std::ostream&,const Stylist&);
    friend std::istream& operator>>(std::istream&,Stylist&);

    std::vector<Appointment> schedule;
    private:

};
#endif