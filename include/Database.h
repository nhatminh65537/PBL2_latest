#ifndef DATABASE_H
#define DATABASE_H
#include <string>
#include <map>
#include <functional>
#include "serviceDone.h"
#include "Appointment.h"
#include "User.h"


const std::string SERVICE_DONE_FILE = "../database/service-done.txt";
const std::string APPOINTMENTS_FILE = "../database/appointments.txt";
const std::string USERS_FILE = "../database/users.txt";


template<typename T>
class Database
{
    public:
        Database(const Database&) = delete; // Singleton design pattern
        Database& operator=(const Database&) = delete; // Singleton design pattern
        static Database<T>& Connect(const std::string& path); // Singleton design pattern
        ~Database();
        void Update(const std::string& ID,const T& newObject);
        void Update(const std::string& ID,const std::string& attributeName,const std::string& newValue);
        void Insert(const T& Object);
        void Delete(const std::string& ID);
        void Show();
    
        [[nodiscard]] int Count() const;
        [[nodiscard]] int Count(const std::string& attributeName, const std::string& val);
        [[nodiscard]] int Count(const std::vector<std::pair<std::string,std::string>>& conditions);
        [[nodiscard]] bool IsExist(const std::string& attr,const std::string& val) const;
        [[nodiscard]] bool IsEmpty() const;
        [[nodiscard]] std::string GetPath() const;

        typename std::map<std::string,T>::const_iterator begin() const; // Từ khóa typename để compiler hiểu const_iterator là 1 data type
        typename std::map<std::string,T>::const_iterator end() const;   // chứ không phải 1 thành viên của map<string,T>

        [[nodiscard]] T Get(const std::string& ID) const;
        Database<T>& Query(const std::string& attribute,const std::string& val);
        [[nodiscard]] std::vector<T> GetResults();
    private:
        explicit Database(const std::string& path); // Thêm keyword explicit để tránh implicit conversions (ép kiểu ngầm định)

        std::string genID(const T& obj);
        void loadData();
        void save();
        void index(const std::string&);
        void initIndex();
        void initMap();
        void removeIndex(const std::string& ID);
        void addIndex(const std::string& ID);
        void resetQuery();

        const std::string path;
        bool isQuerying = false;

        std::map<std::string,T>_list;
        std::vector<T>resultList; // temporary result list for Query method
        std::vector<std::string> resultIDs; // temporary ID list for Query method
        std::unordered_map<std::string,std::function<void(T&,const std::string&)>>updateMap; // Function map used for update function
        std::unordered_map<std::string,std::function<std::string(const T&)>>attributeMap; // Used to return specific attribute of an object
        std::unordered_map<std::string,std::multimap<std::string,std::string>>indexMapList;

};

extern Database<ServiceDone>& dbServiceDone;
extern Database<Appointment>& dbAppointment;
extern Database<User>& dbUser;

#endif // DATABASE_H
