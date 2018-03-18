cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(faPTP-exp
	    LANGUAGES CXX)

find_package(CURL REQUIRED)

add_executable(faptp-exp
    src/experimento.h
    src/experimento.cpp
    src/main.cpp
    )

target_include_directories(faptp-exp PRIVATE ${CURL_INCLUDE_DIRS})

target_link_libraries(faptp-exp PRIVATE faptp-lib)
target_link_libraries(faptp-exp PRIVATE ${CURL_LIBRARIES})
target_link_libraries(faptp-exp PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:pthread>)
target_link_libraries(faptp-exp PRIVATE cxxopts)

target_compile_options(faptp-exp PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4 /MP>)
target_compile_options(faptp-exp PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wpedantic>)
