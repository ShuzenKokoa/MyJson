cmake_minimum_required(VERSION 3.0)
project(MyJson)

set(CMAKE_CXX_STANDARD 11)

add_executable(MyJson main.cpp Json/json.cpp Json/json.h Json/Parser.cpp Json/Parser.h)

#add_compile_options(-Wall -Wextra)
