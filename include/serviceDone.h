#ifndef SERVICEDONE_H
#define SERVICEDONE_H
#include <string>
#include <iostream>
#include "Identifiable.h"
#include "Datetime.h"

class serviceDone : public Identifiable
{
    public:
        serviceDone(const std::string& ID="null", const std::string& customerID="null",const std::string& workerID="null",
                    const int& serviceID=0,const int& rating=0,const bool& bookStatus=false,const Datetime& dt=Datetime::Now());
        ~serviceDone() override;
        [[nodiscard]] const std::string GetCustomerID() const;
        void SetCustomerID(const std::string&);

        [[nodiscard]] const std::string GetStylistID() const;
        void SetStylistID(const std::string&);

        [[nodiscard]] const int GetServiceID() const;
        void SetServiceID(const int&);

        [[nodiscard]] const int GetRating() const;
        void SetRating(const int&);

        [[nodiscard]] const bool GetBookStatus() const;
        void SetBookStatus(const bool&);

        [[nodiscard]] Datetime GetTime() const;
        void SetTime(const Datetime&);
        [[nodiscard]] const Datetime GetDatetime() const;
        void Show() const override;
        friend std::ostream& operator<<(std::ostream& os,const serviceDone& obj);
        friend std::istream& operator>>(std::istream& is,serviceDone& obj);
        friend bool operator<(const serviceDone& a,const serviceDone& b);
    private:
        std::string customerID;
        std::string workerID;
        int serviceID;
        int rating;
        bool bookStatus;
        Datetime time;
};

#endif // SERVICEDONE_H
