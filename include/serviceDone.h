#ifndef SERVICEDONE_H
#define SERVICEDONE_H
#include <string>
#include <iostream>
#include "Identifiable.h"

using namespace std;

class serviceDone : public Identifiable
{
    public:
        serviceDone(const string& ID="null", const string& customerID="null",const string& workerID="null",
                    const string& serviceID="null",const string& feedback="\"null\"",const bool& isBooked=false);
        ~serviceDone() override;
        [[nodiscard]] const string& GetCustomerID() const;
        void SetCustomerID(const string&);
        [[nodiscard]] const string& GetWorkerID() const;
        void SetWorkerID(const string&);
        [[nodiscard]] const string& GetServiceID() const;
        void SetServiceID(const string&);
        [[nodiscard]] const string& GetFeedBack() const;
        void SetFeedBack(const string&);
        [[nodiscard]] const bool& GetBookStatus() const;
        void SetBookStatus(const bool&);
        void Show() const override;
        friend ostream& operator<<(ostream& os,const serviceDone& obj);
        friend istream& operator>>(istream& is,serviceDone& obj);
        friend bool operator<(const serviceDone& a,const serviceDone& b);
    private:
        string customerID;
        string workerID;
        string serviceID;
        string feedback;
        bool bookStatus;
        //Datetime time;
};

#endif // SERVICEDONE_H
