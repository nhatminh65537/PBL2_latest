#include <filesystem>
#include "Database.h"
#include "Datetime.h"
#include "serviceDone.h"
#include "Statistics.h"
#include "define.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <unordered_set>

Statistics::Statistics() {
    //ctor
}

float Statistics::GetAverage(const std::vector<float> vt) {
    float sum = 0.0;
    for(const auto& x : vt) {
        sum += x;
    }
    return sum/vt.size();
}

float Statistics::GetStandardDeviation(const std::vector<float> vt) {
    float avg = GetAverage(vt);
    float sum = 0.0;
    for(const auto& x : vt) {
        sum += (x - avg)*(x - avg);
    }
    return std::sqrt(sum/vt.size());
}

int Statistics::GetServiceDoneCount(const Datetime& T){
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();


    return DB.size();
}

int Statistics::GetServiceDoneCount_UpToNow() {
    std::vector<ServiceDone> DB = dbServiceDone.GetResults();

    return DB.size();
}

std::vector<int> Statistics::GetServiceCount(const Datetime& T){
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceCount(SERVICES_COUNT, 0);

    for(const auto& service : DB){
        serviceCount[service.GetServiceID()]++;
    }

    return serviceCount;
}

std::vector<int> Statistics::GetServiceCount_UpToNow(){
    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceCount(8,0);

    for(const auto& service : DB){
        serviceCount[service.GetServiceID()]++;
    }
    return serviceCount;
}

std::vector<int> Statistics::GetServiceRateCount(const Datetime& T){
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceRateCount(SERVICES_COUNT, 0);

    for(const auto& service : DB){
        if (service.GetRating() == 0) continue;
        serviceRateCount[service.GetRating()]++;
    }

    return serviceRateCount;
}

std::vector<int> Statistics::GetServiceRateSum(const Datetime& T){
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceRateSum(SERVICES_COUNT, 0);

    for(const auto& service : DB){
        if (service.GetRating() == 0) continue;
        serviceRateSum[service.GetRating()] += service.GetRating();
    }

    return serviceRateSum;
}

std::vector<int> Statistics::GetServiceCustomerCount(const Datetime& T){
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceCustomerCount(SERVICES_COUNT, 0);

    for(const auto& service : DB){
        serviceCustomerCount[service.GetServiceID()]++;
    }
    return serviceCustomerCount;
}

int Statistics::GetStylistCustomerCount(std::string stylistID, const Datetime& T){
    dbServiceDone.Query("stylistID", stylistID);
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));
    return dbServiceDone.GetResults().size();
}

int Statistics::GetStylistRateSum(std::string stylistID, const Datetime& T){
    dbServiceDone.Query("stylistID", stylistID);
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    int sum = 0;
    for(const auto& service : DB) {
        sum += service.GetRating();
    }
    return sum;
}

int Statistics::GetStylistRateCount(std::string stylistID, const Datetime& T){
    dbServiceDone.Query("stylistID", stylistID);
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    int cnt = 0;
    for(const auto& service : DB) {
        if(service.GetRating() != 0) cnt++;
    }
    return cnt;
}

std::vector<int> Statistics::GetStylistServiceCustomerCount(std::string stylistID, const Datetime& T){
    dbServiceDone.Query("stylistID", stylistID);
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceCustomerCount(SERVICES_COUNT, 0);

    for(const auto& service : DB){
        serviceCustomerCount[service.GetServiceID()]++;
    }
    return serviceCustomerCount;
}

std::vector<int> Statistics::GetStylistServiceRateCount(std::string stylistID, const Datetime& T){
    dbServiceDone.Query("stylistID", stylistID);
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceRateCount(SERVICES_COUNT, 0);

    for(const auto& service : DB){
        if (service.GetRating() == 0) continue;
        serviceRateCount[service.GetRating()]++;
    }

    return serviceRateCount;
}

std::vector<int> Statistics::GetStylistServiceRateSum(std::string stylistID, const Datetime& T){
    dbServiceDone.Query("stylistID", stylistID);
    dbServiceDone.Query("year", std::to_string(T.GetYear()));
    dbServiceDone.Query("month", std::to_string(T.GetMonth()));
    dbServiceDone.Query("day", std::to_string(T.GetDay()));

    std::vector<ServiceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceRateSum(SERVICES_COUNT, 0);

    for(const auto& service : DB){
        if (service.GetRating() == 0) continue;
        serviceRateSum[service.GetRating()] += service.GetRating();
    }

    return serviceRateSum;
}