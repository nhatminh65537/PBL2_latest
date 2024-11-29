#include "call.h"
#include "Salon.h"
#include "Database.h"
#include "test.h"
#include "Appointment.h"
#include "Datetime.h"
#include "algorithm"

// variables

Appointment tempAppointment;


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
    std::string busyStatus = "Stylists available";
    const Datetime dt(minute,hour, day, month, year);
    if (stylistID == "null") {
        int numberOfAppointments = dbAppointment.Query("time",Datetime::TimeToString(dt)).GetResults().size();
        int numberOfStylist = dbUser.Query("role","2").GetResults().size();
        if (numberOfAppointments >= numberOfStylist) {
            busyStatus = "All stylist is busy at this time";
        }
    }
    else {
        std::cerr << stylistID << " " << busyStatus << ' ' << dt << '\n';

        if (dbAppointment.Query("stylistID",stylistID).Query("time",Datetime::TimeToString(dt)).GetResults().size() > 0) {
            busyStatus = "This stylist is busy at this time";
        }
    }
    return busyStatus;
    // return "This stylist is busy at this time";
    // return "Salon is closed at this time";
    // return "";
}


// Current user call (Customer)

std::vector<std::string> callGetCurrentUserAppointmentIDList(int day, int month, int year, bool services[], int status) 
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

    // filter by day, month, year
    std::vector<std::string> appointmentIDList;
    std::cerr << "  " << day << " " << month << " " << year << '\n';
    if (day > 0 && month > 0 && year > 0)
    {
        dbAppointment.Query("startTime", "0/0/" + dataDay + "/" + dataMonth + "/" + dataYear);
    }

    // filter by customerID
    std::vector<Appointment> appointmentList = dbAppointment.Query("customerID", callGetCurrentUserID()).GetResults();
    
    // filter by status

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
    std::cerr << "callGetCurrentUserID\n";
    Salon& salon = Salon::StartUp();
    // flog << "  " << salon.GetUserID() << '\n';
    std::cerr << "End callGetCurrentUserID\n";
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
    // flog << "  " << firstname << " " << lastname << '\n';
    // flog << "  " << callGetCurrentUserID() << '\n';
    // flog << "  Update firstname\n";
    dbUser.Update(callGetCurrentUserID(), "firstName", firstname);
    // flog << "  Update lastname\n";
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
    dbUser.Update(callGetCurrentUserID(), "gender", gender == 0? "1": "0");
}


// Appointment call and auxiliary

void callCreateNewAppointment(bool services[], std::string selectedStylistID, int selectedDay, int selectedMonth, int selectedYear, int selectedHour, int selectedMinute, std::string requirement)
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
    std::cerr << "callCreateNewAppointment\n";
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
    return "Done";
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

std::vector<std::string> callGetAppointmentServicesByID(std::string id)
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

std::string callGetAppointmentRequirementByID(std::string id)
{
    // get appointment requirement by id
    // example
    return "This is requirement";
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
    std::string dataHour = std::to_string(hour);
    std::string dataMinute = std::to_string(minute);
    
    std::cerr << "callGetApointmentIDList\n";
    // filter by day, month, year, hour, minute
    if (day > 0 && month > 0 && year > 0)
    {
        std::cerr << "  Filter by day, month, year\n";
        dbAppointment.Query("startTime", "0/0/" + dataDay + "/" + dataMonth + "/" + dataYear);
    } 
    // filter by customerID
    if (!customerID.empty())
    {
        std::cerr << "  Filter by customerID\n";
        dbAppointment.Query("customerID", customerID);
    }
    // filter by stylistID
    if (!stylistID.empty())
    {
        std::cerr << "  Filter by stylistID\n";
        dbAppointment.Query("stylistID", stylistID);
    }
    // filter by status

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
    std::cerr << "End callGetApointmentIDList\n";
    return appointmentIDList;
}

void callCancelAppointment(std::string id)
{
    // Huy lich co id
    // example
}

void callDoneAppointment(std::string id)
{
    // Hoan thanh lich co id
    // example
}

void callAssignStylistToAppointment(std::string appointmentID, std::string stylistID)
{
    dbAppointment.Update(appointmentID, "stylistID", stylistID);
}


// Member call and auxiliary

std::string callGetMemberNameByID(std::string id)
{
    return dbUser.Get(id).GetFullName();
}

std::string callGetMemberFirstNameByID(std::string id)
{
    return  dbUser.Get(id).GetFirstName();
}

std::string callGetMemberLastNameByID(std::string id)
{
    return dbUser.Get(id).GetLastName();
}

std::string callGetMemberPhoneByID(std::string id)
{
    return dbUser.Get(id).GetPhoneNumber();
}

std::string callGetMemberAgeByID(std::string id)
{
    return "Trash";
}

std::string callGetMemberGenderByID(std::string id)
{
    return (dbUser.Get(id).GetGender() == 1 ? "Male" : "Female");
}

std::string callGetMemberUsernameByID(std::string id)
{
    return dbUser.Get(id).GetUserName();
}


// Stylist call and auxiliary
// need more
std::vector<std::string> callGetStylistIDList(bool gender[2], std::string name, std::string age, int& count)
{
    // flog << "Start callGetStylistIDList\n";
    std::vector<std::string> stylistListID;
    std::vector<Member> stylistList = dbUser.Query("role", "2").GetResults();
    // flog << "  Print stylist list\n";
    for (auto stylist : stylistList) {
        stylistListID.push_back(stylist.GetID());
        // flog << "  " << dbUser.Get(stylist.GetID()).GetRole() << '\n';
    }
    count = stylistListID.size();
    // flog << "  count = " << count << '\n';
    // flog << "End callGetStylistIDList\n";
    return stylistListID;
}

std::vector<std::string> callGetStylistIDList()
{
    int countStylist;
    return callGetStylistIDList({}, "", "", countStylist);
}

void callUpdateStylist (std::string id, std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password)
{
    // update stylist by id
    // example
}

void callDeleteStylist(std::string id)
{
    // delete stylist by id
    // example
}

void callAddStylist(std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password)
{
    // add stylist
    // example
}

// Customer call and auxiliary
//need more
std::vector<std::string> callGetCustomerIDList(bool gender[2], std::string name, std::string age, int& count)
{
    std::vector<std::string> customerListID;
    return customerListID;
}

void callDeleteCustomer(std::string id)
{
    // delete customer by id
    // example
}


// Service Done call and auxiliary
std::vector<std::string> callGetServiceDoneIDList(int day, int month, int year, std::string customerID, std::string stylistID, bool rating[], bool status[], bool services[], int& count)
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
    std::vector<std::string> serviceDoneList;
    // example
    serviceDoneList.push_back("001");
    serviceDoneList.push_back("002");
    serviceDoneList.push_back("003");
    count = 3;
    return serviceDoneList;
}

std::string callGetServiceDoneCustomerIDByID(std::string id)
{
    // get service done customer id by id
    // example
    return "001";
}

std::string callGetServiceDoneStylistIDByID(std::string id)
{
    // get service done stylist id by id
    // example
    return "002";
}

std::string callGetServiceDoneRatingByID(std::string id)
{
    // get service done rating by id
    // example
    return "5";
}

std::string callGetServiceDoneDateByID(std::string id)
{
    // get service done date by id
    // example
    return "01/01/2021";
}

std::string callGetServiceDoneStatusByID(std::string id)
{
    // get service done status by id
    // example
    return "Completed";
}

std::string callGetServiceDoneServiceByID(std::string id)
{
    // get service done services by id
    // example
    return "Cat toc";
}

void callRateServiceDone(std::string id, int rating)
{
    // rate service done by id
    // example
}

// Statistics call and auxiliary

