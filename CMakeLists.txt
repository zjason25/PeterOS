cmake_minimum_required(VERSION 3.10)
project(143b)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER "g++")

include_directories("include/")

add_executable(extManager src/exm.cpp)