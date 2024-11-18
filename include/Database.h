#ifndef DATABASE_H
#define DATABASE_H
#include <string>
#include <map>
#include <functional>
#include "Identifiable.h"

const string SERVICE_DONE_FILE = "../database/service-done.txt";
const string APPOINTMENTS_FILE = "../database/appointments.txt";
const string CUSTOMERS_FILE = "../database/customers.txt";

using namespace std;

template<typename T>
class Database
{

    public:
        static Database<T>& Connect(const string& path); // Singleton design pattern
        ~Database();
        void Update(const string& ID,const T& newContent);
        void Update(const string& ID,const string& oldContent,const string& newContent);
        void Insert(const T& Object);
        void Delete(const string& ID);
        void Show() const;
        [[nodiscard]] string GetPath() const;
        typename map<string,T>::const_iterator begin() const; // Từ khóa typename để compiler hiểu const_iterator là 1 data type
        typename map<string,T>::const_iterator end() const;   // chứ không phải 1 thành viên của map<string,T>
        [[nodiscard]] int Count() const;
        [[nodiscard]] bool IsExist(const string& attr,const string& val) const;
        [[nodiscard]] bool IsEmpty() const;
        [[nodiscard]] T Get(const string& ID) const;
        [[nodiscard]] vector<T> Query(const string& attribute,const string& val);
        unordered_map<string,multimap<string,string>>indexMapList; // Cái này nằm ở private, nhưng tạm thời chuyển lên public để tiện cho việc debug
    private:
        explicit Database(const string& path); // Thêm keyword explicit để tránh implicit conversions (ép kiểu ngầm định)
        Database(const Database&) = delete;
        Database& operator=(const Database&) = delete;
        void loadData();
        void save();
        void index(const string&);
        void initIndex();
        void removeIndex(const string& ID);
        void addIndex(const string& ID);
        const string path;
        map<string,T>_list;
        vector<T>resultList;
        unordered_map<string,function<void(T&,const string&)>>updateMap; // Function map used for update function
        unordered_map<string,function<string(const T&)>>attributeMap; // Used to return specific attribute of an object
        void initMap();
};

#endif // DATABASE_H
