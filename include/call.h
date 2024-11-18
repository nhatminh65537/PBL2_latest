#ifndef CALL_H
#define CALL_H

#define SERVICES_COUNT 7

#include <string>
#include <vector>
#include <array>

namespace ERROR_CODE
{
    const int LOGIN_USERNAME_EMPTY = 101;
    const int LOGIN_PASSWORD_EMPTY = 102;
    const int LOGIN_USER_AND_PASS_EMPTY = 103;
    const int LOGIN_INCORRECT_INPUT = 104;
    
    const int REGISTER_SOME_FILED_EMPTY = 201;
    const int REGISTER_PASSWORD_NOT_MATCH = 202;
    const int REGISTER_USERNAME_EXIST = 203;
}

void callLogin(std::string username, std::string password, int role);
void callRegister(std:: string firstname, std::string lastname, std::string username, std::string password, std::string confirmpassword, std::string phonenumber, std::string age, int gender, int role);

void callCreateAppointment(std::array<bool, SERVICES_COUNT> states, std::array<int, SERVICES_COUNT> selectedStylist, int selectedDay, int selectedMonth, int selectedYear, int selectedHour, int selectedMinute, std::string requirement);
void callAddAppointment();

std::vector<std::string> callGetStylist(int service);
#endif // CALL_H