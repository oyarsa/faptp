add_executable(faptp src/main.cpp)

target_link_libraries(faptp PRIVATE faptp-lib)
target_link_libraries(faptp PRIVATE fmt::fmt)
target_link_libraries(faptp PRIVATE jsoncpp_lib)
target_link_libraries(faptp PRIVATE cxxopts)
