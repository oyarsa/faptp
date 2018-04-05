#include <faptp-lib/ProfessorDisciplina.h>
#include <faptp-lib/UUID.h>

ProfessorDisciplina::ProfessorDisciplina(Professor* pProfessor, Disciplina* pDisciplina)
    : professor(pProfessor)
      , disciplina(pDisciplina) {}
