cmake_minimum_required(VERSION 3.29)
project(PBL2_Clion)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

option(TEST_UI "Build the tests for the UI" ON)
option(FTXUI_ONLINE "Set to ON to disable online features" OFF)

include_directories(include)

if (FTXUI_ONLINE)
        # Sử dụng FetchContent để tải FTXUI, dùng cho xử lý đồ họa trên terminal.
        include(FetchContent)
        set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
        FetchContent_Declare(ftxui
                GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
                GIT_TAG main  # Sử dụng nhánh chính
        )
        FetchContent_MakeAvailable(ftxui)
else()
        add_subdirectory(FTXUI)
endif()

if (TEST_UI)
        add_definitions(-DTEST_UI)
        add_executable(PBL2_Clion main.cpp
                src/ui.cpp 
                src/call.cpp
                ui_demo.cpp
        )
else()
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
endif()

# Dùng các thành phần trong thư viện FTXUI
target_link_libraries(PBL2_Clion
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)
