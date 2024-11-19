#include <iostream>
#include "define.h"
#include "Database.h"
#include "serviceDone.h"
#include "Customer.h"

using namespace std;

int main() {
    // Customer a("123","Tran Duc","Long",true,19,"123123123","longtran","123456");
    // Customer b;
    // vector<Service> services = {CatToc,NhuomToc};
    // Database<Appointment>& db = Database<Appointment>::Connect(APPOINTMENTS_FILE);
    // db.Show();
    // //a.Show();
    // //a.BookAppointment(dt,services);
    // b.ViewAppointment();
    Datetime a;
    Datetime b = a + 5;
    a.Show();
    (a-(+(-5))).Show();
    return 0;
}