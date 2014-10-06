#include "ProfessorDisciplina.h"

ProfessorDisciplina::ProfessorDisciplina(Professor* pProfessor, Disciplina* pDisciplina) {
  UUID uuid;
  std::string pId;
  
  pId = uuid.GenerateUuid();
  
  init(pProfessor, pDisciplina, pId);
}

ProfessorDisciplina::ProfessorDisciplina(Professor* pProfessor, Disciplina* pDisciplina, std::string pId) {
  init(pProfessor, pDisciplina, pId);
}

void ProfessorDisciplina::init(Professor* pProfessor, Disciplina* pDisciplina, std::string pId) {
  id = pId;

  professor = pProfessor;
  disciplina = pDisciplina;
}

ProfessorDisciplina::~ProfessorDisciplina() {
}

