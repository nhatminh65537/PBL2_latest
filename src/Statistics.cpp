#include "Statistics.h"
#include <filesystem>
#include "Database.h"
#include "Datetime.h"
#include "serviceDone.h"

Statistics::Statistics() {
    //ctor
}

Statistics::~Statistics() {
    //dtor
}

int Statistics::GetCustomerCount(const Datetime& T){
    std::vector<serviceDone> DB = dbServiceDone.GetResults();
    int count = 0;
    for(const auto& service : DB){
        if(IsSameDay(service.GetTime(), T)){
            count++;
        }
    }
    return count;
}

std::vector<int> Statistics::GetServiceCount(const Datetime& T){
    std::vector<serviceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceCount(8,0);

    for(const auto& service : DB){
        if(IsSameDay(service.GetTime(), T)){
            serviceCount[service.GetServiceID()]++;
        }
    }

    return serviceCount;
}

std::vector<int> Statistics::GetServiceCount_UpToNow(){
    std::vector<serviceDone> DB = dbServiceDone.GetResults();
    std::vector<int>serviceCount(8,0);

    for(const auto& service : DB){
        serviceCount[service.GetServiceID()]++;
    }

    return serviceCount;
}

double Statistics::GetAverageRating(const Datetime&  T){
    std::vector<serviceDone> DB = dbServiceDone.GetResults();
    double sum = 0.0;
    int cnt=0;
    for(const auto& service : DB) {
        if(IsSameDay(service.GetTime(), T)) {
            sum += service.GetRating();
            cnt++;
        }
    }
    return sum/double(cnt);
}

std::map<std::string, int> Statistics::GetStylistServeTimes(const Datetime& T){
    std::vector<serviceDone> vtServiceDone = dbServiceDone.GetResults();
    std::vector<Stylist> vtStylist = dbStylist.GetResults();
    std::map<std::string, int> ServeTimesCount;

    for (const auto& x : vtStylist) {
        ServeTimesCount[x.GetID()] = 0;
    }

    for(const auto& service : vtServiceDone){
        if(IsSameDay(service.GetTime(), T)){
            ServeTimesCount[service.GetStylistID()]++;
        }
    }

    return ServeTimesCount;
}

std::map<std::string, double> Statistics::GetStylistQuality(const Datetime& T){
    std::vector<serviceDone> DB = dbServiceDone.GetResults();
    std::map<std::string,double> stylishQuality;
    std::map<std::string,double> countQualityPerStylist;

    for(const auto& service : DB) {
        if(IsSameDay(service.GetTime(), T)) {
            if(stylishQuality.find(service.GetStylistID()) == stylishQuality.end()) {
                stylishQuality[service.GetStylistID()] = static_cast<double>(service.GetRating());
                countQualityPerStylist[service.GetStylistID()] = 1;
            } else {
                stylishQuality[service.GetStylistID()] += static_cast<double>(service.GetRating());
                countQualityPerStylist[service.GetStylistID()]++;
            }
        }
    }

    for(auto& stylist : stylishQuality) {
        stylist.second /= countQualityPerStylist[stylist.first];
    }

    return stylishQuality;
}

void Statistics::statistics(Datetime T){
    std::string time = std::to_string(T.GetDay()) + "/" + std::to_string(T.GetMonth()) + "/" + std::to_string(T.GetYear());
    std::string fileNameCustomer = time + "_Customer.txt";
    std::string fileNameStylist  = time + "_Stylist.txt";

    int customerCount = GetCustomerCount(T);
    std::string customerHabit = getCustomerHabit(T);
    std::string customerFeedback = getCustomerFeedback(T);
    std::map<std::string, int> stylishServe = getStylishServe(T);
    std::map<std::string, float> stylishQuality = getStylishQuality(T);

    //Kiểm tra file có tồn tại? Nhưng vẫn không ảnh hưởng!!!
    if(std::filesystem::exists(fileNameCustomer)) {
        std::cout << "Thư mục " << fileNameCustomer << " đã tồn tại" << '\n';
    }
    else if(std::filesystem::exists(fileNameStylist)) {
        std::cout << "Thư mục " << fileNameStylist << " đã tồn tại" << '\n';
    }


    std::ofstream outCustomer(fileNameCustomer);
    if(outCustomer.is_open()){
        outCustomer << "Số lượng khách hàng trong ngày"          << time << ":" << '\n' <<  customerCount << '\n';
        outCustomer << "Tần suất sử dụng mỗi dịch vụ trong ngày" << time << ":" << '\n' << customerHabit << '\n';
        outCustomer << "Đánh giá mỗi dịch vụ trong ngày"         << time << ":" << '\n' << customerFeedback << '\n';
        outCustomer.close();
    } else {
        std::cout << "Không thể mở file " << fileNameCustomer << '\n';
    }

    std::ofstream outStylist(fileNameStylist);
    if(outStylist.is_open()){
        outStylist << "Số lần phục vụ của mỗi stylist trong ngày" << time << ":" << '\n';
        for(const auto& stylist : stylishServe){
            outStylist << stylist.first << ": " << stylist.second << '\n';
        }
        outStylist << "Chất lượng phục vụ của mỗi stylist trong ngày" << time << ":" << '\n';
        for(const auto& stylist : stylishQuality){
            outStylist << stylist.first << ": " << stylist.second << '\n';
        }
        outStylist.close();
    } else {
        std::cout << "Không thể mở file " << fileNameStylist << '\n';
    }
}

void Statistics::statistics() {
    statistics(Datetime::Now());
}

void Statistics::statistics(Datetime L, Datetime R) {
    do {
        statistics(L);
        L.NextDay();
    }while (!IsSameDay(L, R));
}