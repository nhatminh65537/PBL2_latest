#include "serviceDone.h"

serviceDone::serviceDone(const std::string& ID, const std::string& customerID,const std::string& workerID,
                         const std::string& serviceID,const std::string& feedback,const bool& isBooked){
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

const std::string& serviceDone::GetCustomerID() const{
    return this->customerID;
}
void serviceDone::SetCustomerID(const std::string& customerID){
    this->customerID = customerID;
}

const std::string& serviceDone::GetWorkerID() const{
    return this->workerID;
}
void serviceDone::SetWorkerID(const std::string& workerID){
    this->workerID = workerID;
}
const std::string& serviceDone::GetServiceID() const{
    return this->serviceID;
}
void serviceDone::SetServiceID(const std::string& serviceID){
    this->serviceID = serviceID;
}

void serviceDone::SetFeedBack(const std::string& feedback){
    this->feedback = feedback;
}

const std::string& serviceDone::GetFeedBack() const{
    return this->feedback;
}

const bool& serviceDone::GetBookStatus() const {
    return this->bookStatus;
}

void serviceDone::SetBookStatus(const bool& status) {
    this->bookStatus = status;
}


void serviceDone::Show() const {
    std::cout << "ID: " << this->ID << '\n';
    std::cout << "CustomerID: " << this->customerID << '\n';
    std::cout << "WorkerID: " << this->workerID << '\n';
    std::cout << "ServiceID: " << this->serviceID << '\n';
    std::cout << "Feedback: " << this->feedback << '\n';
    std::cout << "BookStatus: " << (this->bookStatus? "Yes" : "No") << '\n';
}


// operator overloading
std::ostream& operator<<(std::ostream& os, const serviceDone& obj){
    os << obj.ID << " " << obj.customerID << " " << obj.workerID
        << " " << obj.serviceID << " \"" << obj.feedback
        << "\" " << obj.bookStatus;
    return os;
}
std::istream& operator>>(std::istream& is,serviceDone& obj){
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
