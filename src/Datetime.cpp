#include "Datetime.h"

#include <cassert>

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
    if (!IsBetween(this->minute,0,59)) return false;
    if (!IsBetween(this->hour,0,23)) return false;
    if (!IsBetween(this->day,1,this->MonthDays())) return false;
    if (!IsBetween(this->month,1,12)) return false;
    return true;
}

bool Datetime::isValidAppointment() const { // Unfinished
    if(!this->isValidTime()) return false;
    return true;
}

void Datetime::Show() const {
    const char* format = "%02d:%02d %d/%d/%d\n";
    printf(format,this->hour,this->minute,this->day,this->month,this->year);
}

void Datetime::NextDay() {
    this->day = (this->day % this->MonthDays()) + 1;
    if (this->day==1) {
        this->month = (this->month % 12) + 1;
        if(this->month==1) this->year++;
    }
}

void Datetime::PrevDay() {
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
    if (days<0) for (int i=1;i<=-1*days;++i) res.PrevDay();
    else for (int i=1;i<=days;++i) res.NextDay();
    return res;
}

Datetime Datetime::operator-(const int& days) const {
    Datetime res = *this;
    if (days<0) for (int i=1;i<=-1*days;++i) res.NextDay();
    else for (int i=1;i<=days;++i) res.PrevDay();
    return res;
}

Datetime Datetime::Now() {
    const time_t now = time(nullptr);
    const tm* t = localtime(&now);
    return Datetime(t->tm_min,t->tm_hour,t->tm_mday,t->tm_mon+1,t->tm_year+1900);
}

istream& operator>>(istream& is,Datetime& dt) {
    string str;
    is >> str;
    if(str.empty()) return is;
    vector<string> tokens = Split(str,'/'); // minmin/hourhour/dd/mm/yyyy
    if (tokens.size()<5) throw runtime_error("Datetime error");
    dt = Datetime(ToNum(tokens[0]),ToNum(tokens[1]),ToNum(tokens[2]),ToNum(tokens[3]),ToNum(tokens[4]));
    return is;
}

ostream& operator<<(ostream& os,const Datetime& dt) {
    os << dt.minute << '/' << dt.hour << '/' << dt.day << '/' << dt.month << '/' << dt.year;
    return os;
}

bool IsSameYear(const Datetime& a, const Datetime& b) {
    return a.year == b.year;
}

bool IsSameMonth(const Datetime& a, const Datetime& b) {
    return a.month == b.month && IsSameYear(a, b);
}

bool IsSameDay(const Datetime& a, const Datetime& b) {
    return a.day == b.day && IsSameMonth(a, b);
}

tm Datetime::ToTm(const Datetime &dt) {
    std::tm timeStruct = {};
    timeStruct.tm_min = dt.GetMinute();
    timeStruct.tm_hour = dt.GetHour();
    timeStruct.tm_mday = dt.GetDay();
    timeStruct.tm_mon = dt.GetMonth() - 1; // Tháng trong std::tm tính từ 0 (0 = January)
    timeStruct.tm_year = dt.GetYear() - 1900; // Năm trong std::tm tính từ 1900
    return timeStruct;
}

bool IsBetween(const Datetime& dt, const Datetime& start, const Datetime& end) {
    tm tmStart = Datetime::ToTm(start);
    tm tmEnd = Datetime::ToTm(end);
    tm tmCheck = Datetime::ToTm(dt);

    time_t timeStart = std::mktime(&tmStart);
    time_t timeEnd = std::mktime(&tmEnd);
    time_t timeCheck = std::mktime(&tmCheck);

    if (timeEnd < timeStart) throw invalid_argument("End time must be greater than start time");

    return timeCheck >= timeStart && timeCheck <= timeEnd;
}







