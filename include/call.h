#ifndef CALL_H
#define CALL_H

#define SERVICES_COUNT 7
#define STYLISTS_COUNT 5 // exclude "None"
#define REVERSE_NAME true

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

std::string makeName(std::string firstname, std::string lastname, bool reverse = REVERSE_NAME);
void breakName(std::string name, std::string& firstname, std::string& lastname, bool reverse = REVERSE_NAME);

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

std::vector<std::string> callGetCurrentUserHistoryList(int day, int month, int year, std::array<bool, SERVICES_COUNT> services, int status);

void callGetAppointmentDetailByID(std::string id, std::string& name, std::string& status, std::string& date, std::string& time, std::vector<std::string>& services, 
std::vector<std::string>& ,std::string& requirement);
void callCancelAppointment(std::string id);

std::vector<std::string> callGetApointmentHistoryList(int day, int month, int year, int hour, int minute, std::array<bool, SERVICES_COUNT> services, int status, std::string customerID, int& containerAppointmentHistoryList);

std::vector<std::string> callGetStylistList(std::array<bool, SERVICES_COUNT> services, int gender, std::string name, std::string age, int& count);
void callGetStylistDetailByID(std::string id, std::string& name, std::string& gender, std::string& age, std::string& phone, std::vector<std::string>& service, std::string& username, std::string& password);
void callDeleteStylist(std::string id);

void callUpdateStylist (std::string id, std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password, std::array<bool, SERVICES_COUNT>);

void callAddStylist(std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password, std::array<bool, SERVICES_COUNT> services);
#endif // CALL_H