//
// Created by LONG on 11/7/2024.
//

#ifndef DEFINE_H
#define DEFINE_H

#include "Database.h"
#include <string>
#include <vector>



using namespace std;


const string SERVICE_DONE_FILE = "../database/service-done.txt";
const string APPOINTMENTS_FILE = "../database/appointments.txt";
const string CUSTOMERS_FILE = "../database/customers.txt";

class Appointment;
class Customer;
class serviceDone;

enum Service{
    NullRole,
    CatToc,
    NhuomToc,
    TayToc,
    UonToc,
    PhucHoiToc,
    TaoKieu,
    GoiDau
};

inline Database<serviceDone>dbCustomer (SERVICE_DONE_FILE);
inline Database<Appointment>dbAppointments (APPOINTMENTS_FILE);
inline Database<Customer>dbCustomers (CUSTOMERS_FILE);

string ServiceToString(const Service&);
Service StringToService(const string&);

vector<string> Split(const string&,const char& );
string Replace(const string&,const char&,const char&);
int ToNum(const string&);

bool isDigit(const char& x);
bool isNumber(const string&);
bool isBetween(const int&, const int&, const int&);
bool isBetween(const char&, const char&, const char&);


#endif //DEFINE_H
