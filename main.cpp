#include <iostream>
#include "Database.h"
#include "serviceDone.h"
#include "Member.h"
#include "Customer.h"
#include "Stylist.h"
using namespace std;

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
    // Database<Stylist>& db = Database<Stylist>::Connect(STYLISTS_FILE);
    // Stylist a = db.Get("123");
    // a.ViewScheduleByDay();
    // cout << "==========================\n";
    // Database<Appointment>& hehe = Database<Appointment>::Connect(APPOINTMENTS_FILE);
    // hehe.Show();

    Datetime a = Datetime::Now();
    a.NextDay();
    Datetime b = Datetime::Now();
    b.PrevDay();
    Datetime c = Datetime::Now();
    //b.NextDay();
    //c.PrevDay();
    cout << a << '\n' << b << '\n' << c << '\n';
    cout << IsBetween(a,c,b);
}


int main() {
    test();
    return 0;
}