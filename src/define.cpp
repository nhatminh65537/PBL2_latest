//
// Created by LONG on 11/7/2024.
//
#include "define.h"

#include <algorithm>
#include <bits/unordered_map.h>

#include "stdexcept"


std::string ServiceToString(const int& service) {
    switch (service) {
        case CatToc: return "Cat toc";
        case NhuomToc: return "Nhuom toc";
        case TayToc: return "Tay toc";
        case UonToc: return "Uon toc";
        case PhucHoiToc: return "Phuc hoi toc";
        case TaoKieu: return "Tao kieu";
        case GoiDau: return "Goi dau";
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

std::string Hash(const std::string& password) {
    const int MOD1 = 1000000007; // Số nguyên tố lớn
    const int MOD2 = 1000000009; // Một số nguyên tố lớn khác
    const int BASE1 = 31;        // Cơ sở băm 1
    const int BASE2 = 37;        // Cơ sở băm 2
    const int OUTPUT_LENGTH = 128; // Độ dài cố định của hashResult
    const int HASH_BASE = 64;

    std::string hashResult="";
    unsigned long long hash1=0, hash2=0;
    for (int i=0;i<OUTPUT_LENGTH;i++) {
        for (const char& x : password) {
            hash1 = (hash1*BASE1 + x + i)%MOD1;
            hash2 = (hash2*BASE2 + x + i)%MOD2;
        }
    }
    auto toHex = [](int x) -> char {
        return (x < 10 ? x+'0' : x-10+'A');
    };

    const unsigned long long hash1_tmp = hash1;
    const unsigned long long hash2_tmp = hash2;

    while (hashResult.size()<OUTPUT_LENGTH) {
        int digit1 = hash1 % HASH_BASE;
        int digit2 = hash2 % HASH_BASE;

        int val = digit1^digit2;
        //std::cout << val << ' ';
        hashResult += toHex(val);

        hash1 /= HASH_BASE;
        hash2 /= HASH_BASE;

        if (hash1 == 0 && hashResult.size()<OUTPUT_LENGTH) hash1 = hash1_tmp*hashResult.size();
        if (hash2 == 0 && hashResult.size()<OUTPUT_LENGTH) hash2 = hash2_tmp*hashResult.size();
    }

    return hashResult;
}

bool Pwd_match(const std::string& input,const std::string& hashed_password) {
    //std::cout << input << '\n' << hashed_password << '\n';
    return Hash(input) == hashed_password;
}