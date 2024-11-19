#include "serviceDone.h"

using namespace std;

serviceDone::serviceDone(const string& ID, const string& customerID,const string& workerID,
                         const string& serviceID,const string& feedback,const bool& isBooked){
    this->ID = ID;
    this->customerID = customerID;
    this->workerID = workerID;
    this->serviceID = serviceID;
    this->feedback = feedback; // Thêm cặp dấu " để đúng với format của feedback. "content"
    this->bookStatus = isBooked;
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

const string& serviceDone::GetFeedBack() const{
    return this->feedback;
}

const bool& serviceDone::GetBookStatus() const {
    return this->bookStatus;
}

void serviceDone::SetBookStatus(const bool& status) {
    this->bookStatus = status;
}


void serviceDone::Show() const {
    cout << "ID: " << this->ID << '\n';
    cout << "CustomerID: " << this->customerID << '\n';
    cout << "WorkerID: " << this->workerID << '\n';
    cout << "ServiceID: " << this->serviceID << '\n';
    cout << "Feedback: " << this->feedback << '\n';
    cout << "BookStatus: " << (this->bookStatus? "Yes" : "No") << '\n';
}


// operator overloading
ostream& operator<<(ostream& os, const serviceDone& obj){
    os << obj.ID << " " << obj.customerID << " " << obj.workerID
        << " " << obj.serviceID << " \"" << obj.feedback
        << "\" " << obj.bookStatus;
    return os;
}
istream& operator>>(istream& is,serviceDone& obj){
    is >> obj.ID >> obj.customerID >> obj.workerID
        >> obj.serviceID;
    char quote;
    is>>quote;
    getline(is,obj.feedback,'"');
    obj.feedback =  obj.feedback;
    is >> obj.bookStatus;
    return is;
}

bool operator<(const serviceDone& a,const serviceDone& b){
    if (a.ID.size() == b.ID.size())   return a.ID<b.ID;
    return a.ID.size()<b.ID.size();
}
