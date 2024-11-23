#ifndef MEMBER_H
#define MEMBER_H
#include <string>
#include "Identifiable.h"


class Member : public Identifiable {
    public:
        explicit Member(const std::string& ID="null",
            const std::string& firstName="null",
            const std::string& lastName="null",
            const bool& gender=false,const int& age=0,
            const std::string& phoneNumber="null",
            const std::string& username="null",
            const std::string& password="null");

        ~Member() override;

        // [[nodiscard]] để cảnh báo cho compiler khi ta không sử dụng value được trả về từ getter, hoặc không lưu lại = 1 biến khác
        [[nodiscard]] std::string GetFirstName() const;
        void SetFirstName(const std::string& val);
        [[nodiscard]] std::string GetLastName() const;
        void SetLastName(const std::string& val);
        [[nodiscard]] std::string GetFullName() const;
        [[nodiscard]] bool GetGender() const;
        void SetGender(const bool& val);
        [[nodiscard]] int GetAge() const;
        void SetAge(const int& val);
        [[nodiscard]] std::string GetPhoneNumber() const;
        void SetPhoneNumber(const std::string& val);
        [[nodiscard]] std::string GetUserName() const;
        void SetUserName(const std::string& val);
        [[nodiscard]] std::string GetPassword() const;
        void SetPassword(const std::string& val);

        [[nodiscard]] virtual bool Login(const std::string&,const std::string&) const = 0;
        [[nodiscard]] virtual bool Logout() const = 0;
        void Show() const override;

        friend std::ostream& operator<<(std::ostream& os, const Member& obj);
        friend std::istream& operator>>(std::istream& is,Member& obj);

    protected:
        std::string firstName;
        std::string lastName;
        std::string username;
        std::string password;
        bool gender;
        int age;
        std::string phoneNumber;

};

#endif // MEMBER_H
