#ifndef GRADE_H
#define	GRADE_H

#include <vector>

#include "Semana.h"

#include "Representacao.h"
#include "Horario.h"
#include "AlunoPerfil.h"
#include "ProfessorDisciplina.h"

class Grade : public Representacao {
public:
  Grade(int pBlocosTamanho, int pCamadasTamanho, AlunoPerfil* pAlunoPerfil);
  virtual ~Grade();

  bool insert(ProfessorDisciplina* pProfessorDisciplina, Horario* pHorario);
  bool insert(ProfessorDisciplina* pProfessorDisciplina, Horario* pHorario, bool force);

  double getObjectiveFunction();
private:
  AlunoPerfil *alunoPerfil;
  void init();
};

#endif	/* GRADE_H */

