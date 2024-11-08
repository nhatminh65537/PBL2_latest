#include "Database.h"

#include <Customer.h>

#include "serviceDone.h"
#include "Appointment.h"
#include <iostream>
#include <fstream>

using namespace std;

template class Database<serviceDone>;
template class Database<Appointment>;
template class Database<Customer>;

template<typename T>
Database<T>::Database(const string& path) : path(path) {
    LoadData();
    initAttributeMap();
}

template<typename T>
Database<T>::~Database() {
    this->Save();
}


template<typename T>
void Database<T>::LoadData(){
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
void Database<T>::Save() {
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
    this->attributeMap[attributeName](this->_list[ID],newVal);
}

template<typename T>
void Database<T>::Insert(const T& obj) {
    // If object is already in map
    if (this->_list.contains(obj.GetID())){
        cerr << obj.GetID() << " already exists\n";
        exit(1);
    }
    this->_list[obj.GetID()] = obj;
}

template<typename T>
void Database<T>::Delete(const string& id){
    // If object does not exist in map
    if (!this->_list.contains(id)){
        cerr << id << " does not exist\n";
        exit(1);
    }
    this->_list.erase(id);
}

template<typename T>
void Database<T>::Show() const {
    for (const auto& it : this->_list){
        cout << it.second << '\n';
    }
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
bool Database<T>::IsExist(const string &ID) const {
    return this->_list.contains(ID);// this->_list.find(ID) != this->_list.end()
}

template<typename T>
bool Database<T>::IsEmpty() const {
    return this->_list.size() == 0;
}

template<typename T>
T Database<T>::Get(const string &ID) const {
    if (!this->_list.contains(ID)) {
        cerr << ID << " does not exists\n";
        exit(1);
    }
    return this->_list.at(ID);
}

template<>
void Database<serviceDone>::initAttributeMap(){
    // lambda function
    attributeMap["serviceDoneID"] = [](serviceDone& obj,const string& newVal){
        obj.SetID(newVal);
    };
    attributeMap["customerID"] = [](serviceDone& obj,const string& newVal){
        obj.SetCustomerID(newVal);
    };
    attributeMap["workerID"] = [](serviceDone& obj,const string& newVal){
        obj.SetWorkerID(newVal);
    };
    attributeMap["serviceID"] = [](serviceDone& obj,const string& newVal){
        obj.SetServiceID(newVal);
    };
    attributeMap["feedback"] = [](serviceDone& obj,const string& newVal){
        obj.SetFeedBack('"' + newVal + '"'); // Thêm 2 dấu " ở đầu và cuối để phân biệt feedback với các phần khác
    };
}

template<>
void Database<Appointment>::initAttributeMap() {

}

template<>
void Database<Customer>::initAttributeMap() {

}
