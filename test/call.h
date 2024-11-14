#ifndef CALL_H
#define CALL_H

#include <string>
#include <vector>

namespace ERROR_CODE
{
    const int LOGIN_USERNAME_EMPTY = 101;
    const int LOGIN_PASSWORD_EMPTY = 102;
    const int LOGIN_USER_AND_PASS_EMPTY = 103;
    const int LOGIN_INCORRECT_INPUT = 104;

    const int REGISTER_FIRSTNAME_EMPTY = 201;
}

void call_login(std::string username, std::string password, int role);
void call_register(std:: string firstname, std::string lastname, std::string username, std::string password, std::string confirmpassword, std::string phonenum, std::string age, int gender, int role);
// Service
std::vector<std::string> get_stylist();
#endif // CALL_H