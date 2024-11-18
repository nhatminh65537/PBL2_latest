#include "call.h"

void callLogin(std::string username, std::string password, int role)
{
    if (username.empty() && password.empty()) 
        throw ERROR_CODE::LOGIN_USER_AND_PASS_EMPTY;
    if (username.empty()) 
        throw ERROR_CODE::LOGIN_USERNAME_EMPTY;
    if (password.empty()) 
        throw ERROR_CODE::LOGIN_PASSWORD_EMPTY;

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

void callCreateAppointment(std::array<bool, SERVICES_COUNT> states, std::array<int, SERVICES_COUNT> selectedStylist, int selectedDay, int selectedMonth, int selectedYear, int selectedHour, int selectedMinute, std::string requirement)
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
}

void callAddAppointment()
{
    // add the temp global variable (make when call callCreateAppointment) to the database
}

void callRegister(std:: string firstname, std::string lastname, std::string username, std::string password, std::string confirmpassword, std::string phonenumber, std::string age, int gender, int role)
{
    if (firstname.empty() || lastname.empty() || username.empty() || password.empty() || confirmpassword.empty() || phonenumber.empty() || age.empty())
        throw ERROR_CODE::REGISTER_SOME_FILED_EMPTY;
    if (password != confirmpassword)
        throw ERROR_CODE::REGISTER_PASSWORD_NOT_MATCH;
    
    // check if username already exist
    // and update database
}