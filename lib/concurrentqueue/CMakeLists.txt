cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(concurrentqueue)

add_library(concurrentqueue INTERFACE)

target_include_directories(concurrentqueue INTERFACE include)
