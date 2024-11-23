#ifndef SERVICEDONE_H
#define SERVICEDONE_H
#include <string>
#include <iostream>
#include "Identifiable.h"

class serviceDone : public Identifiable
{
    public:
        serviceDone(const std::string& ID="null", const std::string& customerID="null",const std::string& workerID="null",
                    const std::string& serviceID="null",const std::string& feedback="\"null\"",const bool& isBooked=false);
        ~serviceDone() override;
        [[nodiscard]] const std::string& GetCustomerID() const;
        void SetCustomerID(const std::string&);
        [[nodiscard]] const std::string& GetWorkerID() const;
        void SetWorkerID(const std::string&);
        [[nodiscard]] const std::string& GetServiceID() const;
        void SetServiceID(const std::string&);
        [[nodiscard]] const std::string& GetFeedBack() const;
        void SetFeedBack(const std::string&);
        [[nodiscard]] const bool& GetBookStatus() const;
        void SetBookStatus(const bool&);
        void Show() const override;
        friend std::ostream& operator<<(std::ostream& os,const serviceDone& obj);
        friend std::istream& operator>>(std::istream& is,serviceDone& obj);
        friend bool operator<(const serviceDone& a,const serviceDone& b);
    private:
        std::string customerID;
        std::string workerID;
        std::string serviceID;
        std::string feedback;
        bool bookStatus;
        //Datetime time;
};

#endif // SERVICEDONE_H
