// #include <iostream>

#include "Database.h"
#include "serviceDone.h"
#include "Statistics.h"
#include "Member.h"
#include "Salon.h"
#include "test.h"



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


#include "ui.h"
#include <functional>

std::function<void()> currentScreen = screenWelcome;

int test_ui() 
{
    while (currentScreen) {
        currentScreen();
    }
    return 0;
}

// int main() {
//     test_ui();
//     void testthongke() {
//     dbServiceDone.Show();
//     std::cout << Statistics::GetCustomerCount(Datetime::Now()) << '\n';
// }
std::ofstream flog("log.txt");

int main() {
    //dbAppointment.Query("startTime","30/4/27/11/2024").Show();
    // Salon& salon = Salon::StartUp();
    // if (salon.Login("longqt321","123456")) {
    //     std::cout << salon.GetUserID() << ' ' << salon.GetUserRole() << '\n';
    //     salon.ShowAllAppointment(Datetime::StringToTime("30/4/27/11/2024"));
    // }
    
    try{
        test_ui();  
    } catch (std::exception e) {
        flog << e.what() << '\n';
    } catch (int code) {
        flog << "Error code: " << code << '\n';
    } catch (...) {
        flog << "Unknown error\n";
    }
    flog.close();
    return 0;
}