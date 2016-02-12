cmake_minimum_required(VERSION 2.8)

project(faPTP)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ModeloGrade/modelo-grade)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ModeloGrade ${CMAKE_CURRENT_BINARY_DIR}/ModeloGrade)

set(FAPTP_FILES
    src/Aleatorio.cpp
    src/Aleatorio.h
    src/AlunoPerfil.cpp
    src/AlunoPerfil.h
    src/Disciplina.cpp
    src/Disciplina.h
    src/Grade.cpp
    src/Grade.h
    src/Horario.cpp
    src/Horario.h
    src/Output.cpp
    src/Output.h
    src/Professor.cpp
    src/Professor.h
    src/ProfessorDisciplina.cpp
    src/ProfessorDisciplina.h
    src/Representacao.cpp
    src/Representacao.h
    src/Resolucao.cpp
    src/Resolucao.h
    src/Semana.h
    src/Solucao.cpp
    src/Solucao.h
    src/Util.cpp
    src/Util.h
    src/UUID.cpp
    src/UUID.h
    src/parametros.cpp
    src/parametros.h
    src/includes/jsoncpp.cpp
    template/Algorithms.h
    )

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

add_executable(faptp ${FAPTP_FILES} main.cpp)
target_link_libraries(faptp modelo-grade-lib)
