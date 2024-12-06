//
// Created by LONG on 11/27/2024.
//

#include "Salon.h"
#include <Appointment.h>
#include <Database.h>
#include <vector>

Salon& Salon::StartUp() {
    static Salon salon;
    return salon;
}

Salon::~Salon() {
    userID.clear();
}

std::string Salon::GetUserID() {
    return this->userID;
}

int Salon::GetUserRole() {
    return this->userRole;
}


void Salon::ensurePermission(const std::string& requiredRole) {
    if (requiredRole.empty()) throw ERROR_CODE::SALON_SOME_FIELD_EMPTY;
    auto convertRoleToInt = [](const std::string requiredRole) -> int {
        if (requiredRole == "customer") return 1;
        if (requiredRole == "stylist") return 2;
        if (requiredRole == "admin") return 3;
        return 0;
    };
    int role = convertRoleToInt(requiredRole);
    if (userRole != role && userRole != 3)
        throw ERROR_CODE::SALON_ACCESS_DENIED;
}

bool Salon::Login(const std::string& username, const std::string& password) {
    // std::ofstream log;
    // log.open("log.txt", std::ios::app);
    // log << "Salon::Login Test\n";

    if (username.empty() && password.empty())
        throw ERROR_CODE::LOGIN_USER_AND_PASS_EMPTY;
    if (username.empty())
        throw ERROR_CODE::LOGIN_USERNAME_EMPTY;
    if (password.empty())
        throw ERROR_CODE::LOGIN_PASSWORD_EMPTY;

    std::vector<Member> members = dbUser.Query("username",username).
                                        Query("password",Hash(password)).
                                        GetResults();


    // log << "Hello" << members.size() << '\n';
    if (members.size()==1) {
        userID = members[0].GetID();
        userRole = dbUser.Get(userID).GetRole();
        // userRole = 1;
        // log << "User " << username << " logged in\n";
        // log << "Role: " << userRole << "\n";
        return true;
    }
    return false;
}

void Salon::Logout() {
    userID.clear();
    userRole=0;
}

void Salon::Register(const std::string& firstName,const std::string& lastName,const std::string& username,const std::string& password,const std::string& confirmPassword,
                    const bool& gender,const std::string& phoneNumber,const int& role) {
    if (firstName.empty() || lastName.empty() || username.empty() || password.empty() || confirmPassword.empty() || phoneNumber.empty())
        throw ERROR_CODE::REGISTER_SOME_FIELD_EMPTY;
    if (password != confirmPassword)
        throw ERROR_CODE::REGISTER_PASSWORD_NOT_MATCH;
    if (dbUser.IsExist("username",username))
        throw ERROR_CODE::REGISTER_USERNAME_EXIST;
    Member instance("null",firstName,lastName,username,Hash(password),gender,phoneNumber,role);
    dbUser.Insert(instance);
}


void Salon::CreateAppointment(const std::string& stylistID, const Datetime& dt, const std::vector<Service>& serviceList) {
    if(serviceList.empty()) throw ERROR_CODE::CREATE_APPOINTMENT_SERVICES_EMPTY;
    if(this->userID.empty()) throw ERROR_CODE::CREATE_APPOINTMENT_CUSTOMER_EMPTY;

    ensurePermission("customer");

    // by Minh
    time_t now = time(nullptr);
    std::string id = std::to_string(now);
    tempAppointment = Appointment(id, dt, this->userID, stylistID, serviceList);
    tempAppointment.SetStatus("Waiting");
}

// added by Minh
void Salon::AddAppointment() {
    dbAppointment.Insert(tempAppointment);
    tempAppointment = Appointment();
}

// added by Minh
std::string Salon::GetTempAppointmentID() {
    return tempAppointment.GetID();
}

void Salon::DeleteAppointment(const std::string& appointmentID) {
    if (appointmentID.empty()) throw ERROR_CODE::SALON_SOME_FIELD_EMPTY;

    ensurePermission("admin");

    dbAppointment.Delete(appointmentID);
}

void Salon::ShowAppointment(const Datetime& dt) {
    ensurePermission("customer");
    dbAppointment.Query("customerID",userID).Query("date",Datetime::TimeToString(dt)).Show();
}

void Salon::ShowAllAppointment(const Datetime& dt) {
    ensurePermission("admin");
    dbAppointment.Query("date",Datetime::TimeToString(dt)).Show();
}

void Salon::DeleteCustomer(const std::string &ID) {
    if (dbUser.Get(ID).GetRole() != 1) return;
    std::vector<Appointment> appointments = dbAppointment.Query("customerID",ID).GetResults();
    for (const auto& appointment : appointments) {
        dbAppointment.Delete(appointment.GetID());
    }
    dbUser.Delete(ID);
}

void Salon::DeleteStylist(const std::string &stylistID) {
    if (dbUser.Get(stylistID).GetRole() != 2) return;
    std::vector<Appointment> appointments = dbAppointment.Query("stylistID",stylistID).GetResults();
    for (const auto& appointment : appointments) {
        dbAppointment.Delete(appointment.GetID());
    }
    dbUser.Delete(stylistID);
}


void Salon::ShowSchedule(const std::string& stylistID, const Datetime& dt) {

}
void Salon::ShowAllSchedule(const std::string& stylistID) {

}