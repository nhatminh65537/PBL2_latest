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
        // repaired by Minh (it makes the appointment but not yet add to the database)
        void CreateAppointment(const std::string&,const std::string&, const Datetime&,const std::vector<Service>&);
        // added by Minh (it adds the appointment to the database)
        void AddAppointment();
        void DeleteAppointment(const std::string&);
        void ShowAppointment(const Datetime& = Datetime::Now());
        void ShowAllAppointment(const Datetime& = Datetime::Now());
        void ShowSchedule(const std::string&, const Datetime& = Datetime::Now());
        void ShowAllSchedule(const std::string&);

        [[nodiscard]] std::string GetUserID();
        [[nodiscard]] int GetUserRole();
        // added by Minh
        [[nodiscard]] std::string GetTempAppointmentID();
    private:
        Salon() = default;
        std::string userID;
        int userRole;
        // added by Minh (save the appointment but not yet add to the database)
        Appointment tempAppointment;

        void ensurePermission(const std::string&);
};



#endif //SALON_H
