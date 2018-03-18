cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(faPTP
	    LANGUAGES CXX)

find_package(jsoncpp REQUIRED)
find_package(fmt REQUIRED)

add_executable(faptp src/main.cpp)

target_link_libraries(faptp PRIVATE faptp-lib)
target_link_libraries(faptp PRIVATE fmt::fmt)
target_link_libraries(faptp PRIVATE jsoncpp_lib)
target_link_libraries(faptp PRIVATE cxxopts)

target_compile_options(faptp PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4 /MP>)
target_compile_options(faptp PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wpedantic>)
