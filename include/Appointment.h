#ifndef APPOINTMENT_H
#define APPOINTMENT_H
#include <vector>
#include <string>
#include <iostream>
#include "Datetime.h"
#include "define.h"
#include "Identifiable.h"


class Appointment : public Identifiable
{
    public:
        Appointment(const std::string& appointmentID = "null",
            const Datetime& startTime=Datetime::Now(),
            const std::string& customerID = "null",
            const std::string& stylistID = "null",
            const std::vector<Service>& serviceList = {});
        ~Appointment() override;
        void Show() const override;
        [[nodiscard]] std::string GetCustomerID() const;
        void SetCustomerID(const std::string& customerID);
        [[nodiscard]] std::string GetStylistID() const;
        void SetStylistID(const std::string& stylistID);
        [[nodiscard]] Datetime GetStartTime() const;
        void SetStartTime(const Datetime& dt);
        [[nodiscard]] std::vector<Service> GetServices() const;
        void SetServices(const std::vector<Service>& serviceList);
        [[nodiscard]] std::string GetStatus() const;
        void SetStatus(const std::string& status);
        friend std::ostream& operator<<(std::ostream& os, const Appointment& appointment);
        friend std::istream& operator>>(std::istream& is, Appointment& appointment);
    private:
        Datetime startTime;
        std::string stylistID;
        std::string customerID;
        std::vector<Service> serviceList;
        std::string status;
};
#endif // APPOINTMENT_H
