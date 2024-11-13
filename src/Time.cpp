#include "Time.h"
#include "define.h"
#include <stdexcept>

Time::Time(const int& minute, const int& hour, const int& day, const int& month, const int& year)
    : minute(minute),hour(hour),day(day),month(month),year(year){
    if (!this->isValidTime()) {
        throw new std::invalid_argument("Invalid time");
    }
}

Time::~Time() {

}

void Time::SetMinute(const int& minute) {
    this->minute = minute;
}

void Time::SetHour(const int& hour) {
    this->hour = hour;
}

void Time::SetDay(const int& day) {
    this->day = day;
}
void Time::SetMonth(const int& month) {
    this->month = month;
}
void Time::SetYear(const int& year) {
    this->year = year;
}
int Time::GetMinute() const {
    return this->minute;
}
int Time::GetHour() const {
    return this->hour;
}
int Time::GetDay() const {
    return this->day;
}
int Time::GetMonth() const {
    return this->month;
}
int Time::GetYear() const {
    return this->year;
}
bool Time::isLeapYear() const {
    return (this->year % 4 == 0 && this->year % 100 != 0) || this->year % 400 == 0;
}

int Time::MonthDays() const {
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 30, 31, 30, 31, 30, 31, 30, 31};
    if (this->isLeapYear())  daysInMonth[2] = 29;
    return daysInMonth[this->month];
}

bool Time::isValidTime() const {
    if (!isBetween(this->minute,0,59)) return false;
    if (!isBetween(this->hour,0,23)) return false;
    if (!isBetween(this->day,1,this->MonthDays())) return false;
    if (!isBetween(this->month,1,12)) return false;
    return true;
}

bool Time::isValidAppointment() const {
    if(!this->isValidTime()) return false;
}

void Time::Now() const {

}

