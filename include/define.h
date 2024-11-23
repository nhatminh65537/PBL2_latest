//
// Created by LONG on 11/7/2024.
//

#ifndef DEFINE_H
#define DEFINE_H

#include <string>
#include <vector>


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

std::string ServiceToString(const Service&);
Service StringToService(const std::string&);

std::vector<std::string> Split(const std::string&,const char& );
std::string Replace(const std::string&,const char&,const char&);
int ToNum(const std::string&);

bool IsDigit(const char& x);
bool IsNumber(const std::string&);
bool IsBetween(const int&, const int&, const int&);
bool IsBetween(const char&, const char&, const char&);

// Member *m;

#endif //DEFINE_H
