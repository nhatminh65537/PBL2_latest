#include "serviceDone.h"

#include <define.h>

serviceDone::serviceDone(const std::string& ID, const std::string& customerID,const std::string& workerID,
                         const int& serviceID,const int& rating,const bool& bookStatus,const Datetime& time){
    this->ID = ID;
    this->customerID = customerID;
    this->workerID = workerID;
    this->serviceID = serviceID;
    this->rating = rating; // Thêm cặp dấu " để đúng với format của feedback. "content"
    this->bookStatus = bookStatus;
    this->time=time;
}

serviceDone::~serviceDone(){
    //dtor
}

const std::string serviceDone::GetCustomerID() const{
    return this->customerID;
}
void serviceDone::SetCustomerID(const std::string& customerID){
    this->customerID = customerID;
}

const std::string serviceDone::GetWorkerID() const{
    return this->workerID;
}
void serviceDone::SetWorkerID(const std::string& workerID){
    this->workerID = workerID;
}
const int serviceDone::GetServiceID() const{
    return this->serviceID;
}
void serviceDone::SetServiceID(const int& serviceID){
    this->serviceID = serviceID;
}

void serviceDone::SetRating(const int& rating){
    this->rating = rating;
}

const int serviceDone::GetRating() const{
    return this->rating;
}

const bool serviceDone::GetBookStatus() const {
    return this->bookStatus;
}

void serviceDone::SetBookStatus(const bool& status) {
    this->bookStatus = status;
}

const Datetime serviceDone::GetTime() const {
    return this->time;
}

void serviceDone::SetTime(const Datetime& time) {
    this->time = time;
}

void serviceDone::Show() const {
    std::cout << "ID: " << this->ID << '\n';
    std::cout << "CustomerID: " << this->customerID << '\n';
    std::cout << "WorkerID: " << this->workerID << '\n';
    std::cout << "Service: " << ServiceToString(this->serviceID) << '\n';
    std::cout << "Rating: " << this->rating << '\n';
    std::cout << "BookStatus: " << (this->bookStatus? "Yes" : "No") << '\n';
    std::cout << "Time: " << (this->time.GetMinute() < 10 ? "0" + std::to_string(this->time.GetMinute()) : std::to_string(this->time.GetMinute())) << ":" << (this->time.GetHour() < 10 ? "0" + std::to_string(this->time.GetHour()) : std::to_string(this->time.GetHour())) << " " << this->time.GetDay() << "/" << this->time.GetMonth() << "/" << this->time.GetYear() << "\n";
}


// operator overloading
std::ostream& operator<<(std::ostream& os, const serviceDone& obj){
    os << obj.ID << " " << obj.customerID << " " << obj.workerID
        << " " << obj.serviceID << " " << obj.rating
        << " " << obj.bookStatus << " " << obj.time;
    return os;
}
std::istream& operator>>(std::istream& is,serviceDone& obj){
    is >> obj.ID >> obj.customerID >> obj.workerID
        >> obj.serviceID >> obj.rating >> obj.bookStatus >> obj.time;
    return is;
}

bool operator<(const serviceDone& a,const serviceDone& b){
    if (a.ID.size() == b.ID.size())   return a.ID<b.ID;
    return a.ID.size()<b.ID.size();
}
