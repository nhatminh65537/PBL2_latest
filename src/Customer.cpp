#include "Customer.h"
#include "Database.h"



Customer::Customer(const std::string& ID, const std::string &firstName, const std::string &lastName, const bool &gender, const int &age,
    const std::string &phoneNumber, const std::string &username, const std::string &password)
        : Member(ID,firstName,lastName,gender,age,phoneNumber,username,password){
    //ctor
}

Customer::~Customer()
{
    //dtor
}

void Customer::BookAppointment(const Datetime& date,const std::vector<Service>& serviceList) const {
    std::string appointmentID = std::to_string(dbAppointment.Count()+1);
    if (appointmentID.size() < 6)  appointmentID = std::string(6 - appointmentID.size(),'0') + appointmentID;

    dbAppointment.Insert(Appointment(appointmentID,date,this->ID,"null",serviceList));
}

void Customer::CancelAppointment(const std::string &appointmentID) const {
    if(dbAppointment.Get(appointmentID).GetCustomerID() == this->ID) {
        dbAppointment.Delete(appointmentID);
    }
    else {
        std::cout << "You don't have any appointments with ID " << appointmentID << '\n';
    }
}

void Customer::ViewAppointment() const{
    for (const auto& [key,object] : dbAppointment) {
        if (this->GetID() == object.GetCustomerID()) {
            object.Show();
        }
    }
}

bool Customer::Login(const std::string& username,const std::string& password) const {
    Database<Customer>& db = Database<Customer>::Connect(CUSTOMERS_FILE);
    if (db.IsExist("username",username)&&db.IsExist("password",password)) {
        return true;
    }
    return false;
}

bool Customer::Logout() const {
    return true;
}


std::ostream& operator<<(std::ostream& os, const Customer& obj) {
    os << obj.ID << ' ' << Replace(obj.firstName,' ','-')  << ' ' << Replace(obj.lastName,' ','-') << ' '
    << obj.username << ' ' << obj.gender << ' '
    << obj.age << ' ' << obj.phoneNumber;
    return os;
}

std::istream& operator>>(std::istream& is, Customer& obj) {
    is >> obj.ID >> obj.firstName >> obj.lastName >> obj.username >> obj.gender >> obj.age >> obj.phoneNumber;
    return is;
}
