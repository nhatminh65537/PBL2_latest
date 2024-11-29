// #include <iostream>

#include "Database.h"
#include "serviceDone.h"
#include "Statistics.h"
#include "Member.h"
#include "Salon.h"


void test() {
    // serviceDone a;
    // std::cout << a << '\n';
    // a.Show();
    //dbServiceDone.Query("rating","0").Show();
    //std::vector<serviceDone> vt = dbServiceDone.Query("rating","hehe").GetResults();
    // for (const serviceDone &v : vt) v.Show();
    // std::cout << "==========================\n";
    // std::vector<serviceDone> dt = dbServiceDone.GetResults();
    // for (const serviceDone &v : dt) v.Show();
    //
    // cout << "==========================\n";
    // cout << "==========================\n";
    // Database<Appointment>& hehe = Database<Appointment>::Connect(APPOINTMENTS_FILE);
    // //hehe.Insert();
    // hehe.Show();

//     // Datetime a = Datetime::Now();
//     // a.NextDay();
//     // Datetime b = Datetime::Now();
//     // b.PrevDay();
//     // Datetime c = Datetime::Now();
//     // //b.NextDay();
//     // //c.PrevDay();
//     // cout << a << '\n' << b << '\n' << c << '\n';
//     // cout << IsBetween(a,c,b);
//     // Datetime a = Datetime::Now();
//     // a.NextDay();
//     // Datetime b = Datetime::Now();
//     // b.PrevDay();
//     // Datetime c = Datetime::Now();
//     // //b.NextDay();
//     // //c.PrevDay();
//     // std::cout << a << '\n' << b << '\n' << c << '\n';
//     // std::cout << IsBetween(a,b,c);

}

int main() {
    dbAppointment.Query("status","Done").Query("stylistID","HEHE").Show();
    //dbAppointment.Query("startTime","30/4/27/11/2024").Show();
    //freopen("log.txt","w",stdout);
    //Salon& salon = Salon::StartUp();
    //salon.Register("Tran","Long","test","pass","pass",true,"09999999",3);
    // if (salon.Login("longqt321","123456")) {
    //     std::cout << salon.GetUserID() << ' ' << salon.GetUserRole() << '\n';
    //
    // }
    //test_ui();
    return 0;
}