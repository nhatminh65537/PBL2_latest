#include "call.h"

// Login and Register

void callLogin(std::string username, std::string password, int role)
{
    if (username.empty() && password.empty()) 
        throw ERROR_CODE::LOGIN_USER_AND_PASS_EMPTY;
    if (username.empty()) 
        throw ERROR_CODE::LOGIN_USERNAME_EMPTY;
    if (password.empty()) 
        throw ERROR_CODE::LOGIN_PASSWORD_EMPTY;

    // example
    if (username != "test" && password != "pass")
        throw ERROR_CODE::LOGIN_INCORRECT_INPUT;
    
        // if (role == 0) // customer
        //     m = new Customer(username);
        

        // if (!m.login())
        // {
        //     // throw ERROR_CODE::LOGIN_INCORRECT_INPUT;
        // }
        // // set current user



    // set current user
}

void callRegister(std:: string firstname, std::string lastname, std::string username, std::string password, std::string confirmpassword, std::string phonenumber, std::string age, int gender, int role)
{
    if (firstname.empty() || lastname.empty() || username.empty() || password.empty() || confirmpassword.empty() || phonenumber.empty() || age.empty())
        throw ERROR_CODE::REGISTER_SOME_FIELD_EMPTY;
    if (password != confirmpassword)
        throw ERROR_CODE::REGISTER_PASSWORD_NOT_MATCH;
    
    // check if username already exist
    // example
    if (username == "test")
        throw ERROR_CODE::REGISTER_USERNAME_EXIST;
    // and update database
}


// Auxiliary functions

int callGetServiceList(std::vector<std::string>& services)
{
    /*
    services = {"Service 0", "Service 1", "Service 2", "Service 3", "Service 4", "Service 5", "Service 6", "Service 7"};
    */
    services = {
        "Service 0",
        "Service 1",
        "Service 2",
        "Service 3",
        "Service 4",
        "Service 5",
        "Service 6",
        "Service 7",
    };
    return 8;
}  // return the number of services

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
    // std::string stylistID = ""; => stylist is not selected
    // (if stylist is not selected) count all appointment at this time compare to the number of stylist
    return "All stylist is busy at this time";
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
    std::vector<std::string> appointmentIDList;
    // example
    appointmentIDList.push_back("001");
    appointmentIDList.push_back("002");
    appointmentIDList.push_back("003");
    return appointmentIDList;
}

std::string callGetCurrentUserUsername()
{
    return callGetMemberUsernameByID(callGetCurrentUserID());
}

std::string callGetCurrentUserName()
{
    // m->
    // return current user name
    // example
    return "John Doe";
}

std::string callGetCurrentUserID(){return "001";}

void callGetCurrentUserName(std::string& firstname, std::string& lastname)
{
    // return current user name
    // example
    firstname = "John";
    lastname = "Doe";
}

void callGetCurrentUserPersonInfo(std::string& phonenumber, std ::string& age, int& gender)
{
    // return current user person info
    // example
    phonenumber = "0123456789";
    age = "20";
    gender = 0; // 0 = male, 1 = female
}

void callUpdateCurrentUserName(std::string firstname, std::string lastname)
{
    if (firstname.empty() && lastname.empty())
        throw ERROR_CODE::UPDATE_PROFILE_NAME_EMPTY;
    if (firstname.empty())
        throw ERROR_CODE::UPDATE_PROFILE_FIRSTNAME_EMPTY;
    if (lastname.empty())
        throw ERROR_CODE::UPDATE_PROFILE_LASTNAME_EMPTY;
    // update database
}

void callUpdateCurrentPassword(std::string oldPassword, std::string newPassword, std::string confirmpassword)
{
    if (oldPassword != "pass") // example
        throw ERROR_CODE::UPDATE_PROFILE_INVALID_PASSWORD;
    if (newPassword != confirmpassword)
        throw ERROR_CODE::UPDATE_PROFILE_PASSWORD_NOT_MATCH;
    // update database
}

void callUpdateCurrentPersonInfo(std::string phonenumber, std::string age, int gender)
{
    if (phonenumber.empty() || age.empty())
        throw ERROR_CODE::UPDATE_PROFILE_PERSONINFO_EMPTY;
    // update database
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
    selectedMinute = x => x*15 minutes
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
}

std::string callGetNewAppointmentId()
{
    // return the temp global variable (make when call callCreateNewAppointment)
    // example
    return "001256";
}

void callAddNewAppointment()
{
    // add the temp global variable (make when call callCreateNewAppointment) to the database
}

std::string callGetAppointmentCustomerIDByID(std::string id)
{
    // get appointment customer id by id
    // example
    return "001";
}

std::string callGetAppointmentCustomerNameByID(std::string id)
{
    // get appointment customer name by id
    // example
    return "John Doe";
}

std::string callGetAppointmentStatusByID(std::string id)
{
    // get appointment status by id
    // example
    return "Done";
}

std::string callGetAppointmentDateByID(std::string id)
{
    // get appointment date by id
    // example
    return "01/01/2021";
}

std::string callGetAppointmentTimeByID(std::string id)
{
    // get appointment time by id
    // example
    return "08:00";
}

std::vector<std::string> callGetAppointmentServicesByID(std::string id)
{
    // get appointment services by id
    // example
    return {"Cat toc", "Goi dau"};
}

std::string callGetAppointmentStylistByID(std::string id)
{
    // get appointment stylist by id
    // if not found, return "None"

    // example
    if (id == "001")
        return "None";
    return "Stylist 1";
}

std::string callGetAppointmentStylistIDByID(std::string id)
{
    // get appointment stylist id by id
    // example
    return "001";
}

std::string callGetAppointmentRequirementByID(std::string id)
{
    // get appointment requirement by id
    // example
    return "This is requirement";
}

std::vector<std::string> callGetApointmentIDList(int day, int month, int year, int hour, int minute, bool services[], int status, std::string customerID, std::string stylistID, int& count)
{
    // if day, month, year, hour, minute = 0 => all
    // if day, month, year, hour, minute = -1 => Now
    // if day, month, year = -1, hour, minute = 0 => Today
    count = 3;
    return {"001", "002", "003"};
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

}


// Member call and auxiliary

std::string callGetMemberNameByID(std::string id)
{
    // get member name by id
    // example
    return "John Doe";
}

std::string callGetMemberFirstNameByID(std::string id)
{
    // get member first name by id
    // example
    return "John";
}

std::string callGetMemberLastNameByID(std::string id)
{
    // get member last name by id
    // example
    return "Doe";
}

std::string callGetMemberPhoneByID(std::string id)
{
    // get member phone by id
    // example
    return "0123456789";
}

std::string callGetMemberAgeByID(std::string id)
{
    // get member age by id
    // example
    return "20";
}

std::string callGetMemberGenderByID(std::string id)
{
    return "Male";
}

std::string callGetMemberUsernameByID(std::string id)
{
    // get member username by id
    // example
    return "johndoe";
}


// Stylist call and auxiliary

std::vector<std::string> callGetStylistIDList(bool gender[2], std::string name, std::string age, int& count)
{
    std::vector<std::string> stylistList;
    // example
    stylistList.push_back("123");
    stylistList.push_back("234");
    stylistList.push_back("345");
    count = 3;
    return stylistList;
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

std::vector<std::string> callGetCustomerIDList(bool gender[2], std::string name, std::string age, int& count)
{
    std::vector<std::string> customerList;
    // example
    customerList.push_back("001");
    customerList.push_back("002");
    customerList.push_back("003");
    count = 3;
    return customerList;
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

