project(faPTP)
cmake_minimum_required(VERSION 2.8)

set(MODELO FALSE)

if (MODELO)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ModeloGrade)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ModeloGrade ${CMAKE_CURRENT_BINARY_DIR}/ModeloGrade)
    add_definitions(-DIL_STD -DMODELO)
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(FAPTP_INCLUDE
    include/faptp/Aleatorio.h
    include/faptp/AlunoPerfil.h
    include/faptp/Disciplina.h
    include/faptp/Grade.h
    include/faptp/Horario.h
    include/faptp/Output.h
    include/faptp/Professor.h
    include/faptp/ProfessorDisciplina.h
    include/faptp/Representacao.h
    include/faptp/Resolucao.h
    include/faptp/Semana.h
    include/faptp/Solucao.h
    include/faptp/Util.h
    include/faptp/UUID.h
    include/faptp/Algorithms.h
    include/faptp/Configuracao.h
    include/faptp/SA.h
    include/faptp/ILS.h
    include/faptp/WDJU.h
    include/faptp/HySST.h
    include/faptp/Timer.h
    )

set(FAPTP_SOURCE
    src/faptp/Aleatorio.cpp
    src/faptp/AlunoPerfil.cpp
    src/faptp/Disciplina.cpp
    src/faptp/Grade.cpp
    src/faptp/Horario.cpp
    src/faptp/Output.cpp
    src/faptp/Professor.cpp
    src/faptp/ProfessorDisciplina.cpp
    src/faptp/Representacao.cpp
    src/faptp/Resolucao.cpp
    src/faptp/Solucao.cpp
    src/faptp/Util.cpp
    src/faptp/UUID.cpp
    src/faptp/SA.cpp
    src/faptp/ILS.cpp
    src/faptp/WDJU.cpp
    src/faptp/HySST.cpp
    )

set(FAPTP_FILES
    ${FAPTP_INCLUDE}
    ${FAPTP_SOURCE})

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++14 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -march=native ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  add_definitions("-stdlib=libstdc++")
  set(CMAKE_CXX_FLAGS "-std=c++14 ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

if (MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD /Z7")
  string (REPLACE "/D_DEBUG" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4100 -D_CRT_SECURE_NO_WARNINGS")
endif()

add_library(faptp ${FAPTP_FILES})
target_link_libraries(faptp ${CONAN_LIBS})

add_executable(faptp-app src/faptp-app/main.cpp)
target_link_libraries(faptp-app faptp)

if (MODELO)
  target_link_libraries(faptp modelo-grade-lib)
endif()
