//
// Created by LONG on 11/7/2024.
//

#ifndef DEFINE_H
#define DEFINE_H

#include <string>
#include <vector>



using namespace std;




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
