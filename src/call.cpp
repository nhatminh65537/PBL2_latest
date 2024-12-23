#include "call.h"
#include "Salon.h"
#include "Database.h"
#include "Appointment.h"
#include "Datetime.h"
#include "algorithm"

// Login and Register

int callLogin(std::string username, std::string password) // Done
{
    Salon& salon = Salon::StartUp();
    if (!salon.Login(username, password))
        throw ERROR_CODE::LOGIN_INCORRECT_INPUT;
    return salon.GetUserRole();
}

void callRegister(std:: string firstname, std::string lastname, std::string username, std::string password, std::string confirmpassword, std::string phonenumber, int gender, int role) // Done
{
    Salon& salon = Salon::StartUp();
    bool isMale = (gender == 0);
    salon.Register(firstname, lastname, username, password, confirmpassword, isMale, phonenumber, 1);
}


// Auxiliary functions

int callGetServiceList(std::vector<std::string>& services) // Done
{
    for (int i = 1; i < SERVICES_COUNT; i++)
        services.push_back(ServiceToString(i));
    return SERVICES_COUNT - 1;
}

std::string makeName(std::string firstname, std::string lastname, bool reverse)
{
    if (reverse)
        return lastname + " " + firstname;
    return firstname + " " + lastname;
}

std::string callCheckStylistBusy(std::string stylistID, int day, int month, int year, int hour, int minute) // Done
{
    // change to database data
    int dataYear = year + 2021;
    int dataMonth = month + 1;
    int dataDay = day + 1;
    int dataHour = hour;
    int dataMinute = minute*30;

    // check if the stylist is busy at this time
    std::string busyStatus = "";
    const Datetime dt(dataMinute, dataHour, dataDay, dataMonth, dataYear);
    if (stylistID == "null") {
        int numberOfAppointments = dbAppointment.Query("time", Datetime::TimeToString(dt)).GetResults().size();
        int numberOfStylist = dbUser.Query("role","2").GetResults().size();
        if (numberOfAppointments >= numberOfStylist) {
            busyStatus = "All stylist is busy at this time";
        }
    }
    else {
        std::cerr << stylistID << " " << busyStatus << ' ' << dt << '\n';
        if (dbAppointment.Query("stylistID", stylistID).Query("time",Datetime::TimeToString(dt)).GetResults().size() > 0) {
            busyStatus = "This stylist is busy at this time";
        }
    }
    // check if the time is out of work time
    Datetime beginWork(0, 7, dataDay, dataMonth, dataYear);
    Datetime endWork(59, 18, dataDay, dataMonth, dataYear);
    Datetime beginLunch(0, 12, dataDay, dataMonth, dataYear);
    Datetime endLunch(59, 12, dataDay, dataMonth, dataYear);
    if (!IsBetween(dt, beginWork, endWork) || IsBetween(dt, beginLunch, endLunch))
        busyStatus = "This time is out of work time";
    Datetime AppointmentTime(dataMinute,dataHour,dataDay,dataMonth,dataYear);
    if (!AppointmentTime.isValidAppointment())
        busyStatus = "Invalid time";
    // check time in the past

    return busyStatus;
}

std::string callCheckStylistBusy(std::string stylistID, std::string appointmentID) // Done
{
    std::string busyStatus = "";
    if (stylistID == "null")
        return busyStatus;
    Datetime dt = dbAppointment.Get(appointmentID).GetStartTime();
    return callCheckStylistBusy(stylistID, dt.GetDay() - 1, dt.GetMonth() - 1, dt.GetYear() - 2021, dt.GetHour(), dt.GetMinute() / 30);
}

// Current user call (Customer)

std::vector<std::string> callGetCurrentUserAppointmentIDList(int day, int month, int year, bool services[], int status) // Done
{
    /*
    day = 0 => all day
    month = 0 => all month
    year = 0 => all year
    day = x (not 0)=> Day x in month
    month = x (not 0)=> month x
    year = x (not 0) => 2020 + x
    services[0] = true => Service 0 (in enum) is selected
    status = 0 => all status
    status = 1 => done
    status = 2 => pending
    status = 3 => cancel
    */
    // Handle type
    std::cerr << "callGetCurrentUserAppointmentIDList\n";
    // change to database data
    std::vector<Service> serviceList;
    for (int i = 0; i < SERVICES_COUNT - 1; i++)
        if (services[i])
            serviceList.push_back(static_cast<Service> (i + 1));
    std::string dataYear = std::to_string(year + 2020);
    std::string dataMonth = std::to_string(month);
    std::string dataDay = std::to_string(day);
    std::string dataStatus = (status == 1? "Done": (status == 2? "Waiting": "Cancel"));

    // filter by day, month, year
    std::vector<std::string> appointmentIDList;
    if (day > 0) {
        dbAppointment.Query("day", dataDay);
    }
    if (month > 0) {
        dbAppointment.Query("month", dataMonth);
    }
    if (year > 0) {
        dbAppointment.Query("year", dataYear);
    }

    // std::cerr << "  " << day << " " << month << " " << year << '\n';
    // if (day > 0 && month > 0 && year > 0)
    // {
    //     dbAppointment.Query("startTime", "0/0/" + dataDay + "/" + dataMonth + "/" + dataYear);
    // }

    // filter by customerID
    dbAppointment.Query("customerID", callGetCurrentUserID());

    // filter by status
    if (status > 0) {
        dbAppointment.Query("status", dataStatus);
    }

    std::vector<Appointment> appointmentList = dbAppointment.GetResults();

    // filter by services
    for (auto appoint: appointmentList) {
        std::vector<Service> appointServiceList = appoint.GetServices(), temp;
        std::set_intersection(serviceList.begin(), serviceList.end(), appointServiceList.begin(), appointServiceList.end(), std::back_inserter(temp));
        if (temp.size() > 0)
            appointmentIDList.push_back(appoint.GetID());
    }
    std::cerr << "End callGetCurrentUserAppointmentIDList\n";
    return appointmentIDList;
}

std::string callGetCurrentUserUsername() // Done
{
    return callGetMemberUsernameByID(callGetCurrentUserID());
}

std::string callGetCurrentUserName() // Done
{
    return callGetMemberNameByID(callGetCurrentUserID());
}

std::string callGetCurrentUserID() // Done
{
    Salon& salon = Salon::StartUp();
    return salon.GetUserID();
}

void callGetCurrentUserName(std::string& firstname, std::string& lastname) // Done
{
    firstname = callGetMemberFirstNameByID(callGetCurrentUserID());
    lastname = callGetMemberLastNameByID(callGetCurrentUserID());
}

void callGetCurrentUserPersonInfo(std::string& phonenumber, int& gender) // Done
{
    phonenumber = callGetMemberPhoneByID(callGetCurrentUserID());
    gender = (callGetMemberGenderByID(callGetCurrentUserID()) == "Male"? 0 : 1);
}

void callUpdateCurrentUserName(std::string firstname, std::string lastname) // Done
{
    std::cerr << "callUpdateCurrentUserName\n";
    if (firstname.empty() && lastname.empty())
        throw ERROR_CODE::UPDATE_PROFILE_NAME_EMPTY;
    if (firstname.empty())
        throw ERROR_CODE::UPDATE_PROFILE_FIRSTNAME_EMPTY;
    if (lastname.empty())
        throw ERROR_CODE::UPDATE_PROFILE_LASTNAME_EMPTY;
    dbUser.Update(callGetCurrentUserID(), "firstName", firstname);
    dbUser.Update(callGetCurrentUserID(), "lastName", lastname);
    std::cerr << "End callUpdateCurrentUserName\n";
}

void callUpdateCurrentPassword(std::string oldPassword, std::string newPassword, std::string confirmpassword) // Done
{
    if (oldPassword != dbUser.Get(callGetCurrentUserID()).GetPassword()) 
        throw ERROR_CODE::UPDATE_PROFILE_INVALID_PASSWORD;
    if (newPassword != confirmpassword)
        throw ERROR_CODE::UPDATE_PROFILE_PASSWORD_NOT_MATCH;
    dbUser.Update(callGetCurrentUserID(), "password", newPassword);
}

void callUpdateCurrentPersonInfo(std::string phonenumber, int gender) // Done
{
    if (phonenumber.empty())
        throw ERROR_CODE::UPDATE_PROFILE_PERSONINFO_EMPTY;
    dbUser.Update(callGetCurrentUserID(), "phoneNumber", phonenumber);
    dbUser.Update(callGetCurrentUserID(), "gender", gender == 0? "Male": "Female");
}


// Appointment call and auxiliary

void callCreateNewAppointment(bool services[], std::string selectedStylistID, int selectedDay, int selectedMonth, int selectedYear, int selectedHour, int selectedMinute) // Done
{
    std::cerr << "callCreateNewAppointment\n";

    std::vector<Service> serviceList;
    for (int i = 0; i < SERVICES_COUNT - 1; i++)
        if (services[i]){
            serviceList.push_back(static_cast<Service> (i + 1));
        }
    int day = selectedDay + 1;
    int month = selectedMonth + 1;
    int year = 2021 + selectedYear;
    int hour = selectedHour;
    int minute = selectedMinute * 30;

    if (selectedStylistID.empty())
        selectedStylistID = "null";
    Salon& salon = Salon::StartUp();
    salon.CreateAppointment(selectedStylistID, Datetime(minute, hour, day, month, year), serviceList);
    std::cerr << "End callCreateNewAppointment\n";
}

std::string callGetNewAppointmentId() // Done
{
    Salon& salon = Salon::StartUp();;
    return salon.GetTempAppointmentID();
}

void callAddNewAppointment() // Done
{
Salon& salon = Salon::StartUp();
salon.AddAppointment();
}

std::string callGetAppointmentCustomerIDByID(std::string id) // Done
{
    return dbAppointment.Get(id).GetCustomerID();
}

std::string callGetAppointmentCustomerNameByID(std::string id) // Done
{
    return dbUser.Get(dbAppointment.Get(id).GetCustomerID()).GetFullName();
}

std::string callGetAppointmentStatusByID(std::string id)
{
    return dbAppointment.Get(id).GetStatus();
}

std::string callGetAppointmentDateByID(std::string id) // Done
{
    Datetime dt = dbAppointment.Get(id).GetStartTime();
    std::string day = std::to_string(dt.GetDay());
    if (day.length() == 1) day = "0" + day;
    std::string month = std::to_string(dt.GetMonth());
    if (month.length() == 1) month = "0" + month;
    std::string year = std::to_string(dt.GetYear());
    return day + "/" + month + "/" + year;
}

std::string callGetAppointmentTimeByID(std::string id) // Done
{
    Datetime dt = dbAppointment.Get(id).GetStartTime();
    std::string hour = std::to_string(dt.GetHour());
    if (hour.length() == 1) hour = "0" + hour;
    std::string minute = std::to_string(dt.GetMinute());
    if (minute.length() == 1) minute = "0" + minute;
    return hour + ":" + minute;
}

std::vector<std::string> callGetAppointmentServicesByID(std::string id) // Done
{
    std::vector<std::string> services;
    for (auto service : dbAppointment.Get(id).GetServices())
        services.push_back(ServiceToString(service));
    return services;
}

std::string callGetAppointmentStylistByID(std::string id) // Done
{
    std::string stylistID = dbAppointment.Get(id).GetStylistID();
    if (stylistID == "null")
        return "null";
    return dbUser.Get(stylistID).GetFullName();
}

std::string callGetAppointmentStylistIDByID(std::string id) // Done
{
    std::string stylistID = dbAppointment.Get(id).GetStylistID();
    return stylistID;
}

std::vector<std::string> callGetApointmentIDList(int day, int month, int year, int hour, int minute, bool services[], int status, std::string customerID, std::string stylistID, int& count)
{
    std::vector<Service> serviceList;
    for (int i = 0; i < SERVICES_COUNT - 1; i++)
        if (services[i])
            serviceList.push_back(static_cast<Service> (i + 1));
    std::string dataYear = std::to_string(year + 2020);
    std::string dataMonth = std::to_string(month);
    std::string dataDay = std::to_string(day);
    std::string dataHour = std::to_string(hour - 1);
    std::string dataMinute = std::to_string((minute - 1) * 30);
    std::string dataStatus = (status == 1? "Done": (status == 2? "Waiting": "Cancel"));

    // filter by day, month, year, hour, minute
    if (minute > 0) {
        dbAppointment.Query("minute", dataMinute);
    }
    if (hour > 0) {
        dbAppointment.Query("hour", dataHour);
    }
    if (day > 0) {
        dbAppointment.Query("day", dataDay);
    }
    if (month > 0) {
        dbAppointment.Query("month", dataMonth);
    }
    if (year > 0) {
        dbAppointment.Query("year", dataYear);
    }
    if (day == -1 && month == -1 && year == -1 && hour == 0 && minute == 0) {
        Datetime now = Datetime::Now();
        dbAppointment.Query("day", std::to_string(now.GetDay()));
        dbAppointment.Query("month", std::to_string(now.GetMonth()));
        dbAppointment.Query("year", std::to_string(now.GetYear()));
    }
    if (day == -1 && month == -1 && year == -1 && hour == -1 && minute == -1) {
        Datetime now = Datetime::Now();
        dbAppointment.Query("day", std::to_string(now.GetDay()));
        dbAppointment.Query("month", std::to_string(now.GetMonth()));
        dbAppointment.Query("year", std::to_string(now.GetYear()));
        if (now.GetMinute() < 30) {
            dbAppointment.Query("hour", std::to_string(now.GetHour()));
            dbAppointment.Query("minute", "30");
        }
        else {
            dbAppointment.Query("hour", std::to_string(now.GetHour() + 1 > 23? 0: now.GetHour() + 1));
            dbAppointment.Query("minute", "0");
        }
    }
    // filter by customerID
    if (!customerID.empty())
    {
        // std::cerr << "  Filter by customerID\n";
        dbAppointment.Query("customerID", customerID);
    }
    // filter by stylistID
    if (!stylistID.empty())
    {
        // std::cerr << "  Filter by stylistID\n";
        dbAppointment.Query("stylistID", stylistID);
    }
    // filter by status
    if (status > 0)
    {
        dbAppointment.Query("status", dataStatus);
    }

    // filter by services
    std::vector<std::string> appointmentIDList;
    std::vector<Appointment> appointmentList = dbAppointment.GetResults();
    std::cerr << "  " << "Appointment list size: " << appointmentList.size() << '\n';
    for (auto appoint : appointmentList)
    {
        std::vector<Service> temp;
        std::set_intersection(serviceList.begin(), serviceList.end(), appoint.GetServices().begin(), appoint.GetServices().end(), back_inserter(temp));
        if (temp.size() > 0)
            appointmentIDList.push_back(appoint.GetID());
        std::cerr << "    " << "Intersection size:" << temp.size() << '\n';
    }
    count = appointmentIDList.size();
    // std::cerr << "End callGetApointmentIDList\n";
    return appointmentIDList;
}

void callCancelAppointment(std::string id)
{
    if (dbAppointment.Get(id).GetStatus() == "Done")
        throw ERROR_CODE::APPOINTMENT_IS_DONE;
    if (dbAppointment.Get(id).GetStatus() == "Cancel")
        return;
    dbAppointment.Update(id, "status", "Cancel");
}

void callDoneAppointment(std::string id) // Done
{   
    if (dbAppointment.Get(id).GetStylistID() == "null")
        throw ERROR_CODE::APPOINTMENT_HAS_NULL_STYLIST;
    if (dbAppointment.Get(id).GetStatus() == "Cancel")
        throw ERROR_CODE::APPOINTMENT_IS_CANCELLED;
    if (dbAppointment.Get(id).GetStatus() == "Done")
        return;
    // set appointment status to done
    dbAppointment.Update(id, "status", "Done");

    // generate service done
    std::vector<Service> serviceList = dbAppointment.Get(id).GetServices();
    for (int i = 0; i < serviceList.size(); i++)
    {
        std::string sid = std::to_string(dbServiceDone.Count());
        sid = std::string(10 - sid.length(), '0') + sid;
        Datetime now = Datetime::Now();
        Datetime dt = dbAppointment.Get(id).GetStartTime();
        ServiceDone newServiceDone(
            sid, 
            dbAppointment.Get(id).GetCustomerID(),
            dbAppointment.Get(id).GetStylistID(),
            serviceList[i],
            0,
            true,
            dt
        );
        dbServiceDone.Insert(newServiceDone);
    }
}

void callAssignStylistToAppointment(std::string appointmentID, std::string stylistID) // Done
{
    dbAppointment.Update(appointmentID, "stylistID", stylistID);
}


// Member call and auxiliary

std::string callGetMemberNameByID(std::string id) // Done
{
    return dbUser.Get(id).GetFullName();
}

std::string callGetMemberFirstNameByID(std::string id) // Done
{
    return  dbUser.Get(id).GetFirstName();
}

std::string callGetMemberLastNameByID(std::string id) // Done
{
    return dbUser.Get(id).GetLastName();
}

std::string callGetMemberPhoneByID(std::string id) // Done
{
    return dbUser.Get(id).GetPhoneNumber();
}

std::string callGetMemberGenderByID(std::string id) // Done
{
    return (dbUser.Get(id).GetGender() == 1 ? "Male" : "Female");
}

std::string callGetMemberUsernameByID(std::string id) // Done
{
    return dbUser.Get(id).GetUserName();
}


// Stylist call and auxiliary

std::vector<std::string> callGetStylistIDList(bool gender[2], std::string name, int& count)
{
    std::vector<std::string> stylistListID;
    // filter by role
    dbUser.Query("role", "2");

    // filter by gender
    if (!gender[0]) {
        dbUser.Query("gender", "Female");
    }
    if (!gender[1]) {
        dbUser.Query("gender", "Male");
    }
    // filter by name
    if (!name.empty()) {
        dbUser.Query("name", name);
    }

    std::vector<Member> stylistList = dbUser.GetResults();
    for (auto stylist : stylistList) {
        stylistListID.push_back(stylist.GetID());
    }
    count = stylistListID.size();
    return stylistListID;
}

std::vector<std::string> callGetStylistIDList() // Done
{
    int countStylist;
    bool temp[2] = {true, true};
    return callGetStylistIDList(temp, "", countStylist);
}

void callUpdateStylist (std::string id, std::string firstname, std::string lastname, int gender, std::string phonenumber, std::string username, std::string password) // Done
{
    dbUser.Update(id, "firstName", firstname);
    dbUser.Update(id, "lastName", lastname);
    dbUser.Update(id, "gender", gender == 0? "Male": "Female");
    dbUser.Update(id, "phoneNumber", phonenumber);
    dbUser.Update(id, "username", username);
    dbUser.Update(id, "password", password);
}

void callDeleteStylist(std::string id) // Done
{
    Salon& salon = Salon::StartUp();
    salon.DeleteStylist(id);           
    // if (id != "null") 
    // {
    //     dbUser.Delete(id);
    // }
}

void callAddStylist(std::string firstname, std::string lastname, int gender, std::string phonenumber, std::string username, std::string password) // Done
{
    Salon& salon = Salon::StartUp();
    bool isMale = (gender == 0);
    salon.Register(firstname, lastname, username, password, password, isMale, phonenumber, 2);
}


// Customer call and auxiliary

std::vector<std::string> callGetCustomerIDList(bool gender[2], std::string name, int& count) // Done
{
    std::vector<std::string> customerListID;
    // filter by role
    dbUser.Query("role", "1");

    // filter by gender
    if (!gender[0]) {
        dbUser.Query("gender", "Female");
    }
    if (!gender[1]) {
        dbUser.Query("gender", "Male");
    }

    // filter by name
    if (!name.empty()) {
        dbUser.Query("name", name);
    }

    std::vector<Member> customerList = dbUser.GetResults();
    for (auto customer : customerList) {
        customerListID.push_back(customer.GetID());
    }
    count = customerListID.size();
    return customerListID;
}

void callDeleteCustomer(std::string id) // Done
{
    Salon& salon = Salon::StartUp();
    salon.DeleteCustomer(id);
}


// Service Done call and auxiliary

std::vector<std::string> callGetServiceDoneIDList(int day, int month, int year, std::string customerID, std::string stylistID, bool rating[], bool status[], bool services[], int& count) // Done
{
    /*
    day = 0 => all day
    month = 0 => all month
    year = 0 => all year
    day = x (not 0) => Day x in month
    month = x (not 0) => month x
    year = x (not 0) => 2020 + x
    rating[0] = true => Rating 0 (in enum) is selected
    status[0] = true => Status 0 (in enum) is selected
    count = number of service done records
    */ 

    // change to database data
    std::string dataYear = std::to_string(year + 2020);
    std::string dataMonth = std::to_string(month);
    std::string dataDay = std::to_string(day);

    // filter by day, month, year
    if (day > 0) {
        dbServiceDone.Query("day", dataDay);
    }
    if (month > 0) {
        dbServiceDone.Query("month", dataMonth);
    }
    if (year > 0) {
        dbServiceDone.Query("year", dataYear);
    }

    // filter by customerID
    if (!customerID.empty())
    {
        dbServiceDone.Query("customerID", customerID);
    }

    // filter by stylistID
    if (!stylistID.empty())
    {
        dbServiceDone.Query("stylistID", stylistID);
    }

    std::vector<ServiceDone> serviceDoneList = dbServiceDone.GetResults();
    // filter by rating, status, services
    std::vector<std::string> serviceDoneIDList;
    for (auto sDone : serviceDoneList)
    {
        if (rating[sDone.GetRating()] 
            && status[sDone.GetBookStatus()? 1: 0] 
            && services[sDone.GetServiceID() - 1])
            serviceDoneIDList.push_back(sDone.GetID());
    }

    count = serviceDoneIDList.size();
    return serviceDoneIDList;
}

std::string callGetServiceDoneCustomerIDByID(std::string id) // Done
{
    return dbServiceDone.Get(id).GetCustomerID();
}

std::string callGetServiceDoneStylistIDByID(std::string id) // Done
{
    return dbServiceDone.Get(id).GetStylistID();
}

std::string callGetServiceDoneRatingByID(std::string id) // Done
{
    return std::to_string(dbServiceDone.Get(id).GetRating());
}

std::string callGetServiceDoneDateByID(std::string id) // Done
{
    Datetime dt = dbServiceDone.Get(id).GetTime();
    std::string day = std::to_string(dt.GetDay());
    if (day.length() == 1) day = "0" + day;
    std::string month = std::to_string(dt.GetMonth());
    if (month.length() == 1) month = "0" + month;
    std::string year = std::to_string(dt.GetYear());
    return day + "/" + month + "/" + year;
}

std::string callGetServiceDoneStatusByID(std::string id) // Done
{
    return dbServiceDone.Get(id).GetBookStatus()? "True": "False";
}

std::string callGetServiceDoneServiceByID(std::string id) // Done
{
    return ServiceToString(dbServiceDone.Get(id).GetServiceID());
}

void callRateServiceDone(std::string id, int rating ) // Done
{
    dbServiceDone.Update(id, "rating", std::to_string(rating));
}

// Statistics call and auxiliary

#include "Statistics.h"

float callGetAverage(const std::vector<int>& intData) {
    std::vector<float> data;
    for (int i = 0; i < intData.size(); i++)
        data.push_back((float) intData[i]);
    return Statistics::GetAverage(data);
}

float callGetStandardDeviation(const std::vector<int>& intData) {
    std::vector<float> data;
    for (int i = 0; i < intData.size(); i++)
        data.push_back((float) intData[i]);
    return Statistics::GetStandardDeviation(data);
}

std::vector<int> callGetCustomerCountStatistics(int day, int month, int year)
{
    std::vector<int> count;
    if (month == 0) {
        Datetime begin(0, 0, 1, 1, year + 2020);
        for (int i = 0; i < 52; i++) {
            int cnt = 0;
            for (int j = 0; j < 7; j++) {
                Datetime dt = begin + i * 7 + j;
                cnt += Statistics::GetServiceDoneCount(dt);
            }
            count.push_back(cnt);
        }
    } else if (day == 0) {
        Datetime begin(0, 0, 1, month, year + 2020);
        for (int i = 0; i < begin.MonthDays(); i++) {
            Datetime dt = begin + i;
            count.push_back(Statistics::GetServiceDoneCount(dt));
        }
    } else {
        Datetime dt(0, 0, day, month, year + 2020);
        count.push_back(Statistics::GetServiceDoneCount(dt));
    }
    return count;
}

std::vector<int> callGetServiceCustomerCountStatistics(int day, int month, int year)
{
    std::vector<int> count(SERVICES_COUNT, 0);
    if (month == 0) {
        Datetime dt(0, 0, 1, 1, year + 2020);
        for (int i = 0; i < (dt.isLeapYear()? 366: 365); i++) {
            std::vector<int> temp = Statistics::GetServiceCustomerCount(dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++)
                count[j] += temp[j];
        }
    } else if (day == 0) {
        Datetime dt(0, 0, 1, month, year + 2020);
        for (int i = 0; i < dt.MonthDays(); i++) {
            std::vector<int> temp = Statistics::GetServiceCustomerCount(dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++)
                count[j] += temp[j];
        }
    } else {
        std::vector<int> temp = Statistics::GetServiceCustomerCount(Datetime(0, 0, day, month, year + 2020));
        for (int j = 0; j < SERVICES_COUNT; j++)
            count[j] += temp[j];
    } 
    return count;
}

std::vector<float> callGetServiceFrequencyStatistics(int day, int month, int year)
{
    std::vector<int> count = callGetServiceCustomerCountStatistics(day, month, year);
    int sum = 0;
    for (int i = 0; i < count.size(); i++)
        sum += count[i];
    std::vector<float> frequency;
    for (int i = 0; i < count.size(); i++)
        frequency.push_back((float) count[i] / sum * 100);
    return frequency;
}

std::vector<int> callGetServiceRateCountStatistics(int day, int month, int year)
{
    std::vector<int> count(SERVICES_COUNT, 0);
    if (month == 0) {
        Datetime dt(0, 0, 1, 1, year + 2020);
        for (int i = 0; i < (dt.isLeapYear()? 366: 365); i++) {
            std::vector<int> temp = Statistics::GetServiceRateCount(dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++)
                count[j] += temp[j];
        }
    } else if (day == 0) {
        Datetime dt(0, 0, 1, month, year + 2020);
        for (int i = 0; i < dt.MonthDays(); i++) {
            std::vector<int> temp = Statistics::GetServiceRateCount(dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++)
                count[j] += temp[j];
        }
    } else {
        std::vector<int> temp = Statistics::GetServiceRateCount(Datetime(0, 0, day, month, year + 2020));
        for (int j = 0; j < SERVICES_COUNT; j++)
            count[j] += temp[j];
    } 
    return count;
}

std::vector<float> callGetServiceRateAverageStatistics(int day, int month, int year)
{
    std::vector<int> count = callGetServiceRateCountStatistics(day, month, year);
    std::vector<int> sum(SERVICES_COUNT, 0);
    if (month == 0) {
        Datetime dt(0, 0, 1, 1, year + 2020);
        for (int i = 0; i < (dt.isLeapYear()? 366: 365); i++) {
            std::vector<int> temp = Statistics::GetServiceRateSum(dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++)
                sum[j] += temp[j];
        }
    } else if (day == 0) {
        Datetime dt(0, 0, 1, month, year + 2020);
        for (int i = 0; i < dt.MonthDays(); i++) {
            std::vector<int> temp = Statistics::GetServiceRateSum(dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++)
                sum[j] += temp[j];
        }
    } else {
        std::vector<int> temp = Statistics::GetServiceRateSum(Datetime(0, 0, day, month, year + 2020));
        for (int j = 0; j < SERVICES_COUNT; j++)
            sum[j] += temp[j];
    }
    std::vector<float> average;
    for (int i = 0; i < count.size(); i++)
        average.push_back((float) sum[i] / count[i]);
    return average;
}

int callGetStylistCustomerCountStatistics(std::string stylistID, int day, int month, int year) 
{
    if (month == 0)
    {
        Datetime dt(0, 0, 1, 1, year + 2020);
        int cnt = 0;
        for (int i = 0; i < (dt.isLeapYear()? 366: 365); i++)
            cnt += Statistics::GetStylistCustomerCount(stylistID, dt + i);
        return cnt;
    } else if (day == 0) {
        Datetime dt(0, 0, 1, month, year + 2020);
        int cnt = 0;
        for (int i = 0; i < dt.MonthDays(); i++)
            cnt += Statistics::GetStylistCustomerCount(stylistID, dt + i);
        return cnt;
    } else {
        return Statistics::GetStylistCustomerCount(stylistID, Datetime(0, 0, day, month, year + 2020));
    }
}

float callGetStylistRateAverageStatistics(std::string stylistID, int day, int month, int year) {
    int count = 0;
    int sum = 0;
    if (month == 0) {
        Datetime dt(0, 0, 1, 1, year + 2020);
        for (int i = 0; i < (dt.isLeapYear()? 366: 365); i++) {
            count += Statistics::GetStylistRateCount(stylistID, dt + i);
            sum += Statistics::GetStylistRateSum(stylistID, dt + i);
        }
    } else if (day == 0) {
        Datetime dt(0, 0, 1, month, year + 2020);
        for (int i = 0; i < dt.MonthDays(); i++) {
            count += Statistics::GetStylistRateCount(stylistID, dt + i);
            sum += Statistics::GetStylistRateSum(stylistID, dt + i);
        }
    } else {
        count = Statistics::GetStylistRateCount(stylistID, Datetime(0, 0, day, month, year + 2020));
        sum = Statistics::GetStylistRateSum(stylistID, Datetime(0, 0, day, month, year + 2020));
    } 
    return (float) sum / count;
}

std::vector<int> callGetStylistServiceCustomerCountStatistics(std::string stylistID, int day, int month, int year)
{
    std::vector<int> count(SERVICES_COUNT, 0);
    if (month == 0) {
        Datetime dt(0, 0, 1, 1, year + 2020);
        for (int i = 0; i < (dt.isLeapYear()? 366: 365); i++) {
            std::vector<int> temp = Statistics::GetStylistServiceCustomerCount(stylistID, dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++)
                count[j] += temp[j];
        }
    } else if (day == 0) {
        Datetime dt(0, 0, 1, month, year + 2020);
        for (int i = 0; i < dt.MonthDays(); i++) {
            std::vector<int> temp = Statistics::GetStylistServiceCustomerCount(stylistID, dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++)
                count[j] += temp[j];
        }
    } else {
        std::vector<int> temp = Statistics::GetStylistServiceCustomerCount(stylistID, Datetime(0, 0, day, month, year + 2020));
        for (int j = 0; j < SERVICES_COUNT; j++)
            count[j] += temp[j];
    } 
    return count;
}

std::vector<float> callGetStylistServiceRateAverageStatistics(std::string stylistID, int day, int month, int year)
{
    std::vector<int> count(SERVICES_COUNT, 0);
    std::vector<int> sum(SERVICES_COUNT, 0);
    if (month == 0) {
        Datetime dt(0, 0, 1, 1, year + 2020);
        for (int i = 0; i < (dt.isLeapYear()? 366: 365); i++) {
            std::vector<int> tempCount = Statistics::GetStylistServiceRateCount(stylistID, dt + i);
            std::vector<int> tempSum = Statistics::GetStylistServiceRateSum(stylistID, dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++) {
                count[j] += tempCount[j];
                sum[j] += tempSum[j];
            }
        }
    } else if (day == 0) {
        Datetime dt(0, 0, 1, month, year + 2020);
        for (int i = 0; i < dt.MonthDays(); i++) {
            std::vector<int> tempCount = Statistics::GetStylistServiceRateCount(stylistID, dt + i);
            std::vector<int> tempSum = Statistics::GetStylistServiceRateSum(stylistID, dt + i);
            for (int j = 0; j < SERVICES_COUNT; j++) {
                count[j] += tempCount[j];
                sum[j] += tempSum[j];
            }
        }
    } else {
        std::vector<int> tempCount = Statistics::GetStylistServiceRateCount(stylistID, Datetime(0, 0, day, month, year + 2020));
        std::vector<int> tempSum = Statistics::GetStylistServiceRateSum(stylistID, Datetime(0, 0, day, month, year + 2020));
        for (int j = 0; j < SERVICES_COUNT; j++) {
            count[j] += tempCount[j];
            sum[j] += tempSum[j];
        }
    }
    std::vector<float> average;
    for (int i = 0; i < count.size(); i++)
        average.push_back((float) sum[i] / count[i]);
    return average;
}