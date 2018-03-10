#include <faptp-lib/ProfessorDisciplina.h>

ProfessorDisciplina::ProfessorDisciplina(Professor* pProfessor, Disciplina* pDisciplina)
    : id(UUID::GenerateUuid())
      , professor(pProfessor)
      , disciplina(pDisciplina) {}

ProfessorDisciplina::ProfessorDisciplina(Professor* pProfessor,
                                         Disciplina* pDisciplina,
                                         const std::string& pId)
    : id(pId)
      , professor(pProfessor)
      , disciplina(pDisciplina) {}
