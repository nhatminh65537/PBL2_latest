#ifndef STATISTICS_H
#define STATISTICS_H
#include <map>
#include "Datetime.h"
#include "vector"


class Statistics {
    public:
        Statistics();
        ~Statistics();

        //Lượng khách hàng trong 1 ngày
        static int GetCustomerCount(const Datetime& = Datetime::Now()); // OK
        static int GetCustomerCount_UpToNow(); // CHO ĐẾN NAY

        //Tần số sử dụng của mỗi dịch vụ trong 1 ngày
        static std::vector<int> GetServiceCount(const Datetime& = Datetime::Now()); // OK
        static std::vector<int> GetServiceCount_UpToNow(); // CHO ĐẾN NAY // OK

        //Đánh giá trung bình cho toàn bộ dịch vụ trong 1 ngày
        static double GetAverageRating(const Datetime& = Datetime::Now()); // OK
        static double GetAverageRating_UpToNow(); // CHO ĐẾN NAY

        //Số lần phục vụ của mỗi stylist
        static std::map<std::string, int> GetStylistServeTimes(const Datetime& = Datetime::Now()); // Trong 1 ngày
        static std::map<std::string, int> GetStylistServeTimes_UpToNow(); //CHO ĐẾN NAY

        //Chất lượng phục vụ trung bình của mỗi stylist trong 1 ngày
        static std::map<std::string,double> GetStylistQuality(const Datetime& = Datetime::Now());
        static std::map<std::string,double> GetStylistQuality_UpToNow(); // CHO ĐẾN NAY

        // Tóm tắt các thông tin. Gồm (maxRating,minRating,avgRating,customerCount)
        static std::map<std::string,double>GetSummary(const Datetime& = Datetime::Now());
        static std::map<std::string,double> GetSummary_UpToNow(); // CHO ĐẾN NAY

        static void statistics(Datetime);                        ///Ghi dữ liệu vào file
        static void statistics();                                // thống kê ngày now
        static void statistics(Datetime, Datetime);

};
#endif // STATISTICS_H