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
    
    const int REGISTER_SOME_FIELD_EMPTY = 150;
    const int REGISTER_PASSWORD_NOT_MATCH = 151;
    const int REGISTER_USERNAME_EXIST = 152;

    const int CREATE_APPOINTMENT_BUSY_TIME = 201;
    const int CREATE_APPOINTMENT_CLOSED_TIME = 202;
    const int CREATE_APPOINTMENT_SERVICES_EMPTY = 203;

    const int UPDATE_PROFILE_NAME_EMPTY = 249;
    const int UPDATE_PROFILE_FIRSTNAME_EMPTY = 250;
    const int UPDATE_PROFILE_LASTNAME_EMPTY = 251;
    const int UPDATE_PROFILE_INVALID_PASSWORD = 252;
    const int UPDATE_PROFILE_PASSWORD_NOT_MATCH = 253;
    const int UPDATE_PROFILE_PERSONINFO_EMPTY = 254;
}

void callLogin(std::string username, std::string password, int role);
void callRegister(std:: string firstname, std::string lastname, std::string username, std::string password, std::string confirmpassword, std::string phonenumber, std::string age, int gender, int role);

void callCreateNewAppointment(std::array<bool, SERVICES_COUNT> states, std::array<int, SERVICES_COUNT> selectedStylist, int selectedDay, int selectedMonth, int selectedYear, int selectedHour, int selectedMinute, std::string requirement);
std::string callGetNewAppointmentId();
void callAddNewAppointment();

std::vector<std::string> callGetStylist(int service);

std::string callGetCurrentUserName();
void callGetCurrentUserName(std::string& firstname, std::string& lastname);
void callGetCurrentUserPersonInfo(std::string& phonenumber, std ::string& age, int& gender);
std::string callGetCurrentUserUsername();
void callUpdateCurrentUserName(std::string firstname, std::string lastname);
void callUpdateCurrentPassword(std::string oldPassword, std::string newPassword, std::string confirmpassword);
void callUpdateCurrentPersonInfo(std::string phonenumber, std::string age, int gender);
#endif // CALL_H