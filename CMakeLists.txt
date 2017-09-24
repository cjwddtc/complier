cmake_minimum_required(VERSION 3.8)
project(main)

set(CMAKE_CXX_STANDARD 17)

ADD_EXECUTABLE(main template.hpp dfa.h gram_fun.h gram_fun.cpp gram_tree.h gram_tree.cpp id_manager.h id_manager.cpp main.cpp yufashu.hpp yufashu.cpp predef.h regex.hpp map_two.hpp)
