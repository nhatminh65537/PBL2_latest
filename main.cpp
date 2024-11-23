#include <iostream>

#include "Database.h"

#include "Stylist.h"


void test() {
    // Database<serviceDone>& dbServiceDone = Database<serviceDone>::Connect(SERVICE_DONE_FILE);
    // //dbServiceDone.Show();
    // //dbServiceDone.Query("feedback","hehe").Show();
    // vector<serviceDone> vt = dbServiceDone.Query("feedback","hehe").GetResults();
    // for (const serviceDone &v : vt) v.Show();
    // cout << "==========================\n";
    // vector<serviceDone> dt = dbServiceDone.GetResults();
    // for (const serviceDone &v : dt) v.Show();
    //
    // cout << "==========================\n";
    // cout << "==========================\n";
    // Database<Appointment>& hehe = Database<Appointment>::Connect(APPOINTMENTS_FILE);
    // //hehe.Insert();
    // hehe.Show();

    // Datetime a = Datetime::Now();
    // a.NextDay();
    // Datetime b = Datetime::Now();
    // b.PrevDay();
    // Datetime c = Datetime::Now();
    // //b.NextDay();
    // //c.PrevDay();
    // cout << a << '\n' << b << '\n' << c << '\n';
    // cout << IsBetween(a,c,b);
    Datetime a = Datetime::Now();
    a.NextDay();
    Datetime b = Datetime::Now();
    b.PrevDay();
    Datetime c = Datetime::Now();
    //b.NextDay();
    //c.PrevDay();
    std::cout << a << '\n' << b << '\n' << c << '\n';
    std::cout << IsBetween(a,b,c);
}


int main() {
    test();
    return 0;
}