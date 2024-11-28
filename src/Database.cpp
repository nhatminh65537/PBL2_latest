#include "Database.h"
#include <iostream>
#include <fstream>
#include <utility>
#include "serviceDone.h"
#include "test.h"

template class Database<serviceDone>;
template class Database<Appointment>;
template class Database<Member>;


Database<serviceDone>& dbServiceDone = Database<serviceDone>::Connect(SERVICE_DONE_FILE);
Database<Appointment>& dbAppointment = Database<Appointment>::Connect(APPOINTMENTS_FILE);
Database<Member>& dbUser = Database<Member>::Connect(USERS_FILE);

template<typename T>
Database<T>& Database<T>::Connect(const std::string& path) {
    static Database<T> database(path);
    return database;
}

template<typename T>
Database<T>::Database(const std::string&  path) : path(path) {
    loadData();
    initMap();
    initIndex();
}

template<typename T>
Database<T>::~Database() {
    this->save();
}


template<typename T>
void Database<T>::loadData(){
    std::ifstream inputFile(this->path.c_str(), std::ios::in);
    if (!inputFile.is_open()) {
        std::cerr << "File " << this->path << " not found!";
        exit(1);
    }
    T tempObject;
    while (inputFile >> tempObject) {
        this->_list[tempObject.GetID()] = tempObject;
    }
    inputFile.close();
}

template<typename T>
void Database<T>::save() {
    std::ofstream outputFile(this->path.c_str());
    if (!outputFile.is_open()) {
        std::cerr << "File " << this->path << " not found!";
        exit(1);
    }
    for (const auto& it : this->_list){
        outputFile << it.second << '\n';
    }
    outputFile.close();
}

template<typename T>
void Database<T>::Update(const std::string& ID,const T& newObj){
    if (newObj.GetID() != ID){
        std::cerr << ID << " does not exists in database\n";
        exit(1);
    }
    this->_list[ID] = newObj;
}

template<typename T>
void Database<T>::Update(const std::string& ID,const std::string& attributeName, const std::string& newVal){
    if (!this->_list.contains(ID)){
        std::cerr << ID << " does not exists in database\n";
        exit(1);
    }
    if (!updateMap.contains(attributeName)) {
        std::cerr << attributeName << " does not exists in updateMap\n";
        exit(1);
    }
    removeIndex(ID);
    updateMap[attributeName](this->_list.at(ID),newVal);
    addIndex(ID);
}

template<typename T>
void Database<T>::Insert(const T& obj) {
    // If object is already in map
    if (this->_list.contains(obj.GetID())){
        std::cerr << "ID " << obj.GetID() << " already exists\n";
        exit(1);
    }
    time_t now = time(nullptr);
    std::string ID = obj.GetID();
    if (ID == "null") ID = std::to_string(now);
    T tmpObj = obj;
    tmpObj.SetID(ID);
    this->_list[ID] = tmpObj;
    addIndex(ID);
}

template<typename T>
void Database<T>::Delete(const std::string& id){
    // If object does not exist in map
    if (!this->_list.contains(id)){
        std::cerr << id << " does not exist\n";
        exit(1);
    }
    removeIndex(id);
    this->_list.erase(id);
}

template<typename T>
void Database<T>::Show() {
    if (this->isQuerying==true) {
        for (const auto& it : this->resultList) {
            it.Show();
        }
        resetQuery();
        return;
    }
    for (const auto& it : this->_list){
        it.second.Show();
    }
}

template<typename T>
std::string Database<T>::GetPath() const {
    return this->path;
}


template<typename T>
typename std::map<std::string,T>::const_iterator Database<T>::begin() const {
    return this->_list.begin();
}

template<typename T>
typename std::map<std::string,T>::const_iterator Database<T>::end() const {
    return this->_list.end();
}

template<typename T>
int Database<T>::Count() const{
    return this->_list.size();
}

template<typename T>
bool Database<T>::IsExist(const std::string& attr, const std::string &val) const {
    if (!this->attributeMap.contains(attr)) { // class T does not have this attribute
        std::cerr << "Attribute " << attr << " does not exist\n";
        exit(1);
    }
    if (this->indexMapList.contains(attr)) { // If this attribute is indexed
        return this->indexMapList.at(attr).contains(val); // There exists a value <val> of attribute <attr>
    }
    else {
        for (const auto& [ID,obj] : this->_list) {
            if(this->attributeMap.at(attr)(obj) == val) {
                return true;
            }
        }
    }
    return false;
}

template<typename T>
bool Database<T>::IsEmpty() const {
    return this->_list.size() == 0;
}

template<typename T>
T Database<T>::Get(const std::string &ID) const {
    // logf << "Get\n\t" << ID << '\n';
    if (!this->_list.contains(ID)) {
        std::cerr << ID << " does not exist\n";
        exit(1);
    }
    return this->_list.at(ID);
}

template <typename T>
Database<T>& Database<T>::Query(const std::string& attr,const std::string& val) {
    if (!this->attributeMap.contains(attr)) { // class T does not have this attribute
        std::cerr << "Attribute " << attr << " does not exist\n";
        exit(1);
    }

    auto equal = [&](std::string val1,std::string val2) -> bool {
        if (attr == "startTime") { // If checking for startTime, we just have to check if they are on the same day.
            std::vector<std::string> vt1 = Split(val1,'/');
            std::vector<std::string> vt2 = Split(val2,'/');
            if (vt1.size() != vt2.size() || vt1.size() < 5) return false;
            val1 = vt1[2] + "/" + vt1[3] + "/" + vt1[4];
            val2 = vt2[2] + "/" + vt2[3] + "/" + vt2[4];
        }
        return val1 == val2;
    };

    std::vector<T> res;
    if (this->isQuerying==true) {
        for (const auto& obj : this->resultList) {
            if (equal(this->attributeMap.at(attr)(obj),val)) {
                res.push_back(obj);
            }
        }
    }

    else {
        if (this->indexMapList.contains(attr)) { // If this attribute is indexed
            auto range = this->indexMapList[attr].equal_range(val);
            for (auto it = range.first; it != range.second;++it) {
                res.push_back(this->_list[it->second]);
            }
        }
        else {
            for (const auto& [ID,obj] : this->_list) {
                if(equal(this->attributeMap.at(attr)(obj),val)) {
                    res.push_back(obj);
                }
            }
        }
        this->isQuerying=true;
    }
    this->resultList = move(res); // Hàm "move" là chuyển quyền sở hữu data từ res qua this->resultList  => Tăng hiệu suất
    return *this;
}

template<typename T>
std::vector<T> Database<T>::GetResults() {
    std::vector<T> res = this->resultList;
    if (this->isQuerying==true) {
        resetQuery();
    }
    else {
        for (const auto& [ID,obj] : this->_list) res.push_back(obj);
    }
    return res;
}

// Private method

template<typename T>
void Database<T>::resetQuery() {
    this->resultList.clear();
    this->isQuerying=false;
}


template<typename T>
void Database<T>::index(const std::string& attribute) {
    if (indexMapList.contains(attribute)) {
        std::cerr << "Attribute " << attribute << " was already indexed\n";
        return;
    }
    if (!attributeMap.contains(attribute)) {
        std::cerr << "Attribute " << attribute << " does not exist\n";
        return;
    }
    std::multimap<std::string,std::string>indexMap;
    for (const auto& [ID,obj] : this->_list) {
        indexMap.insert({attributeMap[attribute](obj),ID});
    }
    indexMapList[attribute] = indexMap;
}

template<typename T>
void Database<T>::addIndex(const std::string& ID) {
    T obj = this->_list.at(ID);
    for (auto& [attribute,indexMap] : indexMapList) { // update indexMapList
        if (attributeMap.contains(attribute)) {
            indexMap.insert({attributeMap[attribute](obj), ID});
        }
    }
}

template<typename T>
void Database<T>::removeIndex(const std::string& id) {
    T obj = this->_list.at(id);
    for (auto& [attribute,indexMap] : indexMapList) {
        if (!attributeMap.contains(attribute)) continue;

        auto range = indexMap.equal_range(attributeMap[attribute](obj));
        for (auto it = range.first; it != range.second; ++it) {
            if (it->second == id) {
                indexMap.erase(it);
                break;
            }
        }
    }
}

template<>
void Database<serviceDone>::initIndex() {
    index("customerID");
    index("stylistID");
}

template<>
void Database<Appointment>::initIndex() {
    index("stylistID");
    index("customerID");
}

template<>
void Database<Member>::initIndex() {
    index("firstName");
}


template<>
void Database<serviceDone>::initMap(){
    attributeMap["ID"] = [](const serviceDone& obj) -> std::string {
        return obj.GetID();
    };
    attributeMap["customerID"] = [](const serviceDone& obj) -> std::string {
        return obj.GetCustomerID();
    };
    attributeMap["stylistID"] = [](const serviceDone& obj) -> std::string {
        return obj.GetStylistID();
    };
    attributeMap["serviceID"] = [](const serviceDone& obj) -> std::string {
        return std::to_string(obj.GetServiceID());
    };
    attributeMap["rating"] = [](const serviceDone& obj) -> std::string {
        return std::to_string(obj.GetRating());
    };
    attributeMap["bookStatus"] = [](const serviceDone& obj) -> std::string {
        return std::to_string(obj.GetBookStatus());
    };
    updateMap["customerID"] = [](serviceDone& obj, const std::string& newVal) {
        obj.SetCustomerID(newVal);
    };
    updateMap["stylistID"] = [](serviceDone& obj, const std::string& newVal) {
        obj.SetStylistID(newVal);
    };
    updateMap["serviceID"] = [](serviceDone& obj, const std::string& newVal) {
        obj.SetServiceID(ToNum(newVal));
    };
    updateMap["rating"] = [](serviceDone& obj, const std::string& newVal) {
        obj.SetRating(ToNum(newVal));
    };
    updateMap["bookStatus"] = [](serviceDone& obj, const std::string& newVal) {
        obj.SetBookStatus(static_cast<bool>(ToNum(newVal)));
    };
}

template<>
void Database<Appointment>::initMap() {
    attributeMap["ID"] = [](const Appointment& obj) -> std::string {
        return obj.GetID();
    };
    attributeMap["stylistID"] = [](const Appointment& obj) -> std::string {
        return obj.GetStylistID();
    };
    attributeMap["customerID"] = [](const Appointment& obj) -> std::string {
        return obj.GetCustomerID();
    };
    attributeMap["startTime"] = [](const Appointment& obj) -> std::string {
        const Datetime dt = obj.GetStartTime();
        return Datetime::TimeToString(dt);
    };
    attributeMap["time"] = [](const Appointment& obj) -> std::string {
        const Datetime dt = obj.GetStartTime();
        return Datetime::TimeToString(dt);
    };
    attributeMap["status"] = [](const Appointment& obj) -> std::string {
        return obj.GetStatus();
    };
    updateMap["stylistID"] = [](Appointment& obj, const std::string& newVal) {
        obj.SetStylistID(newVal);
    };
    updateMap["customerID"] = [](Appointment& obj, const std::string& newVal) {
        obj.SetCustomerID(newVal);
    };
    updateMap["startTime"] = [](Appointment& obj, const std::string& newVal) {
        std::vector<std::string> tokens = Split(newVal,'/'); // minmin/hourhour/dd/mm/yyyy
        if (tokens.size()<5) throw std::runtime_error("Datetime error");
        obj.SetStartTime(Datetime(ToNum(tokens[0]),ToNum(tokens[1]),ToNum(tokens[2]),ToNum(tokens[3]),ToNum(tokens[4])));
    };
    updateMap["status"] = [](Appointment& obj,const std::string& newVal) {
        obj.SetStatus(newVal);
    };
}

template<>
void Database<Member>::initMap() {
    attributeMap["ID"] = [](const Member& obj) -> std::string {
        return obj.GetID();
    };
    attributeMap["firstName"] = [](const Member& obj) -> std::string {
        return obj.GetFirstName();
    };
    attributeMap["lastName"] = [](const Member& obj) -> std::string {
        return obj.GetLastName();
    };
    attributeMap["gender"] = [](const Member& obj) -> std::string {
        return (obj.GetGender() == 1 ? "Male" : "Female");
    };
    attributeMap["username"] = [](const Member& obj) -> std::string {
        return obj.GetUserName();
    };
    attributeMap["password"] = [](const Member& obj) -> std::string {
        return obj.GetPassword();
    };
    attributeMap["role"] = [](const Member& obj) -> std::string {
        return std::to_string(obj.GetRole());
    };
    attributeMap["phoneNumber"] = [](const Member& obj) -> std::string {
        return obj.GetPhoneNumber();
    };
    updateMap["firstName"] = [](Member& obj, const std::string& newVal) {
        obj.SetFirstName(newVal);
    };
    updateMap["lastName"] = [](Member& obj, const std::string& newVal) {
        obj.SetLastName(newVal);
    };
    updateMap["gender"] = [](Member& obj, const std::string& newVal) {
        obj.SetGender(newVal == "Male" ? true : false);
    };
    updateMap["username"] = [](Member& obj, const std::string& newVal) {
        obj.SetUserName(newVal);
    };
    updateMap["password"] = [](Member& obj, const std::string& newVal) {
        obj.SetPassword(newVal);
    };
    updateMap["role"] = [](Member& obj, const std::string& newVal) {
        obj.SetRole(ToNum(newVal));
    };
    updateMap["phoneNumber"] = [](Member& obj, const std::string& newVal) {
        obj.SetPhoneNumber(newVal);
    };
}
