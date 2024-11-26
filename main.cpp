// #include <iostream>

// #include "Database.h"
// #include "serviceDone.h"
// #include "Stylist.h"


// void test() {
//     serviceDone a;
//     std::cout << a << '\n';
//     a.Show();
//     //dbServiceDone.Query("rating","0").Show();
//     //std::vector<serviceDone> vt = dbServiceDone.Query("rating","hehe").GetResults();
//     // for (const serviceDone &v : vt) v.Show();
//     // std::cout << "==========================\n";
//     // std::vector<serviceDone> dt = dbServiceDone.GetResults();
//     // for (const serviceDone &v : dt) v.Show();
//     //
//     // cout << "==========================\n";
//     // cout << "==========================\n";
//     // Database<Appointment>& hehe = Database<Appointment>::Connect(APPOINTMENTS_FILE);
//     // //hehe.Insert();
//     // hehe.Show();

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

// }


#include "ui.h"
#include <functional>

std::function<void()> currentScreen = screenWelcome;

int test_ui() {
    while (currentScreen) {
        currentScreen();
    }
    return 0;
}

int main() {
    test_ui();
    return 0;
}