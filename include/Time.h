class Time {
    private:
        int minute, hour, day, month, year;
    public:
        explicit Time(const int& = 0, const int& = 0, const int& = 0, const int& = 0, const int& = 0);
        ~Time();
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
        void Now() const;
};
