#include <Appointment.h>
#include <iostream>
#include <string>
#include <vector>
#include "serviceDone.h"
#include "Database.h"
#include "define.h"
#include "Customer.h"

using namespace std;
const string SERVICE_DONE_FILE = "../database/service-done.txt";
const string APPOINTMENTS_FILE = "../database/appointments.txt";
const string CUSTOMERS_FILE = "../database/customers.txt";
// Các hàm thống kê sẽ được nhét vào class Database. Để tiện lấy thông tin
// Không dùng vector để lưu trữ cho database nữa. Dùng map, vì map tiện hơn và chạy nhanh hơn

int main()
{
    Database<Customer>myList (CUSTOMERS_FILE);
    Customer tester("9","Tran Duc","Long",
        true,19,"0912345678",
        "helloworld","123123");
    myList.Insert(tester);
    myList.Show();
    return 0;
}