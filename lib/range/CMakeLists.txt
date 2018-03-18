cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(range)

add_library(range INTERFACE)

target_include_directories(range INTERFACE include)
