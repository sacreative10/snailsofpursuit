
project(snailsofpursuit)
cmake_minimum_required(VERSION 3.5)


add_subdirectory(external/MDCPP)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g")


add_executable(snailsofpursuit src/main.cpp)
target_link_libraries(snailsofpursuit mdcpp)

