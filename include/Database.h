#ifndef DATABASE_H
#define DATABASE_H
#include <string>
#include <map>
#include <functional>
#include "Identifiable.h"

using namespace std;

template<typename T>
class Database
{
    public:
        explicit Database(const string& path); // Thêm keyword explicit để tránh implicit conversions (ép kiểu ngầm định)
        ~Database();
        void Update(const string& ID,const T& newContent);
        void Update(const string& ID,const string& oldContent,const string& newContent);
        void Insert(const T& Object);
        void Delete(const string& ID);
        void Show() const;
        typename map<string,T>::const_iterator begin() const; // Từ khóa typename để compiler hiểu const_iterator là 1 data type
        typename map<string,T>::const_iterator end() const;   // chứ không phải 1 thành viên của map<string,T>
        [[nodiscard]] int Count() const;
        [[nodiscard]] bool IsExist(const string& ID) const;
        [[nodiscard]] bool IsEmpty() const;
        [[nodiscard]] T Get(const string& ID) const;
    private:
        // 2 functions ReadAll and Save shouldn't be allowed to call outside the class.
        // as we are gonna to call them once. When create the list and delete the list.
        void LoadData();
        void Save();
        const string path;
        map<string,T>_list;
        map<string,function<void(T&,const string&)>>attributeMap; // Function map used for update function
        void initAttributeMap();
};

#endif // DATABASE_H
