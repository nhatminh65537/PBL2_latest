#include <ctime>
class Datetime {
public:
    explicit Datetime(const int& = 20, const int& = 4, const int& = 1, const int& = 1, const int& = 1900);
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
    static Datetime Now();
private:
    int minute, hour, day, month, year;
};
