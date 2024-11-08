#include "serviceDone.h"

using namespace std;

serviceDone::serviceDone()
{
    this->ID = "null";
    this->customerID = "null";
    this->workerID = "null";
    this->serviceID = "null";
    this->feedback = "\"null\"";
    this->isBooked = false;
}

serviceDone::serviceDone(const string& ID, const string& customerID,const string& workerID,
                         const string& serviceID,const string& feedback,const bool& isBooked){
    this->ID = ID;
    this->customerID = customerID;
    this->workerID = workerID;
    this->serviceID = serviceID;
    this->feedback = feedback;
    this->isBooked = isBooked;
}

serviceDone::~serviceDone(){
    //dtor
}

const string& serviceDone::GetCustomerID() const{
    return this->customerID;
}
void serviceDone::SetCustomerID(const string& customerID){
    this->customerID = customerID;
}

const string& serviceDone::GetWorkerID() const{
    return this->workerID;
}
void serviceDone::SetWorkerID(const string& workerID){
    this->workerID = workerID;
}
const string& serviceDone::GetServiceID() const{
    return this->serviceID;
}
void serviceDone::SetServiceID(const string& serviceID){
    this->serviceID = serviceID;
}

void serviceDone::SetFeedBack(const string& feedback){
    this->feedback = feedback;
}

void serviceDone::Show() const {
}

const string& serviceDone::GetFeedBack() const{
    return this->feedback;
}

// operator overloading
ostream& operator<<(ostream& os, const serviceDone& obj){
    os << obj.ID << " " << obj.customerID << " " << obj.workerID
        << " " << obj.serviceID << " " << obj.feedback
        << " " << obj.isBooked;
    return os;
}
istream& operator>>(istream& is,serviceDone& obj){
    is >> obj.ID >> obj.customerID >> obj.workerID
        >> obj.serviceID;
    char quote;
    is>>quote;
    getline(is,obj.feedback,'"');
    obj.feedback = "\"" + obj.feedback + "\"";
    is >> obj.isBooked;
    return is;
}

bool operator<(const serviceDone& a,const serviceDone& b){
    string s = a.ID;
    string r = b.ID;
    if (s.size() == r.size())   return s<r;
    return s.size()<r.size();
}
