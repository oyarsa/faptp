#include <faptp-lib/ProfessorDisciplina.h>
#include <faptp-lib/UUID.h>

ProfessorDisciplina::ProfessorDisciplina(Professor* pProfessor, Disciplina* pDisciplina)
    : id(fagoc::UUID::GenerateUuid())
      , professor(pProfessor)
      , disciplina(pDisciplina) {}

ProfessorDisciplina::ProfessorDisciplina(Professor* pProfessor,
                                         Disciplina* pDisciplina,
                                         const std::string& pId)
    : id(pId)
      , professor(pProfessor)
      , disciplina(pDisciplina) {}
