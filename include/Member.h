#ifndef MEMBER_H
#define MEMBER_H
#include <string>
#include "Identifiable.h"


class Member : public Identifiable {
    public:
        explicit Member(const std::string& ID="null",
            const std::string& firstName="null",
            const std::string& lastName="null",
            const std::string& username="null",
            const std::string& password="null",
            const bool& gender=false,
            const std::string& phoneNumber="null",
            const int& role = 0);

        ~Member() override;

        // [[nodiscard]] để cảnh báo cho compiler khi ta không sử dụng value được trả về từ getter, hoặc không lưu lại = 1 biến khác
        [[nodiscard]] std::string GetFirstName() const;
        void SetFirstName(const std::string& val);
        [[nodiscard]] std::string GetLastName() const;
        void SetLastName(const std::string& val);
        [[nodiscard]] std::string GetFullName() const;
        [[nodiscard]] bool GetGender() const;
        void SetGender(const bool& val);
        [[nodiscard]] std::string GetPhoneNumber() const;
        void SetPhoneNumber(const std::string& val);
        [[nodiscard]] std::string GetUserName() const;
        void SetUserName(const std::string& val);
        [[nodiscard]] std::string GetPassword() const;
        void SetPassword(const std::string& val);
        [[nodiscard]] int GetRole() const;
        void SetRole(const int&);
        void Show() const override;

        friend std::ostream& operator<<(std::ostream& os, const Member& obj);
        friend std::istream& operator>>(std::istream& is,Member& obj);

    protected:
        std::string firstName;
        std::string lastName;
        std::string username;
        std::string password;
        bool gender;
        std::string phoneNumber;
        int role; // 1 -> customer. 2 -> stylist. 3 -> admin. Other -> unknown
};

#endif // MEMBER_H
