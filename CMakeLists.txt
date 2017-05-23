project(faPTP)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)
set(MODELO FALSE)

if (MODELO)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ModeloGrade)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ModeloGrade ${CMAKE_CURRENT_BINARY_DIR}/ModeloGrade)
    add_definitions(-DIL_STD -DMODELO)
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vendor)

set(faptp-lib_INCLUDE
    include/faptp-lib/Aleatorio.h
    include/faptp-lib/AlunoPerfil.h
    include/faptp-lib/Disciplina.h
    include/faptp-lib/Grade.h
    include/faptp-lib/Horario.h
    include/faptp-lib/Output.h
    include/faptp-lib/Professor.h
    include/faptp-lib/ProfessorDisciplina.h
    include/faptp-lib/Representacao.h
    include/faptp-lib/Resolucao.h
    include/faptp-lib/Semana.h
    include/faptp-lib/Solucao.h
    include/faptp-lib/Util.h
    include/faptp-lib/UUID.h
    include/faptp-lib/Algorithms.h
    include/faptp-lib/Configuracao.h
    include/faptp-lib/SA.h
    include/faptp-lib/ILS.h
    include/faptp-lib/WDJU.h
    include/faptp-lib/HySST.h
    include/faptp-lib/Timer.h
    )

set(faptp-lib_SOURCE
    src/faptp-lib/Aleatorio.cpp
    src/faptp-lib/AlunoPerfil.cpp
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
    src/faptp-lib/Semana.cpp
    )

set(faptp-lib_FILES
    ${faptp-lib_INCLUDE}
    ${faptp-lib_SOURCE})

add_library(faptp-lib ${faptp-lib_FILES})
target_link_libraries(faptp-lib ${CONAN_LIBS})


set(faptp-exp_INCLUDE
	include/faptp-exp/experimento.h
	)

set(faptp-exp_SOURCE
	src/faptp-exp/experimento.cpp
	src/faptp-exp/main.cpp)

set(faptp-exp_FILES
	${faptp-exp_INCLUDE}
	${faptp-exp_SOURCE})

add_executable(faptp-exp ${faptp-exp_FILES})
target_link_libraries(faptp-exp ${CONAN_LIBS})
target_link_libraries(faptp-exp faptp-lib)

add_executable(faptp src/faptp/main.cpp)
target_link_libraries(faptp ${CONAN_LIBS})
target_link_libraries(faptp faptp-lib)

if (NOT MSVC)
    target_link_libraries(faptp-exp pthread)
    set(GCC_OPTIMIZATIONS "-Ofast -march=native")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${GCC_OPTIMIZATIONS}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${GCC_OPTIMIZATIONS}")
else()
	# Otimizações e configuração do runtime para estático
    set(VC_OPTIMIZATIONS "/Ox /Oi /Ot /GL /arch:AVX /fp:fast")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT ${VC_OPTIMIZATIONS}")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT ${VC_OPTIMIZATIONS}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

	# Nível alto de warnings e compilação paralela, assim como C++17
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /MP /std:c++14")

	# Link Time Codegen
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /LTCG")
	set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /LTCG")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (MODELO)
  target_link_libraries(faptp-lib modelo-grade-lib)
endif()
