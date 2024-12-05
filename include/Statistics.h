#ifndef STATISTICS_H
#define STATISTICS_H
#include <map>
#include "Datetime.h"
#include "vector"
 
 
class Statistics {
    private:
        Statistics();
    public:
        static Statistics& getInstance() {
            static Statistics instance;
            return instance;
        }
        Statistics(Statistics const&) = delete;
        void operator=(Statistics const&) = delete;
        
        // Statistics functions
        static float GetAverage(const std::vector<float>);
        static float GetStandardDeviation(const std::vector<float>);

        //Lượng khách hàng trong 1 ngày
        static int GetCustomerCount(const Datetime& = Datetime::Now()); // OK
        static int GetCustomerCount_UpToNow(); // CHO ĐẾN NAY 
 
        //Tần số sử dụng của mỗi dịch vụ trong 1 ngày
        static std::vector<int> GetServiceCount(const Datetime& = Datetime::Now()); // OK
        static std::vector<int> GetServiceCount_UpToNow(); // CHO ĐẾN NAY // OK

        static std::vector<int> GetServiceRateCount(const Datetime& = Datetime::Now()); // OK
        static std::vector<int> GetServiceRateSum(const Datetime& = Datetime::Now()); // OK
        static std::vector<int> GetServiceCustomerCount(const Datetime& = Datetime::Now()); // OK

        static int GetStylistCustomerCount(std::string stylistID, const Datetime& T);
        static int GetStylistRateSum(std::string stylistID, const Datetime& T);
        static int GetStylistRateCount(std::string stylistID, const Datetime& T);

        static std::vector<int> GetStylistServiceCustomerCount(std::string stylistID, const Datetime& T);
        static std::vector<int> GetStylistServiceRateCount(std::string stylistID, const Datetime& T);
        static std::vector<int> GetStylistServiceRateSum(std::string stylistID, const Datetime& T);
};
#endif // STATISTICS_H