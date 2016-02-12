cmake_minimum_required(VERSION 2.8)

project(faPTP)

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

add_executable(faptp ${FAPTP_FILES} main.cpp)
