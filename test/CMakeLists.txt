cmake_minimum_required(VERSION 3.12)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

project(test)

add_subdirectory("D:\\OneDrive - The University of Technology\\PBL\\Project\\PBL2_latest\\FTXUI" ".\\FTXUI")
add_executable(test main.cpp finish.cpp)

target_link_libraries(test
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)