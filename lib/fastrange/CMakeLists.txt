cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(fastrange)

add_library(fastrange INTERFACE)

target_include_directories(fastrange INTERFACE include)
