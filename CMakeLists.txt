cmake_minimum_required(VERSION 3.29)
project(PBL2_Clion)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(PBL2_Clion main.cpp
            src/Database.cpp
            src/serviceDone.cpp
            src/Identifiable.cpp
            src/Member.cpp
            src/Customer.cpp
            src/Appointment.cpp
        src/define.cpp
)


