#ifndef DATETIME_H
#define DATETIME_H

#include <ctime>
#include <iostream>

class Datetime {
public:
    explicit Datetime(const int& = 1, const int& = 1, const int& = 1, const int& = 1, const int& = 1900);
    ~Datetime();
    void SetMinute(const int&);
    void SetHour(const int&);
    void SetDay(const int&);
    void SetMonth(const int&);
    void SetYear(const int&);
    [[nodiscard]]int GetMinute() const;
    [[nodiscard]]int GetHour() const;
    [[nodiscard]]int GetDay() const;
    [[nodiscard]]int GetMonth() const;
    [[nodiscard]]int GetYear() const;
    [[nodiscard]]bool isLeapYear() const;
    [[nodiscard]]bool isValidTime() const;
    [[nodiscard]]bool isValidAppointment() const;
    [[nodiscard]]int MonthDays() const;
    void Show() const;
    void NextDay();
    void PrevDay();
    Datetime operator+(const int& days) const;
    Datetime operator-(const int& days) const;

    friend std::istream& operator>>(std::istream&, Datetime&);
    friend std::ostream& operator<<(std::ostream&, const Datetime&);
    friend bool IsSameDay(const Datetime&, const Datetime&);
    friend bool IsSameMonth(const Datetime&, const Datetime&);
    friend bool IsSameYear(const Datetime&, const Datetime&);
    friend bool IsBetween(const Datetime&,const Datetime&,const Datetime&);

    static std::string TimeToString(const Datetime&);
    static Datetime StringToTime(const std::string&); // Convert string with format minute/hour/dd/mm/yyyy to Datetime
    static Datetime Now();
private:
    int minute, hour, day, month, year;
    static tm ToTm(const Datetime& dt);
};

#endif // DATETIME_H