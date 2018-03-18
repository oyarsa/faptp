cmake_minimum_required(VERSION 3.9)

project(faPTP
	    LANGUAGES CXX)

option(Modelo "Incluir Modelo matemático da grade" OFF)
option(GUROBI "Utilizar solver Gurobi para gerar o horário" OFF)
option(CPLEX "Utilizar solver Gurobi para gerar o horário" OFF)

find_package(OpenMP REQUIRED)
find_package(CURL REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(fmt REQUIRED)

add_library(faptp-lib
    include/faptp-lib/Aleatorio.h
    include/faptp-lib/Algorithms.h
    include/faptp-lib/AlunoPerfil.h
    include/faptp-lib/Configuracao.h
    include/faptp-lib/Constantes.h
    include/faptp-lib/Cplex.h
    include/faptp-lib/DadosModelo.h
    include/faptp-lib/Disciplina.h
    include/faptp-lib/Grade.h
    include/faptp-lib/Gurobi.h
    include/faptp-lib/Horario.h
    include/faptp-lib/HySST.h
    include/faptp-lib/ILS.h
    include/faptp-lib/Modelo.h
    include/faptp-lib/Output.h
    include/faptp-lib/Professor.h
    include/faptp-lib/ProfessorDisciplina.h
    include/faptp-lib/Representacao.h
    include/faptp-lib/Resolucao.h
    include/faptp-lib/SA.h
    include/faptp-lib/Solucao.h
    include/faptp-lib/Timer.h
    include/faptp-lib/Util.h
    include/faptp-lib/UUID.h
    include/faptp-lib/WDJU.h
    src/faptp-lib/Aleatorio.cpp
    src/faptp-lib/AlunoPerfil.cpp
    src/faptp-lib/Constantes.cpp
    src/faptp-lib/DadosModelo.cpp
    src/faptp-lib/Disciplina.cpp
    src/faptp-lib/Grade.cpp
    src/faptp-lib/Horario.cpp
    src/faptp-lib/Output.cpp
    src/faptp-lib/Professor.cpp
    src/faptp-lib/ProfessorDisciplina.cpp
    src/faptp-lib/Representacao.cpp
    src/faptp-lib/Resolucao.cpp
    src/faptp-lib/Solucao.cpp
    src/faptp-lib/Util.cpp
    src/faptp-lib/UUID.cpp
    src/faptp-lib/SA.cpp
    src/faptp-lib/ILS.cpp
    src/faptp-lib/WDJU.cpp
    src/faptp-lib/HySST.cpp
    )

add_executable(faptp-exp
    include/faptp-exp/experimento.h
    src/faptp-exp/experimento.cpp
    src/faptp-exp/main.cpp
    )
target_link_libraries(faptp-exp PUBLIC faptp-lib)

add_executable(faptp
    src/faptp/main.cpp
    )
target_link_libraries(faptp PRIVATE faptp-lib)

target_link_libraries(faptp-lib PRIVATE OpenMP::OpenMP_CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor)

target_link_libraries(faptp-exp PRIVATE ${CURL_LIBRARIES})
target_include_directories(faptp-exp PRIVATE ${CURL_INCLUDE_DIRS})

if (Modelo)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ModeloGrade ${CMAKE_CURRENT_BINARY_DIR}/ModeloGrade)
    target_include_directories(faptp-lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ModeloGrade)
    target_compile_definitions(faptp-lib PRIVATE IL_STD Modelo)
  	target_link_libraries(faptp-lib Modelo-grade-lib)
endif()

if(MSVC)
	set(GUROBI_LIB_RELEASE "gurobi_c++mt2015.lib")
	set(GUROBI_LIB_DEBUG "gurobi_c++mtd2015.lib")
	set(GUROBI_LIB_C "gurobi70.lib")
else()
	set(GUROBI_LIB_RELEASE "libgurobi_c++.a")
	set(GUROBI_LIB_DEBUG "libgurobi_c++.a")
	set(GUROBI_LIB_C "libgurobi70.so")
endif()

set(GUROBI_LIB_RELEASE "$ENV{GUROBI_HOME}/lib/${GUROBI_LIB_RELEASE}")
set(GUROBI_LIB_DEBUG "$ENV{GUROBI_HOME}/lib/${GUROBI_LIB_DEBUG}")
set(GUROBI_LIB_C "$ENV{GUROBI_HOME}/lib/${GUROBI_LIB_C}")

if (GUROBI)
	target_include_directories(faptp-lib PRIVATE $ENV{GUROBI_HOME}/include)
	target_compile_definitions(faptp-lib PRIVATE GUROBI_ENABLED)
	target_link_libraries(faptp-lib
		debug "${GUROBI_LIB_DEBUG}"
		optimized "${GUROBI_LIB_RELEASE}")
	target_link_libraries(faptp-lib PRIVATE "${GUROBI_LIB_C}")
endif()

target_link_libraries(faptp-lib PRIVATE jsoncpp_lib)
target_link_libraries(faptp-lib PUBLIC fmt::fmt)

target_compile_features(faptp-lib PUBLIC cxx_std_17)

if(NOT MSVC)
    target_link_libraries(faptp-exp PRIVATE pthread)
    target_compile_options(faptp-lib PUBLIC
    	$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:-Ofast -mavx>)

    target_compile_options(faptp PRIVATE -Wall -Wpedantic)
    target_compile_options(faptp-lib PRIVATE -Wall -Wpedantic)
    target_compile_options(faptp-exp PRIVATE -Wall -Wpedantic)
else()
	target_compile_options(faptp-lib PUBLIC
		$<$<OR:$<CONFIG:Release>,$<CONFIG:RelWithDebInfo>>:/O2 /Oi /Ot /GL /Ob2 /arch\:AVX /fp\:fast /MT>)
	target_compile_options(faptp-lib PUBLIC $<$<CONFIG:Debug>:/MTd>)

	target_compile_options(faptp PRIVATE /W4 /MP)
	target_compile_options(faptp-lib PRIVATE /W4 /MP)
	target_compile_options(faptp-exp PRIVATE /W4 /MP)
endif()
