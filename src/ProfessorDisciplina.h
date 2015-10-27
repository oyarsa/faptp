#ifndef PROFESSORDISCIPLINA_H
#define	PROFESSORDISCIPLINA_H

#include <string>

#include "Professor.h"
#include "Disciplina.h"

#include "UUID.h"

#define PROFESSOR_DISCIPLINA_ID           0
#define PROFESSOR_DISCIPLINA_PROFESSOR    1
#define PROFESSOR_DISCIPLINA_DISCIPLINA   2
#define PROFESSOR_DISCIPLINA_PESO         3

class ProfessorDisciplina {
  friend class Representacao;
  friend class Grade;
  friend class Horario;
  friend class Resolucao;
public:
  ProfessorDisciplina(Professor* pProfessor, Disciplina* pDisciplina);
  ProfessorDisciplina(Professor* pProfessor, Disciplina* pDisciplina, std::string pId);
  virtual ~ProfessorDisciplina();
  
  Disciplina* getDisciplina() const;
private:
  std::string id;

  Professor *professor;
  Disciplina *disciplina;
  
  void init(Professor* pProfessor, Disciplina* pDisciplina, std::string pId);
};

#endif	/* PROFESSORDISCIPLINA_H */