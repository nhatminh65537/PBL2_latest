//
// Created by LONG on 11/27/2024.
//

#ifndef SALON_H
#define SALON_H
#include <string>
#include <vector>
#include "define.h"
#include "Datetime.h"

class Salon {
    public:
        static Salon& StartUp();
        Salon(const Salon&) = delete;
        Salon& operator=(const Salon&) = delete;
        ~Salon();
        [[nodiscard]]bool Login(const std::string&,const std::string&);
        void Logout();
        void Register(const std::string&,const std::string&,const std::string&,const std::string&,const std::string&,const bool&,const std::string&,const int&);
        void CreateAppointment(const std::string&,const std::string&, const Datetime&,const std::vector<Service>&);
        void DeleteAppointment(const std::string&);
        void ShowAppointment(const Datetime& = Datetime::Now());
        void ShowAllAppointment(const Datetime& = Datetime::Now());
        void ShowSchedule(const std::string&, const Datetime& = Datetime::Now());
        void ShowAllSchedule(const std::string&);

        [[nodiscard]] std::string GetUserID();
        [[nodiscard]] std::string GetUserRole();
    private:
        Salon() = default;
        std::string userID;
        std::string userRole;

        void ensurePermission(const std::string&);
};



#endif //SALON_H
