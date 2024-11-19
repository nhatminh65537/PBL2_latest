#include "Customer.h"
#include "Database.h"

Database<Appointment>& dbAppointments = Database<Appointment>::Connect(APPOINTMENTS_FILE);

Customer::Customer(const string& ID, const string &firstName, const string &lastName, const bool &gender, const int &age,
    const string &phoneNumber, const string &username, const string &password)
        : Member(ID,firstName,lastName,gender,age,phoneNumber,username,password){
    //ctor
}

Customer::~Customer()
{
    //dtor
}

void Customer::BookAppointment(const Datetime& date,const vector<Service>& serviceList) const {
    string appointmentID = to_string(dbAppointments.Count()+1);
    if (appointmentID.size() < 6)  appointmentID = string(6 - appointmentID.size(),'0') + appointmentID;

    dbAppointments.Insert(Appointment(appointmentID,date,this->ID,serviceList));
}

void Customer::CancelAppointment(const string &appointmentID) const {
    if(dbAppointments.Get(appointmentID).GetCustomerID() == this->ID) {
        dbAppointments.Delete(appointmentID);
    }
    else {
        cout << "You don't have any appointments with ID " << appointmentID << '\n';
    }
}

void Customer::ViewAppointment() const{
    for (const auto& [key,object] : dbAppointments) {
        if (this->GetID() == object.GetCustomerID()) {
            object.Show();
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
