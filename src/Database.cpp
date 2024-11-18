#include "Database.h"
#include <Customer.h>
#include <iostream>
#include <fstream>
#include "serviceDone.h"
#include "Appointment.h"

using namespace std;

template class Database<serviceDone>;
template class Database<Appointment>;
template class Database<Customer>;

template<typename T>
Database<T>& Database<T>::Connect(const string& path) {
    static Database<T> database(path);
    return database;
}

template<typename T>
Database<T>::Database(const string& path) : path(path) {
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
    ifstream inputFile(this->path.c_str(), ios::in);
    if (!inputFile.is_open()) {
        cerr << "File " << this->path << " not found!";
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
    ofstream outputFile(this->path.c_str());
    if (!outputFile.is_open()) {
        cerr << "File " << this->path << " not found!";
        exit(1);
    }
    for (const auto& it : this->_list){
        outputFile << it.second << '\n';
    }
    outputFile.close();
}

template<typename T>
void Database<T>::Update(const string& ID,const T& newObj){
    if (newObj.GetID() != ID){
        cerr << ID << " does not exists in database\n";
        exit(1);
    }
    this->_list[ID] = newObj;
}

template<typename T>
void Database<T>::Update(const string& ID,const string& attributeName, const string& newVal){
    if (!this->_list.contains(ID)){
        cerr << ID << " does not exists in database\n";
        exit(1);
    }
    if (!updateMap.contains(attributeName)) {
        cerr << attributeName << " does not exists in updateMap\n";
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
        cerr << "ID " << obj.GetID() << " already exists\n";
        exit(1);
    }
    this->_list[obj.GetID()] = obj;
    addIndex(obj.GetID());
}

template<typename T>
void Database<T>::Delete(const string& id){
    // If object does not exist in map
    if (!this->_list.contains(id)){
        cerr << id << " does not exist\n";
        exit(1);
    }
    removeIndex(id);
    this->_list.erase(id);
}

template<typename T>
void Database<T>::Show() {

    if (!this->resultList.empty()) {
        if (this->resultList[0].GetID() != "null"){
            for (const auto& it : this->resultList) {
                cout << it << '\n';
            }
        }
        this->resultList.clear();
        return;
    }
    for (const auto& it : this->_list){
        cout << it.second << '\n';
    }
}

template<typename T>
string Database<T>::GetPath() const {
    return this->path;
}


template<typename T>
typename map<string,T>::const_iterator Database<T>::begin() const {
    return this->_list.begin();
}

template<typename T>
typename map<string,T>::const_iterator Database<T>::end() const {
    return this->_list.end();
}

template<typename T>
int Database<T>::Count() const{
    return this->_list.size();
}

template<typename T>
bool Database<T>::IsExist(const string& attr, const string &val) const {
    if (!this->attributeMap.contains(attr)) { // class T does not have this attribute
        cerr << "Attribute " << attr << " does not exist\n";
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
T Database<T>::Get(const string &ID) const {
    if (!this->_list.contains(ID)) {
        cerr << ID << " does not exist\n";
        exit(1);
    }
    return this->_list.at(ID);
}

template <typename T>
Database<T>& Database<T>::Query(const string& attr,const string& val) {
    if (!this->attributeMap.contains(attr)) { // class T does not have this attribute
        cerr << "Attribute " << attr << " does not exist\n";
        exit(1);
    }

    vector<T> res;
    if (!this->resultList.empty()) {
        for (const auto& obj : this->resultList) {
            if (this->attributeMap.at(attr)(obj) == val) {
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
                if(this->attributeMap.at(attr)(obj) == val) {
                    res.push_back(obj);
                }
            }
        }
    }
    if (res.empty()) {
        T obj;
        res.push_back(obj);
    }
    this->resultList = move(res); // Hàm "move" là chuyển quyền sở hữu data từ res qua this->resultList  => Tăng hiệu suất
    return *this;
}

template<typename T>
vector<T> Database<T>::GetResults() const {
    return this->resultList;
}

// Private method

template<typename T>
void Database<T>::index(const string& attribute) {
    if (indexMapList.contains(attribute)) {
        cerr << "Attribute " << attribute << " already exists\n";
        return;
    }
    if (!attributeMap.contains(attribute)) {
        cerr << "Attribute " << attribute << " does not exists\n";
        return;
    }
    multimap<string,string>indexMap;
    for (const auto& [ID,obj] : this->_list) {
        indexMap.insert({attributeMap[attribute](obj),ID});
    }
    indexMapList[attribute] = indexMap;
}

template<typename T>
void Database<T>::addIndex(const string& ID) {
    T obj = this->_list.at(ID);
    for (auto& [attribute,indexMap] : indexMapList) { // update indexMapList
        if (attributeMap.contains(attribute)) {
            indexMap.insert({attributeMap[attribute](obj), ID});
        }
    }
}

template<typename T>
void Database<T>::removeIndex(const string& id) {
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
    index("workerID");
}

template<>
void Database<Appointment>::initIndex() {

}

template<>
void Database<Customer>::initIndex() {
}

template<>
void Database<serviceDone>::initMap(){
    attributeMap["ID"] = [](const serviceDone& obj) -> string {
        return obj.GetID();
    };
    attributeMap["customerID"] = [](const serviceDone& obj) -> string {
        return obj.GetCustomerID();
    };
    attributeMap["workerID"] = [](const serviceDone& obj) -> string {
        return obj.GetWorkerID();
    };
    attributeMap["serviceID"] = [](const serviceDone& obj) -> string {
        return obj.GetServiceID();
    };
    attributeMap["feedback"] = [](const serviceDone& obj) -> string {
        return obj.GetFeedBack();
    };
    attributeMap["bookStatus"] = [](const serviceDone& obj) -> string {
        return to_string(obj.GetBookStatus());
    };
    updateMap["customerID"] = [](serviceDone& obj, const string& newVal) {
        obj.SetCustomerID(newVal);
    };
    updateMap["workerID"] = [](serviceDone& obj, const string& newVal) {
        obj.SetWorkerID(newVal);
    };
    updateMap["serviceID"] = [](serviceDone& obj, const string& newVal) {
        obj.SetServiceID(newVal);
    };
    updateMap["feedback"] = [](serviceDone& obj, const string& newVal) {
        obj.SetFeedBack(newVal); // Thêm 2 dấu " để đúng format của feedback.
    };
    updateMap["bookStatus"] = [](serviceDone& obj, const string& newVal) {
        obj.SetBookStatus(static_cast<bool>(ToNum(newVal)));
    };
}

template<>
void Database<Appointment>::initMap() {

}

template<>
void Database<Customer>::initMap() {

}
