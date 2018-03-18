cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(tsl)

add_library(tsl INTERFACE)

target_include_directories(tsl INTERFACE include)
