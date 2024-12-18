//
// Created by LONG on 11/27/2024.
//

#ifndef SALON_H
#define SALON_H
#include <string>
#include <vector>
#include "define.h"
#include "Datetime.h"
#include "Appointment.h"

class Salon {
    public:
        static Salon& StartUp();
        Salon(const Salon&) = delete;
        Salon& operator=(const Salon&) = delete;
        ~Salon();
        [[nodiscard]]bool Login(const std::string&,const std::string&);
        void Logout();
        void Register(const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const bool&,const std::string&,const int&);
        void CreateAppointment(const std::string&, const Datetime&,const std::vector<Service>&);
        void AddAppointment();
        void DeleteAppointment(const std::string& appointmentID);
        void ShowAppointment(const Datetime& = Datetime::Now());
        void ShowAllAppointment(const Datetime& = Datetime::Now());
        void ShowSchedule(const std::string& stylistID, const Datetime& = Datetime::Now());
        void ShowAllSchedule(const std::string& stylistID);
        void DeleteCustomer(const std::string& ID);
        void DeleteStylist(const std::string& stylistID);

        [[nodiscard]] std::string GetUserID();
        [[nodiscard]] int GetUserRole();
        [[nodiscard]] std::string GetTempAppointmentID();
    private:
        Salon() = default;
        std::string userID;
        int userRole;
        Appointment tempAppointment;

        void ensurePermission(const std::string&);
};



#endif //SALON_H
