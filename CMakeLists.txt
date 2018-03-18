cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(faPTP
	    LANGUAGES CXX)

# Bibliotecas
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "Build examples for CXXOpts")
add_subdirectory(lib/cxxopts)
add_subdirectory(lib/fastrange)
add_subdirectory(lib/range)
add_subdirectory(lib/tsl)

# Módulos
add_subdirectory(src/faptp)
add_subdirectory(src/faptp-exp)
add_subdirectory(src/faptp-lib)
