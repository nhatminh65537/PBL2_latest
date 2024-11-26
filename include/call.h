#ifndef CALL_H
#define CALL_H

#define SERVICES_COUNT 7
#define STYLISTS_COUNT 5 // exclude "None"
#define REVERSE_NAME true

#include <string>
#include <vector>
#include <array>
#include "Member.h"

extern Member* user;

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
// Login and Register
void callLogin(std::string username, std::string password, int role);
void callRegister(std:: string firstname, std::string lastname, std::string username, std::string password, std::string confirmpassword, std::string phonenumber, std::string age, int gender, int role);

// Auxiliary functions
int callGetServiceList(std::vector<std::string>& services);  // return the number of services
std::string makeName(std::string firstname, std::string lastname, bool reverse = REVERSE_NAME);

// Current user call (Customer)
std::string callGetCurrentUserID();
std::string callGetCurrentUserName();
std::string callGetCurrentUserUsername();
void callGetCurrentUserName(std::string& firstname, std::string& lastname);
void callGetCurrentUserPersonInfo(std::string& phonenumber, std ::string& age, int& gender);
std::vector<std::string> callGetCurrentUserAppointmentIDList(int day, int month, int year, bool services[], int status);
void callUpdateCurrentUserName(std::string firstname, std::string lastname);
void callUpdateCurrentPassword(std::string oldPassword, std::string newPassword, std::string confirmpassword);
void callUpdateCurrentPersonInfo(std::string phonenumber, std::string age, int gender);

// Appointment call and auxiliary
void callCreateNewAppointment(bool services[], int selectedStylist, int selectedDay, int selectedMonth, int selectedYear, int selectedHour, int selectedMinute, std::string requirement);
std::string callGetNewAppointmentId();
void callAddNewAppointment();

std::string callGetAppointmentCustomerIDByID(std::string id);
std::string callGetAppointmentCustomerNameByID(std::string id);
std::string callGetAppointmentStatusByID(std::string id);
std::string callGetAppointmentDateByID(std::string id);
std::string callGetAppointmentTimeByID(std::string id);
std::vector<std::string> callGetAppointmentServicesByID(std::string id);
std::string callGetAppointmentStylistByID(std::string id);
std::string callGetAppointmentStylistIDByID(std::string id);
std::string callGetAppointmentRequirementByID(std::string id);

std::vector<std::string> callGetApointmentIDList(int day, int month, int year, int hour, int minute, bool services[], int status, std::string customerID, int& count);
void callCancelAppointment(std::string id);

// Member call and auxiliary
std::string callGetMemberNameByID(std::string id);
std::string callGetMemberFirstNameByID(std::string id);
std::string callGetMemberLastNameByID(std::string id);
std::string callGetMemberPhoneByID(std::string id);
std::string callGetMemberAgeByID(std::string id);
std::string callGetMemberGenderByID(std::string id);
std::string callGetMemberUsernameByID(std::string id);

// Stylist call and auxiliary
std::vector<std::string> callGetStylistIDList(bool gender[2], std::string name, std::string age, int& count);
std::vector<std::string> callGetStylistIDList();
void callDeleteStylist(std::string id);
void callUpdateStylist (std::string id, std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password);
void callAddStylist(std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password);

// Customer call and auxiliary
std::vector<std::string> callGetCustomerIDList(bool gender[2], std::string name, std::string age, int& count);
void callDeleteCustomer(std::string id);


#endif // CALL_H