#ifndef GRADE_H
#define	GRADE_H

#include <algorithm>
#include <string>
#include <vector>

#include "Semana.h"

#include "Representacao.h"
#include "Horario.h"
#include "AlunoPerfil.h"
#include "Disciplina.h"
#include "ProfessorDisciplina.h"

class Grade : public Representacao {
public:
  Grade(int pBlocosTamanho, int pCamadasTamanho, AlunoPerfil *pAlunoPerfil, Horario *pHorario);
  virtual ~Grade();

  bool insert(Disciplina* pDisciplina);
  bool insert(Disciplina* pDisciplina, bool force);
  int remove(Disciplina* pDisciplina);

  double getObjectiveFunction();
private:
  AlunoPerfil *alunoPerfil;
  Horario *horario;
  
  int fo;
  std::vector<ProfessorDisciplina*> professorDisciplinas;
  std::vector<std::string> problemas;
  ProfessorDisciplina *professorDisciplinaTemp;

  void init();
  
  bool havePreRequisitos(Disciplina *pDisciplina);
  bool checkCollision(Disciplina* pDisciplina, int camada);
  bool isViable(Disciplina* pDisciplina, int camada);
  
  void add(Disciplina* pDisciplina, int camada);
};

#endif	/* GRADE_H */

