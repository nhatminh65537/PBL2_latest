#include "call.h"
#include "Salon.h"
#include "Database.h"
#include "test.h"
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

std::string callCheckStylistBusy(std::string stylistID, int day, int month, int year, int hour, int minute)
{
    // check if stylist is busy at the time
    // example
    // std::string stylistID = "null"; => stylist is not selected
    // (if stylist is not selected) count all appointment at this time compare to the number of stylist
    return "All stylist is busy at this time";
    // return "This stylist is busy at this time";
    // return "Salon is closed at this time";
    // return "";
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
    flog << "callGetCurrentUserAppointmentIDList\n";
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
    flog << "  " << day << " " << month << " " << year << '\n';
    if (day > 0) {
        flog << "  Filter by day\n";
        dbAppointment.Query("day", dataDay);
    }
    if (month > 0) {
        flog << "  Filter by month\n";
        dbAppointment.Query("month", dataMonth);
    }
    if (year > 0) {
        flog << "  Filter by year\n";
        dbAppointment.Query("year", dataYear);
    }

    // filter by customerID
    dbAppointment.Query("customerID", callGetCurrentUserID());

    // filter by status
    if (status > 0) {
        flog << "  Filter by status\n";
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
    flog << "End callGetCurrentUserAppointmentIDList\n";
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
    flog << "callGetCurrentUserID\n";
    Salon& salon = Salon::StartUp();
    // flog << "  " << salon.GetUserID() << '\n';
    flog << "End callGetCurrentUserID\n";
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
    flog << "callUpdateCurrentUserName\n";
    if (firstname.empty() && lastname.empty())
        throw ERROR_CODE::UPDATE_PROFILE_NAME_EMPTY;
    if (firstname.empty())
        throw ERROR_CODE::UPDATE_PROFILE_FIRSTNAME_EMPTY;
    if (lastname.empty())
        throw ERROR_CODE::UPDATE_PROFILE_LASTNAME_EMPTY;
    // flog << "  " << firstname << " " << lastname << '\n';
    // flog << "  " << callGetCurrentUserID() << '\n';
    // flog << "  Update firstname\n";
    dbUser.Update(callGetCurrentUserID(), "firstName", firstname);
    // flog << "  Update lastname\n";
    dbUser.Update(callGetCurrentUserID(), "lastName", lastname);
    flog << "End callUpdateCurrentUserName\n";
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
    dbUser.Update(callGetCurrentUserID(), "gender", gender == 0? "1": "0");
}


// Appointment call and auxiliary

void callCreateNewAppointment(bool services[], std::string selectedStylistID, int selectedDay, int selectedMonth, int selectedYear, int selectedHour, int selectedMinute) // Done
{
    /*
    states[0] = true => Service 0 (in enum) is selected
    selectedStylistID = "" => No stylist selected
    selectedDay = x => Day x + 1 in month
    selectedMonth = 0 => January
    selectedYear = x => 2021 + x
    selectedHour = x => x (24-hour format)
    selectedMinute = x => x*30 minutes
    requirement = "x" => Additional requirement is "x"

    Reference:
    days = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    months = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12"};
    years  = {"2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029", "2030"};
    hours = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
    minutes = {"00", "15", "30", "45"};

    create a temp global variable to store the this appointment.
    */
    // test exception
    // throw ERROR_CODE::CREATE_APPOINTMENT_CLOSED_TIME;
    flog << "callCreateNewAppointment\n";
    // flog << "  " << selectedStylistID << '\n';
    // flog << "  " << selectedDay << '/' << selectedMonth << '/' << selectedYear << '\n';
    // flog << "  " << selectedHour << ':' << selectedMinute << '\n';
    // flog << "  " << requirement << '\n';
    // flog << "  " << callGetCurrentUserID() << '\n';
    
    std::vector<Service> serviceList;
    for (int i = 0; i < SERVICES_COUNT - 1; i++)
        if (services[i]){
            serviceList.push_back(static_cast<Service> (i + 1));
            // flog << "  Service: " << ServiceToString(i + 1) << '\n';
        }
    int day = selectedDay + 1;
    int month = selectedMonth + 1;
    int year = 2021 + selectedYear;
    int hour = selectedHour;
    int minute = selectedMinute * 30;

    if (selectedStylistID.empty())
        selectedStylistID = "null";
    Salon& salon = Salon::StartUp();
    // flog << "  ServiceList size: " << serviceList.size() << '\n';   
    salon.CreateAppointment(callGetCurrentUserID(), selectedStylistID, Datetime(minute, hour, day, month, year), serviceList);
    flog << "End callCreateNewAppointment\n";
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
    if (stylistID == "null")
        return "null";
    return stylistID;
}

// Don't know implement yet
std::vector<std::string> callGetApointmentIDList(int day, int month, int year, int hour, int minute, bool services[], int status, std::string customerID, std::string stylistID, int& count)
{
    // if day, month, year, hour, minute = 0 => all
    // if day, month, year, hour, minute = -1 => Now
    // if day, month, year = -1, hour, minute = 0 => Today
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
    
    flog << "callGetApointmentIDList\n";
    // filter by day, month, year, hour, minute
    if (minute > 0) {
        flog << "  Filter by minute\n";
        dbAppointment.Query("minute", dataMinute);
    }
    if (hour > 0) {
        flog << "  Filter by hour\n";
        dbAppointment.Query("hour", dataHour);
    }
    if (day > 0) {
        flog << "  Filter by day\n";
        dbAppointment.Query("day", dataDay);
    }
    if (month > 0) {
        flog << "  Filter by month\n";
        dbAppointment.Query("month", dataMonth);
    }
    if (year > 0) {
        flog << "  Filter by year\n";
        dbAppointment.Query("year", dataYear);
    }
    // filter by customerID
    if (!customerID.empty())
    {
        flog << "  Filter by customerID\n";
        dbAppointment.Query("customerID", customerID);
    }
    // filter by stylistID
    if (!stylistID.empty())
    {
        flog << "  Filter by stylistID\n";
        dbAppointment.Query("stylistID", stylistID);
    }
    // filter by status
    if (status > 0)
    {
        flog << "  Filter by status\n";
        dbAppointment.Query("status", dataStatus);
    }

    // filter by services
    std::vector<std::string> appointmentIDList;
    std::vector<Appointment> appointmentList = dbAppointment.GetResults();
    flog << "  " << "Appointment list size: " << appointmentList.size() << '\n';
    for (auto appoint : appointmentList)
    {
        std::vector<Service> temp;
        std::set_intersection(serviceList.begin(), serviceList.end(), appoint.GetServices().begin(), appoint.GetServices().end(), back_inserter(temp));
        if (temp.size() > 0)
            appointmentIDList.push_back(appoint.GetID());
        flog << "    " << "Intersection size:" << temp.size() << '\n';
    }
    count = appointmentIDList.size();
    flog << "End callGetApointmentIDList\n";
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
    if (dbAppointment.Get(id).GetStatus() == "Cancel")
        throw ERROR_CODE::APPOINTMENT_IS_CANCELLED;
    if (dbAppointment.Get(id).GetStylistID() == "DONE")
        return;
    // set appointment status to done
    dbAppointment.Update(id, "status", "Done");

    // generate service done
    flog << "callDoneAppointment\n";
    std::vector<Service> serviceList = dbAppointment.Get(id).GetServices();
    for (int i = 0; i < serviceList.size(); i++)
    {
        flog << "  " << ServiceToString(serviceList[i]) << '\n';
        std::string sid = std::to_string(dbServiceDone.Count());
        sid = std::string(10 - sid.length(), '0') + sid;
        flog << "  " << sid << '\n';
        Datetime now = Datetime::Now();
        Datetime dt = dbAppointment.Get(id).GetStartTime();
        serviceDone newServiceDone(
            sid, 
            dbAppointment.Get(id).GetCustomerID(),
            dbAppointment.Get(id).GetStylistID(),
            serviceList[i],
            0,
            true,
            dt
        );
        // output parameter
        flog << "  " << dbAppointment.Get(id).GetCustomerID() << '\n';
        flog << "  " << dbAppointment.Get(id).GetStylistID() << '\n';
        flog << "  " << serviceList[i] << '\n';
        flog << "  0\n";
        flog << "  true\n";
        flog << "  " << now.GetDay() << '/' << now.GetMonth() << '/' << now.GetYear() << '\n';
        flog << "  Insert service done\n";
        dbServiceDone.Insert(newServiceDone);
    }
    flog << "End callDoneAppointment\n";
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

std::string callGetMemberAgeByID(std::string id)
{
    return "Trash";
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
std::vector<std::string> callGetStylistIDList(bool gender[2], std::string name, std::string age, int& count)
{
    flog << "Start callGetStylistIDList\n";
    std::vector<std::string> stylistListID;
    // filter by role
    dbUser.Query("role", "2");

    // filter by gender
    flog << "  Male" << gender[0] << '\n';
    if (!gender[0]) {
        flog << "  Erase Female\n";
        dbUser.Query("gender", "Female");
    }
    flog << "  Female" << gender[1] << '\n';
    if (!gender[1]) {
        flog << "  Erase Male\n";
        dbUser.Query("gender", "Male");
    }
    // filter by name
    if (!name.empty()) {
        dbUser.Query("name", name);
    }

    std::vector<Member> stylistList = dbUser.GetResults();
    for (auto stylist : stylistList) {
        stylistListID.push_back(stylist.GetID());
        // flog << "  " << dbUser.Get(stylist.GetID()).GetRole() << '\n';
    }
    count = stylistListID.size();
    // flog << "  count = " << count << '\n';
    flog << "End callGetStylistIDList\n";
    return stylistListID;
}

std::vector<std::string> callGetStylistIDList() // Done
{
    int countStylist;
    bool temp[2] = {true, true};
    return callGetStylistIDList(temp, "", "", countStylist);
}

void callUpdateStylist (std::string id, std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password) // Done
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
    if (id != "null") 
    {
        dbUser.Delete(id);
    }
}

void callAddStylist(std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password) // Done
{
    Salon& salon = Salon::StartUp();
    bool isMale = (gender == 0);
    salon.Register(firstname, lastname, username, password, password, isMale, phonenumber, 2);
}


// Customer call and auxiliary
std::vector<std::string> callGetCustomerIDList(bool gender[2], std::string name, std::string age, int& count) // Done
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
    dbUser.Delete(id);
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

    flog << "callGetServiceDoneIDList\n";
    // flog all arguments
    flog << "  " << day << " " << month << " " << year << '\n';
    flog << "  " << customerID << '\n';
    flog << "  " << stylistID << '\n';
    flog << "  " << rating[0] << " " << rating[1] << " " << rating[2] << '\n';
    flog << "  " << status[0] << " " << status[1] << '\n';
    flog << "  " << services[0] << " " << services[1] << " " << services[2] << " " << services[3] << " " << services[4] << " " << services[5] << " " << services[6] << " " << services[7] << '\n';
    flog << "End callGetServiceDoneIDList\n";
    // change to database data
    std::string dataYear = std::to_string(year + 2020);
    std::string dataMonth = std::to_string(month);
    std::string dataDay = std::to_string(day);

    // filter by day, month, year
    if (day > 0) {
        flog << "  Filter by day\n";
        dbServiceDone.Query("day", dataDay);
    }
    if (month > 0) {
        flog << "  Filter by month\n";
        dbServiceDone.Query("month", dataMonth);
    }
    if (year > 0) {
        flog << "  Filter by year\n";
        dbServiceDone.Query("year", dataYear);
    }
    
    // filter by customerID
    if (!customerID.empty())
    {
        flog << "  Filter by customerID\n";
        dbServiceDone.Query("customerID", customerID);
    }

    // filter by stylistID
    if (!stylistID.empty())
    {
        flog << "  Filter by stylistID\n";
        dbServiceDone.Query("stylistID", stylistID);
    }

    std::vector<serviceDone> serviceDoneList = dbServiceDone.GetResults();
    // filter by rating, status, services
    std::vector<std::string> serviceDoneIDList;
    for (auto sDone : serviceDoneList)
    {
        flog << "  Filter by rating, status, services\n";
        flog << "  " << sDone.GetID() << '\n'; 
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
    flog << "callRateServiceDone\n";
    dbServiceDone.Update(id, "rating", std::to_string(rating));
    flog << "End callRateServiceDone\n";
}

// Statistics call and auxiliary

