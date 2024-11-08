#ifndef SERVICEDONE_H
#define SERVICEDONE_H
#include <string>
#include <iostream>
#include "Identifiable.h"

using namespace std;

class serviceDone : public Identifiable
{
    public:
        serviceDone();
        serviceDone(const string& ID, const string& customerID,const string& workerID,
                    const string& serviceID,const string& feedback,const bool& isBooked);
        ~serviceDone() override;
        [[nodiscard]] const string& GetCustomerID() const;
        void SetCustomerID(const string&);
        [[nodiscard]] const string& GetWorkerID() const;
        void SetWorkerID(const string&);
        [[nodiscard]] const string& GetServiceID() const;
        void SetServiceID(const string&);
        [[nodiscard]] const string& GetFeedBack() const;
        void SetFeedBack(const string&);
        void Show() const override;
        friend ostream& operator<<(ostream& os,const serviceDone& obj);
        friend istream& operator>>(istream& is,serviceDone& obj);
        friend bool operator<(const serviceDone& a,const serviceDone& b);
    protected:

    private:
        string customerID;
        string workerID;
        string serviceID;
        string feedback;
        bool isBooked;
        //Datetime time;
};

#endif // SERVICEDONE_H
