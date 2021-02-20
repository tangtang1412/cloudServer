cmake_minimum_required(VERSION 3.17)
project(cloudServer)

set(CMAKE_CXX_STANDARD 11)

add_executable(cloudServer main.cpp socket.cpp socket.h ptread.cpp ptread.h work.cpp work.h)