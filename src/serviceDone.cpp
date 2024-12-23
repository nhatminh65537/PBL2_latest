#include "serviceDone.h"

#include <define.h>

ServiceDone::ServiceDone(const std::string& ID, const std::string& customerID,const std::string& workerID,
                         const int& serviceID,const int& rating,const bool& bookStatus,const Datetime& time){
    this->ID = ID;
    this->customerID = customerID;
    this->workerID = workerID;
    this->serviceID = serviceID;
    this->rating = rating; // Thêm cặp dấu " để đúng với format của feedback. "content"
    this->bookStatus = bookStatus;
    this->time=time;
}

ServiceDone::~ServiceDone(){
    //dtor
}

const std::string ServiceDone::GetCustomerID() const{
    return this->customerID;
}
void ServiceDone::SetCustomerID(const std::string& customerID){
    this->customerID = customerID;
}

const std::string ServiceDone::GetStylistID() const{
    return this->workerID;
}
void ServiceDone::SetStylistID(const std::string& workerID){
    this->workerID = workerID;
}
const int ServiceDone::GetServiceID() const{
    return this->serviceID;
}
void ServiceDone::SetServiceID(const int& serviceID){
    this->serviceID = serviceID;
}

void ServiceDone::SetRating(const int& rating){
    this->rating = rating;
}

const int ServiceDone::GetRating() const{
    return this->rating;
}

const bool ServiceDone::GetBookStatus() const {
    return this->bookStatus;
}

void ServiceDone::SetBookStatus(const bool& status) {
    this->bookStatus = status;
}

Datetime ServiceDone::GetTime() const {
    return this->time;
}

void ServiceDone::SetTime(const Datetime& time) {
    this->time = time;
}

void ServiceDone::Show() const {
    std::cout << "ID: " << this->ID << '\n';
    std::cout << "CustomerID: " << this->customerID << '\n';
    std::cout << "StylistID: " << this->workerID << '\n';
    std::cout << "Service: " << ServiceToString(this->serviceID) << '\n';
    std::cout << "Rating: " << this->rating << '\n';
    std::cout << "BookStatus: " << (this->bookStatus? "Yes" : "No") << '\n';
    std::cout << "Time: " << (this->time.GetMinute() < 10 ? "0" + std::to_string(this->time.GetMinute()) : std::to_string(this->time.GetMinute())) << ":" << (this->time.GetHour() < 10 ? "0" + std::to_string(this->time.GetHour()) : std::to_string(this->time.GetHour())) << " " << this->time.GetDay() << "/" << this->time.GetMonth() << "/" << this->time.GetYear() << "\n";
}


// operator overloading
std::ostream& operator<<(std::ostream& os, const ServiceDone& obj){
    os << obj.ID << " " << obj.customerID << " " << obj.workerID
        << " " << obj.serviceID << " " << obj.rating
        << " " << obj.bookStatus << " " << obj.time;
    return os;
}
std::istream& operator>>(std::istream& is,ServiceDone& obj){
    is >> obj.ID >> obj.customerID >> obj.workerID
        >> obj.serviceID >> obj.rating >> obj.bookStatus >> obj.time;
    return is;
}

bool operator<(const ServiceDone& a,const ServiceDone& b){
    if (a.ID.size() == b.ID.size())   return a.ID<b.ID;
    return a.ID.size()<b.ID.size();
}
