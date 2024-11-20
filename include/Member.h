#ifndef MEMBER_H
#define MEMBER_H
#include <string>
#include "Identifiable.h"

using namespace std;

class Member : public Identifiable {
    public:
        explicit Member(const string& ID="null",
            const string& firstName="null",
            const string& lastName="null",
            const bool& gender=false,const int& age=0,
            const string& phoneNumber="null",
            const string& username="null",
            const string& password="null");

        ~Member() override;

        // [[nodiscard]] để cảnh báo cho compiler khi ta không sử dụng value được trả về từ getter, hoặc không lưu lại = 1 biến khác
        [[nodiscard]] string GetFirstName() const;
        void SetFirstName(const string& val);
        [[nodiscard]] string GetLastName() const;
        void SetLastName(const string& val);
        [[nodiscard]] string GetFullName() const;
        [[nodiscard]] bool GetGender() const;
        void SetGender(const bool& val);
        [[nodiscard]] int GetAge() const;
        void SetAge(const int& val);
        [[nodiscard]] string GetPhoneNumber() const;
        void SetPhoneNumber(const string& val);
        [[nodiscard]] string GetUserName() const;
        void SetUserName(const string& val);
        [[nodiscard]] string GetPassword() const;
        void SetPassword(const string& val);

        [[nodiscard]] virtual bool Login(const string&,const string&) const = 0;
        [[nodiscard]] virtual bool Logout() const = 0;
        void Show() const override;

        friend ostream& operator<<(ostream& os, const Member& obj);
        friend istream& operator>>(istream& is,Member& obj);

    protected:
        string firstName;
        string lastName;
        string username;
        string password;
        bool gender;
        int age;
        string phoneNumber;

};

#endif // MEMBER_H
