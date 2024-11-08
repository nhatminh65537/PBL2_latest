#include "Customer.h"

Customer::Customer(const string& ID, const string &firstName, const string &lastName, const bool &gender, const int &age,
    const string &phoneNumber, const string &username, const string &password)
        : Member(ID,firstName,lastName,gender,age,phoneNumber,username,password){
    //ctor
}

Customer::~Customer()
{
    //dtor
}

void Customer::BookAppointment(Database<Appointment> &db, const vector<Service>& serviceList) const {
    string appointmentID = to_string(db.Count()+1);
    if (appointmentID.size() < 6)  appointmentID = string(6 - appointmentID.size(),'0') + appointmentID;

    db.Insert(Appointment(appointmentID,this->GetID(),serviceList));
}

void Customer::CancelAppointment(Database<Appointment> &db, const string &appointmentID) const {
    db.Delete(appointmentID);
}

void Customer::ViewAppointment(const Database<Appointment>& db) const{
    for (const auto& x : db) {
        if (this->GetID() == db.Get(x.first).GetCustomerID()) {
            db.Get(x.first).Show();
        }
    }
}

ostream& operator<<(ostream& os, const Customer& obj) {
    os << obj.ID << ' ' << Replace(obj.firstName,' ','-')  << ' ' << Replace(obj.lastName,' ','-') << ' '
    << obj.username << ' ' << obj.gender << ' '
    << obj.age << ' ' << obj.phoneNumber;
    return os;
}

istream& operator>>(istream& is, Customer& obj) {
    is >> obj.ID >> obj.firstName >> obj.lastName >> obj.username >> obj.gender >> obj.age >> obj.phoneNumber;
    return is;
}
