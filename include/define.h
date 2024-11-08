//
// Created by LONG on 11/7/2024.
//

#ifndef DEFINE_H
#define DEFINE_H

#include <string>
#include <vector>

using namespace std;

enum Service{
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

#endif //DEFINE_H
