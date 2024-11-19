#include "Datetime.h"
#include "define.h"

#include <stdexcept>
#include <iostream>

Datetime::Datetime(const int& minute, const int& hour, const int& day, const int& month, const int& year)
    : minute(minute),hour(hour),day(day),month(month),year(year){
    if (!this->isValidTime()) {
        throw new std::invalid_argument("Invalid time");
    }
}

Datetime::~Datetime() {

}

void Datetime::SetMinute(const int& minute) {
    this->minute = minute;
}

void Datetime::SetHour(const int& hour) {
    this->hour = hour;
}

void Datetime::SetDay(const int& day) {
    this->day = day;
}
void Datetime::SetMonth(const int& month) {
    this->month = month;
}
void Datetime::SetYear(const int& year) {
    this->year = year;
}
int Datetime::GetMinute() const {
    return this->minute;
}
int Datetime::GetHour() const {
    return this->hour;
}
int Datetime::GetDay() const {
    return this->day;
}
int Datetime::GetMonth() const {
    return this->month;
}
int Datetime::GetYear() const {
    return this->year;
}
bool Datetime::isLeapYear() const {
    return (this->year % 4 == 0 && this->year % 100 != 0) || this->year % 400 == 0;
}

int Datetime::MonthDays() const {
    int daysInMonth[] = {0, 31, 28, 31,30,31,30,31,31,30,31,30,31};
    if (this->isLeapYear())  daysInMonth[2] = 29;
    return daysInMonth[this->month];
}

bool Datetime::isValidTime() const {
    if (!isBetween(this->minute,0,59)) return false;
    if (!isBetween(this->hour,0,23)) return false;
    if (!isBetween(this->day,1,this->MonthDays())) return false;
    if (!isBetween(this->month,1,12)) return false;
    return true;
}

bool Datetime::isValidAppointment() const {
    if(!this->isValidTime()) return false;
    return true;
}

void Datetime::Show() const {
    const char* format = "%02d:%02d %d/%d/%d\n";
    printf(format,this->hour,this->minute,this->day,this->month,this->year);
}

void Datetime::nextDay() {
    this->day = (this->day % this->MonthDays()) + 1;
    if (this->day==1) {
        this->month = (this->month % 12) + 1;
        if(this->month==1) this->year++;
    }
}

void Datetime::prevDay() {
    if(this->day==1) {
        if (this->month==1) {
            this->year--;
            this->month=12;
        }
        else this->month--;
        this->day = this->MonthDays();
    }
    else this->day--;
}

Datetime Datetime::operator+(const int& days) const {
    Datetime res = *this;
    if (days<0) for (int i=1;i<=-1*days;++i) res.prevDay();
    else for (int i=1;i<=days;++i) res.nextDay();
    return res;
}

Datetime Datetime::operator-(const int& days) const {
    Datetime res = *this;
    if (days<0) for (int i=1;i<=-1*days;++i) res.nextDay();
    else for (int i=1;i<=days;++i) res.prevDay();
    return res;
}

Datetime Datetime::Now() {
    const time_t now = time(nullptr);
    const tm* t = localtime(&now);
    return Datetime(t->tm_min,t->tm_hour,t->tm_mday,t->tm_mon+1,t->tm_year+1900);
}


