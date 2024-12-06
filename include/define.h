//
// Created by LONG on 11/7/2024.
//

#ifndef DEFINE_H
#define DEFINE_H

#include <string>
#include <vector>
#include "Datetime.h"

#define SERVICES_COUNT 8

class Appointment;
class Customer;
class serviceDone;

namespace ERROR_CODE
{
    const int LOGIN_USERNAME_EMPTY = 101;
    const int LOGIN_PASSWORD_EMPTY = 102;
    const int LOGIN_USER_AND_PASS_EMPTY = 103;
    const int LOGIN_INCORRECT_INPUT = 104;

    const int REGISTER_SOME_FIELD_EMPTY = 150;
    const int REGISTER_PASSWORD_NOT_MATCH = 151;
    const int REGISTER_USERNAME_EXIST = 152;

    const int CREATE_APPOINTMENT_BUSY_TIME = 201;
    const int CREATE_APPOINTMENT_CLOSED_TIME = 202;
    const int CREATE_APPOINTMENT_SERVICES_EMPTY = 203;
    const int CREATE_APPOINTMENT_CUSTOMER_EMPTY = 204;

    const int UPDATE_PROFILE_NAME_EMPTY = 249;
    const int UPDATE_PROFILE_FIRSTNAME_EMPTY = 250;
    const int UPDATE_PROFILE_LASTNAME_EMPTY = 251;
    const int UPDATE_PROFILE_INVALID_PASSWORD = 252;
    const int UPDATE_PROFILE_PASSWORD_NOT_MATCH = 253;
    const int UPDATE_PROFILE_PERSONINFO_EMPTY = 254;

    const int SALON_ACCESS_DENIED = 301;
    const int SALON_SOME_FIELD_EMPTY = 302;

    const int APPOINTMENT_IS_DONE = 351;
    const int APPOINTMENT_IS_CANCELLED = 352;
    const int APPOINTMENT_HAS_NULL_STYLIST = 353;   
}

enum Service{
    NullService,
    CatToc,
    NhuomToc,
    TayToc,
    UonToc,
    PhucHoiToc,
    TaoKieu,
    GoiDau
};

enum Filter {
    None,
    CaseSensitive
};

std::string ServiceToString(const int&);
Service StringToService(const std::string&);
;

std::vector<std::string> Split(const std::string&,const char& );
std::string Replace(const std::string&,const char&,const char&);
int ToNum(const std::string&);
char ToLower(const char&);
char ToUpper(const char&);
std::string ToLower(std::string);
std::string ToUpper(std::string);
bool IsDigit(const char& x);
bool IsNumber(const std::string&);
bool IsBetween(const int&, const int&, const int&);
bool IsBetween(const char&, const char&, const char&);

std::string Hash(const std::string& password);
bool Pwd_match(const std::string& input,const std::string& hashed_password);

#endif //DEFINE_H
