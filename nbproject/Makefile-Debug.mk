#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-MacOSX
CND_DLIB_EXT=dylib
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/src/Aleatorio.o \
	${OBJECTDIR}/src/AlunoPerfil.o \
	${OBJECTDIR}/src/Disciplina.o \
	${OBJECTDIR}/src/Grade.o \
	${OBJECTDIR}/src/Horario.o \
	${OBJECTDIR}/src/PTP.o \
	${OBJECTDIR}/src/Professor.o \
	${OBJECTDIR}/src/ProfessorDisciplina.o \
	${OBJECTDIR}/src/Representacao.o \
	${OBJECTDIR}/src/Resolucao.o \
	${OBJECTDIR}/src/Solucao.o \
	${OBJECTDIR}/src/UUID.o \
	${OBJECTDIR}/src/Util.o \
	${OBJECTDIR}/src/includes/jsoncpp.o \
	${OBJECTDIR}/src/includes/parametros.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++11
CXXFLAGS=-std=c++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/faptp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/faptp: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/faptp ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/src/Aleatorio.o: src/Aleatorio.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Aleatorio.o src/Aleatorio.cpp

${OBJECTDIR}/src/AlunoPerfil.o: src/AlunoPerfil.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/AlunoPerfil.o src/AlunoPerfil.cpp

${OBJECTDIR}/src/Disciplina.o: src/Disciplina.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Disciplina.o src/Disciplina.cpp

${OBJECTDIR}/src/Grade.o: src/Grade.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Grade.o src/Grade.cpp

${OBJECTDIR}/src/Horario.o: src/Horario.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Horario.o src/Horario.cpp

${OBJECTDIR}/src/PTP.o: src/PTP.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/PTP.o src/PTP.cpp

${OBJECTDIR}/src/Professor.o: src/Professor.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Professor.o src/Professor.cpp

${OBJECTDIR}/src/ProfessorDisciplina.o: src/ProfessorDisciplina.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ProfessorDisciplina.o src/ProfessorDisciplina.cpp

${OBJECTDIR}/src/Representacao.o: src/Representacao.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Representacao.o src/Representacao.cpp

${OBJECTDIR}/src/Resolucao.o: src/Resolucao.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Resolucao.o src/Resolucao.cpp

${OBJECTDIR}/src/Solucao.o: src/Solucao.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Solucao.o src/Solucao.cpp

${OBJECTDIR}/src/UUID.o: src/UUID.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/UUID.o src/UUID.cpp

${OBJECTDIR}/src/Util.o: src/Util.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Util.o src/Util.cpp

${OBJECTDIR}/src/includes/jsoncpp.o: src/includes/jsoncpp.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/includes
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/includes/jsoncpp.o src/includes/jsoncpp.cpp

${OBJECTDIR}/src/includes/parametros.o: src/includes/parametros.cpp 
	${MKDIR} -p ${OBJECTDIR}/src/includes
	${RM} "$@.d"
	$(COMPILE.cc) -O3 -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/includes/parametros.o src/includes/parametros.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/faptp

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
