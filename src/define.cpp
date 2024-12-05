//
// Created by LONG on 11/7/2024.
//
#include "define.h"
#include "stdexcept"


std::string ServiceToString(const int& service) {
    switch (service) {
        case CatToc: return "Haircut";
        case NhuomToc: return "Hair Dye";
        case TayToc: return "Hair Bleach";
        case UonToc: return "Hair Perm";
        case PhucHoiToc: return "Hair Restoration";
        case TaoKieu: return "Hair Styling";
        case GoiDau: return "Shampoo";
        default: return "Unknown";
    }
}

Service StringToService(const std::string& str) {
    if (str == "CatToc") return CatToc;
    if (str == "NhuomToc") return NhuomToc;
    if (str == "TayToc") return TayToc;
    if (str == "UonToc") return UonToc;
    if (str == "PhucHoiToc") return PhucHoiToc;
    if (str == "TaoKieu") return TaoKieu;
    if (str == "GoiDau") return GoiDau;
    throw std::invalid_argument("Unknown service: " + str);
}

std::vector<std::string> Split(const std::string& s, const char& delim) {
    std::vector<std::string> tokens;
    std::string token;
    for (const char& x : s) {
        if (x == delim) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else token += x;
    }
    if (!token.empty()) tokens.push_back(token);
    token.clear();
    return tokens;
}

std::string Replace(const std::string& s, const char& oldChar, const char& newChar) {
    std::string result = s;
    for (char& x : result) {
        if (x == oldChar) {
            x = newChar;
        }
    }
    return result;
}

int ToNum(const std::string& s) {
    if (!IsNumber(s)) return -1;
    int ans=0;
    for (const char& x : s) {
        ans = ans * 10 + (x - '0');
    }
    return ans;
}


bool IsBetween(const int& v, const int& L, const int& R) {
    return (v >= L && v <= R);
}

bool IsBetween(const char& v, const char& L, const char& R) {
    return (v >= L && v <= R);
}

bool IsDigit(const char &x) {
    return IsBetween(x,'0','9');
}

bool IsNumber(const std::string& s) {
    for (const char& x : s) {
        if (!IsBetween(x, '0', '9'))    return false;
    }
    return true;
}

char ToLower(const char& ch) {
    if (IsBetween(ch, 'A', 'Z')) return ch - 'A' + 'a';
    return ch;
}
char ToUpper(const char& ch) {
    if (IsBetween(ch, 'a', 'z')) return ch - 'a' + 'A';
    return ch;
}
std::string ToLower(std::string s) {
    for (char& x : s) x = ToLower(x);
    return s;
}
std::string ToUpper(std::string s) {
    for (char& x : s) x = ToUpper(x);
    return s;
}
