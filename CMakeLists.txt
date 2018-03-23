cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(faPTP
	    LANGUAGES CXX)

if (MSVC)
	add_compile_options(/W4 /MP)
    set(OPTIMISATION_OPTIONS /O2 /Oi /Ot /Ob2 /arch:AVX /fp:fast)
else()
	add_compile_options(-Wall -Wpedantic)
    set(OPTIMISATION_OPTIONS -Ofast -mavx)
endif()

add_compile_options("$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:${OPTIMISATION_OPTIONS}>")

find_package(fmt REQUIRED)
#find_package(OpenMP REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(CURL REQUIRED)

# Bibliotecas
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "Build examples for CXXOpts")
add_subdirectory(lib/cxxopts)
add_subdirectory(lib/fastrange)
add_subdirectory(lib/range)
add_subdirectory(lib/tsl)
add_subdirectory(lib/optional)

# Módulos
add_subdirectory(src/faptp)
add_subdirectory(src/faptp-exp)
add_subdirectory(src/faptp-lib)
