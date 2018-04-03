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
    src/Aleatorio.cpp
    src/AlunoPerfil.cpp
    src/Constantes.cpp
    src/DadosModelo.cpp
    src/Disciplina.cpp
    src/Grade.cpp
    src/Horario.cpp
    src/Output.cpp
    src/Professor.cpp
    src/ProfessorDisciplina.cpp
    src/Representacao.cpp
    src/Resolucao.cpp
    src/Solucao.cpp
    src/Util.cpp
    src/UUID.cpp
    src/SA.cpp
    src/ILS.cpp
    src/WDJU.cpp
    src/HySST.cpp
    )

target_include_directories(faptp-lib PUBLIC include)

target_link_libraries(faptp-lib PRIVATE jsoncpp_lib)
target_link_libraries(faptp-lib PUBLIC fmt::fmt)
target_link_libraries(faptp-lib PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(faptp-lib PUBLIC tsl)
target_link_libraries(faptp-lib PUBLIC range)
target_link_libraries(faptp-lib PRIVATE fastrange)

target_compile_features(faptp-lib PUBLIC cxx_std_17)
target_compile_definitions(faptp-lib PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)

