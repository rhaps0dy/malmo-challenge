cmake_minimum_required(VERSION 2.8.11)
project(SickleblowOne)

#CXXFLAGS
set(CMAKE_CXX_FLAGS_DEBUG  "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DRELEASE -DNO_LOGS")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g")
add_definitions(-std=c++11 -D_GLIBCXX_DEBUG)

add_executable(main main.cpp)
