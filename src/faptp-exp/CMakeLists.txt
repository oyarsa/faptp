add_executable(faptp-exp
    src/experimento.h
    src/experimento.cpp
    src/main.cpp
    )

target_include_directories(faptp-exp PRIVATE ${CURL_INCLUDE_DIRS})

target_link_libraries(faptp-exp PRIVATE faptp-lib)
target_link_libraries(faptp-exp PRIVATE ${CURL_LIBRARIES})
target_link_libraries(faptp-exp PRIVATE $<$<CXX_COMPILER_ID:GNU>:pthread>)
target_link_libraries(faptp-exp PRIVATE cxxopts)
