cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(readerwriterqueue)

add_library(readerwriterqueue INTERFACE)

target_include_directories(readerwriterqueue INTERFACE include)
