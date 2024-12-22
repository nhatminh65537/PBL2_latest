#ifndef CALL_H
#define CALL_H

#define STYLISTS_COUNT 5
#define REVERSE_NAME true

#include <string>
#include <vector>
#include <array>
#include "define.h"

// Login and Register
int callLogin(std::string username, std::string password);
void callRegister(std:: string firstname, std::string lastname, std::string username, std::string password, std::string confirmpassword, std::string phonenumber, int gender, int role);

// Auxiliary functions
int callGetServiceList(std::vector<std::string>& services);  // return the number of services
std::string makeName(std::string firstname, std::string lastname, bool reverse = REVERSE_NAME);
std::string callCheckStylistBusy(std::string stylistID, int day, int month, int year, int hour, int minute);
std::string callCheckStylistBusy(std::string stylistID, std::string appointmentID);


// Current user call (Customer)
std::string callGetCurrentUserID();
std::string callGetCurrentUserName();
std::string callGetCurrentUserUsername();
void callGetCurrentUserName(std::string& firstname, std::string& lastname);
void callGetCurrentUserPersonInfo(std::string& phonenumber, int& gender);
std::vector<std::string> callGetCurrentUserAppointmentIDList(int day, int month, int year, bool services[], int status);
void callUpdateCurrentUserName(std::string firstname, std::string lastname);
void callUpdateCurrentPassword(std::string oldPassword, std::string newPassword, std::string confirmpassword);
void callUpdateCurrentPersonInfo(std::string phonenumber, int gender);

// Appointment call and auxiliary
void callCreateNewAppointment(bool services[], std::string selectedStylistID, int selectedDay, int selectedMonth, int selectedYear, int selectedHour, int selectedMinute);
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

std::vector<std::string> callGetApointmentIDList(int day, int month, int year, int hour, int minute, bool services[], int status, std::string customerID, std::string stylistID, int& count);
void callCancelAppointment(std::string id);
void callDoneAppointment(std::string id);
void callAssignStylistToAppointment(std::string appointmentID, std::string stylistID);

// Member call and auxiliary
std::string callGetMemberNameByID(std::string id);
std::string callGetMemberFirstNameByID(std::string id);
std::string callGetMemberLastNameByID(std::string id);
std::string callGetMemberPhoneByID(std::string id);
std::string callGetMemberGenderByID(std::string id);
std::string callGetMemberUsernameByID(std::string id);

// Stylist call and auxiliary
std::vector<std::string> callGetStylistIDList(bool gender[2], std::string name, int& count);
std::vector<std::string> callGetStylistIDList();
void callDeleteStylist(std::string id);
void callUpdateStylist (std::string id, std::string firstname, std::string lastname, int gender, std::string phonenumber, std::string username, std::string password);
void callAddStylist(std::string firstname, std::string lastname, int gender, std::string phonenumber, std::string username, std::string password);

// Customer call and auxiliary
std::vector<std::string> callGetCustomerIDList(bool gender[2], std::string name, int& count);
void callDeleteCustomer(std::string id);

// Service done call and auxiliary
std::vector<std::string> callGetServiceDoneIDList(int day, int month, int year, std::string customerID, std::string stylistID, bool rating[], bool status[], bool services[], int& count);
std::string callGetServiceDoneCustomerIDByID(std::string id);
std::string callGetServiceDoneStylistIDByID(std::string id);
std::string callGetServiceDoneRatingByID(std::string id);
std::string callGetServiceDoneDateByID(std::string id);
std::string callGetServiceDoneStatusByID(std::string id);
std::string callGetServiceDoneServiceByID(std::string id);

void callRateServiceDone(std::string id, int rating);

// Statistics call and auxiliary
float callGetAverage(const std::vector<int>&);
float callGetStandardDeviation(const std::vector<int>&);
std::vector<int> callGetCustomerCountStatistics(int day, int month, int year);

std::vector<int> callGetServiceCustomerCountStatistics(int day, int month, int year);
std::vector<float> callGetServiceFrequencyStatistics(int day, int month, int year);
std::vector<int> callGetServiceRateCountStatistics(int day, int month, int year);
std::vector<float> callGetServiceRateAverageStatistics(int day, int month, int year);

int callGetStylistCustomerCountStatistics(std::string stylistID, int day, int month, int year);
float callGetStylistRateAverageStatistics(std::string stylistID, int day, int month, int year);

std::vector<int> callGetStylistServiceCustomerCountStatistics(std::string stylistID, int day, int month, int year);
std::vector<float> callGetStylistServiceRateAverageStatistics(std::string stylistID, int day, int month, int year);

#endif // CALL_H