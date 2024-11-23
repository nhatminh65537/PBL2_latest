#include "call.h"

std::string makeName(std::string firstname, std::string lastname, bool reverse)
{
    std::string name;
    if (reverse)
        name = lastname + " " + firstname;
    else
        name = firstname + " " + lastname;
    return name;
}
void breakName(std::string name, std::string& firstname, std::string& lastname, bool reverse)
{
    if (reverse)
    {
        size_t pos = name.find(" ");
        lastname = name.substr(0, pos);
        firstname = name.substr(pos + 1);
    }
    else
    {
        size_t pos = name.find(" ");
        firstname = name.substr(0, pos);
        lastname = name.substr(pos + 1);
    }
}

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

std::vector<std::string> callGetStylist(int service)
{
    /*
    service = 0 => Service 0 (in enum)

    below is just example
    first element must be "None"
    */
    std::vector<std::string> stylist = {
        "None",
        "Stylist 1",
        "Stylist 2",
        "Stylist 3",
        "Stylist 4",
        "Stylist 5",
    };
    return stylist;
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

void callCreateNewAppointment(std::array<bool, SERVICES_COUNT> services, std::array<int, SERVICES_COUNT> selectedStylist, int selectedDay, int selectedMonth, int selectedYear, int selectedHour, int selectedMinute, std::string requirement)
{
    /*
    states[0] = true => Service 0 (in enum) is selected
    selectedStylist[0] = 1 => Service 0 is assigned to Stylist 1 (in return of callGetStylist)
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

std::string callGetCurrentUserName()
{
    // m->
    // return current user name
    // example
    return "John Doe";
}

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

std::string callGetCurrentUserUsername()
{
    // return current user username
    // example
    return "test";
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

std::vector<std::string> callGetCurrentUserHistoryList(int day, int month, int year, std::array<bool, SERVICES_COUNT> services, int status)
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
    format list:
    "<ID> <Status> <Date> <Time> <Services, ...>"
    */
    std::vector<std::string> historyList;
    // example
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    historyList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    historyList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    historyList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    return historyList;
}

void callGetAppointmentDetailByID(std::string id, std::string& name, std::string& status, std::string& date, std::string& time, std::vector<std::string>& services, std::vector<std::string>& stylists, std::string& requirement)
{
    // get appointment detail by id
    // example
    name = "John Doe";
    status = "Done";
    date = "01/01/2021";
    time = "08:00";
    services = {"Cat toc", "Goi dau"};
    stylists = {"Stylist 1", "Stylist 2"};
    requirement = "None";
}

void callCancelAppointment(std::string id)
{
    // Huy lich co id
    // example
    
}

std::vector<std::string> callGetApointmentHistoryList(int day, int month, int year, int hour, int minute, std::array<bool, SERVICES_COUNT> services, int status, std::string customerID, int& containerAppointmentHistoryList)
{
    /*
    day = 0 => all day
    month = 0 => all month
    year = 0 => all year
    hour = 0 => all hour
    minute = 0 => all minute
    day = x (not 0)=> Day x in month
    month = x (not 0)=> month x
    year = x (not 0) => 2020 + x
    hour = x (not 0) => x
    minute = x (not 0) => x*15
    services[0] = true => Service 0 (in enum) is selected
    status = 0 => all status
    status = 1 => done
    status = 2 => pending
    status = 3 => cancel
    customerID = "x" => Customer ID is "x"
    format list:
    "<ID> <Status> <Date> <Time> <Services, ...>"
    */
    std::vector<std::string> appointmentHistoryList;
    // example
    appointmentHistoryList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    appointmentHistoryList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    appointmentHistoryList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    appointmentHistoryList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    appointmentHistoryList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    appointmentHistoryList.push_back("003 Cancel 31/01/2023 10:00 Tay toc");
    appointmentHistoryList.push_back("001 Done 01/01/2021 08:00 Cat toc, Goi dau");
    appointmentHistoryList.push_back("002 Pending 01/02/2022 09:00 Nhuom toc, Uon toc");
    return appointmentHistoryList;
}

std::vector<std::string> callGetStylistList(std::array<bool, SERVICES_COUNT> services, int gender, std::string name, std::string age, int& count)
{
    // format list: <name> <gender> <age> <phone number>
    std::vector<std::string> stylistList;
    // example
    stylistList.push_back("123,Stylist 1,Male,25,0123456789");
    stylistList.push_back("234,Stylist 2,Female,30,0123456789");
    stylistList.push_back("345,Stylist 3,Male,22,8976543210");
    count = 3;
    return stylistList;
}

void callGetStylistDetailByID(std::string id, std::string& name, std::string& gender, std::string& age, std::string& phone, std::vector<std::string>& service, std::string& username, std::string& password)
{
    // get stylist detail by id
    // example
    name = "Stylist 1";
    gender = "Male";
    age = "25";
    phone = "0123456789";
    service = {"Cat toc", "Goi dau"};
    username = "stylist1";
    password = "pass";
}

void callDeleteStylist(std::string id)
{
    // delete stylist by id
    // example
}

void callUpdateStylist (std::string id, std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password, std::array<bool, SERVICES_COUNT>)
{
    // update stylist by id
    // example
}

void callAddStylist(std::string firstname, std::string lastname, int gender, std::string age, std::string phonenumber, std::string username, std::string password, std::array<bool, SERVICES_COUNT> services)
{
    // add stylist
    // example
}