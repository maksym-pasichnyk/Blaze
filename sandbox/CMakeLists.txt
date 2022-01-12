cmake_minimum_required(VERSION 3.18)
project(sandbox)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_SYSTEM_NAME MATCHES "Android")
    add_library(sandbox SHARED src/main.cpp)
else()
    add_executable(sandbox src/main.cpp)
endif()

target_link_libraries(sandbox PRIVATE blaze)