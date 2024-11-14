cmake_minimum_required(VERSION 3.29)
project(PBL2_Clion)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")



include_directories(include)

# Sử dụng FetchContent để tải FTXUI, dùng cho xử lý đồ họa trên terminal.
include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG main  # Sử dụng nhánh chính
)
FetchContent_MakeAvailable(ftxui)

add_executable(PBL2_Clion main.cpp
            src/Database.cpp
            src/serviceDone.cpp
            src/Identifiable.cpp
            src/Member.cpp
            src/Customer.cpp
            src/Appointment.cpp
            src/define.cpp
            src/Stylist.cpp
            src/Datetime.cpp
)

# Dùng các thành phần trong thư viện FTXUI
target_link_libraries(PBL2_Clion
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)
